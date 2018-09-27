//------------------------------------------------------------------------------------
// Base_8051
//------------------------------------------------------------------------------------
//
// AUTH: DF
// DATE: 18-09-2017
// Target: C8051F020
//
// Tool chain: KEIL UV4
//
// Application de base qui configure seulement l'oscillateur et le watchdog
//-------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------
#include "c8051f020.h" // SFR declarations
#include <intrins.h>  
#include <stdio.h>
#include "init.h"

void fct_tempo(int csg_tempo);

sbit LED = P1^6; // Led verte embarquée sur la carte

//sbit RHT = P2^0;
sbit RHT = P2^0;

int i = 0;
int j = 0;


void config_serie() {
	
	
	
	// Timer 1
	// bit 4 de CKCON à 1 pour utiliser system clock
	CKCON |= 0x10;
	// bit 6 de TCON à 1 pour l'activer
	TCON |= 0x40;
	// bits 7,6,4 de TMOD à 0
	TMOD &= ~0x13;
	// bit 5 de TMOD à 1
	TMOD |= 0x20;
	// valeur de reload = 112
	TH1 = 112;
	
	// UART0 en mode 1 (asynchrone 8 bits)
	
	// bits 7,0 à 0
	SCON0 &= ~0x81;
	// bits 6,5,4,1 à 1
	SCON0 |= 0x72;
	// baud rate divide-by-two disabled
	PCON |= 0x80;
	
}

void wait_sec(int seconds) {
	for( i = 0 ; i < seconds * 1000; i++ ) {
		fct_tempo(1000);
	}
}

void rht_input_mode() {
	
	// Pin 2.0
	
	// open drain
	P2MDOUT &= ~0x01;
	P2MDOUT &= ~0x02;
	
	RHT = 1;
	
}

void rht_output_mode() {
	P2MDOUT |= 0x01;
	P2MDOUT |= 0x02;
}

void rht_wait_one() {
	while (RHT != 1);
}

void rht_wait_zero() {
	while (RHT != 0);
}

void rht_receive() {
	
	rht_wait_one();
	
	rht_wait_zero();
	
	LED=1;
	
	i = 0;
	do {
	
		rht_wait_one();
	
		fct_tempo(50);
	
		if (RHT == 0) {
			// bit 0 reçu
			SBUF0 = '0';
		} else if (RHT == 1) {
			// bit 1 reçu
			SBUF0 = '1';
			
		}
		rht_wait_zero();
		i++;
	} while( i < (4*8 + 8) );
	
	SBUF0 = 'e';
	SBUF0 = '\n';
}


//------------------------------------------------------------------------------------
// MAIN Routine
//------------------------------------------------------------------------------------
void main (void)
{
	
  Init_Device();

	// crossbar
	XBR0      = 0x04;
	XBR2      = 0x40;
	
	// green led
	
	// broche P1.6 (green led) en mode 0 open-drain
	P1MDOUT |= 0x40;
	
	config_serie();
	
	//rht_input_mode();
	
	rht_output_mode();
	
	SBUF0 = 's';
  
  while (1)
  {
		//fct_tempo(19*1000);
		//LED = 1;
		RHT = 1;
		
		LED = 0;
		
		// DOWN for 19ms
		RHT = 0;
		fct_tempo(19*1000);
		//LED = 0;
		
		// UP and wait response
		RHT = 1;
		fct_tempo(20);
		
		rht_input_mode();
		
		// now we receive the response
		rht_receive();
		
		wait_sec(2);
	}	 	 
}

 

 