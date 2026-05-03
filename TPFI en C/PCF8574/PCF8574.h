/*
 * PCF8574.h
 *
 * Created: 17/3/2026 16:39:53
 *  Author: mryin
 */ 


#ifndef PCF8574_H_
#define PCF8574_H_

#include <stdint.h>

#include "../I2C/I2C.h"

//#include "F:\Acceso Directos Posta\Accesos Directos\UADER\2° Año\Sistemas digitales IV\TP final C - Horno insdustrial\TPFI en C\TPFI en C\I2C\I2C.h"
//#include "C:\Users\joaqu\OneDrive\Escritorio\TFI C\TPFI-C---Horno-Industrial\TPFI en C\I2C\I2C.h"


#define PCF8574_IP_READ		0x41		//8bits, ya está desplazado
#define PCF8574_IP_WRITE	0x40		//8bits, ya está desplazado

 char pruebaChar[50];

void Escribir_PSF8574 (char dato);

uint8_t Leer_PSF8574 (void);




#endif /* PCF8574_H_ */