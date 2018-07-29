#include "ltp1245.h"
#include "pinning.h"

#define LINEWIDTH               (64 * 6)
uint8_t LTP1245_Buffer[LINEWIDTH / 8 * LTP1245_BUFFER_LINES];

typedef struct State_t
{
    struct State_t (*fn)(void);
} State_t;

static volatile int Stepper_Delta = 0;
static int PrintLines;
static int CurrentBufferLine;
bool Printing = false;
static volatile int PulseWidth = 2000;

static void InitStepper(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    GPIOA->CRL = (GPIOA->CRL
        & ~(0x0f << (4 * PIN_STEPPER_AM))
        & ~(0x0f << (4 * PIN_STEPPER_AP))
        & ~(0x0f << (4 * PIN_STEPPER_BM))
        & ~(0x0f << (4 * PIN_STEPPER_BP)))
        | (0x01 << (4 * PIN_STEPPER_AM))    // Output, max. 10 MHz
        | (0x01 << (4 * PIN_STEPPER_AP))    // Output, max. 10 MHz
        | (0x01 << (4 * PIN_STEPPER_BM))    // Output, max. 10 MHz
        | (0x01 << (4 * PIN_STEPPER_BP))    // Output, max. 10 MHz
        ;

    TIM3->PSC = 72000000 / 100 / LTP1245_MAX_DRIVE_FREQ - 1;
    TIM3->ARR = 100;
    TIM3->DIER = TIM_DIER_UIE;
    TIM3->CR1 = TIM_CR1_CEN;

    NVIC_EnableIRQ(TIM3_IRQn);
}

static void InitSensors(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    GPIOA->CRL = (GPIOA->CRL
        & ~(0x0f << (4 * PIN_HEAD))
        & ~(0x0f << (4 * PIN_PAPER)))
        | (0x08 << (4 * PIN_HEAD))          // Input with pull-up/pull-down
        | (0x08 << (4 * PIN_PAPER))         // Input with pull-up/pull-down
        ;

    // Use pull-ups
    GPIOA->BSRR = (1 << PIN_HEAD) | (1 << PIN_PAPER);
}

static void InitDataLines(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    TIM2->PSC = 719;                // Each tick corresponds to ten microseconds
    TIM2->ARR = 201;                // 2 milliseconds
    TIM2->CCR3 = 1;
    TIM2->CCR4 = 1;
    TIM2->CR1 = TIM_CR1_OPM;        // One-pulse mode
    TIM2->CCMR2 = TIM_CCMR2_OC3M    // CH3 PWM mode 2
        | TIM_CCMR2_OC4M            // CH4 PWM mode 1
        ;
    TIM2->CCER = 0;
    TIM2->DIER = 0;

    // Remap TIM2 lines
    AFIO->MAPR |= AFIO_MAPR_TIM2_REMAP_FULLREMAP;

    GPIOB->BRR = (1 << PIN_DST1) | (1 << PIN_DST2) | (1 << PIN_LATCH);
    GPIOB->CRH = (GPIOB->CRH
        & ~(0x0f << (4 * PIN_DST1 - 32))
        & ~(0x0f << (4 * PIN_DST2 - 32))
        & ~(0x0f << (4 * PIN_LATCH - 32))
        & ~(0x0f << (4 * PIN_SCK - 32))
        & ~(0x0f << (4 * PIN_DIN - 32)))
        | (0x09 << (4 * PIN_DST1 - 32))     // Output in AF mode, max. 10 MHz
        | (0x09 << (4 * PIN_DST2 - 32))     // Output in AF mode, max. 10 MHz
        | (0x01 << (4 * PIN_LATCH - 32))    // Output, max. 10 MHz
        | (0x09 << (4 * PIN_SCK - 32))      // Output in AF mode, max. 10 MHz
        | (0x09 << (4 * PIN_DIN - 32))      // Output in AF mode, max. 10 MHz
        ;

    SPI2->CR1 = SPI_CR1_BIDIMODE | SPI_CR1_BIDIOE | SPI_CR1_SPE | SPI_CR1_MSTR
        | SPI_CR1_BR_2;
}

static void InitThermistor(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    GPIOB->CRL = (GPIOB->CRL
        & ~(0x0f << PIN_THERMISTOR))
        | (0x00 << PIN_THERMISTOR)          // Analog mode
        ;
    
    // Enable ADC
    ADC1->CR2 |= ADC_CR2_ADON;
    for(volatile int i = 0; i < 100; i++);

    // Calibrate ADC
    ADC1->CR2 |= ADC_CR2_CAL;
    while(ADC1->CR2 & ADC_CR2_CAL);

    // Enable EOC interrupt
    ADC1->CR1 = ADC_CR1_EOCIE;
    NVIC_EnableIRQ(ADC1_2_IRQn);

    // The thermistor is connected to ADC12_IN8 (PB0)
    ADC1->SQR3 = (8 << ADC_SQR3_SQ1_Pos);
    ADC1->SMPR2 = (7 << ADC_SMPR2_SMP8_Pos);

    // Start conversion, continuous mode
    ADC1->CR2 |= ADC_CR2_CONT;
    ADC1->CR2 |= ADC_CR2_ADON;
}

static bool HasPaper(void)
{
    return !(GPIOA->IDR & (1 << PIN_PAPER));
}

static bool HeadDown(void)
{
    return !(GPIOA->IDR & (1 << PIN_HEAD));
}

void ActivateHead(int mask)
{
    // Wait until timer has finished
    while(TIM2->CR1 & TIM_CR1_CEN);

    // Set activation pulse width
    TIM2->ARR = (PulseWidth / 5) + 1;

    TIM2->CCER = 0;
    if(mask & 1)
    {
        TIM2->CCER |= TIM_CCER_CC4E;
    }
    if(mask & 2)
    {
        TIM2->CCER |= TIM_CCER_CC3E;
    }
    TIM2->CNT = 0;
    TIM2->EGR = TIM_EGR_UG;
    TIM2->CR1 |= TIM_CR1_CEN;
}

static void SendLine(uint8_t *line)
{
    for(int i = 0; i < LINEWIDTH / 8; i++)
    {
        while(~SPI2->SR & SPI_SR_TXE);
        *((volatile uint8_t*)(&SPI2->DR)) = line[i];
    }
    while(SPI2->SR & SPI_SR_BSY);
    GPIOB->BSRR = (1 << PIN_LATCH);
    for(volatile int i = 0; i < 1000; i++);
    GPIOB->BRR = (1 << PIN_LATCH);
}

// Main state machine states

static State_t State_Idle(void);
static State_t State_PaperLoad(void);
static State_t State_InitialPaperFeed(void);
static State_t State_Printing(void);
static State_t State_FinalPaperFeed(void);

// Automatic paper loading
static State_t State_PaperLoad(void)
{
    if(!HasPaper())
    {
        if(HeadDown())
        {
            Stepper_Delta = 0;
        }
        else if(Stepper_Delta < 1000)
        {
            Stepper_Delta = 1000;
        }
    }
    else
    {
        if(Stepper_Delta == 0)
        {
            return (State_t){State_Idle};
        }
    }
    return (State_t){State_PaperLoad};
}

// Paper feed before printing
static State_t State_InitialPaperFeed(void)
{
    if(Stepper_Delta == 0)
    {
        return (State_t){State_Printing};
    }
    return (State_t){State_InitialPaperFeed};
}

// Actual printing
static State_t State_Printing(void)
{
    if(!HeadDown() || !HasPaper())
    {
        Printing = false;
        return (State_t){State_Idle};
    }

    if(Stepper_Delta == 1)
    {
        ActivateHead(3);
    }
    else if(Stepper_Delta == 0)
    {
        SendLine(LTP1245_Buffer + CurrentBufferLine * LINEWIDTH / 8);
        ActivateHead(3);
        CurrentBufferLine++;
        if(CurrentBufferLine >= LTP1245_BUFFER_LINES)
        {
            CurrentBufferLine = 0;
        }

        PrintLines--;

        if(PrintLines > 0)
        {
            Stepper_Delta = 2;
        }
        else
        {
            Stepper_Delta = 500;
            return (State_t){State_FinalPaperFeed};
        }
    }
    return (State_t){State_Printing};
}

// Paper feed after printing
static State_t State_FinalPaperFeed(void)
{
    if(!HasPaper() || !HeadDown())
    {
        Printing = false;
        return (State_t){State_Idle};
    }
    if(Stepper_Delta == 0)
    {
        Printing = false;
        return (State_t){State_Idle};
    }
    return (State_t){State_FinalPaperFeed};
}

static State_t State_Idle(void)
{
    if(!HasPaper())
    {
        return (State_t){State_PaperLoad};
    }
    if(Printing)
    {
        Stepper_Delta = 200;
        return (State_t){State_InitialPaperFeed};
    }
    return (State_t){State_Idle};
}

void LTP1245_Print(void)
{
    PrintLines = LTP1245_BUFFER_LINES * 20;
    CurrentBufferLine = 0;
    Printing = true;
}

void LTP1245_Init(void)
{
    InitDataLines();
    InitSensors();
    InitStepper();
    InitThermistor();

    for(int i = 0; i < sizeof(LTP1245_Buffer); i++)
    {
        int shift = (i * 8 / (64 * 6)) % 8;
        LTP1245_Buffer[i] =(0x11 << shift) | (0x11 >> (8 - shift));
    }

    LTP1245_Print();
}

void AdvanceStateMachine(void)
{
    static State_t state = {State_Idle};

    state = state.fn();
}

void TIM3_IRQHandler(void)
{
    if(TIM3->SR & TIM_SR_UIF)
    {
        static int substep = 0;
        static bool off;
        const int TABLE[] = {0, 1, 3, 2};
        const int GPIO_MASK = ((1 << PIN_STEPPER_AM) | (1 << PIN_STEPPER_AP)
            | (1 << PIN_STEPPER_BM) | (1 << PIN_STEPPER_BP));

        if(Stepper_Delta != 0)
        {
            off = false;
            if(Stepper_Delta > 0)
            {
                substep++;
                if(substep > 3)
                    substep = 0;
                Stepper_Delta--;
            }
            else
            {
                substep--;
                if(substep < 0)
                    substep = 3;
                Stepper_Delta++;
            }

            GPIOA->ODR = (GPIOA->ODR & ~GPIO_MASK)
                | ((TABLE[substep] & 1) ? (1 << PIN_STEPPER_AP)
                                        : (1 << PIN_STEPPER_AM))
                | ((TABLE[substep] & 2) ? (1 << PIN_STEPPER_BP)
                                        : (1 << PIN_STEPPER_BM));
        }
        else
        {
            if(off)
            {
                GPIOA->ODR = (GPIOA->ODR & ~GPIO_MASK);
                substep = 0;
            }
            off = true;
        }

        AdvanceStateMachine();

        TIM3->SR &= ~TIM_SR_UIF;
    }
}

void ADC1_2_IRQHandler(void)
{
    const int READINGS[] =
    {
        4095.0 * (1.0 - LTP1245_TH_REXT / (LTP1245_TH_REXT + 375.54)), // -40 °C
        4095.0 * (1.0 - LTP1245_TH_REXT / (LTP1245_TH_REXT + 275.39)), // -35 °C
        4095.0 * (1.0 - LTP1245_TH_REXT / (LTP1245_TH_REXT + 204.55)), // -30 °C
        4095.0 * (1.0 - LTP1245_TH_REXT / (LTP1245_TH_REXT + 153.76)), // -25 °C
        4095.0 * (1.0 - LTP1245_TH_REXT / (LTP1245_TH_REXT + 116.89)), // -20 °C
        4095.0 * (1.0 - LTP1245_TH_REXT / (LTP1245_TH_REXT + 89.82)),  // -15 °C
        4095.0 * (1.0 - LTP1245_TH_REXT / (LTP1245_TH_REXT + 69.71)),  // -10 °C
        4095.0 * (1.0 - LTP1245_TH_REXT / (LTP1245_TH_REXT + 54.61)),  // -5 °C
        4095.0 * (1.0 - LTP1245_TH_REXT / (LTP1245_TH_REXT + 43.17)),  // 0 °C
        4095.0 * (1.0 - LTP1245_TH_REXT / (LTP1245_TH_REXT + 34.42)),  // 5 °C
        4095.0 * (1.0 - LTP1245_TH_REXT / (LTP1245_TH_REXT + 27.66)),  // 10 °C
        4095.0 * (1.0 - LTP1245_TH_REXT / (LTP1245_TH_REXT + 22.40)),  // 15 °C
        4095.0 * (1.0 - LTP1245_TH_REXT / (LTP1245_TH_REXT + 18.27)),  // 20 °C
        4095.0 * (1.0 - LTP1245_TH_REXT / (LTP1245_TH_REXT + 15.00)),  // 25 °C
        4095.0 * (1.0 - LTP1245_TH_REXT / (LTP1245_TH_REXT + 12.40)),  // 30 °C
        4095.0 * (1.0 - LTP1245_TH_REXT / (LTP1245_TH_REXT + 10.31)),  // 35 °C
        4095.0 * (1.0 - LTP1245_TH_REXT / (LTP1245_TH_REXT + 8.63)),   // 40 °C
        4095.0 * (1.0 - LTP1245_TH_REXT / (LTP1245_TH_REXT + 7.26)),   // 45 °C
        4095.0 * (1.0 - LTP1245_TH_REXT / (LTP1245_TH_REXT + 6.14)),   // 50 °C
        4095.0 * (1.0 - LTP1245_TH_REXT / (LTP1245_TH_REXT + 5.22)),   // 55 °C
        4095.0 * (1.0 - LTP1245_TH_REXT / (LTP1245_TH_REXT + 4.46)),   // 60 °C
        4095.0 * (1.0 - LTP1245_TH_REXT / (LTP1245_TH_REXT + 3.83)),   // 65 °C
        4095.0 * (1.0 - LTP1245_TH_REXT / (LTP1245_TH_REXT + 3.30)),   // 70 °C
        4095.0 * (1.0 - LTP1245_TH_REXT / (LTP1245_TH_REXT + 2.86)),   // 75 °C
        4095.0 * (1.0 - LTP1245_TH_REXT / (LTP1245_TH_REXT + 2.48)),   // 80 °C
        4095.0 * (1.0 - LTP1245_TH_REXT / (LTP1245_TH_REXT + 2.17)),   // 85 °C
        4095.0 * (1.0 - LTP1245_TH_REXT / (LTP1245_TH_REXT + 1.90)),   // 90 °C
        4095.0 * (1.0 - LTP1245_TH_REXT / (LTP1245_TH_REXT + 1.67)),   // 95 °C
        4095.0 * (1.0 - LTP1245_TH_REXT / (LTP1245_TH_REXT + 1.47))    // 100 °C
    };

    int adc = ADC1->DR;
    
    // Find first temperature higher than the measured one
    int lower_entry = 0;
    for(int i = 1; i < sizeof(READINGS) / sizeof(READINGS[0]); i++)
    {
        if(adc >= READINGS[i])
        {
            lower_entry = i - 1;
            break;
        }
    }
    int higher_entry = lower_entry + 1;
    int temp = lower_entry * 5 - 40;    // Temperature in °C

    // Interpolate linearly
    if(higher_entry < sizeof(READINGS) / sizeof(READINGS[0]))
    {
        int diff = READINGS[lower_entry] - READINGS[higher_entry];
        temp += (READINGS[lower_entry] - adc) * 5 / diff;
    }

    // Use the formula from section 3.6, adjusted for integer arithmetic and
    // a pulse with in microseconds
    PulseWidth = (285 * 178 - (int)(1000 * 178 * 0.003135) * (temp - 25))
        / (int)((5 * 1.4 - 2.9) * (5 * 1.4 - 2.9));
}