/*
 * Keypad.h
 *
 * Created: 3/4/2026 23:14:40
 *  Author: mryin
 */ 


#ifndef KEYPAD_H_
#define KEYPAD_H_

#include <stdint.h>



void Convertir_Keypad(uint16_t valor_adc,
						 uint8_t* ptrMenu, 
						 uint8_t* ptrCursor,
						 uint8_t* ptrEnter,
						 uint8_t* ptrExit,
						 uint8_t* ptrCambio_Menu,
						 uint8_t* ptrReconocimiento);

#endif /* KEYPAD_H_ */