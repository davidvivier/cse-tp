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

#define BUF_SIZE 50

void fct_tempo(int csg_tempo);

sbit LED = P1^6; // Led verte embarqu�e sur la carte

//sbit RHT = P2^0;
sbit RHT = P2^0;

int i = 0;
int j = 0;
int m = 0;
char octet = 0;
char bit_value = 0;

char buf[BUF_SIZE];


// '0' est � 48 ascii
char ascii[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

int hum = 0;
int temp = 0;

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

void wait_sec(int seconds) {
	for( i = 0 ; i < seconds * 1000; i++ ) {
		fct_tempo(1000);
	}
}

void clear_buf() {
	for (i = 0; i < BUF_SIZE; i++) {
		buf[i] = 0;
	}
}

void send_buf(int length) {
	for (i = 0; i < length; i++) {
		TI0 = 0;
		SBUF0 = buf[i];
		while(!TI0);
	}
}

char ascii_value(int v) {
	/*for (i = 0; i < 10; i++) {
		if (ascii[i] == v) {
			return i;
		}
	}*/
	return 0;
}

char get_octet(int pos) {
	m = 128;
	i = 0;
	
	octet = 0;
	
	for (i = 0; i < 8; i++) {
		if (buf[pos+i] == '0') {
			bit_value = 0;
		} else {
			bit_value = 1;
		}
		octet += m * bit_value;
		m /= 2;
	}
	return octet;
}

void convert_values() {
	
	temp = get_octet(0);
	hum = get_octet(16);
	
}

void prepare_result() {
	clear_buf();
	i = 0;
	buf[i++] = 'T';
	buf[i++] = '=';
	buf[i++] = ascii[temp/10];
	buf[i++] = ascii[temp%10];
	buf[i++] = '�';
	buf[i++] = 'C';
	buf[i++] = ' ';
	
	buf[i++] = 'R';
	buf[i++] = 'H';
	buf[i++] = '=';
	buf[i++] = ascii[hum/10];
	buf[i++] = ascii[hum%10];
	buf[i++] = '%';
	
	buf[i++] = '\r';
	buf[i++] = '\n';
	buf[i++] = '\0';
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
	
	LED=0;
	
	i = 0;
	do {
	
		rht_wait_one();
		
		fct_tempo(50);
		
		if (RHT == 0) {
			// bit 0 re�u
			LED=1;
			buf[i] = '0';
		} else if (RHT == 1) {
			// bit 1 re�u
			LED=1;
			buf[i] = '1';
		}
		rht_wait_zero();
		LED=0;
		i++;
	} while( i < 40 );
	
	buf[i++] = 'e';
	buf[i++] = '\n';
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
		clear_buf();
		rht_receive();
		convert_values();
		prepare_result();
		send_buf(15);
		
		wait_sec(2);
	}	 	 
}

 

 