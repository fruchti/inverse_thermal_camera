.syntax unified
.thumb
.fpu softvfp

.global Reset_Handler
.type Reset_Handler, %function
Reset_Handler:
init_data:
	ldr	r2, =_start_init_data
	ldr	r3, =_start_data
	ldr	r1, =_end_data

init_data_loop:
	cmp	r3, r1
	bhs	zero_bss
	ldr	r0, [r2], #4
	str	r0, [r3], #4
	b init_data_loop

zero_bss:
	ldr	r3, =_start_bss
	ldr	r1, =_end_bss
    movs r0, #0

zero_bss_loop:
	cmp	r3, r1
	bhs	init_finished
	str	r0, [r3], #4
    b zero_bss_loop

init_finished:
	bl SystemInit
	// TODO: Call stdlib initialisation functions here if they turn out to be
	// needed
    bl main

infinite_loop:
    b infinite_loop

.size Reset_Handler, .-Reset_Handler

.section .cortex_vectors, "a"
.word _end_stack
.word Reset_Handler
.word NMI_Handler
.word HardFault_Handler
.word MemManage_Handler
.word BusFault_Handler
.word UsageFault_Handler
.word 0x00000000
.word 0x00000000
.word 0x00000000
.word 0x00000000
.word SVC_Handler
.word DebugMon_Handler
.word 0x00000000
.word PendSV_Handler
.word SysTick_Handler
.word WWDG_IRQHandler
.word PVD_IRQHandler
.word TAMPER_IRQHandler
.word RTC_IRQHandler
.word FLASH_IRQHandler
.word RCC_IRQHandler
.word EXTI0_IRQHandler
.word EXTI1_IRQHandler
.word EXTI2_IRQHandler
.word EXTI3_IRQHandler
.word EXTI4_IRQHandler
.word DMA1_Channel1_IRQHandler
.word DMA1_Channel2_IRQHandler
.word DMA1_Channel3_IRQHandler
.word DMA1_Channel4_IRQHandler
.word DMA1_Channel5_IRQHandler
.word DMA1_Channel6_IRQHandler
.word DMA1_Channel7_IRQHandler
.word ADC1_2_IRQHandler
.word USB_HP_CAN1_TX_IRQHandler
.word USB_LP_CAN1_RX0_IRQHandler
.word CAN1_RX1_IRQHandler
.word CAN1_SCE_IRQHandler
.word EXTI9_5_IRQHandler
.word TIM1_BRK_IRQHandler
.word TIM1_UP_IRQHandler
.word TIM1_TRG_COM_IRQHandler
.word TIM1_CC_IRQHandler
.word TIM2_IRQHandler
.word TIM3_IRQHandler
.word 0x00000000
.word I2C1_EV_IRQHandler
.word I2C1_ER_IRQHandler
.word 0x00000000
.word 0x00000000
.word SPI1_IRQHandler
.word 0x00000000
.word USART1_IRQHandler
.word USART2_IRQHandler
.word 0x00000000
.word EXTI15_10_IRQHandler
.word RTC_Alarm_IRQHandler
.word USBWakeUp_IRQHandler

.type Dummy_Handler, %function
Dummy_Handler:
    b Dummy_Handler

.weak  NMI_Handler
.thumb_set NMI_Handler, Dummy_Handler

.weak  HardFault_Handler
.thumb_set HardFault_Handler, Dummy_Handler

.weak  MemManage_Handler
.thumb_set MemManage_Handler, Dummy_Handler

.weak  BusFault_Handler
.thumb_set BusFault_Handler, Dummy_Handler

.weak  UsageFault_Handler
.thumb_set UsageFault_Handler, Dummy_Handler

.weak  SVC_Handler
.thumb_set SVC_Handler, Dummy_Handler

.weak  DebugMon_Handler
.thumb_set DebugMon_Handler, Dummy_Handler

.weak  PendSV_Handler
.thumb_set PendSV_Handler, Dummy_Handler

.weak  SysTick_Handler
.thumb_set SysTick_Handler, Dummy_Handler

.weak WWDG_IRQHandler
.thumb_set WWDG_IRQHandler, Dummy_Handler

.weak PVD_IRQHandler
.thumb_set PVD_IRQHandler, Dummy_Handler

.weak TAMPER_IRQHandler
.thumb_set TAMPER_IRQHandler, Dummy_Handler

.weak RTC_IRQHandler
.thumb_set RTC_IRQHandler, Dummy_Handler

.weak FLASH_IRQHandler
.thumb_set FLASH_IRQHandler, Dummy_Handler

.weak RCC_IRQHandler
.thumb_set RCC_IRQHandler, Dummy_Handler

.weak EXTI0_IRQHandler
.thumb_set EXTI0_IRQHandler, Dummy_Handler

.weak EXTI1_IRQHandler
.thumb_set EXTI1_IRQHandler, Dummy_Handler

.weak EXTI2_IRQHandler
.thumb_set EXTI2_IRQHandler, Dummy_Handler

.weak EXTI3_IRQHandler
.thumb_set EXTI3_IRQHandler, Dummy_Handler

.weak EXTI4_IRQHandler
.thumb_set EXTI4_IRQHandler, Dummy_Handler

.weak DMA1_Channel1_IRQHandler
.thumb_set DMA1_Channel1_IRQHandler, Dummy_Handler

.weak DMA1_Channel2_IRQHandler
.thumb_set DMA1_Channel2_IRQHandler, Dummy_Handler

.weak DMA1_Channel3_IRQHandler
.thumb_set DMA1_Channel3_IRQHandler, Dummy_Handler

.weak DMA1_Channel4_IRQHandler
.thumb_set DMA1_Channel4_IRQHandler, Dummy_Handler

.weak DMA1_Channel5_IRQHandler
.thumb_set DMA1_Channel5_IRQHandler, Dummy_Handler

.weak DMA1_Channel6_IRQHandler
.thumb_set DMA1_Channel6_IRQHandler, Dummy_Handler

.weak DMA1_Channel7_IRQHandler
.thumb_set DMA1_Channel7_IRQHandler, Dummy_Handler

.weak ADC1_2_IRQHandler
.thumb_set ADC1_2_IRQHandler, Dummy_Handler

.weak USB_HP_CAN1_TX_IRQHandler
.thumb_set USB_HP_CAN1_TX_IRQHandler, Dummy_Handler

.weak USB_LP_CAN1_RX0_IRQHandler
.thumb_set USB_LP_CAN1_RX0_IRQHandler, Dummy_Handler

.weak CAN1_RX1_IRQHandler
.thumb_set CAN1_RX1_IRQHandler, Dummy_Handler

.weak CAN1_SCE_IRQHandler
.thumb_set CAN1_SCE_IRQHandler, Dummy_Handler

.weak EXTI9_5_IRQHandler
.thumb_set EXTI9_5_IRQHandler, Dummy_Handler

.weak TIM1_BRK_IRQHandler
.thumb_set TIM1_BRK_IRQHandler, Dummy_Handler

.weak TIM1_UP_IRQHandler
.thumb_set TIM1_UP_IRQHandler, Dummy_Handler

.weak TIM1_TRG_COM_IRQHandler
.thumb_set TIM1_TRG_COM_IRQHandler, Dummy_Handler

.weak TIM1_CC_IRQHandler
.thumb_set TIM1_CC_IRQHandler, Dummy_Handler

.weak TIM2_IRQHandler
.thumb_set TIM2_IRQHandler, Dummy_Handler

.weak TIM3_IRQHandler
.thumb_set TIM3_IRQHandler, Dummy_Handler

.weak I2C1_EV_IRQHandler
.thumb_set I2C1_EV_IRQHandler, Dummy_Handler

.weak I2C1_ER_IRQHandler
.thumb_set I2C1_ER_IRQHandler, Dummy_Handler

.weak SPI1_IRQHandler
.thumb_set SPI1_IRQHandler, Dummy_Handler

.weak USART1_IRQHandler
.thumb_set USART1_IRQHandler, Dummy_Handler

.weak USART2_IRQHandler
.thumb_set USART2_IRQHandler, Dummy_Handler

.weak EXTI15_10_IRQHandler
.thumb_set EXTI15_10_IRQHandler, Dummy_Handler

.weak RTC_Alarm_IRQHandler
.thumb_set RTC_Alarm_IRQHandler, Dummy_Handler

.weak USBWakeUp_IRQHandler
.thumb_set USBWakeUp_IRQHandler, Dummy_Handler
