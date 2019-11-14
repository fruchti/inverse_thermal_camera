#include "ov7670.h"
#include "pinning.h"
#include "debug.h"

#define REG_GAIN                0x00
#define REG_BLUE                0x01
#define REG_RED                 0x02
#define REG_VREF                0x03
#define REG_COM1                0x04
#define REG_BAVE                0x05
#define REG_GbAVE               0x06
#define REG_AECHH               0x07
#define REG_RAVE                0x08
#define REV_COM2                0x09
#define REG_PID                 0x0a
#define REG_VER                 0x0b
#define REG_COM3                0x0c
#define REG_COM4                0x0d
#define REG_COM5                0x0e
#define REG_COM6                0x0f
#define REG_AECH                0x10
#define REG_CLKRC               0x11
#define REG_COM7                0x12
#define REG_COM8                0x13
#define REG_COM9                0x14
#define REG_COM10               0x15
#define REG_HSTART              0x17
#define REG_HSTOP               0x18
#define REG_VSTRT               0x19
#define REG_VSTOP               0x1a
#define REG_PSHIFT              0x1b
#define REG_MIDH                0x1c
#define REG_MIDL                0x1d
#define REG_MVFP                0x1e
#define REG_LAEC                0x1f
#define REG_ADCCTR0             0x20
#define REG_ADCCTR1             0x21
#define REG_ADCCTR2             0x22
#define REG_ADCCTR3             0x23
#define REG_AEW                 0x24
#define REG_AEB                 0x25
#define REG_VPT                 0x26
#define REG_BBIAS               0x27
#define REG_GbBIAS              0x28
#define REG_EXHCH               0x2a
#define REG_EXHCL               0x2b
#define REG_RBIAS               0x2c
#define REG_ADVFL               0x2d
#define REG_ADVFH               0x2e
#define REG_YAVE                0x2f
#define REG_HSYST               0x30
#define REG_HSYEN               0x31
#define REG_HREF                0x32
#define REG_CHLF                0x33
#define REG_ARBLM               0x34
#define REG_ADC                 0x37
#define REG_ACOM                0x38
#define REG_OFON                0x39
#define REG_TSLB                0x3a
#define REG_COM11               0x3b
#define REG_COM12               0x3c
#define REG_COM13               0x3d
#define REG_COM14               0x3e
#define REG_EDGE                0x3f
#define REG_COM15               0x40
#define REG_COM16               0x41
#define REG_COM17               0x42
#define REG_REG4B               0x4b
#define REG_DNSTH               0x4c
#define REG_MTX1                0x4f
#define REG_MTX2                0x50
#define REG_MTX3                0x51
#define REG_MTX4                0x52
#define REG_MTX5                0x53
#define REG_MTX6                0x54
#define REG_BRIGHT              0x55
#define REG_CONTRAS             0x56
#define REG_CONTRAS_CENTER      0x57
#define REG_MTXS                0x58
#define REG_LCC1                0x62
#define REG_LCC2                0x63
#define REG_LCC3                0x64
#define REG_LCC4                0x65
#define REG_LCC5                0x66
#define REG_MANU                0x67
#define REG_MANV                0x68
#define REG_GFIX                0x69
#define REG_GGAIN               0x6a
#define REG_DBLV                0x6b
#define REG_AWBCTR3             0x6c
#define REG_AWBCTR2             0x6d
#define REG_AWBCTR1             0x6e
#define REG_AWBCTR0             0x6f
#define REG_SCALING_XSC         0x70
#define REG_SCALING_YSC         0x71
#define REG_SCALING_DCWCTR      0x72
#define REG_SCALING_PCLK_DIV    0x73
#define REG_REG74               0x74
#define REG_REG75               0x75
#define REG_REG76               0x76
#define REG_REG77               0x77
#define REG_SLOP                0x7a
#define REG_GAM1                0x7b
#define REG_GAM2                0x7c
#define REG_GAM3                0x7d
#define REG_GAM4                0x7e
#define REG_GAM5                0x7f
#define REG_GAM6                0x80
#define REG_GAM7                0x81
#define REG_GAM8                0x82
#define REG_GAM9                0x83
#define REG_GAM10               0x84
#define REG_GAM11               0x85
#define REG_GAM12               0x86
#define REG_GAM13               0x87
#define REG_GAM14               0x88
#define REG_GAM15               0x89
#define REG_RGB444              0x8c
#define REG_DM_LNL              0x92
#define REG_DM_LNH              0x93
#define REG_LCC6                0x94
#define REG_LCC7                0x95
#define REG_BD50ST              0x9d
#define REG_BD60ST              0x9e
#define REG_HAECC1              0x9f
#define REG_HAECC2              0xa0
#define REG_SCALING_PCLK_DELAY  0xa2
#define REG_NT_CTRL             0xa4
#define REG_BD50MAX             0xa5
#define REG_HAECC3              0xa6
#define REG_HAECC4              0xa7
#define REG_HAECC5              0xa8
#define REG_HAECC6              0xa9
#define REG_HAECC7              0xaa
#define REG_BD60MAX             0xab
#define REG_STR_OPT             0xac
#define REG_STR_R               0xad
#define REG_STR_G               0xae
#define REG_STR_B               0xaf
#define REG_ABLC1               0xb1
#define REG_THL_ST              0xb3
#define REG_THL_DLT             0xb5
#define REG_AD_CHB              0xbe
#define REG_AD_CHR              0xbf
#define REG_AD_CHGb             0xc0
#define REG_AD_CHGr             0xc1
#define REG_SATCR               0xc9

#define I2C_ADDRESS             0x42

uint8_t ImageBuffer[CAMERA_IMAGE_WIDTH * CAMERA_IMAGE_HEIGHT / 8];
static volatile int CurrentLine = 0;
uint8_t LineBuffer[CAMERA_IMAGE_WIDTH + 40];
int LineCount = 0;
static int FrameCount = 0;
volatile int Camera_Captured = 0;

static uint8_t ReadRegister(uint8_t reg)
{
    while(I2C1->SR2 & I2C_SR2_BUSY);
    I2C1->CR1 |= I2C_CR1_START;
    while(~I2C1->SR1 & I2C_SR1_SB);
    I2C1->DR = I2C_ADDRESS;
    while(~I2C1->SR1 & I2C_SR1_ADDR);
    I2C1->SR2;       // Dummy read
    I2C1->DR = reg;  // Write the register number to be read
    while(~I2C1->SR1 & (I2C_SR1_TXE | I2C_SR1_BTF));
    I2C1->CR1 |= I2C_CR1_STOP;

    // Read the register value
    while(I2C1->SR2 & I2C_SR2_BUSY);
    I2C1->CR1 |= I2C_CR1_START;
    while(~I2C1->SR1 & I2C_SR1_SB);
    I2C1->DR = I2C_ADDRESS | 1;
    while(~I2C1->SR1 & I2C_SR1_ADDR);
    I2C1->SR2;       // Dummy read
    I2C1->CR1 |= I2C_CR1_STOP;
    while(~I2C1->SR1 & I2C_SR1_RXNE);
    uint8_t data = I2C1->DR;
    return data;
}

static void WriteRegister(uint8_t reg, uint8_t value)
{
    while(I2C1->SR2 & I2C_SR2_BUSY);
    I2C1->CR1 |= I2C_CR1_START;
    while(~I2C1->SR1 & I2C_SR1_SB);
    I2C1->DR = I2C_ADDRESS;
    while(~I2C1->SR1 & I2C_SR1_ADDR);
    I2C1->SR2;          // Dummy read
    I2C1->DR = reg;     // Write the register number
    while(~I2C1->SR1 & (I2C_SR1_TXE | I2C_SR1_BTF));
    I2C1->DR = value;   // Write the register value
    while(~I2C1->SR1 & (I2C_SR1_TXE | I2C_SR1_BTF));
    I2C1->CR1 |= I2C_CR1_STOP;
}

void Camera_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    // Reset pin
    GPIOB->CRH = (GPIOB->CRH
        & ~(0x0f << (4 * PIN_CAMERA_RESET - 32)))
        | (0x01 << (4 * PIN_CAMERA_RESET - 32))     // Output, max. 10 MHz
        ;
    GPIOB->BRR = (1 << PIN_CAMERA_RESET);
    GPIOB->BSRR = (1 << PIN_CAMERA_RESET);


    // Enable MCO for camera main clock line (PLL / 2 -> 24 MHz)
    RCC->CFGR |= RCC_CFGR_MCO;
    GPIOA->CRH = (GPIOA->CRH
        & ~(0x0f << (4 * PIN_CAMERA_MCLK - 32)))
        | (0x0b << (4 * PIN_CAMERA_MCLK - 32))      // Output, max. 50 MHz
        ;

    AFIO->MAPR |= AFIO_MAPR_I2C1_REMAP;

    // I2C interface for camera configuration
    GPIOB->CRH = (GPIOB->CRH
        & ~(0x0f << (4 * PIN_CAMERA_SCL - 32))
        & ~(0x0f << (4 * PIN_CAMERA_SDA - 32)))
        | (0x0e << (4 * PIN_CAMERA_SCL - 32))       // AF OD output, 2 MHz
        | (0x0e << (4 * PIN_CAMERA_SDA - 32))       // AF OD output, 2 MHz
        ;

    I2C1->CR1 = I2C_CR1_SWRST;
    I2C1->CR1 = 0;
    I2C1->CR2 = (24 << I2C_CR2_FREQ_Pos);
    I2C1->CCR = I2C_CCR_FS | I2C_CCR_DUTY | (1 << I2C_CCR_CCR_Pos) | 5;
    I2C1->CR1 = I2C_CR1_PE;

    // Timer setup
    AFIO->MAPR |= AFIO_MAPR_TIM3_REMAP_PARTIALREMAP;
    GPIOB->CRL = (GPIOB->CRL
        & ~(0x0f << (PIN_CAMERA_HSYNC * 4))
        & ~(0x0f << (PIN_CAMERA_PCLK * 4)))
        | (0x04 << (PIN_CAMERA_HSYNC * 4))          // Floating input
        | (0x04 << (PIN_CAMERA_PCLK * 4))           // Floating input
        ;
    GPIOA->CRH = (GPIOA->CRH
        & ~(0x0f << (PIN_CAMERA_VSYNC * 4 - 32)))
        | (0x04 << (PIN_CAMERA_VSYNC * 4 - 32))     // Floating input
        ;

    // TIM1_CH2 is VSYNC
    TIM1->PSC = 0;
    TIM1->ARR = 65535;
    TIM1->CCMR1 = TIM_CCMR1_CC2S_0;
    TIM1->CCER = TIM_CCER_CC2E;
    TIM1->DIER = TIM_DIER_CC2IE;
    TIM1->CR1 = TIM_CR1_CEN;
    NVIC_SetPriority(TIM1_CC_IRQn, 0);
    NVIC_EnableIRQ(TIM1_CC_IRQn);

    // TIM3_CH2 is HSYNC and should trigger an interrupt, while TIM3_CH1 is the
    // pixel clock and should trigger DMA transfers
    TIM3->PSC = 0;
    TIM3->ARR = 1;
    TIM3->CCMR1 = TIM_CCMR1_CC2S_0 | TIM_CCMR1_CC1S_0 | TIM_CCMR1_IC1PSC_1;
    TIM3->CCER = TIM_CCER_CC2P | TIM_CCER_CC2E | TIM_CCER_CC1E | TIM_CCER_CC1P;
    TIM3->DIER = TIM_DIER_CC2IE;
    TIM3->CR1 = TIM_CR1_CEN;
    NVIC_SetPriority(TIM3_IRQn, 0);
    NVIC_EnableIRQ(TIM3_IRQn);

    // Fetch GPIOA IDR lower byte
    DMA1_Channel6->CPAR = (uint32_t)&(GPIOA->IDR);

    // Startup delay
    for(volatile int i = 0; i < 1000; i++);

    // Camera configuration
    ReadRegister(REG_PID);

    // Disable timing resets
    WriteRegister(REG_COM6, 0x00);

    // Set clock prescaler to 2
    WriteRegister(REG_CLKRC, 0x4 | 1);

    // Enable scaling
    WriteRegister(REG_COM3, 0x08);

    // Use QCIF output format
    WriteRegister(REG_COM7, 0x08);

    // Blank pixel clock during sync pulses
    WriteRegister(REG_COM10, 0x20);

    // Enable pixel clock scaling
    WriteRegister(REG_COM14, 0x18 | 1);
    WriteRegister(REG_SCALING_PCLK_DIV, 1);
}

void TIM1_CC_IRQHandler(void)
{
    // VSYNC

    // GPIOC->BRR = (1 << PIN_LED);

    LineCount = CurrentLine;
    CurrentLine = 0;
    FrameCount++;

    if(FrameCount == 5)
    {
        Camera_Captured = 1;
        // Disable everything
        TIM3->CR1 = 0;
        TIM1->CR1 = 0;
    }

    // Dummy read
    TIM1->CCR2;
    TIM1->SR &= ~TIM_SR_CC2IF;

    // GPIOC->BSRR = (1 << PIN_LED);
}

void TIM3_IRQHandler(void)
{
    // HSYNC
    TIM3->DIER &= ~TIM_DIER_CC1DE;
    TIM3->SR &= ~TIM_SR_CC1IF;

    DMA1_Channel6->CCR = 0;
    DMA1_Channel6->CNDTR = sizeof(LineBuffer);
    DMA1_Channel6->CMAR = (uint32_t)LineBuffer;
    DMA1_Channel6->CCR = DMA_CCR_PL | DMA_CCR_MINC | DMA_CCR_EN;
    TIM3->DIER |= TIM_DIER_CC1DE;

#ifdef CAMERA_USE_2D_DITHERING
    static int8_t y_errors[CAMERA_IMAGE_WIDTH + 2] = {0};

    if(CurrentLine == 0)
    {
        memset(y_errors, 0, sizeof(y_errors));
    }
#endif

    if(!Camera_Captured && (~CurrentLine & 1)
        && (CurrentLine / 2 < CAMERA_IMAGE_HEIGHT))
    {

#ifdef CAMERA_USE_2D_DITHERING
        // Apply errors propagated from the previous line. Since y_errors is
        // overwritten during x error diffusion, this is done now.
        for(int i = 0; i < CAMERA_IMAGE_WIDTH; i++)
        {
            LineBuffer[i + 15] += y_errors[i + 1];
            y_errors[i + 1] = 0;
        }
#endif

        int x_error = 0;
        for(int i = 0; i < CAMERA_IMAGE_WIDTH; i++)
        {
            int pixel = LineBuffer[i + 15] + x_error;
            int line = CurrentLine / 2;
            int error;
            if(pixel < 127)
            {
                error = pixel;
                ImageBuffer[(line * CAMERA_IMAGE_WIDTH + i) / 8] |=
                    0x80 >> (i % 8);
            }
            else
            {
                error = pixel - 255;
                ImageBuffer[(line * CAMERA_IMAGE_WIDTH + i) / 8] &=
                    ~(0x80 >> (i % 8));
            }

#ifdef CAMERA_USE_2D_DITHERING
            // Error propagated to the next pixel in the same line
            x_error = error * 7 / 16;

            // Error distributed to the next line's pixels (offset by 1 so
            // bounds checking isn't necessary)
            y_errors[i] += error * 3 / 16;
            y_errors[i + 1] += error * 5 / 16;
            y_errors[i + 2] = error * 1 / 16;
#else
            x_error = error;
#endif
        }
    }

    CurrentLine++;

    // Dummy read
    TIM3->CCR2;
    TIM3->SR &= ~TIM_SR_CC2IF;
}