/*
 * ADS1115.h
 *
 * Created: 12/3/2026 17:07:52
 *  Author: 
 */ 


#ifndef ADS1115_H_
#define ADS1115_H_

#include <avr/io.h>
#include <stdint.h>

#include "F:\Acceso Directos Posta\Accesos Directos\UADER\2° Año\Sistemas digitales IV\TP final C - Horno insdustrial\TPFI en C\TPFI en C\I2C\I2C.h"


//etiquetas con direcciones.
#define ads1115_IP_GND_write  (0x48  << 1)      
#define ads1115_IP_VCC_write  (0x49  << 1)    


//etiquetas con registros.
#define ads1115_Conversion_Reg 0x00 
#define ads1115_Config_Reg 0x01
#define ads1115_LoThresh_Reg 0x02
#define ads1115_HiThresh_Reg 0x03

volatile uint8_t ads1115_address;
volatile uint8_t ads1155_pointer_register;

volatile uint8_t ads1155_LSB_Config;


unsigned short int Leer_ads1115(uint8_t ads1115_address);

void cofigurar_ads1115(uint8_t ads1115_address,
					   uint8_t ads1155_pointer_register,
					   uint8_t ads1155_MSB_Config,
					   uint8_t ads1155_LSB_Config);


#endif /* ADS1115_H_ */