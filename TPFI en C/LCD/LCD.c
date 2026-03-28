/*
 * LCD.c
 *
 * Created: 17/3/2026 18:24:30
 *  Author: mryin
 */ 

#include "LCD.h"
#include "F:\Acceso Directos Posta\Accesos Directos\UADER\2° Año\Sistemas digitales IV\TP final C - Horno insdustrial\TPFI en C\TPFI en C\PCF8574\PCF8574.h"

void Iniciar_LCD (void){
	
	_delay_ms(20);
													// (DB7)(DB6)(DB5)(DB4) (PD3)(E)(RW)(RS)
	Escribir_PSF8574( 0b00110000 | 0b00000100);     //   0    0    1    1     0   1   0   0
	Escribir_PSF8574( 0b00110000 | 0b00000000);     //   0    0    1    1     0   0   0   0

	_delay_ms(5);
													// (DB7)(DB6)(DB5)(DB4) (PD3)(E)(RW)(RS)
	Escribir_PSF8574( 0b00110000 | 0b00000100);     //   0    0    1    1     0   1   0   0
	Escribir_PSF8574( 0b00110000 | 0b00000000);     //   0    0    1    1     0   0   0   0

	_delay_us(100);
													// (DB7)(DB6)(DB5)(DB4) (PD3)(E)(RW)(RS)
	Escribir_PSF8574( 0b00110000 | 0b00000100);     //   0    0    1    1     0   1   0   0
	Escribir_PSF8574( 0b00110000 | 0b00000000);     //   0    0    1    1     0   0   0   0

	_delay_us(100);
												    // (DB7)(DB6)(DB5)(DB4) (PD3)(E)(RW)(RS)
	Escribir_PSF8574( 0b00100000 | 0b00000100);     //   0    0    1    0     0   1   0   0
	Escribir_PSF8574( 0b00100000 | 0b00000000);	    //   0    0    1    0     0   0   0   0

	_delay_us(100);
	   

//function set											   	     (DL) (N) (F)
		Escribir_Comando_LCD(0b00101000);				//		   0   1   0
		
		Escribir_Comando_LCD(0b00001000); //display off
		Escribir_Comando_LCD(LCD_CLEAR); //clear
		Escribir_Comando_LCD(0b00000110); //entry mode set
		Escribir_Comando_LCD(0b00001111); //display on		
		
		USART_SendString("LCD_init\r\n");//**************************************
}


void Escribir_Caracter_LCD (char letra){
	char Letra_Low;
	char Letra_High;
	
	Letra_High = (letra & 0xF0);
	Letra_Low =  (letra << 4);
	
	Escribir_PSF8574 (Letra_High | 0b00001101);
	Escribir_PSF8574 (Letra_High | 0b00001001);	

	Escribir_PSF8574 (Letra_Low | 0b00001101);
	Escribir_PSF8574 (Letra_Low | 0b00001001);	
	
	_delay_us(60);
}


void Escribir_Comando_LCD (char comando){
	char Letra_Low;
	char Letra_High;
	
	Letra_High = (comando & 0xF0);
	Letra_Low =  (comando << 4);

	Escribir_PSF8574 (Letra_High | 0b00001100);
	Escribir_PSF8574 (Letra_High | 0b00001000);

	Escribir_PSF8574 (Letra_Low | 0b00001100);
	Escribir_PSF8574 (Letra_Low | 0b00001000);
	
	
	 if(comando == LCD_CLEAR || comando == LCD_HOME)	//clear y home demoran más tiempo
	 _delay_ms(2);
	 else
	 _delay_us(60);
	
}


void Escribir_Texto_LCD (char *puntero){
	
	while (*puntero){
		
		Escribir_Caracter_LCD(*puntero++);
	}
}




/*
_delay_ms(20);
Escribir_PSF8574( 0b00110000 | 0b00000000);		// (DB7)(DB6)(DB5)(DB4) (PD3)(E)(RW)(RS)
Escribir_PSF8574( 0b00110000 | 0b00000100);     //   0    0    1    1     0   1   0   0
Escribir_PSF8574( 0b00110000 | 0b00000000);     //   0    0    1    1     0   0   0   0

_delay_ms(5);
Escribir_PSF8574( 0b00110000 | 0b00000000);		// (DB7)(DB6)(DB5)(DB4) (PD3)(E)(RW)(RS)
Escribir_PSF8574( 0b00110000 | 0b00000100);     //   0    0    1    1     0   1   0   0
Escribir_PSF8574( 0b00110000 | 0b00000000);     //   0    0    1    1     0   0   0   0

_delay_us(100);
Escribir_PSF8574( 0b00110000 | 0b00000000);		// (DB7)(DB6)(DB5)(DB4) (PD3)(E)(RW)(RS)
Escribir_PSF8574( 0b00110000 | 0b00000100);     //   0    0    1    1     0   1   0   0
Escribir_PSF8574( 0b00110000 | 0b00000000);     //   0    0    1    1     0   0   0   0

_delay_us(100);
Escribir_PSF8574( 0b00100000 | 0b00000000);	    // (DB7)(DB6)(DB5)(DB4) (PD3)(E)(RW)(RS)
Escribir_PSF8574( 0b00100000 | 0b00000100);     //   0    0    1    0     0   1   0   0
Escribir_PSF8574( 0b00100000 | 0b00000000);	    //   0    0    1    0     0   0   0   0

_delay_us(100);
*/