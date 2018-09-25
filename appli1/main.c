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
	
	// sortie LED à 1 pour allumer
	//LED = 1;
  
  while (1)
  {
		fct_tempo(500*1000);
		LED = 1;
		fct_tempo(500*1000);
		LED = 0;
		
	}	 	 
}

 

 