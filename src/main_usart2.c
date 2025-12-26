#include <stdio.h>

// Get register names (GPIOD, RCC) and addresses
#include "stm32f407xx.h"

// Required by startup code
void SystemInit(void) {}

// Global millisecond counter
volatile uint32_t msTicks = 0;

// Returns current millisecond count
uint32_t get_time_safe(void) {
    __disable_irq();
    uint32_t time = msTicks;
    __enable_irq();
    return time;
}

// Initializes SysTick registers
void SysTick_Init(void) {
    // From ARM Cortex-M4 Technical Reference Manual, Section 4.2
    // SYST_CSR: Control and Status Register; Enable, clock source, interrupt
    // SYST_RVR: Reload Value Register; Value to reload when counter reaches 0
    // SYST_CVR: Current Value Register; Current counter value
    // SYST_CALIB: Calibration Value Register; Calibration info

    // Disable during configuration
    SysTick->CTRL = 0;

    // Set reload value for 1ms tick @ 16MHz
    SysTick->LOAD = 15999;

    // Clear current value register
    SysTick->VAL = 0;

    // Configure and enable SysTick
    SysTick->CTRL = (1 << 2) |  // CLKSOURCE = 1 (SysTick_CTRL_CLKSOURCE_Msk)
                    (1 << 1) |  // TICKINT = 1 (SysTick_CTRL_TICKINT_Msk)
                    (1 << 0);   // ENABLE = 1 (SysTick_CTRL_ENABLE_Msk)
}

// Increments millisecond counter
void SysTick_Handler(void) {
    msTicks++;
}

// Initializes GPIO for LEDs
void GPIO_Init(void) {
    // Enable GPIOD clock
    RCC->AHB1ENR |= (1 << 3);

    // Turn on power to GPIOD
    RCC->AHB1ENR |= (1 << 3);

    // Configure pins 12-15 as outputs (all 4 LEDs)
    GPIOD->MODER &= ~((3 << 24) | (3 << 26) | (3 << 28) | (3 << 30));
    GPIOD->MODER |= ((1 << 24) | (1 << 26) | (1 << 28) | (1 << 30));
}

void TIM2_Init(void) {
    // Enable TIM2 clock; 1 at bit 0
    RCC->APB1ENR |= (1 << 0);

    // Set TIM2 target register
    TIM2->ARR = 99;

    // Set TIM2 prescaler register
    TIM2->PSC = 15999;

    // Tell the CPU to listen for interrupts
    // ISER = Interrupt Set Enable Register; register 0 covers interrupts 0-31
    NVIC->ISER[0] |= (1 << 28);  // Equivalent to NVIC_EnableIRQ(TIM2_IRQn)

    // Enable TIM2 update interrupt; 1 at bit 0
    TIM2->DIER |= (1 << 0);

    // Start counting
    TIM2->CR1 |= (1 << 0);
}

void TIM2_IRQHandler(void) {
    // Toggle LEDs
    GPIOD->ODR ^= (1 << 13);

    // Clear TIM2 status register; 0 at bit 0 (no update occurred)
    TIM2->SR &= ~(1 << 0);
}

void USART2_SendChar(char c) {
    // Wait until transmit buffer is empty
    while (!(USART2->SR & (1 << 7)))
        ;
    
    // Write "c" to data register
    USART2->DR = c;
}

void USART2_Init(void) {
    // Enable USART2 clock
    RCC->APB1ENR |= (1 << 17);

    // Enable GPIOA clock
    RCC->AHB1ENR |= (1 << 0);

    // Configure GPIO pins to receive and transmit
    // Define port A, pins 2 and 3 as alternate functions
    GPIOA->MODER |= (2 << 4) | (2 << 6);
    GPIOA->AFR[0] |= (7 << 8) | (7 << 12);

    // Set the baud rate; BRR = f_clock / baud_rate
    USART2->BRR = 139;

    // Enable transmitter
    USART2->CR1 |= (1 << 3);

    // Enable USART2
    USART2->CR1 |= (1 << 13);

    // Print something
    USART2_SendChar('H');
    USART2_SendChar('i');
}

int main(void) {
    GPIO_Init();
    USART2_Init();
    TIM2_Init();
    SysTick_Init();

    uint32_t lastToggle = 0;

    while (1) {
        if ((get_time_safe() - lastToggle) >= 500) {
            GPIOD->ODR ^= (1 << 15);
            lastToggle = get_time_safe();
        }
    }
}