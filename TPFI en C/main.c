/*
 * TPFI en C.c
 *
 * Created: 5/3/2026 00:37:49
 * Author : 
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include <stdlib.h>			//para el abs
//#include <string.h>


/*--------variables menu lcd---------*/

struct pantallaPPL {
		uint8_t dni;
		void(*graficos)(void);
		char hijos [4];				//opciones
		char hermanos [3];
		uint8_t opcionMax;
		uint8_t opcionMin;
		uint8_t hermanosMax;			
	};

char maq_estado_pantalla = 100;
uint8_t Menu = 0;
uint8_t Cursor = 0;
uint8_t Enter = 0;
uint8_t Exit = 0;
uint8_t reconocimiento = 0;

uint8_t Actualizar_Menu = 0;
uint8_t no_repetir = 1;
uint8_t cambio_menu = 0;

char renglon[4] = {0x80, 0xC0, 0x90, 0xD0};
PROGMEM const char espacio[] = " ";

char renglon_WarningSensores [8] = {0x85, 0xC5, 0x95, 0xD5, 0x8D, 0xCD, 0x9D, 0xDD};
uint8_t reconocidos_Sensores [8];
char renglon_AlarmPID [] = {0xC6, 0x96, 0xD6, 0xCE, 0x9E, 0xDE};
uint8_t reconocidos_PIDS [6];
char renglon_WarAlaTemps [8] = {0xCA, 0x9A, 0xDA};
uint8_t reconocidos_AvisosTemp [3];
uint8_t reconocidos_AlarmasTemp [3];

uint8_t Estado_Planta = 1;
PROGMEM const char Apagado[] = "Apagado     ";
PROGMEM const char Arranque[] = "Arranque    ";
PROGMEM const char Activo[] = "Activo      ";
PROGMEM const char PControlada[] = "P.Controlada";
PROGMEM const char PEmergencia[] = "P.Emergencia";
PROGMEM const char * const Estado[] = {Apagado, Arranque, Activo, PControlada, PEmergencia};
char arrayProm[5];
/*-----------------------------------------*/

/*--------variables para el Keypad---------*/
volatile uint16_t habilitar_conversion = 0;
volatile uint16_t contador = 0;
volatile uint16_t valor_adc;
volatile uint8_t  Habilitar_Teclado;
		 uint8_t Keypad_Letra;
/*-----------------------------------------*/

/*-------variables para el ads1115---------*/
volatile uint16_t temperatura;
volatile uint8_t Habilitar_LeerTemperatura = 0;
volatile uint8_t Sensor_Temperatura = 0;
volatile uint16_t Vector_Temperaturas [8];
volatile uint8_t Canal_Temp = 0;
/*-----------------------------------------*/

#include "ADS1115/ADS1115.h"
#include "I2C/I2C.h"
#include "MAX5822/MAX5822.h"
#include "PCF8574/PCF8574.h"
#include "LCD/LCD.h"
#include "Keypad/Keypad.h"

#ifndef Variables
#define Variables

void UART_enviar_char(char c);
void enviar_frase(const char *frase);
void UART_enviar_string(char *str);
void Cursor_Fil_Col(uint8_t fila, uint8_t columna);

void Pantalla_6_Uart(void);
void FinFLechas_P0  (void);
void adecuacion_sensores(void);
void comparaciones(uint16_t SP_Temp, uint16_t Prom, uint16_t Aviso, uint16_t Alarma, uint8_t Zona);
void alarmas_avisos_T(void);
void alarmas_avisos_Sens(void);
void PIDs(uint8_t Actuador);
void Chequear_PIDs_Fallas(void);
void ParadaEmergencia(void);
void ParadaControlada(void);
void SecuenciaArranque(void);
void Signals_Habilitacion(void);

void guardar_eeprom(uint8_t direccion,uint16_t valor,uint8_t bytes);
void leer_eeprom(void);
void EEPROM_write(uint16_t address, uint8_t data);
uint8_t EEPROM_read(uint16_t address);
void EEPROM_write_uint16(uint16_t address, uint16_t data);
uint16_t EEPROM_read_uint16(uint16_t address);

#define Temp_deseada_Z1 (0)
#define Temp_deseada_Z2 (2)
#define Caudal_vol_deseado_Z1 (4)
#define Caudal_vol_deseado_Z2 (5)
#define Caudal_vol_deseado_Z3 (6)
#define Vel_cinta_transp (8)

#define Rango_TT1 (10)
#define Rango_TT2 (12)
#define Rango_TT3 (14)
#define Rango_TT4 (16)
#define Rango_TT5 (18)
#define Rango_TT6 (20)
#define Rango_TT7 (22)
#define Rango_TT8 (24)

#define Rango_C1 (42)
#define Rango_H1 (44)
#define Rango_H2 (46)
#define Rango_B1 (48)
#define Rango_B2 (50)
#define Rango_B3 (52)

#define Aviso_temp_z1 (70)
#define Aviso_temp_z2 (71)
#define Aviso_temp_z3 (72)
#define Alarma_temp_z1 (74)
#define Alarma_temp_z2 (75)
#define Alarma_temp_z3 (76)

#define Tiempo_de_precalentamiento (78)
#define Tiempo_de_calentamiento (80)
#define Tiempo_de_enfriamiento (82)

PROGMEM const char F_arrowLeft    	[]="\x1b[30;42m >> ";
PROGMEM const char F_arrowRigth		[]=" << \033[0m";

PROGMEM const char vacio_3			[]="          ";
PROGMEM const char vacio_2       	[]=" ";
PROGMEM const char Borrar_Pantalla	[]="\033[2J\033[H";
PROGMEM const char ocultar_cursor	[]="\033[?25l";
PROGMEM const char Dato_Recibido	[]="Valor Ingresado:----";
PROGMEM const char F_Reconocer		[]="Reconocer";

PROGMEM const char F_Unid_Temp		[]=" C";
PROGMEM const char F_Unid_caudal	[]=" m3/h";
PROGMEM const char F_Unid_cinta		[]=" cm/s";
PROGMEM const char F_Unid_Time		[]=" s";
PROGMEM const char F_Unid_Porc      []=" %";

PROGMEM const char F_spTemp_Z1		[]="SP H1:  ";
PROGMEM const char F_spCV_Z1		[]="SP B1:  ";
PROGMEM const char F_spTemp_Z2		[]="SP H2:  ";
PROGMEM const char F_spCV_Z2		[]="SP B2:  ";
PROGMEM const char F_spCV_Z3		[]="SP B3:  ";
PROGMEM const char F_VelCinta		[]="SP C1:  ";

PROGMEM const char F_H1				[]="R H1:   ";
PROGMEM const char F_B1				[]="R B1:   ";
PROGMEM const char F_H2				[]="R H2:   ";
PROGMEM const char F_B2				[]="R B2:   ";
PROGMEM const char F_B3				[]="R B3:   ";
PROGMEM const char F_C1				[]="R C1:   ";

PROGMEM const char F_RTT1			[]="R TT1:  ";
PROGMEM const char F_RTT2			[]="R TT2:  ";
PROGMEM const char F_RTT3			[]="R TT3:  ";
PROGMEM const char F_RTT4			[]="R TT4:  ";
PROGMEM const char F_RTT5			[]="R TT5:  ";
PROGMEM const char F_RTT6			[]="R TT6:  ";
PROGMEM const char F_RTT7			[]="R TT7:  ";
PROGMEM const char F_RTT8			[]="R TT8:  ";

PROGMEM const char F_TT1			[]="FD TT1: ";
PROGMEM const char F_TT2			[]="FD TT2: ";
PROGMEM const char F_TT3			[]="FD TT3: ";
PROGMEM const char F_TT4			[]="FD TT4: ";
PROGMEM const char F_TT5			[]="FD TT5: ";
PROGMEM const char F_TT6			[]="FD TT6: ";
PROGMEM const char F_TT7			[]="FD TT7: ";
PROGMEM const char F_TT8			[]="FD TT8: ";

PROGMEM const char F_TimePrecal		[]="Ts Precal:";
PROGMEM const char F_TimeCal		[]="Ts Cal:   ";
PROGMEM const char F_TimeEnf		[]="Ts Enf:   ";

PROGMEM const char F_A		    	[]="Alarma: ";
PROGMEM const char F_W   		    []="Aviso:  ";

PROGMEM const char F_Titulo_P6_2	[]="MONITOREO";
PROGMEM const char F_PromZ1			[]="ZONA 1";
PROGMEM const char F_PromZ2			[]="ZONA 2";
PROGMEM const char F_PromZ3			[]="ZONA 3";
PROGMEM const char F_EstadoHorno	[]="Estado Horno: ";
PROGMEM const char F_Estado	        []="Estado: ";
PROGMEM const char F_Prom			[]="T Prom: ";

PROGMEM const char F_ZW				[]="Aviso T";
PROGMEM const char F_ZA				[]="Alarma T";

PROGMEM const char F_Normal			[]="Normal";
PROGMEM const char F_Emergencia		[]="Parada Emergencia";
PROGMEM const char F_Controlada		[]="Parada Controlada";
PROGMEM const char F_Arranque		[]="Secuencia Arranque";
PROGMEM const char F_Regimen		[]="Regimen";
PROGMEM const char F_Apagado		[]="Apagado";

PROGMEM const char F_Fase     		[]="Fase: ";
PROGMEM const char F_Tiempo		    []="Tiempo: ";

PROGMEM const char F_moniFalla		[]="Falla ";
PROGMEM const char F_monSens		[]=" Sens";

PROGMEM const char F_PID1		    []="PID H1";
PROGMEM const char F_PID2		    []="PID B1";
PROGMEM const char F_PID3		    []="PID H2";
PROGMEM const char F_PID4		    []="PID B2";
PROGMEM const char F_PID5		    []="PID B3";
PROGMEM const char F_PID6		    []="PID C3";

uint8_t Maquina_Estado = 26;
volatile uint8_t dato;
volatile char Dato_RX[5];
volatile uint8_t c = 0;
volatile uint8_t Flecha_LOW = 0;
volatile uint8_t Flecha_UP = 0;
volatile uint8_t Flecha_Right = 0;
volatile uint8_t Flecha_Left = 0;
char Numero[5];

uint8_t Flag_Norepetir = 0;
uint8_t C_Enter = 0;

uint8_t Cont_Enter = 0;
volatile uint8_t Ingresar_Valor = 0;
uint16_t valor = 65535;
uint16_t address = 65535;
uint16_t data = 65535;

uint8_t Flecha_P6 = 0;

uint16_t R_H1;
uint16_t R_H2;
uint16_t R_B3;
uint8_t R_B1;
uint8_t R_B2;
uint8_t R_C1;

uint16_t R_TT1;
uint16_t R_TT2;
uint16_t R_TT3;
uint16_t R_TT4;
uint16_t R_TT5;
uint16_t R_TT6;
uint16_t R_TT7;
uint16_t R_TT8;

uint16_t Tiempo_PreCalentamiento = 2;
uint16_t Tiempo_Enfriamiento = 2;
uint16_t Tiempo_Calentamiento = 2;

uint16_t SP_TempZ1 = 0;
uint16_t SP_TempZ2 = 0;
uint16_t SP_CaudalZ3 = 500;
uint8_t SP_CaudalZ1 = 250;
uint8_t SP_CaudalZ2 = 250;
uint8_t SP_VelocidadZ3 = 100;

uint8_t Aviso_TZ1 = 10;
uint8_t Aviso_TZ2 = 10;
uint16_t Aviso_TZ3 = 1;

uint8_t Alarma_TZ1 = 20;
uint8_t Alarma_TZ2 = 20;
uint16_t Alarma_TZ3 = 2;

volatile uint16_t Vec_SsT[8];
uint16_t Vec_Sens2[8];
uint16_t Vec_TT[8];

uint16_t  PromedioZona_1 = 0;
uint16_t  PromedioZona_2 = 0;
uint16_t  PromedioZona_3 = 0;

uint8_t	Led_Rojo = 0;
uint8_t	Contador_Alarma = 0;
uint8_t	Contador_Aviso = 0;
uint8_t	Led_Verde = 0;
uint8_t	Estado_Actual[3];
uint8_t	led_Ambar = 0;


uint16_t SP_Temp;
uint16_t prom;
uint16_t aviso;
uint16_t alarma;
uint8_t Zona = 0;
volatile uint8_t Flag_adecuacion_sensores;
volatile uint8_t tiempo = 0;
volatile uint8_t Enter_2 = 0;

uint16_t DAC_1A;
uint16_t DAC_1B;
uint16_t DAC_2A;
uint16_t DAC_2B;
uint16_t DAC_3A;
uint16_t DAC_3B;
uint8_t Actuador = 0;

uint8_t Hab_ANterior;
uint8_t Hab_POSterior;
uint16_t Cont_Tiempo;
uint8_t Fase = 0;
uint8_t Flag_Parada_Controlada;
uint8_t Flag_Secuencia_Arranque;
uint8_t Flag_Parada_Emergencia = 0;
uint8_t Botton_Emergencia = 0; 

uint8_t estado_anterior_B;
uint8_t estado_anterior;
uint8_t estado_anterior_D;
uint8_t estado_actual_D;
volatile uint8_t estado_anterior_PC2 = 1;
volatile uint8_t Vec_PIDs[6] = {0};
uint8_t Estado_Anterior_WA[3];
uint8_t Estado_Actual_WA[3];
uint8_t Cable_Cortado[8];
uint8_t Estado_Anterior_CC[8];
uint8_t Estado_Actual_CC[8];
uint8_t Estado_Actual_ultimo[3];

int min;
int max;

uint8_t Flag_Norepetir_SA = 0;
uint8_t Fin_Arranque = 0;
uint8_t Estado_Horno = 0;
volatile uint8_t Contador_Temp = 0;

volatile uint8_t ads1115_address;
volatile uint8_t ads1155_pointer_register;
volatile uint8_t ads1155_MSB_Config;
volatile uint8_t ads1155_LSB_Config;

#endif
	
int main(void){
	
	maq_estado_pantalla = 81;

					
	#ifndef Configuracion
	#define Configuracion
	
				
									  //dni,	graficos,				hijos,			hermanos,		opcionMax,  opcionMin,  hermanosMax
	struct pantallaPPL principal1 =		{100, &Pantalla_Principal_1, {90, 80, 70, 60},	{100, 101},		 3,			 0,			 1};
	struct pantallaPPL principal2 =		{101, &Pantalla_Principal_2, {50, 40, 30},		{100, 101},		 2,			 0,			 1};
	
	struct pantallaPPL avisos =			{80,  &Menu_Avisos,			 {0, 81, 82},		{0},			 2,			 1,			 0};
	struct pantallaPPL alarmas =		{70,  &Menu_Alarmas,		 {0, 71, 72},		{0},		 	 2,			 1,			 0};
	
	struct pantallaPPL setPoint =		{60,  NULL,					 {0},				{60, 61, 62},	 0,			 0,			 2};
	struct pantallaPPL R_Sensores =		{40,  NULL,					 {0},				{40, 41, 42},	 0,			 0,			 2};
	struct pantallaPPL R_Actuadores =	{40,  NULL,					 {0},				{30, 31, 32},	 0,			 0,			 2};   
   
   
   
 /*------------------PCInt PD5----------------------*/
 DDRD &= ~(1 << DDD5);     // PD5 como entrada
 PORTD |= (1 << PORTD5);   // Pull-up habilitado
 PCICR |= (1 << PCIE2);    // Habilita grupo PCINT[23:16]
 PCMSK2 |= (1 << PCINT21);
 
 DDRD &= ~(1 << PD4);	// PD4 como entrada
 PORTD |= (1 << PD4);	// Activar pull-up interno
 PCMSK2 |= (1 << PCINT20);	// Habilitar PD4 (PCINT20)
 estado_anterior_D = PIND;	// Guardar estado inicial
 
 /*-------------------------------------------------*/
 
 
 /*-------------------------- I2C -----------------------------*/
 DDRC &= ~((1<<PC4) | (1<<PC5));   //como entradas para el i2c
 TWSR &= ~((1<<TWPS0) | (1<<TWPS1));// Prescaler = 1 (TWPS = 0)
 TWBR = 72;// Bit rate para 100 kHz
 TWCR = (1<<TWEN);// Habilitar TWI
 /*-------------------------------------------------------------*/

 /*------------- Timer1 --------------*/
 TCCR1A = 0;// Modo CTC
 TCCR1B = (1 << WGM12);
 OCR1A = 15624;// Valor de comparación para 1 segundo
 TIMSK1 = (1 << OCIE1A);// Habilitar interrupción por compare match A
 TCCR1B |= (1 << CS12) | (1 << CS10);// Prescaler 1024
 /*-----------------------------------*/

 /*------------- INT0 --------------*/
DDRD &= ~(1 << PD2);        
PORTD |= (1 << PD2);        

EICRA &= ~(1 << ISC01);     
EICRA |= (1 << ISC00);     

EIMSK |= (1 << INT0);      		
 /*---------------------------------*/


 /*------------- INT1 --------------*/
 DDRD &= ~(1<<DDD3);				// PD3 como entrada
 EICRA |= (1<<ISC11) | (1<<ISC10);	 // Flanco ascendente
 EIMSK |= (1<<INT1);				// Habilitar INT1
 /*---------------------------------*/


 /*-------------------------- Usart ----------------------------*/
 uint16_t ubrr = 8; // 115200 modo normal
    
 UBRR0H = (ubrr >> 8);// Baudrate
 UBRR0L = ubrr;
 UCSR0A = 0; // Modo normal
   
 UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0); // Habilitar TX, RX y la INTERRUPCIÓN DE RECEPCIÓN
 UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);  // 8N1
 /*-------------------------------------------------------------*/

 /*---------------------------- ADC ------------------------------*/
 ADMUX = (1<<REFS0);  // AVcc como referencia, canal ADC0 (MUX=0000)
 ADMUX = (ADMUX & 0xF0) | 0x00;  // Seleccionar canal 0
 ADCSRA = (1<<ADEN)  |  // habilita ADC
		  (1<<ADIE)  |  // habilita interrupción ADC
          (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0); // prescaler 128
 ADCSRB = 0x00;
 DIDR0 = 0x00;

 DDRC &= ~(1 << PC0);   // PC0 como entrada
 PORTC &= ~(1 << PC0);
 /*---------------------------------------------------------------*/

 /*-------------------- CONFIGURACION PINES ----------------------*/
 // PINES señales ANTERIOR Y POSTERIOR
 DDRC |= (1 << PC1) | (1 << PC3);// PC1 y PC3 como salida
 
 DDRC &= ~(1 << PC2);		// PC2 como entrada
 PORTC |= (1 << PC2);		// Activar pull-up interno
 PCICR |= (1 << PCIE1);		// Habilitar interrupciones por cambio en PCINT[14:8] (PORTC)
 PCMSK1 |= (1 << PCINT10);	// Habilitar PCINT10 (que corresponde a PC2)
 
 // PINES PIDs
 DDRB &= ~0x3F;				// PB0–PB5 como entrada
 PORTB |= 0x3F;				// Activar pull-up internos
 PCICR |= (1 << PCIE0);		// Habilitar interrupción por cambio en PORTB
 PCMSK0 |= 0x3F;			// Habilitar PB0–PB5
 estado_anterior_B = PINB;	// Guardar estado inicial
 
 // Patitas led RGB
 DDRD |= (1 << PD6) | (1 << PD7);
 /*------------------ FIN CONFIGURACION PINES --------------------*/

	sei();
	
/*----------------------Configuración "Conversion Ready Pin" ADS1115-------------------*/
//low thres - hight thres
cofigurar_ads1115(ads1115_IP_VCC_write, ads1115_LoThresh_Reg, 0x00, 0x00);
cofigurar_ads1115(ads1115_IP_VCC_write, ads1115_HiThresh_Reg, 0x80, 0x00);
cofigurar_ads1115(ads1115_IP_GND_write, ads1115_LoThresh_Reg, 0x00, 0x00);
cofigurar_ads1115(ads1115_IP_GND_write, ads1115_HiThresh_Reg, 0x80, 0x00);
/*-------------------------------------------------------------------------------------*/



/*------------------------Secuencia de Power-up device.-------------------------------*/
Escribir_MAX5822 (DAC1, 0b11110000, 0b000000001100);
Escribir_MAX5822 (DAC2, 0b11110000, 0b000000001100);
Escribir_MAX5822 (DAC3, 0b11110000, 0b000000001100);
/*------------------------------------------------------------------------------------*/

	Iniciar_LCD();
	#endif
	
	leer_eeprom();
	Chequear_PIDs_Fallas();
	enviar_frase(ocultar_cursor);
    while (1) {
	
	if(Flag_Secuencia_Arranque == 1){SecuenciaArranque();}

	if(Flag_Parada_Controlada == 1){ParadaControlada();}
	
	if(Flag_Parada_Emergencia == 1){ParadaEmergencia();}
	
	Pantalla_6_Uart();
	
	Signals_Habilitacion();
	
/*-------------------------------------------Teclado------------------------------------------*/	
	if(Habilitar_Teclado == 1){
		Habilitar_Teclado =0;		
		Convertir_Keypad (valor_adc, &Menu, &Cursor, &Enter, &Exit, &cambio_menu, &reconocimiento);
		Actualizar_Menu = 0;		
	}//teclado
/*---------------------------------------------------------------------------------------------*/		

if (Actualizar_Menu == 0){
	Actualizar_Menu =1;
	
/*-----------------------principal 1----------------------------------*/
	if (maq_estado_pantalla == 100){

		if (no_repetir)	{
			no_repetir =0;
		principal1.graficos();
		}
		if(Enter == 1){
			Enter =0;
			no_repetir =1;
			maq_estado_pantalla = principal1.hijos[Cursor];
			Cursor = 0;
			Menu = 0;	
		}
		
		if (Cursor == 255) Cursor = principal1.opcionMin;	
		if (Cursor > (principal1.opcionMax)) Cursor = principal1.opcionMax;
		
		for(uint8_t i = 0; i <= principal1.opcionMax; i++){
			Escribir_Comando_LCD(renglon[i]);
			Escribir_FraseFlash_LCD(espacio);
		}

		Escribir_Comando_LCD(renglon[Cursor]);
		Escribir_Caracter_LCD(Right_Arrow);
		
		if (Menu == 255) Menu = 0;
		if (Menu > (principal1.hermanosMax)) Menu = principal1.hermanosMax;	
		if (cambio_menu){
			cambio_menu =0;
			no_repetir =1;
			maq_estado_pantalla = principal1.hermanos[Menu];
		}
	}//maq100
/*--------------------------------------------------------------------*/	
/*------------------------principal 2---------------------------------*/
	if(maq_estado_pantalla == 101){
	
		if (no_repetir)	{
			no_repetir =0;
		principal2.graficos();
		}
		if(Enter == 1){
			Enter =0;
			no_repetir =1;
			maq_estado_pantalla = principal2.hijos[Cursor];
			Menu = 0;
		}
		if (Exit == 1){
			Exit = 0;
			no_repetir =1;
			maq_estado_pantalla = principal1.dni;
		}
		
		if (Cursor == 255) Cursor = principal2.opcionMin;
		if (Cursor > (principal2.opcionMax)) Cursor = principal2.opcionMax;
		
		for(uint8_t i = 0; i <= principal2.opcionMax; i++){
			Escribir_Comando_LCD(renglon[i]);
			Escribir_FraseFlash_LCD(espacio);
		}
		Escribir_Comando_LCD(renglon[Cursor]);
		Escribir_Caracter_LCD(Right_Arrow);	
	
		if (Menu == 255) Menu = 0;
		if (Menu > (principal1.hermanosMax)) Menu = principal1.hermanosMax;
		if (cambio_menu){
			cambio_menu =0;
			no_repetir =1;
			maq_estado_pantalla = principal1.hermanos[Menu];
		}	
	}//maq101
/*--------------------------------------------------------------------*/	

/*------------------------General---------------------------------*/
	if (maq_estado_pantalla == 90){
		if (no_repetir)	{
			no_repetir =0;
			Menu_General();
		}
				
		Escribir_Comando_LCD(0x88);
		sprintf(arrayProm,"%4u", PromedioZona_1);
		Escribir_Texto_LCD(arrayProm);
		Escribir_Comando_LCD(0xC8);
		sprintf(arrayProm,"%4u", PromedioZona_2);
		Escribir_Texto_LCD(arrayProm);		
		Escribir_Comando_LCD(0x98);
		sprintf(arrayProm,"%4u", PromedioZona_3);
		Escribir_Texto_LCD(arrayProm);
		
		Escribir_Comando_LCD(Linea4_);
		for (uint8_t i = 0; i< 15; i++){
			Escribir_FraseFlash_LCD(espacio);
		}
		Escribir_Comando_LCD(Linea4_);
		char *Puntero_Estado = (char*) pgm_read_word (&(Estado[Estado_Horno]));	//(char*) es el cast, le dice que el número lo trate como lugar de memoria, ej= va a leer 0x1024. Eso es un lugar de memoria, no un entero
		Escribir_FraseFlash_LCD(Puntero_Estado);
		
		if (Exit == 1){
			Exit = 0;
			no_repetir =1;
			maq_estado_pantalla = principal1.dni;
		}
	}
/*--------------------------------------------------------------------*/


/*------------------------Avisos---------------------------------*/	
	if (maq_estado_pantalla == 80){
		if (no_repetir)	{
			no_repetir =0;
			avisos.graficos();
		}
		if(Enter == 1){
			Enter =0;
			no_repetir =1;
			maq_estado_pantalla = avisos.hijos[Cursor];
		}
		if (Exit == 1){
			Exit = 0;
			no_repetir =1;
			maq_estado_pantalla = principal1.dni;
		}
		if (Cursor < avisos.opcionMin) Cursor = avisos.opcionMin;
		if (Cursor > (avisos.opcionMax)) Cursor = avisos.opcionMax;
		
		for(uint8_t i = 0; i <= avisos.opcionMax; i++){
			Escribir_Comando_LCD(renglon[i]);
			Escribir_FraseFlash_LCD(espacio);
		}
		Escribir_Comando_LCD(renglon[Cursor]);
		Escribir_Caracter_LCD(Right_Arrow);
	}
/*--------------------------------------------------------------------*/

/*------------------------avisos sensores---------------------------------*/
	if (maq_estado_pantalla == 81){
		if (no_repetir)	{
			no_repetir =0;
			Menu_Avisos_Sensores();
		}
		if (Exit == 1){
			Exit = 0;
			no_repetir =1;
			maq_estado_pantalla = avisos.dni;
		}
	
		if (reconocimiento){
			reconocimiento = 0;
			//acá hago el reconocimiento posta de todos los sensores
			for (uint8_t i =0; i<8; i++){	
				Escribir_Comando_LCD(renglon_WarningSensores[i]);
				Escribir_FraseFlash_LCD(espacio);
				reconocidos_Sensores[i] = 1;
				//Reconocer Sensores
				for(uint8_t i = 0; i < 8; i++){
					if(Cable_Cortado[i] == 1){
						Cable_Cortado[i] = 2;
					}
				}
				alarmas_avisos_Sens();
			}
		}
		for (uint8_t i = 0; i < 8; i++){
			if (Vec_SsT[i] != 9999) {								//si anda bien
				reconocidos_Sensores[i] = 0;
				Escribir_Comando_LCD(renglon_WarningSensores[i]);
				Escribir_FraseFlash_LCD(espacio);					
			}
			else if (Vec_SsT[i] == 9999 && reconocidos_Sensores[i] == 0) {	//si se rompio y no está reconocido
				Escribir_Comando_LCD(renglon_WarningSensores[i]);
				Escribir_Caracter_LCD(asterisco);
			}
			else if (Vec_SsT[i] == 9999 && reconocidos_Sensores[i] == 1) {	//si se rompió y está reconocido
				Escribir_Comando_LCD(renglon_WarningSensores[i]);
				Escribir_FraseFlash_LCD(espacio);
			}
		}
	}
/*--------------------------------------------------------------------*/

	
/*------------------------avisos temperaturas---------------------------------*/	
	if (maq_estado_pantalla == 82){
		if (no_repetir)	{
			no_repetir =0;
			Menu_Avisos_Temperatura();
		}
		if (Exit == 1){
			Exit = 0;
			no_repetir =1;
			maq_estado_pantalla = avisos.dni;
		}
		if (reconocimiento){
			reconocimiento = 0;
			//acá hago el reconocimiento posta de todos los sensores
			for (uint8_t i =0; i<3; i++){
				if(Estado_Actual_WA[i] == 1){
					Estado_Actual_WA[i] = 3;
				}
				Escribir_Comando_LCD(renglon_WarAlaTemps[i]);
				Escribir_FraseFlash_LCD(espacio);
				alarmas_avisos_T();
			}
		}
		for (uint8_t i = 0; i < 3; i++) {
			switch(Estado_Actual_WA[i]){
				case 0:
				//todo normal
				//reconocidos_AvisosTemp[i] = 0;
				Escribir_Comando_LCD(renglon_WarAlaTemps[i]);
				Escribir_FraseFlash_LCD(espacio);
				break;
				case 1:
				//mandar por pantalla AVISO NO reconocido
				Escribir_Comando_LCD(renglon_WarAlaTemps[i]);
				Escribir_Caracter_LCD(asterisco);
				break;
				case 3:
				//mandar por pantalla AVISO reconocido
				Escribir_Comando_LCD(renglon_WarAlaTemps[i]);
				Escribir_FraseFlash_LCD(espacio);
				break;
			}
		}
	}
/*--------------------------------------------------------------------*/	


/*------------------------Alarmas---------------------------------*/
if (maq_estado_pantalla == 70){
	if (no_repetir)	{
		no_repetir =0;
		alarmas.graficos();
	}
	if(Enter == 1){
		Enter =0;
		no_repetir =1;
		maq_estado_pantalla = alarmas.hijos[Cursor];
	}
	if (Exit == 1){
		Exit = 0;
		no_repetir =1;
		maq_estado_pantalla = principal1.dni;
	}
	if (Cursor < alarmas.opcionMin) Cursor = alarmas.opcionMin;
	if (Cursor > (alarmas.opcionMax)) Cursor = alarmas.opcionMax;
	
	for(uint8_t i = 0; i <= alarmas.opcionMax; i++){
		Escribir_Comando_LCD(renglon[i]);
		Escribir_FraseFlash_LCD(espacio);
	}
	Escribir_Comando_LCD(renglon[Cursor]);
	Escribir_Caracter_LCD(Right_Arrow);
}
/*--------------------------------------------------------------------*/

/*------------------------Alarmas temperaturas---------------------------------*/
if (maq_estado_pantalla == 71){
	if (no_repetir)	{
		no_repetir =0;
		Menu_Alarmas_Temperatura();
	}
	if (Exit == 1){
		Exit = 0;
		no_repetir =1;
		maq_estado_pantalla = alarmas.dni;
	}
	if (reconocimiento){
		reconocimiento = 0;
		//acá hago el reconocimiento posta de todas las temperaturas
		for (uint8_t i =0; i<3; i++){
			if(Estado_Actual_WA[i] == 2){
				Estado_Actual_WA[i] = 4;
			}
			Escribir_Comando_LCD(renglon_WarAlaTemps[i]);
			Escribir_FraseFlash_LCD(espacio);
			alarmas_avisos_T();
		}
	}
	for (uint8_t i = 0; i < 3; i++){
		switch(Estado_Actual_WA[i]){
			case 0:
			//todo normal
			Escribir_Comando_LCD(renglon_WarAlaTemps[i]);
			Escribir_FraseFlash_LCD(espacio);
			break;
			case 2:
			//mandar por pantalla AVISO NO reconocido
			Escribir_Comando_LCD(renglon_WarAlaTemps[i]);
			Escribir_Caracter_LCD(asterisco);
			break;
			case 4:
			//mandar por pantalla AVISO reconocido
			Escribir_Comando_LCD(renglon_WarAlaTemps[i]);
			Escribir_FraseFlash_LCD(espacio);
			break;
		}
	}
}
/*--------------------------------------------------------------------*/


/*------------------------Alarmas PIDS---------------------------------*/
if (maq_estado_pantalla == 72){
	if (no_repetir)	{
		no_repetir =0;
		Menu_Alarmas_PIDS();
	}
	if (Exit == 1){
		Exit = 0;
		no_repetir =1;
		maq_estado_pantalla = alarmas.dni;
	}
	if (reconocimiento){
		reconocimiento = 0;
		//acá hago el reconocimiento posta de los PIDs
		for (uint8_t i = 0; i<6; i++){
			Escribir_Comando_LCD(renglon_AlarmPID[i]);
			Escribir_FraseFlash_LCD(espacio);
			reconocidos_PIDS[i] = 1;
			if(Flag_Parada_Emergencia == 2){
				Led_Rojo = 1;
				if(Botton_Emergencia == 0){
					Chequear_PIDs_Fallas();
				}
			}
		}
	}
	for (uint8_t i = 0; i < 6; i++) {
		if (Vec_PIDs[i] == 0) {						//si está bien, saco el asterisco
			reconocidos_PIDS[i] = 0;
			Escribir_Comando_LCD(renglon_AlarmPID[i]);
			Escribir_FraseFlash_LCD(espacio);
		}
		else if (Vec_PIDs[i] == 1 && reconocidos_PIDS[i] == 0) {	//si se rompio y no está reconocido
			Escribir_Comando_LCD(renglon_AlarmPID[i]);				
			Escribir_Caracter_LCD(asterisco);						//pongo el asterisco
		}
		else if (Vec_PIDs[i] == 1 && reconocidos_PIDS[i] == 1) {	//si se rompió y está reconocido
			Escribir_Comando_LCD(renglon_AlarmPID[i]);				
			Escribir_FraseFlash_LCD(espacio);						//saco el asterisco
		}
	}
}
/*--------------------------------------------------------------------*/



/*------------------------Set Points---------------------------------*/
if (maq_estado_pantalla == 60){
	if (Menu == 255) Menu = 0;
	if (Menu > (setPoint.hermanosMax)) Menu = setPoint.hermanosMax;

	if (cambio_menu){
		cambio_menu =0;
		no_repetir =1;
	}
	if (no_repetir)	{
		no_repetir =0;
		switch (Menu){
			case 0:
				Menu_SetPoints (Menu, SP_TempZ1, SP_CaudalZ1);
			break;
			case 1:
				Menu_SetPoints (Menu, SP_TempZ2, SP_CaudalZ2);
			break;
			case 2:
				Menu_SetPoints (Menu, SP_CaudalZ3, SP_VelocidadZ3);
			break;
		}	
	}
	if (Exit == 1){
		Exit = 0;
		no_repetir =1;
		maq_estado_pantalla = principal1.dni;
	}
}
/*--------------------------------------------------------------------*/


/*------------------------Tiempos---------------------------------*/
	if (maq_estado_pantalla == 50){
		if (no_repetir)	{
			no_repetir =0;
			Menu_Tiempos(Tiempo_PreCalentamiento, Tiempo_Calentamiento, Tiempo_Enfriamiento);
		}
		if (Exit == 1){
			Exit = 0;
			no_repetir =1;
			maq_estado_pantalla = principal2.dni;
		}
	}
/*--------------------------------------------------------------------*/


/*------------------------Rangos Sensores---------------------------------*/
if (maq_estado_pantalla == 40){
	if (Menu == 255) Menu = 0;
	if (Menu > (R_Sensores.hermanosMax)) Menu = R_Sensores.hermanosMax;

	if (cambio_menu){
		cambio_menu =0;
		no_repetir =1;
	}
	if (no_repetir)	{
		no_repetir =0;
		switch (Menu){
			case 0:
			Menu_RangSensores (Menu, R_TT1, R_TT2, R_TT3);
			break;
			case 1:
			Menu_RangSensores (Menu, R_TT4, R_TT5, R_TT6);
			break;
			case 2:
			Menu_RangSensores (Menu, R_TT7, R_TT8, 0);
			break;
		}
	}
	if (Exit == 1){
		Exit = 0;
		no_repetir =1;
		maq_estado_pantalla = principal2.dni;
	}
}
/*-------------------------------------------------------------------------*/

/*------------------------Rangos Actuadores---------------------------------*/
if (maq_estado_pantalla == 30){
	if (Menu == 255) Menu = 0;
	if (Menu > (R_Actuadores.hermanosMax)) Menu = R_Actuadores.hermanosMax;

	if (cambio_menu){
		cambio_menu =0;
		no_repetir =1;
	}
	if (no_repetir)	{
		no_repetir =0;
		switch (Menu){
			case 0:
			Menu_RangActuadores (Menu, R_H1, R_B2);
			break;
			case 1:
			Menu_RangActuadores (Menu, R_H2, R_B2);
			break;
			case 2:
			Menu_RangActuadores (Menu, R_B3, R_C1);
			break;
		}
	}
	if (Exit == 1){
		Exit = 0;
		no_repetir = 1;
		maq_estado_pantalla = principal2.dni;
	}
}
/*-------------------------------------------------------------------------*/
}//actualizar menu


/*-------------------------------------------Leer ads1115------------------------------------------*/	
		if (Habilitar_LeerTemperatura == 1){
			Habilitar_LeerTemperatura = 0;
			Canal_Temp += 1;
			switch (Canal_Temp){
				case 1:
				ads1155_MSB_Config = 0xC1;
				break;
				case 2:
				ads1155_MSB_Config = 0xD1;
				break;						
				case 3:
				ads1155_MSB_Config = 0xE1;					
				break;						
				case 4:
				ads1155_MSB_Config = 0xF1;
				break;	
				default:					
				break;					
			}//switchCanal
			cofigurar_ads1115(ads1115_IP_VCC_write, ads1115_Config_Reg, ads1155_MSB_Config, 0b11100000);
			cofigurar_ads1115(ads1115_IP_GND_write, ads1115_Config_Reg, ads1155_MSB_Config, 0b11100000);	
		}//habilitarLeer
/*---------------------------------------------------------------------------------------------*/	

}//while
}//main

ISR(USART_RX_vect){
	dato = UDR0;
	
	switch(dato){
		case 'A':
		Flecha_UP = 1;
		break;
		
		case 'D':
		Flecha_Left = 1;
		break;
		
		case 'B':
		Flecha_LOW = 1;
		break;
		
		case 'C':
		Flecha_Right = 1;
		break;
		
		case 13:
		case 32:
		C_Enter = 1;
		break;
	}
	if(Ingresar_Valor == 1){
		Enter_2 = 1;
		if(dato == 8 || dato == 127){
			Cursor_Fil_Col(26,46+c);
			enviar_frase(vacio_2);
			c--;
		}
		else{
			if(c < 5){
				if(c == 0){
					enviar_frase(vacio_3);
				}
				Dato_RX[c++] = dato;
				Cursor_Fil_Col(26,46+c);
				UART_enviar_char(dato);
			}
		}
	}
}

ISR(TIMER1_COMPA_vect) {

/*----Actualizar pantallas LCD----*/
	Actualizar_Menu = 0;
/*--------------------------------*/

/*----------leer los ads1115----------------*/
	Habilitar_LeerTemperatura = 1;
	Canal_Temp = 0;
/*------------------------------------------*/
	if(Cont_Tiempo != 65535){
		Cont_Tiempo++;
	}
	
	Flag_adecuacion_sensores = 1;
	tiempo++;
	
	switch(Led_Rojo){
		case 1:
		PORTD |= (1 << PD7);		// rojo en 1 PD7
		PORTD &= ~(1 << PD6);		// verde en 0 PD7
		break;
		case 2:
		if(tiempo == 1){
			PORTD &= ~(1 << PD7);
			PORTD &= ~(1 << PD6);
			}else{
			PORTD |= (1 << PD7);
			PORTD &= ~(1 << PD6);
		}
		break;
		
		case 0:
		switch(led_Ambar){
			
			case 1:
			PORTD |= (1 << PD6);
			PORTD |= (1 << PD7);
			break;
			
			case 2:
			if(tiempo == 1){
				PORTD &= ~(1 << PD7); 
				PORTD &= ~(1 << PD6); 
			}else{
				PORTD |= (1 << PD6);
				PORTD |= (1 << PD7);
			}

			break;
			
			case 0:	
			switch(Led_Verde){
				
				case 1:
				// verde en alto PD6
				PORTD |= (1 << PD6);
				PORTD &= ~(1 << PD7);
				break;
				
				case 2:
				if(tiempo == 1){
					PORTD &= ~(1 << PD6);
					PORTD &= ~(1 << PD7);
					}else{
					PORTD |= (1 << PD6);
					PORTD &= ~(1 << PD7);
				}
				break;
				
				case 0:
				//apago led verde
				PORTD &= ~(1 << PD6); 
				break;
				}
			if(Led_Rojo == 0){PORTD &= ~(1 << PD7);}
			if(led_Ambar == 0 && Led_Rojo == 0 && Led_Verde == 0){
				PORTD &= ~(1 << PD7);
				PORTD &= ~(1 << PD6);
						
			break;
			}	
		break;	
		}
	}	
		
	if(tiempo == 2){
		tiempo = 0;
	}
}

ISR(PCINT0_vect){
	uint8_t estado_actual = PINB;
	uint8_t mask = 0x3F;
	    
	// Detectar cambios
	uint8_t cambios = (estado_actual ^ estado_anterior_B) & mask;
	    
	// Flanco de bajada (1 -> 0)
	uint8_t bajaron_a_0 = cambios & (~estado_actual);
	    
	// Flanco de subida (0 -> 1)
	uint8_t subieron_a_1 = cambios & estado_actual;

	// --- FALLAS (1 -> 0) ---
	if (bajaron_a_0 & (1 << PB0)) {Vec_PIDs[0] = 1;}
	if (bajaron_a_0 & (1 << PB1)) {Vec_PIDs[1] = 1;}
	if (bajaron_a_0 & (1 << PB2)) {Vec_PIDs[2] = 1;}
	if (bajaron_a_0 & (1 << PB3)) {Vec_PIDs[3] = 1;}
	if (bajaron_a_0 & (1 << PB4)) {Vec_PIDs[4] = 1;}
	if (bajaron_a_0 & (1 << PB5)) {Vec_PIDs[5] = 1;}

	// --- RECUPERACIÓN (0 -> 1) ---
	if (subieron_a_1 & (1 << PB0)) {Vec_PIDs[0] = 0;}
	if (subieron_a_1 & (1 << PB1)) {Vec_PIDs[1] = 0;}
	if (subieron_a_1 & (1 << PB2)) {Vec_PIDs[2] = 0;}
	if (subieron_a_1 & (1 << PB3)) {Vec_PIDs[3] = 0;}
	if (subieron_a_1 & (1 << PB4)) {Vec_PIDs[4] = 0;}
	if (subieron_a_1 & (1 << PB5)) {Vec_PIDs[5] = 0;}

	// --- PARADA DE EMERGENCIA ---
	// Si al menos uno está en falla 
	if ((~estado_actual) & mask){
		Flag_Parada_Emergencia = 1;
		}else{
		Flag_Parada_Emergencia = 0;
	}
	estado_anterior_B = estado_actual;
		}

ISR(PCINT1_vect){
	uint8_t estado_PC2_actual;

	// Leer estado actual de PC2
	estado_PC2_actual = (PINC & (1 << PC2));

	// Detectar cambio
	if (estado_PC2_actual != estado_anterior_PC2){
		if (estado_PC2_actual == 0){
			// FLANCO DESCENDENTE 
			Hab_POSterior = 1;
			PORTC |= (1 << PC3);  // PC3 en 1
		}else{
			// FLANCO ASCENDENTE 
			Hab_POSterior = 0;
			PORTC &= ~(1 << PC3);  // PC3 en 0
		}
	// Actualizar estado previo
	estado_anterior_PC2 = estado_PC2_actual;
	}
}

ISR(PCINT2_vect){
	if (!(PIND & (1 << PIND5))) {
		/*----------------------------------Leer ads1115--------------------------------------*/
		if (Canal_Temp == 1 || Canal_Temp == 2 || Canal_Temp == 3 || Canal_Temp == 4){

			temperatura = Leer_ads1115(ads1115_IP_VCC_write);
			Vec_SsT [(Canal_Temp - 1)] = temperatura;
			
			temperatura = Leer_ads1115(ads1115_IP_GND_write);
			Vec_SsT [(Canal_Temp - 1) + 4] = temperatura;
			
			Habilitar_LeerTemperatura = 1;
			if(Contador_Temp == 4){
				adecuacion_sensores();
				Habilitar_LeerTemperatura = 0;
				Contador_Temp = 0;
				if(Maquina_Estado == 26){
					Flag_Norepetir = 0;
				}
			}
			Contador_Temp++;
		}
		/*------------------------------------------------------------------------------------*/
	}
		uint8_t estado_actual_D = PIND;

		// Detectar cambio en PD4
		uint8_t cambio = (estado_actual_D ^ estado_anterior_D) & (1 << PD4);

		if(cambio){
			if( (estado_actual_D & (1 << PD4)) == 0 ){
				if(Estado_Horno == 0){
				Flag_Secuencia_Arranque = 1;
				//bajo a 0	
				}
			}else{
				if(Estado_Horno == 2){
					Flag_Parada_Controlada = 1;
					//subio a 1
				}
			}
		}
		estado_anterior_D = estado_actual_D;
}

ISR(INT0_vect) {
	
	Flag_Parada_Emergencia = !Flag_Parada_Emergencia;
	Botton_Emergencia = !Botton_Emergencia;
	
	if(Botton_Emergencia == 0){
	Estado_Horno = 0;
	Led_Rojo = 0;
	}
	
}

ISR(INT1_vect){
	ADCSRA |= (1<<ADSC);   	// Iniciar conversión
}

ISR(ADC_vect) {
	valor_adc = ADC;  // lee ADCL + ADCH automáticamente
	Habilitar_Teclado = 1;
}	

void UART_enviar_char(char c)
{
	while (!(UCSR0A & (1<<UDRE0)));
	UDR0 = c;
}

void UART_enviar_string(char *str)
{
	while (*str)
	{
		UART_enviar_char(*str++);
	}
}

void enviar_frase(const char *frase){
	char c;
	
	while ((c = pgm_read_byte(frase++)) != '\0'){
		
		UART_enviar_char(c);
	}
}

void Cursor_Fil_Col(uint8_t fila, uint8_t columna){
	
	char buffer[16];
	sprintf(buffer, "\033[%d;%dH", fila, columna);
	UART_enviar_string(buffer);
}

void FinFLechas_P0(){
	Flag_Norepetir = 0;
	Flecha_LOW = 0;
	Flecha_UP = 0;
	Flecha_Right = 0;
	Flecha_Left = 0;
	Cont_Enter = 0;
	Ingresar_Valor = 0;
}

void Pantalla_6_Uart(){
	
	#ifndef Frases
	#define Frases

	if(Cont_Enter == 0){
		
	if (Flag_Norepetir == 0){
	
		enviar_frase(Borrar_Pantalla);
		Cursor_Fil_Col(0,28);
		enviar_frase(F_Titulo_P6_2);	

	Cursor_Fil_Col(3,9);
	enviar_frase(F_EstadoHorno);
	
	
	Cursor_Fil_Col(26,31);
	enviar_frase(Dato_Recibido);
	

	switch(Estado_Horno){
	
		case 0:
		Cursor_Fil_Col(3,24);
		enviar_frase(F_Apagado);
		break;
	
		case 1:
		Cursor_Fil_Col(3,24);
		enviar_frase(F_Arranque);
	
		Cursor_Fil_Col(3,24);
		enviar_frase(F_Fase);
		sprintf(Numero, "%d", Fase);
		UART_enviar_string(Numero);
	
		Cursor_Fil_Col(3,24);
		enviar_frase(F_Tiempo);
		sprintf(Numero, "%d", Cont_Tiempo);
		UART_enviar_string(Numero);
		break;
	
		case 2:
		Cursor_Fil_Col(3,24);
		enviar_frase(F_Regimen);
		break;
	
		case 3:
		Cursor_Fil_Col(3,24);
		enviar_frase(F_Controlada);
	
		Cursor_Fil_Col(3,24);
		enviar_frase(F_Fase);
		sprintf(Numero, "%d", Fase);
		UART_enviar_string(Numero);
	
		Cursor_Fil_Col(3,24);
		enviar_frase(F_Tiempo);
		sprintf(Numero, "%d", Cont_Tiempo);
		UART_enviar_string(Numero);
		break;
	
		case 4:
		Cursor_Fil_Col(3,24);
		enviar_frase(F_Emergencia);
		break;
	}

	Cursor_Fil_Col(5,12);
	enviar_frase(F_PromZ1);
	Cursor_Fil_Col(5,35);
	enviar_frase(F_PromZ2);
	Cursor_Fil_Col(5,58);
	enviar_frase(F_PromZ3);

	Cursor_Fil_Col(7,7);
	enviar_frase(F_Estado);
	switch(Estado_Actual[0]){
		
		case 1:
		case 3:
		
		Cursor_Fil_Col(7,15);
		enviar_frase(F_ZW);
		break;
		
		case 2:
		case 4:
		Cursor_Fil_Col(7,15);
		enviar_frase(F_ZA);
		break;
		
		case 0:
		Cursor_Fil_Col(7,15);
		enviar_frase(F_Normal);
		break;
	}
	
	Cursor_Fil_Col(8,7);
	enviar_frase(F_Prom);
	sprintf(Numero, "%4d", PromedioZona_1);
	UART_enviar_string(Numero);
	enviar_frase(F_Unid_Temp);
	
	Cursor_Fil_Col(7,31);
	enviar_frase(F_Estado);
	switch(Estado_Actual[1]){
		case 1:
		case 3:
		Cursor_Fil_Col(7,39);
		enviar_frase(F_ZW);
		break;
		
		case 2:
		case 4:
		Cursor_Fil_Col(7,39);
		enviar_frase(F_ZA);
		break;
		
		case 0:
		Cursor_Fil_Col(7,39);
		enviar_frase(F_Normal);
		break;
	}

	Cursor_Fil_Col(8,31);
	enviar_frase(F_Prom);
	sprintf(Numero, "%4d", PromedioZona_2);
	UART_enviar_string(Numero);
	enviar_frase(F_Unid_Temp);
	
	Cursor_Fil_Col(7,54);
	enviar_frase(F_Estado);
	switch(Estado_Actual[2]){
		case 1:
		case 3:
		Cursor_Fil_Col(7,62);
		enviar_frase(F_ZW);
		break;
		
		case 2:
		case 4:
		Cursor_Fil_Col(7,62);
		enviar_frase(F_ZA);
		break;
		
		case 0:
		Cursor_Fil_Col(7,62);
		enviar_frase(F_Normal);
		break;
	}

	Cursor_Fil_Col(8,54);
	enviar_frase(F_Prom);
	sprintf(Numero, "%4d", PromedioZona_3);
	UART_enviar_string(Numero);
	enviar_frase(F_Unid_Temp);
	
//------------------------------COLUMNA 1---------------------------------------------//
	Cursor_Fil_Col(9,7);
	if(Flecha_P6 == 1){Cursor_Fil_Col(9,4);enviar_frase(F_arrowLeft);}
	enviar_frase(F_spTemp_Z1);
	sprintf(Numero, "%4d", SP_TempZ1);
	UART_enviar_string(Numero);
	enviar_frase(F_Unid_Temp);
	if(Flecha_P6 == 1){enviar_frase(F_arrowRigth);}

	Cursor_Fil_Col(11,7);
	if(Flecha_P6 == 2){Cursor_Fil_Col(11,3);enviar_frase(F_arrowLeft);}
	enviar_frase(F_spCV_Z1);
	sprintf(Numero, "%4d", SP_CaudalZ1);
	UART_enviar_string(Numero);
	enviar_frase(F_Unid_caudal);
	if(Flecha_P6 == 2){enviar_frase(F_arrowRigth);}

	Cursor_Fil_Col(12,7);
	if(Flecha_P6 == 3){Cursor_Fil_Col(12,3);enviar_frase(F_arrowLeft);}
	enviar_frase(F_W);
	sprintf(Numero, "%4d", Aviso_TZ1);
	UART_enviar_string(Numero);
	enviar_frase(F_Unid_Porc);
	if(Flecha_P6 == 3){enviar_frase(F_arrowRigth);}

	Cursor_Fil_Col(13,7);
	if(Flecha_P6 == 4){Cursor_Fil_Col(13,3);enviar_frase(F_arrowLeft);}
	enviar_frase(F_A);
	sprintf(Numero, "%4d", Alarma_TZ1);
	UART_enviar_string(Numero);
	enviar_frase(F_Unid_Porc);
	if(Flecha_P6 == 4){enviar_frase(F_arrowRigth);}
	
	Cursor_Fil_Col(15,7);	
	enviar_frase(F_TT1);
	sprintf(Numero, "%4d", Vec_Sens2[0]);
	UART_enviar_string(Numero);
	enviar_frase(F_Unid_Temp);
	
	
	Cursor_Fil_Col(16,7);
	enviar_frase(F_TT2);
	sprintf(Numero, "%4d", Vec_Sens2[1]);
	UART_enviar_string(Numero);
	enviar_frase(F_Unid_Temp);
	
	Cursor_Fil_Col(17,7);
	enviar_frase(F_TT3);
	sprintf(Numero, "%4d", Vec_Sens2[2]);
	UART_enviar_string(Numero);
	enviar_frase(F_Unid_Temp);
	
	Cursor_Fil_Col(19,7);
	if(Flecha_P6 == 5){Cursor_Fil_Col(19,3);enviar_frase(F_arrowLeft);}
	enviar_frase(F_H1);
	sprintf(Numero, "%4d", R_H1);
	UART_enviar_string(Numero);
	enviar_frase(F_Unid_Temp);
	if(Flecha_P6 == 5){enviar_frase(F_arrowRigth);}
	
	Cursor_Fil_Col(20,7);
	if(Flecha_P6 == 6){Cursor_Fil_Col(20,3);enviar_frase(F_arrowLeft);}
	enviar_frase(F_B1);
	sprintf(Numero, "%4d", R_B1);
	UART_enviar_string(Numero);
	enviar_frase(F_Unid_caudal);
	if(Flecha_P6 == 6){enviar_frase(F_arrowRigth);}
		
	Cursor_Fil_Col(21,7);
	if(Flecha_P6 == 7){Cursor_Fil_Col(21,3);enviar_frase(F_arrowLeft);}
	enviar_frase(F_RTT1);
	sprintf(Numero,"%4d", R_TT1);
	UART_enviar_string(Numero);
	enviar_frase(F_Unid_Temp);
	if(Flecha_P6 == 7){enviar_frase(F_arrowRigth);}

	Cursor_Fil_Col(22,7);
	if(Flecha_P6 == 8){Cursor_Fil_Col(22,3);enviar_frase(F_arrowLeft);}
	enviar_frase(F_RTT2);
	sprintf(Numero,"%4d", R_TT2);
	UART_enviar_string(Numero);
	enviar_frase(F_Unid_Temp);
	if(Flecha_P6 == 8){enviar_frase(F_arrowRigth);}
	
	Cursor_Fil_Col(23,7);
	if(Flecha_P6 == 9){Cursor_Fil_Col(23,3);enviar_frase(F_arrowLeft);}
	enviar_frase(F_RTT3);
	sprintf(Numero,"%4d", R_TT3);
	UART_enviar_string(Numero);
	enviar_frase(F_Unid_Temp);
	if(Flecha_P6 == 9){enviar_frase(F_arrowRigth);}
		
	Cursor_Fil_Col(25,7);
	if(Flecha_P6 == 27){Cursor_Fil_Col(25,3);enviar_frase(F_arrowLeft);}
	enviar_frase(F_TimePrecal);
	sprintf(Numero, "%4d", Tiempo_PreCalentamiento);
	UART_enviar_string(Numero);
	enviar_frase(F_Unid_Time);
	if(Flecha_P6 == 27){enviar_frase(F_arrowRigth);}
		
	Cursor_Fil_Col(26,7);
	if(Flecha_P6 == 28){Cursor_Fil_Col(26,3);enviar_frase(F_arrowLeft);}
	enviar_frase(F_TimeCal);
	sprintf(Numero, "%4d", Tiempo_Calentamiento);
	UART_enviar_string(Numero);
	enviar_frase(F_Unid_Time);
	if(Flecha_P6 == 28){enviar_frase(F_arrowRigth);}
		
	Cursor_Fil_Col(27,7);
	if(Flecha_P6 == 29){Cursor_Fil_Col(27,3);enviar_frase(F_arrowLeft);}
	enviar_frase(F_TimeEnf);
	sprintf(Numero, "%4d", Tiempo_Enfriamiento);
	UART_enviar_string(Numero);
	enviar_frase(F_Unid_Time);
	if(Flecha_P6 == 29){enviar_frase(F_arrowRigth);}
		
	Cursor_Fil_Col(28,7);
	if(Flecha_P6 == 30){Cursor_Fil_Col(28,3);enviar_frase(F_arrowLeft);}
	enviar_frase(F_Reconocer);
	if(Flecha_P6 == 30){enviar_frase(F_arrowRigth);}
	
//------------------------------COLUMNA 2---------------------------------------------//
	Cursor_Fil_Col(9,31);
	if(Flecha_P6 == 10){Cursor_Fil_Col(9,27);enviar_frase(F_arrowLeft);}
	enviar_frase(F_spTemp_Z2);
	sprintf(Numero, "%4d", SP_TempZ2);
	UART_enviar_string(Numero);
	enviar_frase(F_Unid_Temp);
	if(Flecha_P6 == 10){enviar_frase(F_arrowRigth);}

	Cursor_Fil_Col(11,31);
	if(Flecha_P6 == 11){Cursor_Fil_Col(11,27);enviar_frase(F_arrowLeft);}
	enviar_frase(F_spCV_Z2);
	sprintf(Numero, "%4d", SP_CaudalZ2);
	UART_enviar_string(Numero);
	enviar_frase(F_Unid_caudal);
	if(Flecha_P6 == 11){enviar_frase(F_arrowRigth);}

	Cursor_Fil_Col(12,31);
	if(Flecha_P6 == 12){Cursor_Fil_Col(12,27); enviar_frase(F_arrowLeft);}
	enviar_frase(F_W);
	sprintf(Numero, "%4d", Aviso_TZ2);
	UART_enviar_string(Numero);
	enviar_frase(F_Unid_Porc);
	if(Flecha_P6 == 12){enviar_frase(F_arrowRigth);}

	Cursor_Fil_Col(13,31);
	if(Flecha_P6 == 13){Cursor_Fil_Col(13,27); enviar_frase(F_arrowLeft);}
	enviar_frase(F_A);
	sprintf(Numero, "%4d", Alarma_TZ2);
	UART_enviar_string(Numero);
	enviar_frase(F_Unid_Porc);
	if(Flecha_P6 == 13){enviar_frase(F_arrowRigth);}
	
	Cursor_Fil_Col(15,31);	
	enviar_frase(F_TT4);
	sprintf(Numero, "%4d", Vec_Sens2[3]);
	UART_enviar_string(Numero);
	enviar_frase(F_Unid_Temp);

	Cursor_Fil_Col(16,31);
	enviar_frase(F_TT5);
	sprintf(Numero, "%4d", Vec_Sens2[4]);
	UART_enviar_string(Numero);
	enviar_frase(F_Unid_Temp);

	Cursor_Fil_Col(17,31);
	enviar_frase(F_TT6);
	sprintf(Numero, "%4d", Vec_Sens2[5]);
	UART_enviar_string(Numero);
	enviar_frase(F_Unid_Temp);

	Cursor_Fil_Col(19,31);
	if(Flecha_P6 == 14){Cursor_Fil_Col(19,27);enviar_frase(F_arrowLeft);}
	enviar_frase(F_H2);
	sprintf(Numero, "%4d", R_H2);
	UART_enviar_string(Numero);
	enviar_frase(F_Unid_Temp);
	if(Flecha_P6 == 14){enviar_frase(F_arrowRigth);}
		
	Cursor_Fil_Col(20,31);
	if(Flecha_P6 == 15){Cursor_Fil_Col(20,27);enviar_frase(F_arrowLeft);}
	enviar_frase(F_B2);
	sprintf(Numero, "%4d", R_B2);
	UART_enviar_string(Numero);
	enviar_frase(F_Unid_caudal);
	if(Flecha_P6 == 15){enviar_frase(F_arrowRigth);}
		
	Cursor_Fil_Col(21,31);
	if(Flecha_P6 == 16){Cursor_Fil_Col(21,27);enviar_frase(F_arrowLeft);}
	enviar_frase(F_RTT4);
	sprintf(Numero,"%4d", R_TT4);
	UART_enviar_string(Numero);
	enviar_frase(F_Unid_Temp);
	if(Flecha_P6 == 16){enviar_frase(F_arrowRigth);}

	Cursor_Fil_Col(22,31);
	if(Flecha_P6 == 17){Cursor_Fil_Col(22,27);enviar_frase(F_arrowLeft);}
	enviar_frase(F_RTT5);
	sprintf(Numero,"%4d", R_TT5);
	UART_enviar_string(Numero);
	enviar_frase(F_Unid_Temp);
	if(Flecha_P6 == 17){enviar_frase(F_arrowRigth);}
		
	Cursor_Fil_Col(23,31);
	if(Flecha_P6 == 18){Cursor_Fil_Col(23,27);enviar_frase(F_arrowLeft);}
	enviar_frase(F_RTT6);
	sprintf(Numero,"%4d", R_TT6);
	UART_enviar_string(Numero);
	enviar_frase(F_Unid_Temp);
	if(Flecha_P6 == 18){enviar_frase(F_arrowRigth);}
		
//------------------------------COLUMNA 3---------------------------------------------//
	Cursor_Fil_Col(9,54);
	if(Flecha_P6 == 19){Cursor_Fil_Col(9,50);enviar_frase(F_arrowLeft);}
	enviar_frase(F_VelCinta);
	sprintf(Numero, "%4d", SP_VelocidadZ3);
	UART_enviar_string(Numero);
	enviar_frase(F_Unid_cinta);
	if(Flecha_P6 == 19){enviar_frase(F_arrowRigth);}

	Cursor_Fil_Col(11,54);
	if(Flecha_P6 == 20){Cursor_Fil_Col(11,50); enviar_frase(F_arrowLeft);}
	enviar_frase(F_spCV_Z3);
	sprintf(Numero, "%4d", SP_CaudalZ3);
	UART_enviar_string(Numero);
	enviar_frase(F_Unid_caudal);
	if(Flecha_P6 == 20){enviar_frase(F_arrowRigth);}

	Cursor_Fil_Col(12,54);
	if(Flecha_P6 == 21){Cursor_Fil_Col(12,50);enviar_frase(F_arrowLeft);}
	enviar_frase(F_W);
	sprintf(Numero, "%4d", Aviso_TZ3);
	UART_enviar_string(Numero);
	enviar_frase(F_Unid_Temp);
	if(Flecha_P6 == 21){enviar_frase(F_arrowRigth);}

	Cursor_Fil_Col(13,54);
	if(Flecha_P6 == 22){Cursor_Fil_Col(13,50);enviar_frase(F_arrowLeft);}
	enviar_frase(F_A);
	sprintf(Numero, "%4d", Alarma_TZ3);
	UART_enviar_string(Numero);
	enviar_frase(F_Unid_Temp);
	if(Flecha_P6 == 22){enviar_frase(F_arrowRigth);}
	
	Cursor_Fil_Col(15,54);	
	enviar_frase(F_TT7);
	sprintf(Numero, "%4d", Vec_Sens2[6]);
	UART_enviar_string(Numero);
	enviar_frase(F_Unid_Temp);
	
	Cursor_Fil_Col(16,54);
	enviar_frase(F_TT8);
	sprintf(Numero, "%4d", Vec_Sens2[7]);
	UART_enviar_string(Numero);
	enviar_frase(F_Unid_Temp);
		
	Cursor_Fil_Col(19,54);
	if(Flecha_P6 == 23){Cursor_Fil_Col(19,50);enviar_frase(F_arrowLeft);}
	enviar_frase(F_C1);
	sprintf(Numero, "%4d", R_C1);
	UART_enviar_string(Numero);
	enviar_frase(F_Unid_cinta);
	if(Flecha_P6 == 23){enviar_frase(F_arrowRigth);}
		
	Cursor_Fil_Col(20,54);
	if(Flecha_P6 == 24){Cursor_Fil_Col(20,50);enviar_frase(F_arrowLeft);}
	enviar_frase(F_B3);
	sprintf(Numero,"%4d", R_B3);
	UART_enviar_string(Numero);
	enviar_frase(F_Unid_caudal);
	if(Flecha_P6 == 24){enviar_frase(F_arrowRigth);}
		
	Cursor_Fil_Col(21,54);
	if(Flecha_P6 == 25){Cursor_Fil_Col(21,50);enviar_frase(F_arrowLeft);}
	enviar_frase(F_RTT7);
	sprintf(Numero,"%4d", R_TT7);
	UART_enviar_string(Numero);
	enviar_frase(F_Unid_Temp);
	if(Flecha_P6 == 25){enviar_frase(F_arrowRigth);}
		
	Cursor_Fil_Col(22,54);
	if(Flecha_P6 == 26){Cursor_Fil_Col(22,50);enviar_frase(F_arrowLeft);}
	enviar_frase(F_RTT8);
	sprintf(Numero,"%4d", R_TT8);
	UART_enviar_string(Numero);
	enviar_frase(F_Unid_Temp);
	if(Flecha_P6 == 26){enviar_frase(F_arrowRigth);}
		
	
	Flag_Norepetir = 1;
	}
	#endif
	/*
		if(Falla_Sens[0] != 0){
			Cursor_Fil_Col(11,40);
			enviar_frase(F_moniFalla);
			sprintf(Numero, "%d", Falla_Sens[0]);
			UART_enviar_string(Numero);
			enviar_frase(F_moniZ1);
		}
		
		if(Falla_Sens[1] != 0){
			Cursor_Fil_Col(13,40);
			enviar_frase(F_moniFalla);
			sprintf(Numero, "%d", Falla_Sens[1]);
			UART_enviar_string(Numero);
			enviar_frase(F_moniZ2);
		}
		
		if(Falla_Sens[2] != 0){
			Cursor_Fil_Col(15,40);
			enviar_frase(F_moniFalla);
			sprintf(Numero, "%d", Falla_Sens[2]);
			UART_enviar_string(Numero);
			enviar_frase(F_moniZ3);
		}
		
		if(Vec_PIDs[0] == 1){
			Cursor_Fil_Col(3,62);
			enviar_frase(F_moniFalla);
			enviar_frase(F_PID1);
		}
		if(Vec_PIDs[1] == 1){
			Cursor_Fil_Col(5,62);
			enviar_frase(F_moniFalla);
			enviar_frase(F_PID2);
		}
		if(Vec_PIDs[2] == 1){
			Cursor_Fil_Col(7,62);
			enviar_frase(F_moniFalla);
			enviar_frase(F_PID3);
		}
		if(Vec_PIDs[3] == 1){
			Cursor_Fil_Col(9,62);
			enviar_frase(F_moniFalla);
			enviar_frase(F_PID4);
		}
		if(Vec_PIDs[4] == 1){
			Cursor_Fil_Col(11,62);
			enviar_frase(F_moniFalla);
			enviar_frase(F_PID5);
		}
		if(Vec_PIDs[5] == 1){
			Cursor_Fil_Col(13,62);
			enviar_frase(F_moniFalla);
			enviar_frase(F_PID6);
		}
		*/
	
	}
		if(Flecha_UP == 1){
			if(Flecha_P6 == 27){
				Flecha_P6 = 9;
				FinFLechas_P0();
			}else{
			if(Flecha_P6 == 1 || Flecha_P6 == 0 ){
				Flecha_P6 = 29;
				FinFLechas_P0();
				}else{
				Flecha_P6--;
				FinFLechas_P0();
				}
			}
		}
		if(Flecha_LOW == 1){
			if(Flecha_P6 == 9){
				Flecha_P6 = 27;
				FinFLechas_P0();
			}else{
			if(Flecha_P6 >= 32){
				Flecha_P6 = 1;
				FinFLechas_P0();
				}else{
				Flecha_P6++;
				FinFLechas_P0();
			    }	
			}	
		}	
		if(Flecha_Right == 1){
			if(Flecha_P6 < 19){
			Flecha_P6 += 9;
			FinFLechas_P0();
			}
		}
		if(Flecha_Left == 1){
			if(Flecha_P6 > 0 || Flecha_P6 > 1){
			Flecha_P6 -= 9; 
			FinFLechas_P0();
			}
		}
		
	if(C_Enter == 1){
		C_Enter = 0;
		Cont_Enter++;
			if(Cont_Enter >= 2){
				Ingresar_Valor = 0;
				Flag_Norepetir = 3;
				Cont_Enter = 0;
				valor = atoi((char*)Dato_RX);
				Enter_2 = 0;
			}else{	
				Ingresar_Valor = 1;
				c = 0;
				Flag_Norepetir = 255;
		}
	}
	
	if(Flag_Norepetir == 3){
		Flag_Norepetir = 0;
		switch(Flecha_P6){
		
			case 1:
			SP_TempZ1 = valor;
			guardar_eeprom(Temp_deseada_Z1,SP_TempZ1,2);
			PIDs(1);
			if(maq_estado_pantalla == 60){
				no_repetir = 1;
			}
			break;
			
			case 2:
			SP_CaudalZ1 = valor;
			guardar_eeprom(Caudal_vol_deseado_Z1 ,SP_CaudalZ1,1);
			PIDs(2);
			if(maq_estado_pantalla == 60){
				no_repetir = 1;
			}
			break;
			
			case 3:
			Aviso_TZ1 = valor;
			guardar_eeprom(Aviso_temp_z1,Aviso_TZ1,1);
			break;
			
			case 4:
			Alarma_TZ1 = valor;
			guardar_eeprom(Alarma_temp_z1,Alarma_TZ1,1);
			break;
			
			case 5:
			R_H1 = valor;
			guardar_eeprom(Rango_H1,R_H1,2);
			if(maq_estado_pantalla == 30){
				no_repetir = 1;
			}
			break;
			
			case 6:
			R_B1 = valor;
			guardar_eeprom(Rango_B1,R_B1,1);
			if(maq_estado_pantalla == 30){
				no_repetir = 1;
			}
			break;
			
			case 7:
			R_TT1  = valor;
			guardar_eeprom(Rango_TT1,R_TT1,2);
			if(maq_estado_pantalla == 40){
				no_repetir = 1;
			}
			break;
			
			case 8:
			R_TT2  = valor;
			guardar_eeprom(Rango_TT2,R_TT2,2);
			if(maq_estado_pantalla == 40){
				no_repetir = 1;
			}
			break;
			
			case 9:
			R_TT3  = valor;
			guardar_eeprom(Rango_TT3,R_TT3,2);
			if(maq_estado_pantalla == 40){
				no_repetir = 1;
			}
			break;
			
			case 10:
			SP_TempZ2 = valor;
			guardar_eeprom(Temp_deseada_Z2,SP_TempZ2,2);
			PIDs(3);
			if(maq_estado_pantalla == 61){
				no_repetir = 1;
			}
			break;
			
			case 11:
			SP_CaudalZ2 = valor;
			guardar_eeprom(Caudal_vol_deseado_Z2 ,SP_CaudalZ2,1);
			PIDs(4);
			if(maq_estado_pantalla == 61){
				no_repetir = 1;
			}
			break;
			
			case 12:
			Aviso_TZ2  = valor;
			guardar_eeprom(Aviso_temp_z2,Aviso_TZ2,1);
			break;
			
			case 13:
			Alarma_TZ2 = valor;
			guardar_eeprom(Alarma_temp_z2,Alarma_TZ2,1);
			break;
			
			case 14:
			R_H2 = valor;
			guardar_eeprom(Rango_H2,R_H2,2);
			if(maq_estado_pantalla == 31){
				no_repetir = 1;
			}
			break;
			
			case 15:
			R_B2 = valor;
			guardar_eeprom(Rango_B2,R_B2,1);
			if(maq_estado_pantalla == 31){
				no_repetir = 1;
			}
			break;
			
			case 16:
			R_TT4  = valor;
			guardar_eeprom(Rango_TT4,R_TT4,2);
			if(maq_estado_pantalla == 41){
				no_repetir = 1;
			}
			break;
			
			case 17:
			R_TT5  = valor;
			guardar_eeprom(Rango_TT5,R_TT5,2);
			if(maq_estado_pantalla == 41){
				no_repetir = 1;
			}
			break;
			
			case 18:
			R_TT6  = valor;
			guardar_eeprom(Rango_TT6,R_TT6,2);
			if(maq_estado_pantalla == 41){
				no_repetir = 1;
			}
			break;
			
			case 19:
			SP_VelocidadZ3 = valor;
			guardar_eeprom(Vel_cinta_transp,SP_VelocidadZ3,1);
			PIDs(6);
			if(maq_estado_pantalla == 62){
				no_repetir = 1;
			}
			break;
			
			case 20:
			SP_CaudalZ3 = valor;
			guardar_eeprom(Caudal_vol_deseado_Z3 ,SP_CaudalZ3,2);
			PIDs(5);
			if(maq_estado_pantalla == 62){
				no_repetir = 1;
			}
			break;
			
			case 21:
			Aviso_TZ3 = valor;
			guardar_eeprom(Aviso_temp_z3,valor,2);
			break;
			
			case 22:
			Alarma_TZ3 = valor;
			guardar_eeprom(Alarma_temp_z3,valor,2);
			break;
			
			case 23:
			R_C1 = valor;
			guardar_eeprom(Rango_C1,R_C1,1);
			if(maq_estado_pantalla == 32){
				no_repetir = 1;
			}
			break;
			
			case 24:
			R_B3 = valor;
			guardar_eeprom(Rango_B3,R_B3,2);
			if(maq_estado_pantalla == 32){
				no_repetir = 1;
			}
			break;
			
			case 25:
			R_TT7  = valor;
			guardar_eeprom(Rango_TT7,R_TT7,2);
			if(maq_estado_pantalla == 42){
				no_repetir = 1;
			}
			break;
			
			case 26:
			R_TT8  = valor;
			guardar_eeprom(Rango_TT8,R_TT8,2);
			if(maq_estado_pantalla == 42){
				no_repetir = 1;
			}
			break;
			
			case 27:
			Tiempo_PreCalentamiento = valor;
			guardar_eeprom(Tiempo_de_precalentamiento,valor,2);
			if(maq_estado_pantalla == 50){
				no_repetir = 1;
			}
			break;
			
			case 28:
			Tiempo_Calentamiento  = valor;
			guardar_eeprom(Tiempo_de_calentamiento,valor,2);
			if(maq_estado_pantalla == 50){
				no_repetir = 1;
			}
			break;
			
			case 29:
			Tiempo_Enfriamiento = valor;
			guardar_eeprom(Tiempo_de_enfriamiento,valor,2);
			if(maq_estado_pantalla == 50){
				no_repetir = 1;
			}
			break;
			
			case 30:
	
			if(Flag_Parada_Emergencia == 2){
				Led_Rojo = 1;
				if(Botton_Emergencia == 0){
					Chequear_PIDs_Fallas();
				}
			}else{
				
				//AVISOS
				for(uint8_t i = 0; i < 3; i++){
					if(Estado_Actual_WA[i] == 1){
						Estado_Actual_WA[i] = 3;
					}
				}
				//ALARMAS Temperatura
				for(uint8_t i = 0; i < 3; i++){
					if(Estado_Actual_WA[i] == 2){
						Estado_Actual_WA[i] = 4;
					}
				}
				alarmas_avisos_T();
				
				//Reconocer Sensores
				for(uint8_t i = 0; i < 8; i++){
					if(Cable_Cortado[i] == 1){
						Cable_Cortado[i] = 2;
					}
				}
				alarmas_avisos_Sens();
			}
			break;
		}
	}
}

void adecuacion_sensores(){
	
	uint16_t Temp = 0;
	int  Temp_Z1 = 0;
	int  Temp_Z2 = 0;
	int  Temp_Z3 = 0;
	PromedioZona_1 = 0;
	PromedioZona_2 = 0;
	PromedioZona_3 = 0;
	
	for(uint8_t N = 0; N < 8 ; N++){
		Temp = 0;
		
		if(Vec_SsT[N] < 5333){
			Temp = 9999;
			Cable_Cortado[N] = 1;
			}else{
			Cable_Cortado[N] = 0;
			switch(N){
				
				case 0:
				PromedioZona_1++;
				Temp = (int32_t)(Vec_SsT[0]-5333)*R_TT1/21333;
				Temp_Z1 += Temp;
				break;
				
				case 1:
				PromedioZona_1++;
				Temp = (int32_t)(Vec_SsT[1]-5333)*R_TT2/21333;
				Temp_Z1 += Temp;
				break;
				
				case 2:
				PromedioZona_1++;
				Temp = (int32_t)(Vec_SsT[2]-5333)*R_TT3/21333;
				Temp_Z1 += Temp;
				break;
				
				case 3:
				PromedioZona_2++;
				Temp = (int32_t)(Vec_SsT[3] - 5333)*R_TT4/21333;
				Temp_Z2 = Temp;
				break;
				
				case 4:
				PromedioZona_2++;
				Temp = (int32_t)(Vec_SsT[4]-5333)*R_TT5/21333;
				Temp_Z2 += Temp;
				break;
				
				case 5:
				PromedioZona_2++;
				Temp = (int32_t)(Vec_SsT[5]-5333)*R_TT6/21333;	
				Temp_Z2 += Temp;
				break;
				
				case 6:
				PromedioZona_3++;
				Temp = (int32_t)(Vec_SsT[6]-5333)*R_TT7/21333;
				Temp_Z3 += Temp;
				break;
				
				case 7:
				PromedioZona_3++;
				Temp = (int32_t)(Vec_SsT[7]-5333)*R_TT8/21333;
				Temp_Z3 += Temp;
				break;
			}
		}
		Vec_SsT[N] = Temp;
		Vec_Sens2[N] = Temp;
	}
	
	switch(PromedioZona_1){
		
		case 0:
		if(Estado_Horno == 2){
		Flag_Parada_Emergencia = 1;
		}
		break;
		
		case 1:
		if(Estado_Horno == 2){
		Flag_Parada_Controlada = 1;
		}
		break;
	}
	
	switch(PromedioZona_2){
		
		case 0:
		if(Estado_Horno == 2){
		Flag_Parada_Emergencia = 1;
		}
		break;
		
		case 1:
		if(Estado_Horno == 2){
		Flag_Parada_Controlada = 1;
		}
		break;
	}
	
	
	PromedioZona_1 = (Temp_Z1)/PromedioZona_1;
	
	if(Flag_Secuencia_Arranque != 1){
		comparaciones(SP_TempZ1, PromedioZona_1, Aviso_TZ1, Alarma_TZ1,1);
	}
	PromedioZona_2 = Temp_Z2/PromedioZona_2;
	
	if(Flag_Secuencia_Arranque != 1){
		comparaciones(SP_TempZ2, PromedioZona_2, Aviso_TZ2, Alarma_TZ2,2);
	}
	
	PromedioZona_3 = Temp_Z3/PromedioZona_3;
	
	if(Flag_Secuencia_Arranque != 1){
		comparaciones(0, PromedioZona_3, Aviso_TZ3, Alarma_TZ3,3);
	}
	
	
	for(uint8_t i = 0; i < 3; i++){
		Estado_Actual_ultimo[i] = Estado_Actual[i];
	}
	
	//if(Estado_Horno == 2){
		
		for(uint8_t i = 0; i < 3; i++){
			if(Estado_Actual_ultimo[i] != Estado_Anterior_WA[i]){
				for(uint8_t i = 0; i < 3; i++){
					Estado_Actual_WA[i] = Estado_Actual[i];
				}
				alarmas_avisos_T();
				break;
			}
		}

		for(uint8_t i = 0; i < 3; i++){
			Estado_Anterior_WA[i] = Estado_Actual_ultimo[i];
		}
	//}

	for(uint8_t i = 0; i < 8; i++){
		Estado_Actual_CC[i] = Cable_Cortado[i];
	}
		
		for(uint8_t i = 0; i < 8; i++){
			if(Estado_Actual_CC[i] != Estado_Anterior_CC[i]){
				alarmas_avisos_Sens();
				break;
			}
		}

	for(uint8_t i = 0; i < 8; i++){
		Estado_Anterior_CC[i] = Estado_Actual_CC[i];
	}
}

void alarmas_avisos_T(){			
	

	uint8_t hay_Alarma_NOR = 0;
	uint8_t hay_Alarma_SIR = 0;
	uint8_t hay_Aviso_NOR = 0;
	uint8_t hay_Aviso_SIR = 0;

	for(uint8_t i = 0; i <= 2; i++)
	{
		if(Estado_Actual_WA[i] == 2){
		hay_Alarma_NOR = 1;	
		}
		if(Estado_Actual_WA[i] == 4){
		hay_Alarma_SIR = 1;	
		}
		if(Estado_Actual_WA[i] == 1){
		hay_Aviso_NOR = 1;	
		}
		if(Estado_Actual_WA[i] == 3){
		hay_Aviso_SIR = 1;
		}
	}
	
	//Evaluar con Jerarquia
	if(hay_Alarma_NOR){
		
		Led_Rojo = 2;
		led_Ambar = 0;
		
	}else if(hay_Alarma_SIR){
		
		Led_Rojo = 1;
		led_Ambar = 0;
		
	}else{
		
		Led_Rojo = 0;
		
		if(hay_Aviso_NOR){
			
			led_Ambar = 2;
			
			}else if(hay_Aviso_SIR){
				
			led_Ambar = 1;
			
			}else{
			led_Ambar = 0;
			Led_Verde = 1;
		}
	}
	
}

void alarmas_avisos_Sens(){
		
		uint8_t Aviso_NOR = 0;
		uint8_t Aviso_SIR = 0;

		for(uint8_t i = 0; i <= 2; i++)
		{
			
			if(Cable_Cortado[i] == 1){
				Aviso_NOR = 1;
			}
			if(Cable_Cortado[i] == 2){
				Aviso_SIR = 1;
			}
		}
		
		if(Aviso_NOR){
				
			led_Ambar = 2;
				
			}else if(Aviso_SIR){
				
			led_Ambar = 1;
				
			}else{
			led_Ambar = 0;
				
		}
}

void comparaciones(uint16_t SP_Temp, uint16_t Prom, uint16_t Aviso, uint16_t Alarma, uint8_t Zona){
	
	if(Zona == 3){
		if(Prom >= Aviso){
			
			if(Prom >= Alarma){
				
				Estado_Actual[Zona-1] = 2;
				}else{
				
				Estado_Actual[Zona-1] = 1;
				}
			}else{
			Estado_Actual[Zona-1] = 0;
		}
		}else{
		if( Prom > (SP_Temp + Aviso*SP_Temp/100) || Prom < (SP_Temp - Aviso*SP_Temp/100)){
			
			if(Prom > (SP_Temp + Alarma*SP_Temp/100) || Prom < (SP_Temp - Alarma*SP_Temp/100)){
					
				Estado_Actual[Zona-1] = 2;
				}else{
				Estado_Actual[Zona-1] = 1;	
			}
			}else{
			Contador_Aviso++;
			Contador_Alarma++;
			Estado_Actual[Zona-1] = 0;
		}
	}
}

void Signals_Habilitacion(){
	
	if(Fin_Arranque == 1 && Hab_POSterior == 1){
		Hab_ANterior = 1;
		PORTC |= (1 << PC1);  // PC1 en 1
		}else{
		Hab_ANterior = 0;
		PORTC &= ~(1 << PC1);  // PC1 en 0
	}
}

void PIDs(uint8_t Actuador){
	
	if(Hab_ANterior == 1 && Hab_POSterior == 1){
		
		switch(Actuador){
			
			case 1:
			DAC_1A = 819 + ((uint32_t)(SP_TempZ1) * 3276U) / R_H1;
			Escribir_MAX5822 (DAC1, canalA, DAC_1A);
			Actuador = 0;
			break;
			
			case 2:
			DAC_1B = 819 + ((uint32_t)(SP_CaudalZ1) * 3276U) / R_B1;
			Escribir_MAX5822 (DAC1, canalB, DAC_1B);
			break;
			
			case 3:
			DAC_2A = 819 + ((uint32_t)(SP_TempZ2)* 3276U) / R_H2;
			Escribir_MAX5822 (DAC2, canalA, DAC_2A);
			break;
			
			case 4:
			DAC_2B = 819 + ((uint32_t)(SP_CaudalZ2)* 3276U) / R_B2;
			Escribir_MAX5822 (DAC2, canalB, DAC_2B);
			break;
			
			case 5:
			DAC_3A = 819 + ((uint32_t)(SP_CaudalZ3)* 3276U) / R_B3;
			Escribir_MAX5822 (DAC3, canalA, DAC_3A);
			break;
			
			case 6:
			DAC_3B = 819 + ((uint32_t)(SP_VelocidadZ3) * 3276U) / R_C1;
			Escribir_MAX5822 (DAC3, canalB, DAC_3B);
			break;
		}
		Actuador = 0;
	}
}

void Chequear_PIDs_Fallas(void){
	uint8_t estado_actual = PINB & 0x3F;   // Solo PB0–PB5

	uint8_t hay_falla = 0;

	for (uint8_t i = 0; i < 6; i++){
		if ((estado_actual & (1 << i)) == 0){
			Vec_PIDs[i] = 1;
			hay_falla = 1;
			}else{
			Vec_PIDs[i] = 0;
		}
	}
	
	if (hay_falla){
		Flag_Parada_Emergencia = 1;
	}else{
		Flag_Parada_Emergencia = 0;
		Estado_Horno = 0;
	}
}

void ParadaEmergencia(){
	
	Flag_Parada_Emergencia = 2;
	Estado_Horno = 4;
	Led_Rojo = 2;
	Actuador = 0;
	
	Fase = 0;
	Cont_Tiempo = 65535;
	Flag_Secuencia_Arranque = 0;
	
	DAC_1A = 0;
	Escribir_MAX5822 (DAC1, canalA, DAC_1A);
	
	DAC_1B = 0;
	Escribir_MAX5822 (DAC1, canalB, DAC_1B);
	
	DAC_2A = 0;
	Escribir_MAX5822 (DAC2, canalA, DAC_2A);
	
	DAC_2B = 0;
	Escribir_MAX5822 (DAC2, canalB, DAC_2B);
	
	DAC_3A = 0;
	Escribir_MAX5822 (DAC3, canalA, DAC_3A);
	
	DAC_3B = 0;
	Escribir_MAX5822 (DAC3, canalB, DAC_3B);
}

void ParadaControlada(){
	
	Flag_Secuencia_Arranque = 0;
	
	switch(Fase){
		
		case 0:
		Led_Verde = 2;
		Hab_ANterior = 0;
		PORTC &= ~(1 << PC1);  // PC1 en 0
		Estado_Horno = 3;
		Cont_Tiempo = 0;
		Fase = 1;
		break;
		
		case 1:
		if(Cont_Tiempo == (int32_t)120 * 1000 / (100 * SP_VelocidadZ3)){
			Hab_POSterior = 0;
			PORTC &= ~(1 << PC3);  // PC3 en 0
			Cont_Tiempo = 65535;
			Fase = 2;
		}
		break;
		
		case 2:
		DAC_1A = 819;
		Escribir_MAX5822 (DAC1, canalA, DAC_1A);
		
		DAC_2A = 819;
		Escribir_MAX5822 (DAC2, canalA, DAC_2A);
		
		DAC_1B = 819 + (uint32_t)SP_CaudalZ1 * 3 * 3276 / (4 * (uint32_t)R_B1);
		Escribir_MAX5822 (DAC1, canalB, DAC_1B);
		
		DAC_2B = 819 + (uint32_t)SP_CaudalZ2 * 3 * 3276 / (4 * (uint32_t)R_B2);
		Escribir_MAX5822 (DAC2, canalB, DAC_2B);
		
		DAC_3A = 819 + (uint32_t)SP_CaudalZ3 * 3 * 3276 / (4 * (uint32_t)R_B3);
		Escribir_MAX5822 (DAC3, canalA, DAC_3A);
		
		Cont_Tiempo = 0;
		Fase = 3;
		break;
		
		case 3:
		if(Cont_Tiempo == Tiempo_Enfriamiento){
			
			DAC_1B = 819 + (uint32_t)SP_CaudalZ1 * 3276 / (4 * (uint32_t)R_B1);
			Escribir_MAX5822 (DAC1, canalB, DAC_1B);
			
			DAC_2B = 819 + (uint32_t)SP_CaudalZ2 * 3276 / (4 * (uint32_t)R_B2);
			Escribir_MAX5822 (DAC2, canalB, DAC_2B);
			
			DAC_3A = 819 + (uint32_t)SP_CaudalZ3 * 3276 / (4 * (uint32_t)R_B3);
			Escribir_MAX5822 (DAC3, canalA, DAC_3A);
			
			Cont_Tiempo = 65535;
			Fase = 4;
		}
		break;
		
		case 4:
		if(PromedioZona_1  < 40 && PromedioZona_2 < 40 && PromedioZona_3 < 40){
			
			DAC_1B = 819;
			Escribir_MAX5822 (DAC1, canalB, DAC_1B);
			
			DAC_2B = 819;
			Escribir_MAX5822 (DAC2, canalB, DAC_2B);
			
			DAC_3A = 819;
			Escribir_MAX5822 (DAC3, canalA, DAC_3A);
			
			DAC_3B = 819;
			Escribir_MAX5822 (DAC3, canalB, DAC_3B);
			
			Led_Verde = 0;
			Flag_Parada_Controlada = 0;
			Estado_Horno = 0;
		}
		break;
	}
}

void SecuenciaArranque(){
	
	Flag_Parada_Controlada = 0;
	
	switch(Fase){
		
		case 0:
		Led_Rojo = 0;
		led_Ambar = 0;
		
		Led_Verde = 2;
		Estado_Horno = 1;
		DAC_1A = 819 + (int32_t)((((int32_t)SP_TempZ1 / 2) * 3276) / R_H1);
		Escribir_MAX5822 (DAC1, canalA, DAC_1A);
		
		DAC_2A = 819 + (int32_t)((((int32_t)SP_TempZ2 / 2) * 3276) / R_H2);
		Escribir_MAX5822 (DAC2, canalA, DAC_2A);
		Fase = 1;
		Cont_Tiempo = 65535;
		break;
		
		case 1:
		if(PromedioZona_1 > SP_TempZ1/2 && PromedioZona_2 > SP_TempZ2/2){
			Cont_Tiempo = 0;
			Flag_Norepetir_SA = 0;
			Fase = 2;
		}
		break;
		
		case 2:
		if(Cont_Tiempo==Tiempo_PreCalentamiento/2 && Flag_Norepetir_SA == 0){
			
			DAC_1B = 819 + (int32_t)(( (int32_t)SP_CaudalZ1 * 3 * 3276 ) / (4 * R_B1));
			Escribir_MAX5822 (DAC1, canalB, DAC_1B);
			
			DAC_2B = 819 + (int32_t)(( (int32_t)SP_CaudalZ2 * 3 * 3276 ) / (4 * R_B2));
			Escribir_MAX5822 (DAC2, canalB, DAC_2B);
			
			Flag_Norepetir_SA = 1;
		}
		
		if(Cont_Tiempo == (Tiempo_PreCalentamiento*2)/3 && Flag_Norepetir_SA == 1){
			
			DAC_3B = 819 + (int32_t)((((int32_t)SP_VelocidadZ3 * 3 / 4) * 3276) / R_C1);
			Escribir_MAX5822 (DAC3, canalB, DAC_3B);
			
			DAC_1A = 819 + (int32_t)((((int32_t)SP_TempZ1 * 3 / 4) * 3276) / R_H1);
			Escribir_MAX5822 (DAC1, canalA, DAC_1A);
			
			DAC_2A = 819 + (int32_t)((((int32_t)SP_TempZ2 * 3 / 4) * 3276) / R_H2);
			Escribir_MAX5822 (DAC2, canalA, DAC_2A);
			
			Flag_Norepetir_SA = 2;
		}
		
		if(Cont_Tiempo==(Tiempo_PreCalentamiento*3)/4 && Flag_Norepetir_SA == 2){
			
			DAC_3A = 819 + (int32_t)((((int32_t)SP_CaudalZ3 * 3 / 4) * 3276) / R_B3);
			
			Escribir_MAX5822 (DAC3, canalA, DAC_3A);
			
			Flag_Norepetir_SA = 3;
		}
		
		if(Cont_Tiempo == Tiempo_PreCalentamiento){
			if(Flag_Norepetir_SA == 3){
				
				DAC_1A = 819 + (int32_t)(((int32_t)SP_TempZ1 * 3276) / R_H1);
				Escribir_MAX5822 (DAC1, canalA, DAC_1A);
				
				DAC_1B = 819 + (int32_t)(((int32_t)SP_CaudalZ1 * 3276) / R_B1);
				Escribir_MAX5822 (DAC1, canalB, DAC_1B);
				
				DAC_2A = 819 + (int32_t)(((int32_t)SP_TempZ2 * 3276) / R_H2);
				Escribir_MAX5822 (DAC2, canalA, DAC_2A);
				
				DAC_2B = 819 + (int32_t)(((int32_t)SP_CaudalZ2 * 3276) / R_B2);
				Escribir_MAX5822 (DAC2, canalB, DAC_2B);
				
				DAC_3A = 819 + (int32_t)(((int32_t)SP_CaudalZ3 * 3276) / R_B3);
				Escribir_MAX5822 (DAC3, canalA, DAC_3A);
				
				DAC_3B = 819 + (int32_t)(((int32_t)SP_VelocidadZ3 * 3276) / R_C1);
				Escribir_MAX5822 (DAC3, canalB, DAC_3B);
				
				Cont_Tiempo = 65535;
				Fase = 3;
				Flag_Norepetir_SA = 0;
			}
		}
		break;
		
		case 3:
		if(PromedioZona_1 >= SP_TempZ1 && PromedioZona_2 >= SP_TempZ2){
			Cont_Tiempo = 0;
			Fase = 4;
		}
		break;
		
		case 4:
		if(Cont_Tiempo >= Tiempo_Calentamiento){
			Flag_Secuencia_Arranque = 0;
			Fin_Arranque = 1;
			Led_Verde = 1;
			Estado_Horno = 2;
			Fase = 0;
			Cont_Tiempo = 65535;
		}
		break;
	}
}

void leer_eeprom(){
	
	R_C1 = EEPROM_read(Rango_C1);
	R_H1 = EEPROM_read_uint16(Rango_H1);
	R_H2 = EEPROM_read_uint16(Rango_H2);
	R_B1 = EEPROM_read(Rango_B1);
	R_B2 = EEPROM_read(Rango_B2);
	R_B3 = EEPROM_read_uint16(Rango_B3);
	
	R_TT1 = EEPROM_read_uint16(Rango_TT1);
	R_TT2 = EEPROM_read_uint16(Rango_TT2);
	R_TT3 = EEPROM_read_uint16(Rango_TT3);
	R_TT4 = EEPROM_read_uint16(Rango_TT4);
	R_TT5 = EEPROM_read_uint16(Rango_TT5);
	R_TT6 = EEPROM_read_uint16(Rango_TT6);
	R_TT7 = EEPROM_read_uint16(Rango_TT7);
	R_TT8 = EEPROM_read_uint16(Rango_TT8);
	
	Tiempo_PreCalentamiento = EEPROM_read_uint16(Tiempo_de_precalentamiento);
	Tiempo_Calentamiento = EEPROM_read_uint16(Tiempo_de_calentamiento);
	Tiempo_Enfriamiento = EEPROM_read_uint16(Tiempo_de_enfriamiento);
	
	SP_TempZ1 = EEPROM_read_uint16(Temp_deseada_Z1);
	SP_TempZ2 = EEPROM_read_uint16(Temp_deseada_Z2);
	SP_CaudalZ1 = EEPROM_read(Caudal_vol_deseado_Z1);
	SP_CaudalZ2 = EEPROM_read(Caudal_vol_deseado_Z2);
	SP_CaudalZ3 = EEPROM_read_uint16(Caudal_vol_deseado_Z3);
	SP_VelocidadZ3 = EEPROM_read(Vel_cinta_transp);
	
	Aviso_TZ1 = EEPROM_read(Aviso_temp_z1);
	Aviso_TZ2 = EEPROM_read(Aviso_temp_z2);
	Aviso_TZ3 = EEPROM_read_uint16(Aviso_temp_z3);
	
	Alarma_TZ1 = EEPROM_read(Alarma_temp_z1);
	Alarma_TZ2 = EEPROM_read(Alarma_temp_z2);
	Alarma_TZ3 = EEPROM_read_uint16(Alarma_temp_z3);
}

void guardar_eeprom(uint8_t address,uint16_t data,uint8_t bytes){
	
	uint8_t leer_8b;
	uint8_t valor1;
	uint16_t leer_16b;
	
	if(bytes == 1){
		valor1 = data;
		leer_8b = EEPROM_read(address);
		if(leer_8b != valor1){
			EEPROM_write(address, data);
		}
	}
	
	if(bytes == 2){
		leer_16b = EEPROM_read_uint16(address);
		if(leer_16b != data){
			EEPROM_write_uint16(address, data);
		}
	}
}

// eeprom ************ eeprom ************* eeprom
void EEPROM_write(uint16_t address, uint8_t data)
{
	while (EECR & (1 << EEPE));   // Espera escritura previa

	EEAR = address;
	EEDR = data;

	EECR |= (1 << EEMPE);         // Habilita escritura
	EECR |= (1 << EEPE);          // Ejecuta escritura
}

uint8_t EEPROM_read(uint16_t address)
{
	while (EECR & (1 << EEPE));   // Espera escritura previa

	EEAR = address;
	EECR |= (1 << EERE);          // Ejecuta lectura

	return EEDR;
}

void EEPROM_write_uint16(uint16_t address, uint16_t data)
{
	EEPROM_write(address,     (uint8_t)(data & 0xFF));   // LSB
	EEPROM_write(address + 1, (uint8_t)(data >> 8));     // MSB
}

uint16_t EEPROM_read_uint16(uint16_t address)
{
	uint8_t low  = EEPROM_read(address);
	uint8_t high = EEPROM_read(address + 1);

	return (uint16_t)(low | (high << 8));
}


