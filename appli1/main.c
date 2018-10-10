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

#define BUF_SIZE 20


void fct_tempo(int csg_tempo);

sbit LED = P1^6; // Led verte embarquee sur la carte

//sbit RHT = P2^0;
sbit RHT = P2^0;

sbit SS = P0^6;
sbit NSS = P0^5;

int i = 0;
int j = 0;
int m = 0;
char octet = 0;
char bit_value = 0;

char buf[BUF_SIZE];

char adxl_buf[6];

char rep = 0;
char ignore = 0;

char cmd = 0x00;

// '0' est à 48 ascii
char ascii[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

int hum = 0;
int temp = 0;

int tmp = 0;
float value = 0.0f;
float dataX = 0.0f;
float dataY = 0.0f;
float dataZ = 0.0f;

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
void config_spi() {
	// crossbar
	
	  XBR0      = 0x06;
    XBR2      = 0x40;
	
	// phase, polarite et taille de trame
	SPI0CFG |= 0xC7;
	
	SPI0CN &= ~0x80;
	
		SPI0CN |= 0x03;
	
		// push pull
		P0MDOUT |= 0x14;
		P0MDOUT &= ~0x08;
	
		// commande NSS sur P0.6
		P0MDOUT |= 0x40;
	

	
	// clockrate
	SPI0CKR = 255;
	
	
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


void spi_write_byte(char byte) {
	
		SPIF = 0;
		SPI0DAT = byte;
		while (SPIF == 0); // wait end of SPI transmission
		// ignore the response
		ignore = SPI0DAT;
}

char spi_read_byte() {
		SPIF = 0;
	// write dummy byte to get next byte
		SPI0DAT = 0x80;
		while (SPIF == 0); // wait end of SPI transmission
		// return the received byte
		return SPI0DAT;
}



char adxl_read(char reg_address, int n) {
	
	for (i = 0; i < 6; i++) {
		adxl_buf[i] = 0x00;
	}
	
	// construct command byte
	// mode READ
	cmd = 0x80;
	
	if (n < 1) {
		return 0x00; // error
	}
	
	if (n > 1) {
		// multi-byte : MB = 1
		cmd |= 0x40;
	}
	
	// add register address
	cmd |= reg_address;
	
	i = 0;
	SS = 0;
	spi_write_byte(cmd);
	while (i < n) {
		adxl_buf[i] = spi_read_byte();
		i++;
	}
	
	// end of transaction
	SS = 1;
	
	// return number of bytes read
	return i;
}

void adxl_write(char reg_address, char value) {
	
	// construct command byte
	// mode WRITE single-bit
	cmd = 0x00;
	
	// add register address
	cmd |= reg_address;
	
	// write full sequence
	SS = 0;
	spi_write_byte(cmd);
	spi_write_byte(value);
	SS = 1;
	
}

void config_adxl() {
	
	fct_tempo(1100);
	
	// init sequence
	adxl_write(0x31, 0x0B);
	adxl_write(0x2D, 0x08);
	adxl_write(0x2E, 0x80);
	
}

float adxl_convert_value(char high_byte, char low_byte) {
	
	value = 0.0f;
	
	// la conversion n'est pas correcte
	
	tmp = high_byte << 8;
	tmp |= low_byte;
	
	value = tmp*3.9f;
	
	return value;
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
	
	config_spi();
	
	config_adxl();
	
	SBUF0 = 's';
  
  while (1)
  {
		LED = 1;
		
		// On va lire les 6 registres contenant les valeurs mesurées 
		rep = adxl_read(0x32, 6);
		
		fct_tempo(20*1000);
		LED = 0;
		
		// Les données reçues sont maintenant dans adxl_buf
		// Il reste à les convertir puis les afficher
		
		
		wait_sec(2);
	}	 	 
}

 

 