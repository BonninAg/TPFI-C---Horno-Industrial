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

char buffer[20];
char imprimir [20];


void USART_Transmit(unsigned char data);
void USART_SendString(char* s);


volatile short unsigned int habilitar_conversion = 0;
volatile uint16_t contador = 0;
volatile uint16_t valor_adc;
		 uint8_t Habilitar_Teclado;


uint8_t Convertir_Keypad (uint16_t valor_adc);
uint8_t Keypad_Letra;

/*-------variables para el ads1115---------*/
volatile uint16_t temperatura;
volatile uint8_t Habilitar_LeerTemperatura = 0;
volatile uint8_t Sensor_Temperatura = 0;
volatile uint16_t Vector_Temperaturas [8];
volatile uint8_t Canal_Temp = 0;

volatile uint8_t ads1155_MSB_Config;
/*-----------------------------------------*/

#include "ADS1115/ADS1115.h"
#include "I2C/I2C.h"
#include "MAX5822/MAX5822.h"
#include "PCF8574/PCF8574.h"
#include "LCD/LCD.h"


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
ADCSRA = (1<<ADEN)  |  // habilita ADC
(1<<ADIE)  |  // habilita interrupción ADC
(1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0); // prescaler 128
ADCSRB = 0x00;
DIDR0 = 0x00;

DDRC &= ~(1 << PC0);   // PC0 como entrada
PORTC &= ~(1 << PC0);

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

USART_SendString("configure\r\n");//************************************
 
 
uint16_t deleteME = 500;
uint16_t deleteME2 = 100;
uint16_t deleteME3 = 5;

Menu_SetPoints_Z1(2, deleteME, deleteME3);
 
 
 
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
		
		
		if(Habilitar_Teclado == 1){
			Keypad_Letra = Convertir_Keypad (valor_adc);
			
			sprintf(imprimir, "Teclado: %u\r\n", Keypad_Letra);	// Convertir el valor numérico a una
			USART_SendString(imprimir);						// Enviar el texto por el puerto serie
			
			if(Keypad_Letra == 4){
				
			}
			
			Habilitar_Teclado =0;
		}//teclado
			


/*
		if (Habilitar_LeerTemperatura == 1){
			
			switch (Sensor_Temperatura){
				
				case 1:
					cofigurar_ads1115(ads1115_IP_GND_write, ads1115_Config_Reg, ads1155_MSB_Config, 0b11100000);
					ads1155_MSB_Config += 0x10;
					Habilitar_LeerTemperatura = 0;
					if (ads1155_MSB_Config > 0xF1){
						Sensor_Temperatura = 2;
						ads1155_MSB_Config = 0xC1;
					}
				break;//case1
			
				case 2:
					cofigurar_ads1115(ads1115_IP_VCC_write, ads1115_Config_Reg, ads1155_MSB_Config, 0b11100000);
					ads1155_MSB_Config += 0x10;
					Habilitar_LeerTemperatura = 0;
					if (ads1155_MSB_Config > 0xF1){
						Sensor_Temperatura = 0;
						ads1155_MSB_Config = 0xC1;
				}
				break;//case2
			
				default:
				Sensor_Temperatura = 0;
				ads1155_MSB_Config = 0xC1;
			}//switch
		}//habilitarLeer
	*/	
	}//while
    
}//main

ISR(TIMER1_COMPA_vect) {
USART_SendString("timer1\r\n");//**************************************

 //for (ads1155_MSB_Config = 0xC1; ads1155_MSB_Config < 0x101; ads1155_MSB_Config += 0x10){

ads1155_MSB_Config = 0xC1;

for (char i = 0; i <4; i++){	 
	 cofigurar_ads1115(ads1115_IP_VCC_write, ads1115_Config_Reg, ads1155_MSB_Config, 0b11100000);

	_delay_ms(5);

	 temperatura = Leer_ads1115(ads1115_IP_VCC_write);
	 
	 //--------------cambio de canal--------------
	 ads1155_MSB_Config += 0x10;
	 
	 if(ads1155_MSB_Config > 0xF1){
		 ads1155_MSB_Config = 0xC1;
	 }
	 //-------------------------------------------
	 
	 sprintf(buffer, "temp: %u\r\n", temperatura);	// Convertir el valor numérico a una cadena de texto
	 USART_SendString(buffer);								// Enviar el texto por el puerto serie
 }
 
/*
//----------leer los ads1115----------------

	Habilitar_LeerTemperatura = 1;
	Sensor_Temperatura = 1;

//------------------------------------------

//ads1155_MSB_Config = 0xC1;
//cofigurar_ads1115(ads1115_IP_GND_write, ads1115_Config_Reg, 0xF1, 0b11100000);
*/

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

}


ISR(INT0_vect) {
	USART_SendString("int0\r\n");//**************************************
//sprintf(buffer, "config: %u\r\n", ads1155_MSB_Config);	// Convertir el valor numérico a una cadena de texto
//USART_SendString(buffer);							     	// Enviar el texto por el puerto serie


/*
	if (Sensor_Temperatura == 1){
		temperatura = Leer_ads1115(ads1115_IP_GND_write);
		Habilitar_LeerTemperatura = 1;
	}

	if(Sensor_Temperatura == 2){
		temperatura = Leer_ads1115(ads1115_IP_VCC_write);
		Habilitar_LeerTemperatura = 1;
	}

//	Vector_Temperaturas [Canal_Temp] = temperatura;
//	Canal_Temp++;
	
	sprintf(buffer, "temp: %u\r\n", temperatura);	// Convertir el valor numérico a una cadena de texto
	USART_SendString(buffer);								// Enviar el texto por el puerto serie
*/



/*
sprintf(buffer, "valorReg: %u\r\n", ads1155_MSB_Config);	// Convertir el valor numérico a una cadena de texto
USART_SendString(buffer);
	temperatura = Leer_ads1115(ads1115_IP_GND_write);
					
//--------------cambio de canal--------------
		ads1155_MSB_Config += 0x10;
		
		if(ads1155_MSB_Config > 0xF1){
			ads1155_MSB_Config = 0xC1;	
		}
//-------------------------------------------	

	sprintf(buffer, "valorReg: %u\r\n", ads1155_MSB_Config);	// Convertir el valor numérico a una cadena de texto
	USART_SendString(buffer);

	cofigurar_ads1115(ads1115_IP_GND_write, ads1115_Config_Reg, ads1155_MSB_Config, 0b11100000);


	sprintf(buffer, "Temp: %u\r\n", temperatura);	// Convertir el valor numérico a una cadena de texto
	USART_SendString(buffer);						// Enviar el texto por el puerto serie


*/
}








ISR(INT1_vect){
	// Seleccionar canal 0 (por si venías usando otro)
	ADMUX = (ADMUX & 0xF0) | 0x00;
	// Iniciar conversión
	ADCSRA |= (1<<ADSC);
}

ISR(ADC_vect) {
	valor_adc = ADC;  // lee ADCL + ADCH automáticamente
	
	sprintf(imprimir, "ADC: %u\r\n", valor_adc);	// Convertir el valor numérico a una cadena de texto
	USART_SendString(imprimir);						// Enviar el texto por el puerto serie
	Habilitar_Teclado =1;
	
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


uint8_t Convertir_Keypad (uint16_t valor_adc){

	if(valor_adc >= 795 && valor_adc < 810)	
		return 0;
	else if(valor_adc >= 765 && valor_adc < 775) 
		return 1;
	else if(valor_adc >= 900 && valor_adc < 910)
		return 2;
	else if(valor_adc >= 1010 && valor_adc < 1020)
		return 3;
	else if(valor_adc >= 745 && valor_adc < 760)
		return 4;
	else if(valor_adc >= 870 && valor_adc < 880)
		return 5;
	else if(valor_adc >= 975 && valor_adc < 985)
		return 6;
	else if(valor_adc >= 715 && valor_adc < 725)
		return 7;
	else if(valor_adc >= 835 && valor_adc < 850)
		return 8;
	else if(valor_adc >= 925 && valor_adc < 935){
		return 9;
	}
	return 0;
		
//	else if(valor_adc >= 690 && valor_adc < 700) (Asterisco)
//		return = 0b00101010; 
//	else if(valor_adc >= 875 && valor_adc < 895) (numeral)
//		return = 0b00100011;
	
}





/*
	if(valor_adc >= 795 && valor_adc < 805)
	return '0';
	else if(valor_adc >= 765 && valor_adc < 770)
	return '1';
	else if(valor_adc >= 900 && valor_adc < 910)
	return '2';
	else if(valor_adc >= 1010 && valor_adc < 1020)
	return '3';
	else if(valor_adc >= 745 && valor_adc < 760)
	return '4';
	else if(valor_adc >= 870 && valor_adc < 880)
	return '5';
	else if(valor_adc >= 975 && valor_adc < 985)
	return '6';
	else if(valor_adc >= 715 && valor_adc < 720)
	return '7';
	else if(valor_adc >= 835 && valor_adc < 850)
	return '8';
	else if(valor_adc >= 925 && valor_adc < 935){
		return '9';
*/