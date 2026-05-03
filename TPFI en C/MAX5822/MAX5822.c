/*
 * MAX5822.c
 *
 * Created: 14/3/2026 02:11:22
 *  Author: 
 */ 

#include <avr/io.h>
#include "../MAX5822/MAX5822.h"
#include "../I2C/I2C.h"

//#include "F:\Acceso Directos Posta\Accesos Directos\UADER\2° Año\Sistemas digitales IV\TP final C - Horno insdustrial\TPFI en C\TPFI en C\MAX5822\MAX5822.h"
//#include "F:\Acceso Directos Posta\Accesos Directos\UADER\2° Año\Sistemas digitales IV\TP final C - Horno insdustrial\TPFI en C\TPFI en C\I2C\I2C.h"
//#include "C:\Users\joaqu\OneDrive\Escritorio\TFI C\TPFI-C---Horno-Industrial\TPFI en C\MAX5822\MAX5822.h"
//#include "C:\Users\joaqu\OneDrive\Escritorio\TFI C\TPFI-C---Horno-Industrial\TPFI en C\I2C\I2C.h"

//--------------------------Escribir en el DAC-------------------------
void Escribir_MAX5822 (uint8_t MAX5822_address, uint8_t canal, uint16_t convertirValor){

//USART_SendString("max5822\r\n");**************************************

	uint16_t lalala;
	uint8_t byte1;
	uint8_t byte2;

	convertirValor = (convertirValor & 0x0FFF);		//máscara para tener solo 12bits
	
	byte1 = (canal | (convertirValor >> 8));		//control más los 4bits más altos
	byte2 = (convertirValor & 0x00FF);				//byte bajo

 
	Start_I2C();
	Enviar_Dato_I2C(MAX5822_address);				//address
	Enviar_Dato_I2C(byte1);							//4 bits de comando y 4 bits mas altos
	Enviar_Dato_I2C(byte2);							//8 más bajos
	Stop_I2C();

 //lalala = (byte1<<8)|byte2;

//sprintf (pruebaChar, "valorMAX: %u\r\n", lalala);		// Convertir el valor numérico a una cadena de texto
//USART_SendString(prueba);
	

	//------------------------------------------------------------------------------------------------

}
