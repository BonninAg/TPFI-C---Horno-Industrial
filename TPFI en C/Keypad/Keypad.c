/*
 * Keypad.c
 *
 * Created: 3/4/2026 23:14:58
 *  Author: mryin
 */ 


#include "F:\Acceso Directos Posta\Accesos Directos\UADER\2° Año\Sistemas digitales IV\TP final C - Horno insdustrial\TPFI-C---Horno-Industrial\TPFI en C\Keypad\Keypad.h"



uint8_t Convertir_Keypad (uint16_t valor_adc, uint8_t* ptrMenu, uint8_t* ptrCursor, char* ptrEnter ,char* ptrExit){

	
	if(valor_adc >= 900 && valor_adc < 910){
		*ptrCursor -= 1;
		return 2;
	}
	
	else if(valor_adc >= 745 && valor_adc < 760){
		*ptrMenu -= 1;		
		return 4;	
	}
	
	else if(valor_adc >= 870 && valor_adc < 880){
		*ptrEnter = 1;
		return 5;	
	}
	
	
	else if(valor_adc >= 975 && valor_adc < 985){
		*ptrMenu += 1;
		return 6;
	}
	
	else if(valor_adc >= 835 && valor_adc < 850){
		*ptrCursor += 1;
		return 8;
	}
	
	else if(valor_adc >= 875 && valor_adc < 895){ // #
		*ptrExit = 1;
		return 11;
	}
	else if(valor_adc >= 795 && valor_adc < 810)
	return 0;
	else if(valor_adc >= 765 && valor_adc < 775)
	return 1;
	else if(valor_adc >= 1010 && valor_adc < 1020)
	return 3;
	else if(valor_adc >= 715 && valor_adc < 725)
	return 7;	
	else if(valor_adc >= 925 && valor_adc < 935)
	return 9;
	else if(valor_adc >= 690 && valor_adc < 700) // *
	return 10;
	

	return 255; // ningún botón
}


















/*
uint8_t Menu = 0;

typedef struct {uint16_t centro; uint8_t tecla;} Key;

Key tabla[] = {
	{800, 0},
	{770, 1},
	{905, 2},
	{1015, 3},
	{750, 4},
	{875, 5},
	{980, 6},
	{720, 7},
	{840, 8},
	{930, 9},
	{695, 10},
	{885, 11}
};

uint8_t Convertir_Keypad (uint16_t valor_adc){
	for(int i = 0; i < 12; i++){
		if(abs(valor_adc - tabla[i].centro) < 10)
		tecla = tabla[i].tecla;
	}
	
	if (tecla == 6){
		Menu +=1;
	}
	return Menu;
	
	if (tecla == 4){
		Menu = Menu -1;
	}
	return Menu;
}
*/

/*
uint8_t Convertir_Keypad (uint16_t valor_adc){

	if(valor_adc >= 795 && valor_adc < 810)
	return 0;
	else if(valor_adc >= 765 && valor_adc < 775)
	return 1;
	else if(valor_adc >= 900 && valor_adc < 910)
	return 2;
	else if(valor_adc >= 1010 && valor_adc < 1020)
	return 3;
	else if(valor_adc >= 745 && valor_adc < 760)
	return 4;
	else if(valor_adc >= 870 && valor_adc < 880)
	return 5;
	else if(valor_adc >= 975 && valor_adc < 985)
	return 6;
	else if(valor_adc >= 715 && valor_adc < 725)
	return 7;
	else if(valor_adc >= 835 && valor_adc < 850)
	return 8;
	else if(valor_adc >= 925 && valor_adc < 935){
		return 9;
	}
	else if(valor_adc >= 690 && valor_adc < 700) //(Asterisco)
		return 10;
	else if(valor_adc >= 875 && valor_adc < 895) //(numeral)
		return 11;
	return 0;
}
*/
