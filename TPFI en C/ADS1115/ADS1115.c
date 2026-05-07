/*
 * ADS1115.c
 *
 * Created: 12/3/2026 17:08:20
 *  Author: 
 */ 



#include "ADS1115.h"


//----------------------Escribir el registro de configuración ads1115-----------------------------------
void cofigurar_ads1115(uint8_t ads1115_address,
			  		   uint8_t ads1155_pointer_register,
					   uint8_t ads1155_MSB_Config,
					   uint8_t ads1155_LSB_Config){
	
	Start_I2C();
	
	Enviar_Dato_I2C(ads1115_address);			//mandamos la: target address + R/W bit (= 0 para W)
	
	Enviar_Dato_I2C(ads1155_pointer_register);  //pointer register (= 1 para escribir en el registro de config)
	
	Enviar_Dato_I2C(ads1155_MSB_Config);		//data byte 1 (escribimos la parte alta del registro de config)
	
	Enviar_Dato_I2C(ads1155_LSB_Config);		//data byte 2 (escribimos la parte baja del registro de config)
	
	Stop_I2C();

	//--------------------------------------------------------------------------------------------
}


//------------------------Leer el registro de conversión ads1115------------------------------
unsigned short int Leer_ads1115(uint8_t ads1115_address){
	
short unsigned int temp;
	Start_I2C();
	
	Enviar_Dato_I2C(ads1115_address);			//mandamos la: address + R/W bit (= 0 para W)
	
	Enviar_Dato_I2C(ads1115_Conversion_Reg);	//mandamos el: pointer register (= 0 para leer el registro de conversión)
	
	Start_I2C();								//REPEATED START
	
	Enviar_Dato_I2C((ads1115_address) | 0x01);	//mandamos la: target address + R/W bit (= 1 para R)
		
	temp = (Leer_dato_I2C() <<8 );				//parte alta
	temp |= Leer_dato_I2C_NACK();				//parte baja

	Stop_I2C();
	return temp;
	//------------------------------------------------------------------------------------------------
}