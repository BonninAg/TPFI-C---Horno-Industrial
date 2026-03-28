/*
 * LCD.h
 *
 * Created: 17/3/2026 18:24:19
 *  Author: mryin
 */ 


#ifndef LCD_H_
#define LCD_H_

#define F_CPU 16000000UL
#include <util/delay.h>

/*------------------- Definiciones de Comandos de LCD ------------------------*/

#define     LCD_CLEAR	0b00000001  // Limpia pantalla
#define     LCD_HOME	0b00000010  // Retorno al inicio

#define		Linea1			0b10000000
#define		Linea2			0b11000000
#define		Linea3			0b10010000
#define		Linea4			0b11010000

#define		Right_Arrow		0b01111110
#define		Left_Arrow		0b01111111

#define LCD_Shift_R			0b00011100
#define LCD_Shift_L			0b00011000

#define LCD_Cursor_R		0b00010100
#define LCD_Cursor_L		0b00010000

#define gradito		0b11011111

void Iniciar_LCD(void);

void Escribir_Caracter_LCD (char Letra_High);

void Escribir_Comando_LCD (char Letra_High);

void Escribir_Texto_LCD (char *puntero);


#endif /* LCD_H_ */