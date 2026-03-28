/*
 * I2C.c
 *
 * Created: 12/3/2026 16:43:51
 *  Author: mryin
 */ 


#include "I2C.h"

	//start
	void Start_I2C (void){
		TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
		while (!(TWCR & (1<<TWINT)));
	}
	
	//STOP
	void Stop_I2C (void){
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);		
	}
	
	void Enviar_Dato_I2C (uint8_t dato){
	TWDR = dato;
	TWCR = (1<<TWINT)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)));		
	}


uint8_t Leer_dato_I2C (void){
	uint8_t dato;
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
	while (!(TWCR & (1<<TWINT)));
	dato=TWDR;
	return dato;
}

uint8_t Leer_dato_I2C_NACK(){
	TWCR = (1<<TWINT)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	return TWDR;
}
