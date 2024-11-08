/*
File: main.c for Lab 6
Author: Josh Brake and Luke Summers
Email: jbrake@hmc.edu lsummers@g.hmc.edu
*/


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "main.h"

/////////////////////////////////////////////////////////////////
// Provided Constants and Functions
/////////////////////////////////////////////////////////////////

//Author: Josh Brake
//Defining the web page in two chunks: everything before the current time, and everything after the current time
char* webpageStart = "<!DOCTYPE html><html><head><title>E155 Web Server Demo Webpage</title>\
	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\
	</head>\
	<body><h1>E155 Web Server Demo Webpage</h1>";
char* ledStr = "<p>LED Control:</p><form action=\"ledon\"><input type=\"submit\" value=\"Turn the LED on!\"></form>\
	<form action=\"ledoff\"><input type=\"submit\" value=\"Turn the LED off!\"></form>";
char* webpageEnd   = "</body></html>";
//Author: Luke Summers
char* tempForm = "<p>Temperature Reading:</p><form action=\"8\"><input type=\"submit\" value=\"8 Bit Accuracy\"></form>\
                    <form action=\"9\"><input type=\"submit\" value=\"9 Bit Accuracy\"></form>\
                    <form action=\"10\"><input type=\"submit\" value=\"10 Bit Accuracy\"></form>\
                    <form action=\"11\"><input type=\"submit\" value=\"11 Bit Accuracy\"></form>\
                    <form action=\"12\"><input type=\"submit\" value=\"12 Bit Accuracy\"></form>";

//Author: Josh Brake
//determines whether a given character sequence is in a char array request, returning 1 if present, -1 if not present
int inString(char request[], char des[]) {
	if (strstr(request, des) != NULL) {return 1;}
	return -1;
}

//Author: Luke Summers
//function for converting temp chip register data to a float
float convert(uint8_t m, uint8_t l, int res){
  int neg = (m >> 7) & 1;
  int half = ((l >> 7) & 1) & (res > 8);
  int quarter = ((l >> 6) & 1) & (res > 9);
  int eigth = ((l >> 5) & 1) & (res > 10);
  int sixteenth = ((l >> 4) & 1) & (res > 11);
  float adjust = 0;
  if(neg){adjust =  adjust -128;}
  if(half){adjust = adjust + 0.5;}
  if(quarter){adjust = adjust + 0.25;}
  if(eigth){adjust = adjust + 0.125;}
  if(sixteenth){adjust = adjust + 0.0625;}
  return (float) (m & 0b01111111) + adjust;
}

//Author: Luke Summers
//function for updating temperature upon a request
float updateTemp(char request[]){
        int resoultuion_status;
        if (inString(request, "8")==1) {
          digitalWrite(CE_PIN, 1);
          spiSendReceive(SPI1, CFGR_ADR);
          spiSendReceive(SPI1, 0b11100000);
          digitalWrite(CE_PIN, 0);
          resoultuion_status = 8;
        }else if (inString(request, "9")==1) {
          digitalWrite(CE_PIN, 1);
          spiSendReceive(SPI1, CFGR_ADR);
          spiSendReceive(SPI1, 0b11100010);
          digitalWrite(CE_PIN, 0);
          resoultuion_status = 9;
        }else if (inString(request, "10")==1) {
          digitalWrite(CE_PIN, 1);
          spiSendReceive(SPI1, CFGR_ADR);
          spiSendReceive(SPI1, 0b11100100);
          digitalWrite(CE_PIN, 0);
          resoultuion_status = 10;
        }else if (inString(request, "11")==1) {
          digitalWrite(CE_PIN, 1);
          spiSendReceive(SPI1, CFGR_ADR);
          spiSendReceive(SPI1, 0b11100110);
          digitalWrite(CE_PIN, 0);
          resoultuion_status = 11;
        }else if (inString(request, "12")==1) {
          digitalWrite(CE_PIN, 1);
          spiSendReceive(SPI1, CFGR_ADR);
          spiSendReceive(SPI1, 0b11101000);
          digitalWrite(CE_PIN, 0);
          resoultuion_status = 12;
        } else {
          resoultuion_status = 12;
        }
    
    digitalWrite(CE_PIN, 1);
    spiSendReceive(SPI1, MSB_ADR);
    uint8_t tempM = spiSendReceive(SPI1, 0b00000000);
    digitalWrite(CE_PIN, 0);
    uint8_t tempL = 0;
    if (resoultuion_status > 8) {
      digitalWrite(CE_PIN, 1);
      spiSendReceive(SPI1, LSB_ADR);
      tempL = spiSendReceive(SPI1, 0b00000000);
      digitalWrite(CE_PIN, 0);
    }
    //convert to float
    return convert(tempM, tempL, resoultuion_status);
}

//Author: Josh Brake
int updateStatus(char request[])
{
        int led_status;
	// The request has been received. now process to determine whether to turn the LED on or off
	if (inString(request, "ledoff")==1) {
          digitalWrite(LED_PIN, PIO_LOW);
          led_status = 0;
	} else if (inString(request, "ledon")==1) {
          digitalWrite(LED_PIN, PIO_HIGH);
          led_status = 1;
	}
	return led_status;
}

/////////////////////////////////////////////////////////////////
// Solution Functions
/////////////////////////////////////////////////////////////////

//Author: Josh Brake, lines 133-145, 171-185, 194-213, 222
//Author: Luke Summers, lines 147-168, 188-192, 215-219
int main(void) {
  configureFlash();
  configureClock();

  gpioEnable(GPIO_PORT_A);
  gpioEnable(GPIO_PORT_B);
  gpioEnable(GPIO_PORT_C);

  pinMode(PB3, GPIO_OUTPUT);
  
  RCC->APB2ENR |= (RCC_APB2ENR_TIM15EN);
  initTIM(TIM15);
  
  USART_TypeDef * USART = initUSART(USART1_ID, 125000);

  //configure spi

  // configure pins for SPI
  pinMode(SCK_PIN, GPIO_ALT);
  pinMode(CIPO_PIN, GPIO_ALT);
  pinMode(COPI_PIN, GPIO_ALT);
  pinMode(CE_PIN, GPIO_OUTPUT);
  digitalWrite(CE_PIN, 0);

  GPIOA->AFR[0] |= (0b101 << 20);
  GPIOA->AFR[0] |= (0b101 << 24);
  GPIOA->AFR[1] |= (0b101 << 16);

  RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

  initSPI(SPI1, 255, 0, 1);

  //configure ds1722 via spi in continueous mode to 12 bit readings
  digitalWrite(CE_PIN, 1);
  spiSendReceive(SPI1, CFGR_ADR);
  spiSendReceive(SPI1, 0b11101000);
  digitalWrite(CE_PIN, 0);

  while(1) {
    /* Wait for ESP8266 to send a request.
    Requests take the form of '/REQ:<tag>\n', with TAG begin <= 10 characters.
    Therefore the request[] array must be able to contain 18 characters.
    */

    // Receive web request from the ESP
    char request[BUFF_LEN] = "                  "; // initialize to known value
    int charIndex = 0;
  
    // Keep going until you get end of line character
    while(inString(request, "\n") == -1) {
      // Wait for a complete request to be transmitted before processing
      while(!(USART->ISR & USART_ISR_RXNE));
      request[charIndex++] = readChar(USART);
    }

    
    //update temperature with request
    float temp = updateTemp(request);
    char tempStr[40];

    sprintf(tempStr, "The temp is %f degrees C \n", temp);
  
    // Update string with current LED state
  
    int led_status = updateStatus(request);

    char ledStatusStr[20];
    if (led_status == 1)
      sprintf(ledStatusStr,"LED is on!");
    else if (led_status == 0)
      sprintf(ledStatusStr,"LED is off!");

    // finally, transmit the webpage over UART
    sendString(USART, webpageStart); // webpage header code
    sendString(USART, ledStr); // button for controlling LED

    sendString(USART, "<h2>LED Status</h2>");


    sendString(USART, "<p>");
    sendString(USART, ledStatusStr);
    sendString(USART, "</p>");

    sendString(USART, tempForm);

    sendString(USART, "<p>");
    sendString(USART, tempStr);
    sendString(USART, "</p>");

  
    sendString(USART, webpageEnd);
  }
}
