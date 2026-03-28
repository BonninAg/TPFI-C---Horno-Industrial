/*
 * I2C.h
 *
 * Created: 12/3/2026 16:42:21
 *  Author: mryin
 */ 


#ifndef I2C_H_
#define I2C_H_

#include <avr/io.h>
#include <stdint.h>

void Start_I2C (void);
void Stop_I2C (void);
void Enviar_Dato_I2C (uint8_t dato);

uint8_t Leer_dato_I2C (void);
uint8_t Leer_dato_I2C_NACK (void);

#endif /* I2C_H_ */