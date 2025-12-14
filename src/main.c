// Get register names (GPIOD, RCC) and addresses
#include "stm32f407xx.h"

// Required by startup code
void SystemInit(void) {}

// PE3 = CS_I2C/SPI
// PA5 = SPI1_SCK
// PA6 = SPI1_MISO
// PA7 = SPI1_MOSI

// Send / receive a byte on SPI1
uint8_t SPI1_Transfer(uint8_t data) {
    // Wait until TX buffer is empty
    while (!(SPI1->SR & (1 << 1)))
        ;

    // Write data to data register
    SPI1->DR = data;

    // Wait until RX buffer has data
    while (!(SPI1->SR & (1 << 0)))
        ;

    // Read and return data register
    return SPI1->DR;
}

// Read a register from accelerometer
uint8_t LIS3DSH_ReadReg(uint8_t reg) {
    // Pull chip select low (wake up accel)
    GPIOE->ODR &= ~(1 << 3);

    // Send register address with read bit
    SPI1_Transfer(reg | 0x80);

    // Send dummy byte to generate clock pulses
    uint8_t result;
    result = SPI1_Transfer(0x00);

    // Pull CS high (sleep accel)
    GPIOE->ODR |= (1 << 3);

    // Return response
    return result;
}

int main(void) {
    // Configure PE3
    // Enable GPIOE clock
    RCC->AHB1ENR |= (1 << 4);

    // Enable SPI1 clock
    RCC->APB2ENR |= (1 << 12);

    // Set PE3 as output
    GPIOE->MODER |= (1 << 6);

    // Set PE3's output data register
    GPIOE->ODR |= (1 << 3);

    // Configure PA5, PA6, PA7
    // Enable GPIOA clock
    RCC->AHB1ENR |= (1 << 0);

    // Configure pins as alternate functions
    GPIOA->MODER |= (2 << 10) | 
                    (2 << 12) | 
                    (2 << 14);

    // Link alternate functions to SPI1 (SPI1/2 are on AF5)
    GPIOA->AFR[0] |= (5 << 20) | 
                     (5 << 24) | 
                     (5 << 28);
    
    // Configure SPI1 peripheral
    // Set clock phase
    SPI1->CR1 |= (1 << 0);

    // Set clock polarity
    SPI1->CR1 |= (1 << 1);

    // Set STM32 as master
    SPI1->CR1 |= (1 << 2);

    // Set baud rate
    SPI1->CR1 |= (1 << 3);

    // Set internal slave select
    SPI1->CR1 |= (1 << 8);

    // Set software slave management
    SPI1->CR1 |= (1 << 9);

    // Enable SPI1
    SPI1->CR1 |= (1 << 6);

    // Read from accel
    uint8_t whoami = LIS3DSH_ReadReg(0x0F);

    if (whoami == 0x3F) {
        // Success - green LED
        RCC->AHB1ENR |= (1 << 3);
        GPIOD->MODER |= (1 << 24);
        GPIOD->ODR |= (1 << 12);
    } else {
        // Fail - red LED
        RCC->AHB1ENR |= (1 << 3);
        GPIOD->MODER |= (1 << 28);
        GPIOD->ODR |= (1 << 14);
    }
}