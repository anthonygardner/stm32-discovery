// Get register names (GPIOD, RCC) and addresses
#include "stm32f407xx.h"

// Required by startup code
void SystemInit(void) {}

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
    // Set PD0 high (ODR = Output Data Register)
    GPIOD->ODR ^= (1 << 0);

    GPIOD->ODR ^= (1 << 14);

    // Clear TIM2 status register; 0 at bit 0 (no update occurred)
    TIM2->SR &= ~(1 << 0);
}

int main(void) {
    // Enable GPIOD clock (RCC = Reset and Clock Control)
    RCC->AHB1ENR |= (1 << 3);

    // Configure PD0 as output (MODER = Mode Register)
    GPIOD->MODER |= (1 << 0);

    TIM2_Init();
    
    while (1)
        ;
}