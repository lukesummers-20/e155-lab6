// STM32L432KC_SPI.c
// Luke Summers
// lsummers@g.hmc.edu
// implimentation file for SPI functions

#include <stm32l432xx.h>

void initSPI(SPI_TypeDef* SPI, int br, int cpol, int cpha){

  //set MCU spi as master
  SPI->CR1 |= (1 << 2);

  //set baud rate divide by 256
  SPI->CR1 |= (0b111 << 3);

  //fifo recep thresh to 8 bit
  SPI->CR2 |= (1 << 12);

  // set phase
  SPI->CR1 |= (1 << 0);

  // set ssoe
  SPI->CR2 |= (1 << 2);

  //set ssm bit
  SPI->CR1 |= (1 << 9);

  //data size 8
  SPI->CR2 |= (0b111 << 8);
  SPI->CR2 &= ~(1 << 11);

  // enable spi
  SPI1->CR1 |= (1 << 6);
}

char spiSendReceive(SPI_TypeDef* SPI, char send) {
  //wait for txr to empty
  while(!(SPI->SR & (1 << 1)));
  *((char *) &SPI->DR) = send;
  //wait for rxr to fill
  while(!(SPI->SR & (1 << 0)));
  char data = SPI->DR;
  return data;
}