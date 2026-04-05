/*
 * TPFI en C.c
 *
 * Created: 5/3/2026 00:37:49
 * Author : 
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include <stdlib.h>			//para el abs

char buffer[20];
char imprimir [20];


void USART_Transmit(unsigned char data);
void USART_SendString(char* s);

/*--------variables menu lcd---------*/
char maq_estado_pantalla = 100;
uint8_t Mostrar_Menu = 0;
uint8_t no_repetir;
//char menuPPL[] = 99, 100, 101, 102;

/*-----------------------------------------*/

/*--------variables para el Keypad---------*/
volatile uint16_t habilitar_conversion = 0;
volatile uint16_t contador = 0;
volatile uint16_t valor_adc;
volatile uint8_t  Habilitar_Teclado;
		 uint8_t Keypad_Letra;
/*-----------------------------------------*/

/*-------variables para el ads1115---------*/
volatile uint16_t temperatura;
volatile uint8_t Habilitar_LeerTemperatura = 0;
volatile uint8_t Sensor_Temperatura = 0;
volatile uint16_t Vector_Temperaturas [8];
volatile uint8_t Canal_Temp = 0;
/*-----------------------------------------*/

#include "ADS1115/ADS1115.h"
#include "I2C/I2C.h"
#include "MAX5822/MAX5822.h"
#include "PCF8574/PCF8574.h"
#include "LCD/LCD.h"
#include "Keypad/Keypad.h"


int main(void){
   
 /*-------------------------- I2C -----------------------------*/
   DDRC &= ~((1<<PC4) | (1<<PC5));   //como entradas para el i2c
    // Prescaler = 1 (TWPS = 0)
    TWSR &= ~((1<<TWPS0) | (1<<TWPS1));
    // Bit rate para 100 kHz
    TWBR = 72;
    // Habilitar TWI
    TWCR = (1<<TWEN);
/*-------------------------------------------------------------*/

/*------------- Timer1 --------------*/
    // Modo CTC
    TCCR1A = 0;
    TCCR1B = (1 << WGM12);
    // Valor de comparación para 1 segundo
    OCR1A = 15624;
    // Habilitar interrupción por compare match A
    TIMSK1 = (1 << OCIE1A);
    // Prescaler 1024
    TCCR1B |= (1 << CS12) | (1 << CS10);
/*-----------------------------------*/

/*------------- INT0 --------------*/
DDRD &= ~(1 << DDD2);
PORTD |= (1 << PORTD2);
EICRA |= (1 << ISC01);
EICRA &= ~(1 << ISC00);
EIMSK |= (1 << INT0);
/*---------------------------------*/


/*------------- INT1 --------------*/
// PD3 como entrada
DDRD &= ~(1<<DDD3);
// Flanco ascendente
EICRA |= (1<<ISC11) | (1<<ISC10);
// Habilitar INT1
EIMSK |= (1<<INT1);
/*---------------------------------*/


/*-------------------------- Usart ----------------------------*/
uint16_t ubrr = 103;   // Para 9600 baud con F_CPU = 16 MHz
// Cargar baud rate
UBRR0H = (ubrr >> 8);
UBRR0L = ubrr;
// Habilitar transmisión y recepción
UCSR0B = (1 << RXEN0) | (1 << TXEN0);
// Formato del frame:
// 8 data bits
// Paridad EVEN
// 1 stop bit
UCSR0C = (1 << UCSZ01) | (1 << UCSZ00) |   // 8 bits
(1 << UPM01);                     // EVEN parity
/*-------------------------------------------------------------*/

/*---------------------------- ADC ------------------------------*/
ADMUX = (1<<REFS0);  // AVcc como referencia, canal ADC0 (MUX=0000)
ADMUX = (ADMUX & 0xF0) | 0x00;  // Seleccionar canal 0 
ADCSRA = (1<<ADEN)  |  // habilita ADC
(1<<ADIE)  |  // habilita interrupción ADC
(1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0); // prescaler 128
ADCSRB = 0x00;
DIDR0 = 0x00;

DDRC &= ~(1 << PC0);   // PC0 como entrada
PORTC &= ~(1 << PC0);
/*---------------------------------------------------------------*/

	sei();
	
	//low thres
	cofigurar_ads1115(ads1115_IP_VCC_write, ads1115_LoThresh_Reg, 0x00, 0x00);
	//hight thres
	cofigurar_ads1115(ads1115_IP_VCC_write, ads1115_HiThresh_Reg, 0x80, 0x00);

	//low thres
	cofigurar_ads1115(ads1115_IP_GND_write, ads1115_LoThresh_Reg, 0x00, 0x00);
	//hight thres
	cofigurar_ads1115(ads1115_IP_GND_write, ads1115_HiThresh_Reg, 0x80, 0x00);


	//secuencia de Power-up device.
	Escribir_MAX5822 (DAC1, 0b11110000, 0b000000001100);
	Escribir_MAX5822 (DAC2, 0b11110000, 0b000000001100);
	Escribir_MAX5822 (DAC3, 0b11110000, 0b000000001100);

	Iniciar_LCD();

USART_SendString("Fin Config\r\n");//************************************
 
 
//1uint16_t deleteME = 500;
//uint16_t deleteME2 = 100;
//uint16_t deleteME3 = 5;


 /*
for (int i = 0; i < 16; i++){
Escribir_Comando_LCD(LCD_Shift_L);
}
for (int i = 0; i < 16; i++){
	Escribir_Comando_LCD(LCD_Shift_L);
}
 
 Escribir_Texto_LCD("125");
 Escribir_Caracter_LCD(gradito);
 */
 

 
    while (1) {
/*-------------------------------------------Teclado------------------------------------------*/	
		if(Habilitar_Teclado == 1){
			Habilitar_Teclado =0;		
			Mostrar_Menu = Convertir_Keypad (valor_adc, &maq_estado_pantalla);

			
				sprintf(imprimir, "maqEst: %u\r\n", maq_estado_pantalla);	// Convertir el valor numérico a una cadena de texto
				USART_SendString(imprimir);						// Enviar el texto por el puerto serie
			
		}//teclado
/*---------------------------------------------------------------------------------------------*/		
	

/*-------------------------------------------Pantallas------------------------------------------*/	
		if (maq_estado_pantalla == 100 || maq_estado_pantalla == 99){
				maq_estado_pantalla=100;
			if (no_repetir == 0){
				no_repetir = 1;
				Pantalla_Principal_1();
			}
		}
		
		if (maq_estado_pantalla == 101 || maq_estado_pantalla == 102){
			maq_estado_pantalla=101;
			if (no_repetir == 1){
				no_repetir = 0;
				Pantalla_Principal_2();
			}
		}
/*---------------------------------------------------------------------------------------------*/	

/*-------------------------------------------Leer ads1115------------------------------------------*/	
		if (Habilitar_LeerTemperatura == 1){
			Habilitar_LeerTemperatura = 0;
			Canal_Temp += 1;
			switch (Canal_Temp){
				case 1:
				ads1155_MSB_Config = 0xC1;
				break;
				case 2:
				ads1155_MSB_Config = 0xD1;
				break;						
				case 3:
				ads1155_MSB_Config = 0xE1;					
				break;						
				case 4:
				ads1155_MSB_Config = 0xF1;
				break;	
				default:					
				break;					
			}//switchCanal
			cofigurar_ads1115(ads1115_IP_VCC_write, ads1115_Config_Reg, ads1155_MSB_Config, 0b11100000);
			cofigurar_ads1115(ads1115_IP_GND_write, ads1115_Config_Reg, ads1155_MSB_Config, 0b11100000);		
		}//habilitarLeer
/*---------------------------------------------------------------------------------------------*/	

}//while
}//main


ISR(TIMER1_COMPA_vect) {
//USART_SendString("timer1\r\n");//**************************************

//----------leer los ads1115----------------
	Habilitar_LeerTemperatura = 1;
	Canal_Temp = 0;
	
for (char i = 0; i < 8; i++){
	//sprintf(buffer, "Temp: %u\r\n", Vector_Temperaturas[i]);	// Convertir el valor numérico a una cadena de texto
	//USART_SendString(buffer);							// Enviar el texto por el puerto serie
}	
//------------------------------------------


/*------------------dacs----------------------------
	contador += 10;
	if (contador > 4096){
		contador = 0;
	}
	
	Escribir_MAX5822 (DAC1, canalA, contador);
	Escribir_MAX5822 (DAC1, canalB, contador);
	
	Escribir_MAX5822 (DAC2, canalA, contador);
	Escribir_MAX5822 (DAC2, canalB, contador);
	
	Escribir_MAX5822 (DAC3, canalA, contador);
	Escribir_MAX5822 (DAC3, canalB, contador);
---------------------------------------------------*/
}


ISR(INT0_vect) {
	//USART_SendString("int0\r\n");

/*----------------------------------Leer ads1115--------------------------------------*/
	if (Canal_Temp == 1 || Canal_Temp == 2 || Canal_Temp == 3 || Canal_Temp == 4){

		temperatura = Leer_ads1115(ads1115_IP_VCC_write);
		Vector_Temperaturas [(Canal_Temp - 1)] = temperatura;
		
		temperatura = Leer_ads1115(ads1115_IP_GND_write);
		Vector_Temperaturas [(Canal_Temp - 1) + 4] = temperatura;
	
	Habilitar_LeerTemperatura = 1;	
	}
/*------------------------------------------------------------------------------------*/

}


ISR(INT1_vect){
	ADCSRA |= (1<<ADSC);   	// Iniciar conversión
}

ISR(ADC_vect) {
	valor_adc = ADC;  // lee ADCL + ADCH automáticamente
	Habilitar_Teclado =1;
	
	sprintf(imprimir, "ADC: %u\r\n", valor_adc);	// Convertir el valor numérico a una cadena de texto
	USART_SendString(imprimir);						// Enviar el texto por el puerto serie	
}	
	





void USART_Transmit(unsigned char data) {
	// Esperar a que el buffer de transmisión esté vacío
	while (!(UCSR0A & (1 << UDRE0)));
	// Poner el dato en el registro, esto envía el byte
	UDR0 = data;
}

void USART_SendString(char* s) {
	while (*s) {
		USART_Transmit(*s++);
	}
}
