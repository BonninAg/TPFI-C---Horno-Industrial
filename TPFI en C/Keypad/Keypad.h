/*
 * Keypad.h
 *
 * Created: 3/4/2026 23:14:40
 *  Author: mryin
 */ 


#ifndef KEYPAD_H_
#define KEYPAD_H_

#include <stdint.h>



uint8_t tecla;

uint8_t Convertir_Keypad(uint16_t valor_adc, char* puntero);




#endif /* KEYPAD_H_ */