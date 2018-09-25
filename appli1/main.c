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

sbit LED = P1^6; // Led verte embarqu�e sur la carte



void config_serie() {
	
	
	
	// Timer 1
	// bit 4 de CKCON � 1 pour utiliser system clock
	CKCON |= 0x10;
	// bit 6 de TCON � 1 pour l'activer
	TCON |= 0x40;
	// bits 7,6,4 de TMOD � 0
	TMOD &= ~0x13;
	// bit 5 de TMOD � 1
	TMOD |= 0x20;
	// valeur de reload = 112
	TH1 = 112;
	
	// UART0 en mode 1 (asynchrone 8 bits)
	
	// bits 7,0 � 0
	SCON0 &= ~0x81;
	// bits 6,5,4,1 � 1
	SCON0 |= 0x72;
	// baud rate divide-by-two disabled
	PCON |= 0x80;
	
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
	
	SBUF0 = 'c';
  
  while (1)
  {
		fct_tempo(500*1000);
		LED = 1;
		fct_tempo(500*1000);
		LED = 0;
		
	}	 	 
}

 

 