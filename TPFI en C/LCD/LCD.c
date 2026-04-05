/*
 * LCD.c
 *
 * Created: 17/3/2026 18:24:30
 *  Author: 
 */ 

#include "LCD.h"
#include "F:\Acceso Directos Posta\Accesos Directos\UADER\2° Año\Sistemas digitales IV\TP final C - Horno insdustrial\TPFI en C\TPFI en C\PCF8574\PCF8574.h"



void Iniciar_LCD (void){
	
	_delay_ms(20);
													// (DB7)(DB6)(DB5)(DB4) (PD3)(E)(RW)(RS)
	Escribir_PSF8574( 0b00110000 | 0b00000100);     //   0    0    1    1     0   1   0   0
	Escribir_PSF8574( 0b00110000 | 0b00000000);     //   0    0    1    1     0   0   0   0

	_delay_ms(5);
													// (DB7)(DB6)(DB5)(DB4) (PD3)(E)(RW)(RS)
	Escribir_PSF8574( 0b00110000 | 0b00000100);     //   0    0    1    1     0   1   0   0
	Escribir_PSF8574( 0b00110000 | 0b00000000);     //   0    0    1    1     0   0   0   0

	_delay_us(100);
													// (DB7)(DB6)(DB5)(DB4) (PD3)(E)(RW)(RS)
	Escribir_PSF8574( 0b00110000 | 0b00000100);     //   0    0    1    1     0   1   0   0
	Escribir_PSF8574( 0b00110000 | 0b00000000);     //   0    0    1    1     0   0   0   0

	_delay_us(100);
												    // (DB7)(DB6)(DB5)(DB4) (PD3)(E)(RW)(RS)
	Escribir_PSF8574( 0b00100000 | 0b00000100);     //   0    0    1    0     0   1   0   0
	Escribir_PSF8574( 0b00100000 | 0b00000000);	    //   0    0    1    0     0   0   0   0

	_delay_us(100);
	   

//function set											   	     (DL) (N) (F)
		Escribir_Comando_LCD(0b00101000);				//		   0   1   0
		
		Escribir_Comando_LCD(0b00001000); //display off
		Escribir_Comando_LCD(LCD_CLEAR); //clear
		Escribir_Comando_LCD(0b00000110); //entry mode set
		Escribir_Comando_LCD(0b00001111); //display on		
}

void Escribir_Caracter_LCD (char letra){
	char Letra_Low;
	char Letra_High;
	
	Letra_High = (letra & 0xF0);
	Letra_Low =  (letra << 4);
	
	Escribir_PSF8574 (Letra_High | 0b00001101);
	Escribir_PSF8574 (Letra_High | 0b00001001);	

	Escribir_PSF8574 (Letra_Low | 0b00001101);
	Escribir_PSF8574 (Letra_Low | 0b00001001);	
	
	_delay_us(60);
}

void Escribir_Comando_LCD (char comando){
	char Letra_Low;
	char Letra_High;
	
	Letra_High = (comando & 0xF0);
	Letra_Low =  (comando << 4);

	Escribir_PSF8574 (Letra_High | 0b00001100);
	Escribir_PSF8574 (Letra_High | 0b00001000);

	Escribir_PSF8574 (Letra_Low | 0b00001100);
	Escribir_PSF8574 (Letra_Low | 0b00001000);
	
	
	 if(comando == LCD_CLEAR || comando == LCD_HOME)	//clear y home demoran más tiempo
	 _delay_ms(2);
	 else
	 _delay_us(60);
	
}

void Escribir_Texto_LCD (char *puntero){
	
	while (*puntero){
		
		Escribir_Caracter_LCD(*puntero++);
	}
}


void Escribir_FraseFlash_LCD (const char *puntero_Flash){
	
	char caracter;
	
	    while ((caracter = pgm_read_byte(puntero_Flash++))) {
		    Escribir_Caracter_LCD(caracter);
	
	}
}


PROGMEM const char General[] = "General";
PROGMEM const char Avisos[] = "Avisos";
PROGMEM const char Alarmas[] = "Alarmas";
PROGMEM const char SetPoints[] = "Set Points ";
void Pantalla_Principal_1 (void){
	
	Escribir_Comando_LCD(LCD_CLEAR);
	Escribir_Comando_LCD(LCD_Cursor_R);
	Escribir_FraseFlash_LCD(General);
	
	Escribir_Comando_LCD(Linea2);
	Escribir_FraseFlash_LCD(Avisos);
	
	Escribir_Comando_LCD(Linea3);
	Escribir_FraseFlash_LCD(Alarmas);

	Escribir_Comando_LCD(Linea4);
	Escribir_FraseFlash_LCD(SetPoints);

	Escribir_Comando_LCD(0xDF); //posición en pantalla.
	Escribir_Caracter_LCD(Right_Arrow);
	
}


PROGMEM const char Tiempos[] = "Tiempos";
PROGMEM const char R_Sensores[] = "Rang.Sensores";
PROGMEM const char R_Actuadores[] = "Rang.Actuadores";
void Pantalla_Principal_2 (void){
	
	Escribir_Comando_LCD(LCD_CLEAR);
	Escribir_Comando_LCD(LCD_Cursor_R);
	Escribir_FraseFlash_LCD(Tiempos);
	
	Escribir_Comando_LCD(Linea2);
	Escribir_FraseFlash_LCD(R_Sensores);
	
	Escribir_Comando_LCD(Linea3);
	Escribir_FraseFlash_LCD(R_Actuadores);

	Escribir_Comando_LCD(0xDF); //posición en pantalla.
	Escribir_Caracter_LCD(Left_Arrow);
	
}



PROGMEM const char Z1[] = "Z1:";
PROGMEM const char Z2[] = "Z2:";
PROGMEM const char Z3[] = "Z3:";
PROGMEM const char W[] = "W:";
PROGMEM const char A[] = "A:";

PROGMEM const char Arranque[] = "Arranque";
PROGMEM const char Activo[] = "Activo";
PROGMEM const char PControlada[] = "P.Controlada";
PROGMEM const char PEmergencia[] = "P.Emergencia";
PROGMEM const char * const Estado[] = {Arranque, Activo, PControlada, PEmergencia};
void Menu_General (uint8_t indice){
	
		Escribir_Comando_LCD(LCD_CLEAR);
		Escribir_FraseFlash_LCD(Z1);
		//variable
		Escribir_Comando_LCD(0x8B); //posición en pantalla.
		Escribir_FraseFlash_LCD(W);
		//variable
		Escribir_Comando_LCD(0xCB); //posición en pantalla.
		Escribir_FraseFlash_LCD(A);
		//variable
		Escribir_Comando_LCD(Linea2);
		Escribir_FraseFlash_LCD(Z2);
		//variable
		Escribir_Comando_LCD(Linea3);
		Escribir_FraseFlash_LCD(Z3);

Escribir_Comando_LCD(Linea4);
char *Puntero_Estado = (char*) pgm_read_word (&(Estado[indice])); //(char*) es el cast, le dice que el número lo trate como lugar de memoria, ej= va a leer 0x1024. Eso es un lugar de memoria, no un entero
Escribir_FraseFlash_LCD(Puntero_Estado);

		Escribir_Comando_LCD(0xDF); //posición en pantalla.
		Escribir_Caracter_LCD(Left_Arrow);

}



PROGMEM const char Sensores[] = "Sensores";
PROGMEM const char Temperatura[] = "Temperatura";
void Menu_Avisos (void){
	
	Escribir_Comando_LCD(0x85); //posición en pantalla.
	Escribir_FraseFlash_LCD(Avisos);
	
	Escribir_Comando_LCD(Linea2);
	Escribir_FraseFlash_LCD(Sensores);
	
	Escribir_Comando_LCD(Linea3);
	Escribir_FraseFlash_LCD(Temperatura);
	
	Escribir_Comando_LCD(0xDF); //posición en pantalla.
	Escribir_Caracter_LCD(Left_Arrow);
}


PROGMEM const char TT1[] = "TT1: ";
PROGMEM const char TT2[] = "TT2: ";
PROGMEM const char TT3[] = "TT3: ";
PROGMEM const char TT4[] = "TT4: ";
PROGMEM const char TT5[] = "TT5: ";
PROGMEM const char TT6[] = "TT6: ";
PROGMEM const char TT7[] = "TT7: ";
PROGMEM const char TT8[] = "TT8: ";
void Menu_Avisos_Sensores (void){
	
	Escribir_Comando_LCD(LCD_CLEAR);
	Escribir_FraseFlash_LCD(W);
	Escribir_FraseFlash_LCD(TT1);
	Escribir_Comando_LCD(0x89); //posición en pantalla.
	Escribir_FraseFlash_LCD(W);
	Escribir_FraseFlash_LCD(TT5);
	
	Escribir_Comando_LCD(Linea2);
	Escribir_FraseFlash_LCD(W);
	Escribir_FraseFlash_LCD(TT2);
	Escribir_Comando_LCD(0xC9); //posición en pantalla.
	Escribir_FraseFlash_LCD(W);
	Escribir_FraseFlash_LCD(TT6);
	
	Escribir_Comando_LCD(Linea3);
	Escribir_FraseFlash_LCD(W);
	Escribir_FraseFlash_LCD(TT3);
	Escribir_Comando_LCD(0x99); //posición en pantalla.
	Escribir_FraseFlash_LCD(W);
	Escribir_FraseFlash_LCD(TT7);
	
	Escribir_Comando_LCD(Linea4);
	Escribir_FraseFlash_LCD(W);
	Escribir_FraseFlash_LCD(TT4);
	Escribir_Comando_LCD(0xD9); //posición en pantalla.
	Escribir_FraseFlash_LCD(W);
	Escribir_FraseFlash_LCD(TT7);

	
	Escribir_Comando_LCD(0xDF); //posición en pantalla.
	Escribir_Caracter_LCD(Left_Arrow);
}



PROGMEM const char Temp[] = " Temp";
PROGMEM const char Tzona1[] = "T  Zona 1";
PROGMEM const char Tzona2[] = "T  Zona 2";
PROGMEM const char Tzona3[] = "T  Zona 3";
void Menu_Avisos_Temperatura (void){
	
	Escribir_Comando_LCD(LCD_CLEAR);
	Escribir_FraseFlash_LCD(Avisos);
	Escribir_FraseFlash_LCD(Temp);
	Escribir_Caracter_LCD(gradito);
	
	Escribir_Comando_LCD(Linea2);
	Escribir_FraseFlash_LCD(Tzona1);
		Escribir_Comando_LCD(0xC1);
		Escribir_Caracter_LCD(gradito);
	
	Escribir_Comando_LCD(Linea3);
	Escribir_FraseFlash_LCD(Tzona2);
		Escribir_Comando_LCD(0x91);
		Escribir_Caracter_LCD(gradito);
	
	Escribir_Comando_LCD(Linea4);
	Escribir_FraseFlash_LCD(Tzona3);		
		Escribir_Comando_LCD(0xD1);
		Escribir_Caracter_LCD(gradito);
		
	Escribir_Comando_LCD(0xDF); //posición en pantalla.
	Escribir_Caracter_LCD(Left_Arrow);		
		
}


PROGMEM const char PID[] = " PID ";
void Menu_Alarmas (void){
	
	Escribir_Comando_LCD(LCD_CLEAR);
	Escribir_Comando_LCD(0x85); //posición en pantalla.
	Escribir_FraseFlash_LCD(Alarmas);
	
	Escribir_Comando_LCD(Linea2);
	Escribir_FraseFlash_LCD(Sensores);
	
	Escribir_Comando_LCD(Linea3);
	Escribir_FraseFlash_LCD(Temperatura);
	
	Escribir_Comando_LCD(Linea4);
	Escribir_FraseFlash_LCD(PID);
	
	Escribir_Comando_LCD(0xDF); //posición en pantalla.
	Escribir_Caracter_LCD(Left_Arrow);
}


void Menu_Alarmas_Sensores(void){
	
	
	
}


void Menu_Alarmas_Temperatura (void){
	
	Escribir_Comando_LCD(LCD_CLEAR);
	Escribir_FraseFlash_LCD(Alarmas);
	Escribir_FraseFlash_LCD(Temp);
	Escribir_Caracter_LCD(gradito);
	
	Escribir_Comando_LCD(Linea2);
	Escribir_FraseFlash_LCD(Tzona1);
	Escribir_Comando_LCD(0xC1);
	Escribir_Caracter_LCD(gradito);
	
	Escribir_Comando_LCD(Linea3);
	Escribir_FraseFlash_LCD(Tzona2);
	Escribir_Comando_LCD(0x91);
	Escribir_Caracter_LCD(gradito);
	
	Escribir_Comando_LCD(Linea4);
	Escribir_FraseFlash_LCD(Tzona3);
	Escribir_Comando_LCD(0xD1);
	Escribir_Caracter_LCD(gradito);
	
	Escribir_Comando_LCD(0xDF); //posición en pantalla.
	Escribir_Caracter_LCD(Left_Arrow);
	
}

/*

PROGMEM const char PIDH1[] = "PID H1";
PROGMEM const char PIDH2[] = "PID H2";
PROGMEM const char PIDB1[] = "PID B1";
PROGMEM const char PIDB2[] = "PID B2";
PROGMEM const char PIDB3[] = "PID B3";
PROGMEM const char PIDC3[] = "PID C3";

PROGMEM const char H1[] = " H1";
PROGMEM const char H2[] = " H2";
PROGMEM const char B1[] = " B1";
PROGMEM const char B2[] = " B2";
PROGMEM const char B3[] = " B3";
PROGMEM const char C3[] = " C3";
void Menu_Alarmas_PIDS(void){
	
	Escribir_Comando_LCD(LCD_CLEAR);
	Escribir_FraseFlash_LCD(Alarmas);
	Escribir_FraseFlash_LCD(PID);
	
	Escribir_Comando_LCD(Linea2);
	Escribir_FraseFlash_LCD(PIDH1);
	Escribir_Comando_LCD(0xC8);
	Escribir_FraseFlash_LCD(PIDH2);
	
	Escribir_Comando_LCD(Linea3);
	Escribir_FraseFlash_LCD(PIDB1);
	Escribir_Comando_LCD(0x98);
	Escribir_FraseFlash_LCD(PIDB2);
	
	Escribir_Comando_LCD(Linea4);
	Escribir_FraseFlash_LCD(PIDB3);
	Escribir_Comando_LCD(0xD8);
	Escribir_FraseFlash_LCD(PIDC3);
	
}
*/

PROGMEM const char Temp2[] = "Temp";
PROGMEM const char caudal_vol[]	= "CV";
PROGMEM const char C[] = "C";
PROGMEM const char unidad_caudal[] = " m3/h";
void Menu_SetPoints_Z1(uint8_t zona, uint16_t varTemp, uint16_t varCV){
	
	char arrayTemp[4];
	char arrayCV[4];
	sprintf(arrayTemp,": \n%u", varTemp);
	sprintf(arrayCV,": \n%u", varCV);
	
	Escribir_Comando_LCD(LCD_CLEAR);	
	Escribir_Comando_LCD(0X81);
	Escribir_FraseFlash_LCD(SetPoints);
	
	switch (zona){
		case 1: Escribir_FraseFlash_LCD(Z1);
		break;
		case 2:	Escribir_FraseFlash_LCD(Z2);
		Escribir_Comando_LCD(0xDF); //posición en pantalla.
		Escribir_Caracter_LCD(Right_Arrow);
	}
	
	Escribir_Comando_LCD(Linea2);
	Escribir_FraseFlash_LCD(Temp2);
	Escribir_Texto_LCD(arrayTemp);
//	Escribir_Comando_LCD(LCD_Cursor_R);
	Escribir_Caracter_LCD(gradito);
	Escribir_FraseFlash_LCD(C);
	
	Escribir_Comando_LCD(Linea3);
	Escribir_FraseFlash_LCD(caudal_vol);
	Escribir_Texto_LCD(arrayCV);
	Escribir_FraseFlash_LCD(unidad_caudal);
	
	Escribir_Comando_LCD(0xDE); //posición en pantalla.
	Escribir_Caracter_LCD(Left_Arrow);


}


PROGMEM const char Velocidad[] = "Vel.C: ";
PROGMEM const char unidad_Velocidad[] = " cm/s";
void Menu_SetPoints_Z3(uint16_t varCV, uint16_t varVel){
	
	char arrayVel[4];
	char arrayCV[4];
	sprintf(arrayVel,"%u", varVel);
	sprintf(arrayCV,"%u", varCV);
	
	Escribir_Comando_LCD(LCD_CLEAR);
	Escribir_Comando_LCD(0X81);
	Escribir_FraseFlash_LCD(SetPoints);
	Escribir_FraseFlash_LCD(Z3);
	
	Escribir_Comando_LCD(Linea2);
	Escribir_FraseFlash_LCD(caudal_vol);
	Escribir_Texto_LCD(arrayCV);
	Escribir_FraseFlash_LCD(unidad_caudal);
	
	Escribir_Comando_LCD(Linea3);
	Escribir_FraseFlash_LCD(Velocidad);
	Escribir_Texto_LCD(arrayVel);
	Escribir_FraseFlash_LCD(unidad_Velocidad);

	Escribir_Comando_LCD(0xDE); //posición en pantalla.
	Escribir_Caracter_LCD(Left_Arrow);
	
}


PROGMEM const char PreCal[] = "Pre.Cal ";
PROGMEM const char Cal[] = "Cal.";
PROGMEM const char Enf[] = "Enf.";
PROGMEM const char Seg[] = " s";
void Menu_Tiempos (uint16_t varPreCal, uint16_t varCal, uint16_t varEnf){
	
	char arrayPreCal[4];
	char arrayCal[4];
	char arrayEnf[4];
		
	sprintf(arrayPreCal,"%u", varPreCal);
	sprintf(arrayCal,"%u", varCal);
	sprintf(arrayEnf,"%u", varEnf);
	
	Escribir_Comando_LCD(LCD_CLEAR);
	Escribir_Comando_LCD(0X84);
	Escribir_FraseFlash_LCD(Tiempos);
		
	Escribir_Comando_LCD(Linea2);
	Escribir_FraseFlash_LCD(PreCal);
	Escribir_Texto_LCD(arrayPreCal);
	Escribir_FraseFlash_LCD(Seg);
	
	Escribir_Comando_LCD(Linea3);
	Escribir_FraseFlash_LCD(Cal);
	Escribir_Texto_LCD(arrayCal);
	Escribir_FraseFlash_LCD(Seg);
	
	Escribir_Comando_LCD(Linea4);
	Escribir_FraseFlash_LCD(Enf);
	Escribir_Texto_LCD(arrayEnf);
	Escribir_FraseFlash_LCD(Seg);
	
	Escribir_Comando_LCD(0xDF); //posición en pantalla.
	Escribir_Caracter_LCD(Left_Arrow);
}



void Menu_RangSensores (uint8_t menu, uint16_t varTTa, uint16_t varTTb, uint16_t varTTc){
	
		char arrayTTa[4];
		char arrayTTb[4];
		char arrayTTc[4];
		sprintf(arrayTTa,"%u", varTTa);
		sprintf(arrayTTb,"%u", varTTb);
		sprintf(arrayTTc,"%u", varTTc);

		Escribir_Comando_LCD(LCD_CLEAR);
		Escribir_Comando_LCD(0X81);
		Escribir_FraseFlash_LCD(R_Sensores);
	
	switch (menu){
		case 1:
			Escribir_Comando_LCD(Linea2);
			Escribir_FraseFlash_LCD(TT1);
			Escribir_Texto_LCD(arrayTTa);
			Escribir_Caracter_LCD(gradito);
			Escribir_FraseFlash_LCD(C);
	
			Escribir_Comando_LCD(Linea3);
			Escribir_FraseFlash_LCD(TT2);
			Escribir_Texto_LCD(arrayTTb);
			Escribir_Caracter_LCD(gradito);
			Escribir_FraseFlash_LCD(C);		
		
			Escribir_Comando_LCD(Linea4);
			Escribir_FraseFlash_LCD(TT3);
			Escribir_Texto_LCD(arrayTTc);
			Escribir_Caracter_LCD(gradito);
			Escribir_FraseFlash_LCD(C);		
			
			Escribir_Comando_LCD(0xDF); //posición en pantalla.
			Escribir_Caracter_LCD(Right_Arrow);
		break;
		case 2:
			Escribir_Comando_LCD(Linea2);
			Escribir_FraseFlash_LCD(TT4);
			Escribir_Texto_LCD(arrayTTa);
			Escribir_Caracter_LCD(gradito);
			Escribir_FraseFlash_LCD(C);
		
			Escribir_Comando_LCD(Linea3);
			Escribir_FraseFlash_LCD(TT5);
			Escribir_Texto_LCD(arrayTTb);
			Escribir_Caracter_LCD(gradito);
			Escribir_FraseFlash_LCD(C);
		
			Escribir_Comando_LCD(Linea4);
			Escribir_FraseFlash_LCD(TT6);
			Escribir_Texto_LCD(arrayTTc);
			Escribir_Caracter_LCD(gradito);
			Escribir_FraseFlash_LCD(C);

			Escribir_Comando_LCD(0xDE); //posición en pantalla.
			Escribir_Caracter_LCD(Left_Arrow);	
		
			Escribir_Comando_LCD(0xDF); //posición en pantalla.
			Escribir_Caracter_LCD(Right_Arrow);
		break;	
		case 3:
			Escribir_Comando_LCD(Linea2);
			Escribir_FraseFlash_LCD(TT7);
			Escribir_Texto_LCD(arrayTTa);
			Escribir_Caracter_LCD(gradito);
			Escribir_FraseFlash_LCD(C);
		
			Escribir_Comando_LCD(Linea3);
			Escribir_FraseFlash_LCD(TT8);
			Escribir_Texto_LCD(arrayTTb);
			Escribir_Caracter_LCD(gradito);
			Escribir_FraseFlash_LCD(C);

			Escribir_Comando_LCD(0xDE); //posición en pantalla.
			Escribir_Caracter_LCD(Left_Arrow);
	}//switch

	
	
	
	
	
}


/*
void Menu_RangActuadores (uint8_t menu, uint16_t varAct1, uint16_t varAct2){
	
		char arrayAct1[4];
		char arrayAct2[4];
		sprintf(arrayAct1,"%u", varAct1);
		sprintf(arrayAct2,"%u", varAct2);
		
		Escribir_Comando_LCD(LCD_CLEAR);
		Escribir_FraseFlash_LCD(R_Actuadores);
		
			
		switch (menu){
			case 1:
				Escribir_Comando_LCD(Linea2);
				Escribir_FraseFlash_LCD(H1);
				Escribir_Texto_LCD(arrayTemp);
				Escribir_Comando_LCD(LCD_Cursor_R);
				Escribir_Caracter_LCD(gradito);
				Escribir_FraseFlash_LCD(C);
			
			
			
			break;
			case 2:	Escribir_FraseFlash_LCD(Z2);
			Escribir_Comando_LCD(0xDF); //posición en pantalla.
			Escribir_Caracter_LCD(Right_Arrow);
		}
		
		Escribir_Comando_LCD(Linea2);
		Escribir_FraseFlash_LCD(Temp2);
		Escribir_Texto_LCD(arrayTemp);
		Escribir_Comando_LCD(LCD_Cursor_R);
		Escribir_Caracter_LCD(gradito);
		Escribir_FraseFlash_LCD(C);
		
		Escribir_Comando_LCD(Linea3);
		Escribir_FraseFlash_LCD(caudal_vol);
		Escribir_Texto_LCD(arrayCV);
		Escribir_FraseFlash_LCD(unidad_caudal);
		
		Escribir_Comando_LCD(0xDE); //posición en pantalla.
		Escribir_Caracter_LCD(Left_Arrow);
	
	
	
	
	
}



*/