// **** INCLUDES *****
#include "LowPower.h"

void setup(){
}

void loop()
{
  // Enter idle state for 8 s with the rest of peripherals turned off
  // Each microcontroller comes with different number of peripherals
  // Comment off line of code where necessary

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
  LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF,
                SPI_OFF, USART0_OFF, TWI_OFF);
#elif defined __AVR_ATmega644P__ || defined(__AVR_ATmega1284P__)
  LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF,
                TIMER1_OFF, TIMER0_OFF, SPI_OFF,
                USART1_OFF, USART0_OFF, TWI_OFF);
#endif


}
