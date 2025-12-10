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

int main(void) {
    GPIO_Init();
    SysTick_Init();

    uint32_t lastToggle = 0;

    while(1) {
        if ((get_time_safe() - lastToggle) >= 500) {
            GPIOD->ODR ^= (1 << 15);
            lastToggle = get_time_safe();
        }
    }
}