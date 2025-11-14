// Bit shifting: <<
// 1 << 3 => shift the number 1 left by 3 positions in binary
// 1 in binary: 0000 0001
// 1 << 1: 0000 0010 = 2, 1 << 2: 0000 0100 = 4, 1 << 3: 0000 1000 = 8
// 1 << 4: 0001 0000 = 16, 0100 0000 = 32, 1000 0000 = 64

// Bitwise OR: |=
// (x |= y) is shorthand for (x = x | y)
// Use case: Turn ON specific bits without affecting others

// Bitwise AND with NOT: &= ~
// (x &= ~y) is shorthand for (x = x & (~y))
// ~ flips all bits (NOT operation)
// Use case: Turn OFF specific bits without affecting others

// Arrow operator: ->
// Used for accessing a member of a struct through a pointer

// Binary numbers (only uses 2 digits, 0 and 1)
// Note: In decimal, each position is a power of 10
// Decimal | Binary | How to read it
// --------|--------|---------------
// 0       | 0      | zero
// 1       | 1      | one
// 2       | 10     | one-zero
// 3       | 11     | one-one
// 4       | 100    | one-zero-zero
// 5       | 101    | one-zero-one
// 6       | 110    | one-one-zero
// 7       | 111    | one-one-one

// Get register names (GPIOD, RCC) and addresses
#include "stm32f407xx.h"

// Required by startup code
void SystemInit(void) {}

int main(void) {
    // Turn on clock to GPIOA
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    // Configure PA0 as input
    GPIOA->MODER &= ~(3U << 0);

    // Turn on power to GPIOD
    RCC->AHB1ENR |= (1 << 3);
    
    // Configure pins 12-15 as outputs (all 4 LEDs)
    GPIOD->MODER &= ~((3 << 24) | (3 << 26) | (3 << 28) | (3 << 30));
    GPIOD->MODER |= ((1 << 24) | (1 << 26) | (1 << 28) | (1 << 30));

    while(1) {
        // Read button state and toggle LEDs if "high" signal is sent
        // IDR: Input Data Register
        if (GPIOA->IDR & (1 << 0)) {
            // Turn on LEDs
            GPIOD->ODR |= (1 << 12);
            GPIOD->ODR |= (1 << 13);
            GPIOD->ODR |= (1 << 14);
            GPIOD->ODR |= (1 << 15);
        } else {
            // Keep LEDs off
            GPIOD->ODR &= ~(1 << 12);
            GPIOD->ODR &= ~(1 << 13);
            GPIOD->ODR &= ~(1 << 14);
            GPIOD->ODR &= ~(1 << 15);
        }
    }
}