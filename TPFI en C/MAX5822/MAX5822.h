/*
 * MAX5822.h
 *
 * Created: 14/3/2026 02:11:08
 *  Author: mryin
 */ 


#ifndef MAX5822_H_
#define MAX5822_H_

#include <stdint.h>
#include <stdio.h>

//ya con el último bit para escritura.
#define MAX5822L_IP_GND_write  (0b01110000)
#define MAX5822L_IP_VCC_write  (0b01110010)
#define MAX5822M_IP_GND_write  (0b10110000)

//0000 xxxx
//0001 xxxx
#define canalA  0x00
#define canalB  0x10


void Escribir_MAX5822 (uint8_t MAX5822_address, uint8_t canal, uint16_t convertirValor);




#endif /* MAX5822_H_ */