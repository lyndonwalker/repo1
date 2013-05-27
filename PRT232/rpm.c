/*********************************************
* Author: 		Lyndon  Walker
* Description: 	Measures period of interrupt on INT0 for RPM determination
*				and reads two switch inputs
* 
*  
* Chip type           : ATtiny2313
* Clock frequency     : 4.0 MHz
*
* Initial revision: 2/22/2011
* 
*
* 
*********************************************/
/* Includes */
#include <avr/io.h>
#include <string.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdint.h>



/* Prototypes */
void InitUART( char baudrate );
char ReceiveByte( char *status );
void TransmitByte( char data );
void PrintString(char *str);
char ReceiveGoodByte(char *rxb);
void getRpm(char* buf);

#define RX_ERRORS 	24
#define GOOD_RX		152
#define TRUE		1
#define FALSE		0
#define CRLF		13
#define SW1			2
#define SW2			4
#define SW3			8

static const unsigned char BUFFER_SIZE= 6;
static const unsigned char PRINT_BUFFER_SIZE=20;

// Globals
unsigned int _lastCount=0;
unsigned long int _totalRevolutions=0;
int _lastPeriod=0;
unsigned char _intTriggered= 0;

// Handler for external interrupt 0
SIGNAL(SIG_INTERRUPT0)
{
	unsigned int thisTime= TCNT1L + TCNT1H * 256;
	_lastPeriod= thisTime - _lastCount;
	_lastCount= thisTime;
	_totalRevolutions++;
	_intTriggered= 1;
}


int main( void )
{
	char rxBuf[BUFFER_SIZE];
	char *pBufIn;
	pBufIn= rxBuf;
	memset(rxBuf,0, BUFFER_SIZE);
	
	DDRB= 0xFF;
	DDRD= 0x02;
	// Make input pullups high
	PORTD= 0xFF;
	PORTB= 0x0;

	
	/* Set the baudrate to 19,200 bps using a 4MHz crystal */
	InitUART( 12 ); 

	// Set prescaler on timer 1 and disconnect it from the outputs
	// Prescaler is set to divide by 1024, so each bit is a period of
	// 0.25uS * 1024 = 256uS
	TCCR1B= 5;
	TCCR1A= 0;
	
	// Disable timer interrupts
	TIMSK= 0;
	// Enable INT0, falling edge only
	MCUCR= (1 << ISC01);
	GIMSK=(1 << INT0 );
	sei();
	
	// Print startup msg
	PrintString("\rDIO2");


	// Clear receive buffer
	UDR;
		
	// Main loop
	for(;;) 	    
	{
		// Process incoming commands
		if (FALSE == ReceiveGoodByte(pBufIn))
			continue;
		// Is this the end?
		if (CRLF == *pBufIn)
		{
			// Zero terminate string
			*pBufIn=0;
			// Reset to start
			pBufIn= rxBuf;
			
			// Get total count?
			if (strncmp(rxBuf,"c",1) == 0)
			{
				 char buffer[PRINT_BUFFER_SIZE];
				memset(buffer, 0, PRINT_BUFFER_SIZE);			 
				cli();
				ltoa(_totalRevolutions, buffer, 10);
				sei();
				// Print it
				PrintString(buffer);
			}		
			// Get period?
			else if (strncmp(rxBuf,"p",1) == 0)
			{
				 char buffer[PRINT_BUFFER_SIZE];
				 getRpm(buffer);
				// Print it
				 PrintString(buffer);
			}		
			// Check for READ commands
			else if (strncmp(rxBuf,"s1",2) == 0)
			{
				char switchRead= PINB;
				// Hold switchread data
				char sw[]= {0,0,0,0};
				sw[0]= ((switchRead & SW1) == SW1) ? '1':'0';
				PrintString(sw);
			}		
			else if (strncmp(rxBuf,"s2",2) == 0)
			{
				char switchRead= PINB;
				// Hold switchread data
				char sw[]= {0,0,0,0};
				sw[0]= ((switchRead & SW2) == SW2) ? '1':'0';
				PrintString(sw);
			}	
			else if (strncmp(rxBuf,"s3",2) == 0)
			{
				char switchRead= PINB;
				// Hold switchread data
				char sw[]= {0,0,0,0};
				sw[0]= ((switchRead & SW3) == SW3) ? '1':'0';
				PrintString(sw);
			}	
			else if (strncmp(rxBuf,"s",1) == 0)
			{
				// Hold switchread data
				char sw[]= {0,0,0,0};
				char switchRead= PINB;
				sw[0]= ((switchRead & SW1) == SW1) ? '1':'0';
				sw[1]= ((switchRead & SW2) == SW2) ? '1':'0';
				sw[2]= ((switchRead & SW3) == SW3) ? '1':'0';
				PrintString(sw);
			}		
			// Clear RPM reading
			else if (strncmp(rxBuf, "z", 1) == 0)
			{
				cli();
				_lastCount= 0;
				_lastPeriod=0;
				_totalRevolutions=0;
				sei();
			}	
			// Check for output commands
			else if (rxBuf[0] == 'o')
			{
				// Guarantee zero terminated string
				rxBuf[4]=0;	
				int i= strnlen(&rxBuf[1],3);
				int m=1;
				// Convert to number				
				int driver=0;
				while (i > 0)
				{
					// convert from ASCII and handle various digit positions
					driver += m * (rxBuf[i]-48);
					m*= 10;
					i--;
				}
				if (driver < 256)	
					PORTB= (char)driver;
			}		
			
			else
				PrintString("Error");
		}
		else
		{
			// Wrap pointer at end
			if (pBufIn != &rxBuf[BUFFER_SIZE - 1])
				pBufIn++;
			else
				pBufIn= rxBuf;
		}
		
	}
	return 0;
}

// Get period of rotation. Period measured in increments of 256 uSec
void getRpm(char* buf)
{
	memset(buf, 0, PRINT_BUFFER_SIZE);			 
	// Protect shared variableS
	cli();
	_intTriggered= 0;
	int period= _lastPeriod;
	sei();
	// avr-libc doesn't have an ltoa (unsigned) function, so we simulate it here
	if (period < 0)
		period *= -1;
	itoa(period, buf, 10);
}


/* Initialize UART */
void InitUART( char baudrate )
{
	/* Set the baud rate */
	UBRRL= (unsigned char)baudrate;    
	/* Enable UART receiver and transmitter */
	UCSRB= ( (1<<RXEN) | (1<<TXEN) ); 	
	// framing: 8N1
	UCSRC= 6; 
}

/* Read and write functions */
//
// Wait for a byte to be received
//
char ReceiveByte( char *status )
{
	/* Wait for incomming data */
	*status= UCSRA;
	while ( !(*status & (1<<RXC)) )			                
		*status= UCSRA;
	return UDR;
}

//
// Return TRUE if a byte was received without errors, FALSE otherwise
//
char ReceiveGoodByte(char *rxb)
{
	unsigned int status= UCSRA;
	if ( status & (1<<RXC))
		*rxb= UDR;

	if (status & GOOD_RX)
		return TRUE;
	else
		return FALSE;
}

void TransmitByte( char data )
{
	/* Wait for empty transmit buffer */
	while ( !(UCSRA & (1<<UDRE)) ); 			                
	/* Start transmission */
	UDR= data; 			        
}


void PrintString( char *str)
{
	while(*str)
		TransmitByte(*str++);
    TransmitByte('\r');
    TransmitByte('\n');
}


