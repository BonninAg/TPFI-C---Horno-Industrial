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

uint8_t deleteME;
//uint16_t deleteME2 = 100;
//uint16_t deleteME3 = 5;

void USART_Transmit(unsigned char data);
void USART_SendString(char* s);

/*--------variables menu lcd---------*/

struct pantallaPPL {
		uint8_t dni;
		void(*graficos)(void);
		char hijos [5];				//opciones
		uint8_t opcionMax;
		uint8_t opcionMin;			
	};

char maq_estado_pantalla = 100;
uint8_t Menu = 0;
uint8_t Cursor = 0;
char Enter = 0;
char Exit = 0;

uint8_t Actualizar_Menu = 0;
uint8_t no_repetir = 1;

char renglon[4] = {0x80, 0xC0, 0x90, 0xD0};

#define Opcion1 1
#define Opcion2 2
#define Opcion3 3
#define Opcion4 4

PROGMEM const char espacio[] = " ";


uint8_t Estado_Planta =1;
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
	//dni, graficos, hijos, opcionesMax
	struct pantallaPPL principal1 = {100, &Pantalla_Principal_1, {90, 80, 70, 60, 101}, 3, 0};
	struct pantallaPPL principal2 = {101, &Pantalla_Principal_2, {50, 40, 30}, 2, 0};
	
	struct pantallaPPL avisos = {80, &Menu_Avisos, {0, 81, 82}, 2, 1};
	struct pantallaPPL alarmas = {70, &Menu_Alarmas, {0, 71, 72, 73}, 3, 1};	   

   
   
  /*------------------PCInt PD5----------------------*/
  DDRD &= ~(1 << DDD5);     // PD5 como entrada
  PORTD |= (1 << PORTD5);   // Pull-up habilitado
  PCICR |= (1 << PCIE2);    // Habilita grupo PCINT[23:16]
  PCMSK2 |= (1 << PCINT21);
  
  /*-------------------------------------------------*/ 
   
   
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
 
 


 
    while (1) {
/*-------------------------------------------Teclado------------------------------------------*/	
		if(Habilitar_Teclado == 1){
			Habilitar_Teclado =0;		
			deleteME = Convertir_Keypad (valor_adc, &Menu, &Cursor, &Enter, &Exit);
			Actualizar_Menu = 0;
			
				
					
		}//teclado
/*---------------------------------------------------------------------------------------------*/		
	

while (Actualizar_Menu < 2) {
	Actualizar_Menu += 1;
	

	sprintf(imprimir, "maqEst: %u\r\n", maq_estado_pantalla);
	USART_SendString(imprimir);
/*
	sprintf(imprimir, "cursor: %u\r\n", Cursor);
	USART_SendString(imprimir);
	sprintf(imprimir, "Menu: %u\r\n", Menu);
	USART_SendString(imprimir);
*/	
/*-----------------------principal 1----------------------------------*/
	if (maq_estado_pantalla == 100){

		if (no_repetir)	{
			no_repetir =0;
		principal1.graficos();
		}
		if(Enter == 1){
			Enter =0;
			no_repetir =1;
			maq_estado_pantalla = principal1.hijos[Cursor];	
		}
		
		if (Cursor == 255) Cursor = principal1.opcionMin;	
		if (Cursor > (principal1.opcionMax)) Cursor = principal1.opcionMax;
		
		for(uint8_t i = 0; i <= principal1.opcionMax; i++){
			Escribir_Comando_LCD(renglon[i]);
			Escribir_FraseFlash_LCD(espacio);
		}

		Escribir_Comando_LCD(renglon[Cursor]);
		Escribir_Caracter_LCD(Right_Arrow);

	
	}//maq100
/*--------------------------------------------------------------------*/	
/*------------------------principal 2---------------------------------*/
	if(maq_estado_pantalla == 101){
	
		if (no_repetir)	{
			no_repetir =0;
		principal2.graficos();
		}
		if(Enter == 1){
			Enter =0;
			no_repetir =1;
			maq_estado_pantalla = principal2.hijos[Cursor];
		}
		if (Exit == 1){
			Exit = 0;
			no_repetir =1;
			maq_estado_pantalla = principal1.dni;
		}
		
		if (Cursor == 255) Cursor = principal2.opcionMin;
		if (Cursor > (principal2.opcionMax)) Cursor = principal2.opcionMax;
		
		for(uint8_t i = 0; i <= principal2.opcionMax; i++){
			Escribir_Comando_LCD(renglon[i]);
			Escribir_FraseFlash_LCD(espacio);
		}
		Escribir_Comando_LCD(renglon[Cursor]);
		Escribir_Caracter_LCD(Right_Arrow);	
	
	}//maq101
/*--------------------------------------------------------------------*/	



/*------------------------General---------------------------------*/
	if (maq_estado_pantalla == 90){
		if (no_repetir)	{
			no_repetir =0;
			Menu_General(Estado_Planta);
		}
		if (Exit == 1){
			Exit = 0;
			no_repetir =1;
			maq_estado_pantalla = principal1.dni;
		}
	}
/*--------------------------------------------------------------------*/


/*------------------------Avisos---------------------------------*/	
	if (maq_estado_pantalla == 80){
		if (no_repetir)	{
			no_repetir =0;
			avisos.graficos();
		}
		if(Enter == 1){
			Enter =0;
			no_repetir =1;
			maq_estado_pantalla = avisos.hijos[Cursor];
		}
		if (Exit == 1){
			Exit = 0;
			no_repetir =1;
			maq_estado_pantalla = principal1.dni;
		}
		if (Cursor < avisos.opcionMin) Cursor = avisos.opcionMin;
		if (Cursor > (avisos.opcionMax)) Cursor = avisos.opcionMax;
		
		for(uint8_t i = 0; i <= avisos.opcionMax; i++){
			Escribir_Comando_LCD(renglon[i]);
			Escribir_FraseFlash_LCD(espacio);
		}
		Escribir_Comando_LCD(renglon[Cursor]);
		Escribir_Caracter_LCD(Right_Arrow);
	
	}
/*--------------------------------------------------------------------*/
	
/*------------------------avisos sensores---------------------------------*/
	if (maq_estado_pantalla == 81){
		if (no_repetir)	{
			no_repetir =0;
			Menu_Avisos_Sensores();
		}
		if (Exit == 1){
			Exit = 0;
			no_repetir =1;
			maq_estado_pantalla = avisos.dni;
		}
	}
/*--------------------------------------------------------------------*/
/*------------------------avisos temperaturas---------------------------------*/	
	if (maq_estado_pantalla == 82){
		if (no_repetir)	{
			no_repetir =0;
			Menu_Avisos_Temperatura();
		}
		if (Exit == 1){
			Exit = 0;
			no_repetir =1;
			maq_estado_pantalla = avisos.dni;
		}
	}
/*--------------------------------------------------------------------*/	


/*------------------------Alarmas---------------------------------*/
if (maq_estado_pantalla == 70){
	if (no_repetir)	{
		no_repetir =0;
		alarmas.graficos();
	}
	if(Enter == 1){
		Enter =0;
		no_repetir =1;
		maq_estado_pantalla = alarmas.hijos[Cursor];
	}
	if (Exit == 1){
		Exit = 0;
		no_repetir =1;
		maq_estado_pantalla = principal1.dni;
	}
	if (Cursor < alarmas.opcionMin) Cursor = alarmas.opcionMin;
	if (Cursor > (alarmas.opcionMax)) Cursor = alarmas.opcionMax;
	
	for(uint8_t i = 0; i <= alarmas.opcionMax; i++){
		Escribir_Comando_LCD(renglon[i]);
		Escribir_FraseFlash_LCD(espacio);
	}
	Escribir_Comando_LCD(renglon[Cursor]);
	Escribir_Caracter_LCD(Right_Arrow);

}
/*--------------------------------------------------------------------*/

/*------------------------Alarmas Sensores---------------------------------*/
/*--------------------------------------------------------------------*/

/*------------------------Alarmas temperaturas---------------------------------*/
if (maq_estado_pantalla == 72){
	if (no_repetir)	{
		no_repetir =0;
		Menu_Alarmas_Temperatura();
	}
	if (Exit == 1){
		Exit = 0;
		no_repetir =1;
		maq_estado_pantalla = alarmas.dni;
	}
}
/*--------------------------------------------------------------------*/

/*------------------------Alarmas PIDS---------------------------------*/
/*--------------------------------------------------------------------*/


}



	

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
	sprintf(buffer, "Temp: %u\r\n", Vector_Temperaturas[i]);	// Convertir el valor numérico a una cadena de texto
	USART_SendString(buffer);							// Enviar el texto por el puerto serie
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

ISR(PCINT2_vect) {
	if (!(PIND & (1 << PIND5))) {
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
}



ISR(INT0_vect) {
	//USART_SendString("int0\r\n");



}


ISR(INT1_vect){
	ADCSRA |= (1<<ADSC);   	// Iniciar conversión
}

ISR(ADC_vect) {
	valor_adc = ADC;  // lee ADCL + ADCH automáticamente
	Habilitar_Teclado = 1;
	
//	sprintf(imprimir, "ADC: %u\r\n", valor_adc);	// Convertir el valor numérico a una cadena de texto
//	USART_SendString(imprimir);						// Enviar el texto por el puerto serie	
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





/*-------------------------------------------Pantallas------------------------------------------*/	
/*
if (Actualizar_Menu == 1){
	Actualizar_Menu = 0;

		if (maq_estado_pantalla == 100 || maq_estado_pantalla == 99){
				maq_estado_pantalla=100;
			if (no_repetir == 0){
				no_repetir = 1;
				Pantalla_Principal_1();
			}
		
			if (cursor == 0)
				cursor = Opcion1;
			if (cursor == 5)
				cursor = Opcion4;	
				
			Escribir_Comando_LCD(Linea1_);		
			if (cursor == Opcion1){
				Escribir_Caracter_LCD(Right_Arrow);			
			}else
				Escribir_FraseFlash_LCD(espacio);
				
			Escribir_Comando_LCD(Linea2_);
			if (cursor == Opcion2){	
				Escribir_Caracter_LCD(Right_Arrow);
			}else
				Escribir_FraseFlash_LCD(espacio);	
			
			Escribir_Comando_LCD(Linea3_);		
			if (cursor == Opcion3){
				Escribir_Caracter_LCD(Right_Arrow);
			}else
				Escribir_FraseFlash_LCD(espacio);	
				
			Escribir_Comando_LCD(Linea4_);	
			if (cursor == Opcion4){
				Escribir_Caracter_LCD(Right_Arrow);
			}else
				Escribir_FraseFlash_LCD(espacio);		
			
		}
		
		if (maq_estado_pantalla == 101 || maq_estado_pantalla == 102){
			maq_estado_pantalla=101;
			if (no_repetir == 1){
				no_repetir = 0;
				Pantalla_Principal_2();
			}
		
			if (cursor == 0)
			cursor = Opcion1;
			if (cursor == 4)
			cursor = Opcion3;
			
			Escribir_Comando_LCD(Linea1_);
			if (cursor == Opcion1){
				Escribir_Caracter_LCD(Right_Arrow);
			}else
				Escribir_FraseFlash_LCD(espacio);
			
			Escribir_Comando_LCD(Linea2_);	
			if (cursor == Opcion2){
				Escribir_Caracter_LCD(Right_Arrow);
			}else
				Escribir_FraseFlash_LCD(espacio);
			
			Escribir_Comando_LCD(Linea3_);	
			if (cursor == Opcion3){
				Escribir_Caracter_LCD(Right_Arrow);
			}else
				Escribir_FraseFlash_LCD(espacio);

		
		}
}//actualizar menu
*/
/*---------------------------------------------------------------------------------------------*/