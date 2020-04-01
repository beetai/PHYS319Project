#include <msp430.h>

#define     TXD                   BIT2                      // TXD on P1.2
#define     RXD                   BIT1                      // RXD on P1.1

#define		  SPIN1						      BIT4
#define     SPIN2                 BIT5

unsigned int TXByte;
int cycleVal;
int prevCycleVal;

void main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
  ADC10CTL0 = ADC10SHT_2 + ADC10ON; 	    // ADC10ON
  ADC10CTL1 = INCH_4;                       // input A4
  ADC10AE0 |= SPIN1;                         // PA.4 ADC option select
  ADC10AE0 |= SPIN2;
  P1DIR |= 0x01 ;                            // Set P1.0 to output direction
  
  P1DIR |= TXD;
  P1OUT |= TXD;
  
  /* Configure hardware UART */
  P1SEL = BIT1 + BIT2 ; // P1.1 = RXD, P1.2=TXD
  P1SEL2 = BIT1 + BIT2 ; // P1.1 = RXD, P1.2=TXD
  UCA0CTL1 |= UCSSEL_2; // Use SMCLK
  UCA0BR0 = 104; // Set baud rate to 9600 with 1MHz clock (Data Sheet 15.3.13)
  UCA0BR1 = 0; // Set baud rate to 9600 with 1MHz clock
  UCA0MCTL = UCBRS0; // Modulation UCBRSx = 1
  UCA0CTL1 &= ~UCSWRST; // Initialize USCI state machine
  
  prevCycleVal = 25;

  while (1) {
    ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
    while (ADC10CTL1 &ADC10BUSY);          // ADC10BUSY?
    if (ADC10MEM < 0x2FF)
      P1OUT &= ~0x01;                       // Clear P1.0 LED off
    else
      P1OUT |= 0x01;                        // Set P1.0 LED on
    
    cycleVal = (unsigned char)(ADC10MEM>>2) % 51;
    TXByte = 0;
    if (cycleVal - prevCycleVal < -40) { // if it goes from 50 to 0, counter-clockwise
      TXByte |= BIT0;
    } else if (cycleVal - prevCycleVal > 40) { // if it goes from 0 to 50, clock-wise
      TXByte |= BIT1;
    }
      
    //TXByte = (unsigned char)(ADC10MEM>>2) % 51;
    while (! (IFG2 & UCA0TXIFG)); // wait for TX buffer to be ready for new data
    UCA0TXBUF = TXByte;

    unsigned i;
    for (i = 0xFFFF; i > 0; i--);           // Delay
    
    prevCycleVal = cycleVal;
  }
}
