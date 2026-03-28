/*
 * PCF8574.c
 *
 * Created: 17/3/2026 16:40:08
 *  Author: 
 */ 

#include "PCF8574.h"

void Escribir_PSF8574 (char dato){
	
	Start_I2C();
	Enviar_Dato_I2C(0x7E);				//address
	Enviar_Dato_I2C(dato);						
	Stop_I2C();
	
//	sprintf (pruebaChar, "envioPCF: %u\r\n", dato);		// Convertir el valor numérico a una cadena de texto
//	USART_SendString(pruebaChar);
}

uint8_t Leer_PSF8574 (void){
	uint8_t Busy_Flag;
	
	Start_I2C();
	Enviar_Dato_I2C(PCF8574_IP_WRITE);				//address
	Busy_Flag = Leer_dato_I2C_NACK();
	Stop_I2C();
	
	return Busy_Flag;
}