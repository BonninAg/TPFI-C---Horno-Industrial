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
#include <string.h>

char buffer[20];
char imprimir [20];

uint8_t deleteME;
//uint16_t deleteME2 = 100;
//uint16_t deleteME3 = 5;

void USART_Transmit(unsigned char data);
void USART_SendString(char* s);

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
char Enter = 0;
char Exit = 0;

uint8_t Actualizar_Menu = 0;
uint8_t no_repetir = 1;
uint8_t cambio_menu = 0;

char renglon[4] = {0x80, 0xC0, 0x90, 0xD0};

PROGMEM const char espacio[] = " ";

uint8_t Estado_Planta =1;
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

void Pantalla_0_Uart(void);
void Pantalla_1_Uart(void);
void Pantalla_2_Uart(void);
void Pantalla_3_Uart(void);
void Pantalla_4_Uart(void);
void Pantalla_5_Uart(void);
void Pantalla_6_Uart(void);
void FinFLechas_P0  (void);

void adecuacion_sensores(void);
void comparaciones(uint16_t SP_Temp, uint16_t Prom, uint16_t Aviso, uint16_t Alarma, uint8_t Zona);
void PIDs(uint8_t Actuador);
void Chequear_PIDs_Fallas(void);
void ParadaEmergencia(void);
void ParadaControlada(void);
void SecuenciaArranque(void);
void Signals_Habilitacion(void);

void guardar_eeprom(uint8_t direccion,uint16_t valor,uint8_t bytes);
void leer_eeprom(void);

void EEPROM_write_byte(uint16_t address, uint8_t data);
uint8_t EEPROM_read_byte(uint16_t address);

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
PROGMEM const char Control			[]="\r\n\r\n";
PROGMEM const char Control_2		[]="\r\n";

PROGMEM const char Cursor_Home		[]="\033[H";
PROGMEM const char Borrar_Pantalla	[]="\033[2J\033[H";
PROGMEM const char ocultar_cursor	[]="\033[?25l";
//PROGMEM const char ocultar_cursor	[]="\033[?25h";
PROGMEM const char F_Exit			[]="Volver";
PROGMEM const char Dato_Recibido	[]="Valor Ingresado:";
PROGMEM const char guion			[]="\e[97m""----""\e[0m";
PROGMEM const char vacio_3			[]="          ";
PROGMEM const char vacio			[]="    ";
PROGMEM const char vacio_2       	[]=" ";
PROGMEM const char vacio_4       	[]="   ";
PROGMEM const char F_Error         	[]="\033[31m""\U0001F6AB""\033[0m""ERROR";
PROGMEM const char F_NocumpleR     	[]="";


PROGMEM const char F_Titulo_Menu_P0	[]="MEN\xC3\x9A CONFIGURACION\r\n\r\n";
PROGMEM const char F_SetPoints		[]="Set Points";
PROGMEM const char F_rActuadores	[]="Rangos Actuadores";
PROGMEM const char F_rSens			[]="Rangos Sensores";
PROGMEM const char F_Tiempos		[]="Tiempos";
PROGMEM const char F_Aviso_Alarma	[]="Avisos y Alarmas";


PROGMEM const char F_Unid_Temp		[]=" ""\xC2\xB0""C";
PROGMEM const char F_Unid_caudal	[]=" m\xC2\xB3/h";
PROGMEM const char F_Unid_cinta		[]=" cm/s]";
PROGMEM const char F_Unid_Time		[]=" s";
PROGMEM const char F_Unid_Porc      []=" %";

PROGMEM const char F_Titulo_P1		[]="SET POINTs\r\n\r\n\r\n";
PROGMEM const char F_spTemp_Z1		[]="Calefactor Zona 1: ";
PROGMEM const char F_spCV_Z1		[]="Forzador   Zona 1: ";
PROGMEM const char F_spTemp_Z2		[]="Calefactor Zona 2: ";
PROGMEM const char F_spCV_Z2		[]="Forzador   Zona 2: ";
PROGMEM const char F_spCV_Z3		[]="Forzador   Zona 3: ";
PROGMEM const char F_VelCinta		[]="Velocidad  Cinta : ";

PROGMEM const char F_Titulo_P2		[]="RANGOS ACTUADORES\r\n\r\n";
PROGMEM const char F_H1				[]="H1:[";
PROGMEM const char F_B1				[]="B1:[";
PROGMEM const char F_H2				[]="H2:[";
PROGMEM const char F_B2				[]="B2:[";
PROGMEM const char F_B3				[]="B3:[";
PROGMEM const char F_C1				[]="C1:[";
PROGMEM const char F_Unid_caudal_2	[]=" m\xC2\xB3/h]";


PROGMEM const char F_Titulo_P3		[]="RANGOS SENSORES\r\n\r\n";
PROGMEM const char F_TT1			[]="TT1:[";
PROGMEM const char F_TT2			[]="TT2:[";
PROGMEM const char F_TT3			[]="TT3:[";
PROGMEM const char F_TT4			[]="TT4:[";
PROGMEM const char F_TT5			[]="TT5:[";
PROGMEM const char F_TT6			[]="TT6:[";
PROGMEM const char F_TT7			[]="TT7:[";
PROGMEM const char F_TT8			[]="TT8:[";
PROGMEM const char F_Unid_Temp_MQ3	[]="""\xC2\xB0""C]";


PROGMEM const char F_Titulo_P4		[]="TIEMPOS\r\n\r\n";
PROGMEM const char F_TimePrecal		[]="Precalentamiento: ";
PROGMEM const char F_TimeCal		[]="Calentamiento   : ";
PROGMEM const char F_TimeEnf		[]="Enfriamiento    : ";

PROGMEM const char F_Titulo_P5		[]="AVISOS\r\n\r\n";
PROGMEM const char F_Titulo_P5_2	[]="ALARMAS\r\n\r\n";
PROGMEM const char F_WZ1			[]="T""\xC2\xB0"" Zona 1: ";
PROGMEM const char F_WZ2			[]="T""\xC2\xB0"" Zona 2: ";
PROGMEM const char F_WZ3			[]="T""\xC2\xB0"" Zona 3: ";


PROGMEM const char F_Titulo_P6		[]="Monitoreo";
PROGMEM const char F_Titulo_P6_2	[]="MONITOREO\r\n\r\n";
PROGMEM const char F_PromZ1			[]="T""\xC2\xB0""Promedio Zona 1:[";
PROGMEM const char F_PromZ2			[]="T""\xC2\xB0""Promedio Zona 2:[";
PROGMEM const char F_PromZ3			[]="T""\xC2\xB0""Promedio Zona 3:[";

PROGMEM const char F_Z1W			[]="Aviso T""\xC2\xB0"" Zona 1";
PROGMEM const char F_Z1A			[]="Alarma T""\xC2\xB0"" Zona 1";
PROGMEM const char F_Z2W			[]="Aviso T""\xC2\xB0"" Zona 2";
PROGMEM const char F_Z2A			[]="Alarma T""\xC2\xB0"" Zona 2";
PROGMEM const char F_Z3W			[]="Aviso T""\xC2\xB0"" Zona 3";
PROGMEM const char F_Z3A			[]="Alarma T""\xC2\xB0"" Zona 3";

PROGMEM const char F_Normal			[]="Normal";
PROGMEM const char F_Emergencia		[]="Parada Emergencia";
PROGMEM const char F_Controlada		[]="Parada Controlada";
PROGMEM const char F_Arranque		[]="Secuencia Arranque";

PROGMEM const char F_Fase     		[]="Fase: ";
PROGMEM const char F_Tiempo		    []="Tiempo: ";

PROGMEM const char F_moniFalla		[]="Falla ";
PROGMEM const char F_monSens		[]=" Sens";
PROGMEM const char F_moniZ1		    []=" en Zona 1";
PROGMEM const char F_moniZ2     	[]=" en Zona 2";
PROGMEM const char F_moniZ3		    []=" en Zona 3";

PROGMEM const char F_PID1		    []="PID H1";
PROGMEM const char F_PID2		    []="PID B1";
PROGMEM const char F_PID3		    []="PID H2";
PROGMEM const char F_PID4		    []="PID B2";
PROGMEM const char F_PID5		    []="PID B3";
PROGMEM const char F_PID6		    []="PID C3";

uint8_t Maquina_Estado = 20;
volatile uint8_t dato;
volatile char Dato_RX[9];
volatile uint8_t c = 0;
volatile uint8_t Flecha_LOW = 0;
volatile uint8_t Flecha_UP = 0;
char Numero[5];

uint8_t Flag_Norepetir = 0;
uint8_t Flecha_P0 = 0;
uint8_t C_Enter = 0;
uint8_t Escape = 0;
volatile uint8_t Flecha_ESC = 0;
uint8_t Set_Point_Mod = 0;
uint8_t Cont_Enter = 0;
volatile uint8_t Ingresar_Valor = 0;
uint16_t valor = 65535;

uint8_t Flecha_P1 = 0;
uint8_t Flecha_P2 = 0;
uint8_t Flecha_P3 = 0;
uint8_t Flecha_P4 = 0;
uint8_t Flecha_P5 = 0;
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


uint16_t Tiempo_PreCalentamiento = 10;
uint16_t Tiempo_Enfriamiento = 10;
uint16_t Tiempo_Calentamiento = 10;

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
uint16_t Vec_Pelado[8];


uint16_t  PromedioZona_1 = 0;
uint16_t  PromedioZona_2 = 0;
uint16_t  PromedioZona_3 = 0;

uint8_t	Led_Rojo = 0;
uint8_t	Contador_Alarma = 0;
uint8_t	Contador_Aviso = 0;
uint8_t	Led_Verde = 0;
uint8_t	Estado_Actual[3];
uint8_t	led_Ambar = 0;
uint8_t Flag_Aviso_CW = 0;

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
uint8_t Flag_Parada_Emergencia;
uint8_t estado_anterior_B;
uint8_t estado_anterior;
uint8_t estado_anterior_D;
uint8_t estado_actual_D;
volatile uint8_t estado_anterior_PC2 = 1;
uint8_t Falla_Sens[3];
volatile uint8_t Vec_PIDs[6] = {0};

uint8_t F_parControlada = 0;

int min;
int max;

uint8_t Flag_Norepetir_SA = 0;
uint8_t Fin_Arranque = 0;
uint8_t Estado_Horno = 0;
volatile uint8_t Contador_Temp = 0;

#endif

int main(void){
					
	#ifndef Configuracion
	#define Configuracion
	
				
									  //dni,	graficos,				hijos,			hermanos,		opcionMax,  opcionMin,  hermanosMax
	struct pantallaPPL principal1 =		{100, &Pantalla_Principal_1, {90, 80, 70, 60},	{100, 101},		 3,			 0,			 1};
	struct pantallaPPL principal2 =		{101, &Pantalla_Principal_2, {50, 40, 30},		{100, 101},		 2,			 0,			 1};
	
	struct pantallaPPL avisos =			{80,  &Menu_Avisos,			 {0, 81, 82},		{0},			 2,			 1,			 0};
	struct pantallaPPL alarmas =		{70,  &Menu_Alarmas,		 {0, 71, 72, 73},	{0},		 	 3,			 1,			 0};
	
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
 // Prescaler = 1 (TWPS = 0)
 TWSR &= ~((1<<TWPS0) | (1<<TWPS1));
 // Bit rate para 100 kHz
 TWBR = 72;
 // Habilitar TWI
 TWCR = (1<<TWEN);
 /*-------------------------------------------------------------*/

 /*------------- Timer1 --------------*/
 // Modo CTC
 TCCR1A = 0;
 TCCR1B = (1 << WGM12);
 // Valor de comparación para 1 segundo
 OCR1A = 15624;
 // Habilitar interrupción por compare match A
 TIMSK1 = (1 << OCIE1A);
 // Prescaler 1024
 TCCR1B |= (1 << CS12) | (1 << CS10);
 /*-----------------------------------*/

 /*------------- INT0 --------------*/
 //int 0
 DDRD &= ~(1 << PD2);					// PD2 (INT0) como entrada
 PORTD |= (1 << PD2);					// Pull-up habilitado
 EICRA |= (1 << ISC01) | (1 << ISC00);	// Configurar INT0 en flanco ascendente (0 ? 1)
 EIMSK |= (1 << INT0);					// Habilitar INT0
 /*---------------------------------*/


 /*------------- INT1 --------------*/
 DDRD &= ~(1<<DDD3);						// PD3 como entrada
 EICRA |= (1<<ISC11) | (1<<ISC10);		 // Flanco ascendente
 EIMSK |= (1<<INT1);						// Habilitar INT1
 /*---------------------------------*/


 /*-------------------------- Usart ----------------------------*/
    uint16_t ubrr = 8; // 115200 @16MHz modo normal

    // Baudrate
    UBRR0H = (ubrr >> 8);
    UBRR0L = ubrr;

    // Modo normal
    UCSR0A = 0;

    // Habilitar TX, RX y la INTERRUPCIÓN DE RECEPCIÓN
    UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);

    // Frame: 8N1
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
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
 PCMSK0 |= 0x3F;				// Habilitar PB0–PB5
 estado_anterior_B = PINB;	// Guardar estado inicial
 
 // Patitas led RGB
 DDRD |= (1 << PD6) | (1 << PD7);
 /*------------------ FIN CONFIGURACION PINES --------------------*/

	sei();
	
	//low thres
	cofigurar_ads1115(ads1115_IP_VCC_write, ads1115_LoThresh_Reg, 0x00, 0x00);
	//hight thres
	cofigurar_ads1115(ads1115_IP_VCC_write, ads1115_HiThresh_Reg, 0x80, 0x00);

	//low thres
	cofigurar_ads1115(ads1115_IP_GND_write, ads1115_LoThresh_Reg, 0x00, 0x00);
	//hight thres
	cofigurar_ads1115(ads1115_IP_GND_write, ads1115_HiThresh_Reg, 0x80, 0x00);


	//secuencia de Power-up device.
	Escribir_MAX5822 (DAC1, 0b11110000, 0b000000001100);
	Escribir_MAX5822 (DAC2, 0b11110000, 0b000000001100);
	Escribir_MAX5822 (DAC3, 0b11110000, 0b000000001100);

	Iniciar_LCD();
	#endif

    // USART_SendString("Fin Config\r\n");//************************************ ACÁ CAMBIE A COMENTARIO LO DE LA USARTT
	leer_eeprom();
 
    while (1) {
	
	if(Flag_Secuencia_Arranque == 1){
		SecuenciaArranque();
	}
	
	if(Flag_Parada_Controlada == 1){
		ParadaControlada();
	}
	
	if(Flag_Parada_Emergencia == 1){
		ParadaEmergencia();
	}
	
	switch(Maquina_Estado){
		
		case 20:
		Pantalla_0_Uart();
		break;
		
		case 21:
		Pantalla_1_Uart();
		break;
		
		case 22:
		Pantalla_2_Uart();
		break;
		
		case 23:
		Pantalla_3_Uart();
		break;
		
		case 24:
		Pantalla_4_Uart();
		break;
		
		case 25:
		Pantalla_5_Uart();
		break;
		
		case 26:
		Pantalla_6_Uart();
		break;
		
	}
	Chequear_PIDs_Fallas();
	Signals_Habilitacion();
	
/*-------------------------------------------Teclado------------------------------------------*/	
		if(Habilitar_Teclado == 1){
			Habilitar_Teclado =0;		
			deleteME = Convertir_Keypad (valor_adc, &Menu, &Cursor, &Enter, &Exit, &cambio_menu);
			Actualizar_Menu = 0;
					
		}//teclado
/*---------------------------------------------------------------------------------------------*/		
	

while (Actualizar_Menu < 2) {
	Actualizar_Menu += 1;
	
/*
//	sprintf(imprimir, "maqEst: %u\r\n", maq_estado_pantalla);             ACA CAMBIEN A COMENTARIO LO DE LA USART
//	USART_SendString(imprimir);

	sprintf(imprimir, "cursor: %u\r\n", Cursor);
	USART_SendString(imprimir);
	sprintf(imprimir, "Menu: %u\r\n", Menu);
	USART_SendString(imprimir);
*/	
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
			Menu_General(Estado_Planta);
		}
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

/*------------------------Alarmas Sensores---------------------------------*/
/*-------------------------------------------------------------------------*/

/*------------------------Alarmas temperaturas---------------------------------*/
if (maq_estado_pantalla == 72){
	if (no_repetir)	{
		no_repetir =0;
		Menu_Alarmas_Temperatura();
	}
	if (Exit == 1){
		Exit = 0;
		no_repetir =1;
		maq_estado_pantalla = alarmas.dni;
	}
}
/*--------------------------------------------------------------------*/

/*------------------------Alarmas PIDS---------------------------------*/
if (maq_estado_pantalla == 73){
	if (no_repetir)	{
		no_repetir =0;
		Menu_Alarmas_PIDS();
	}
	if (Exit == 1){
		Exit = 0;
		no_repetir =1;
		maq_estado_pantalla = alarmas.dni;
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
	//UART_enviar_char(dato);
	
	switch(dato)
	{
		
		case 'A':
		case 'D':
		Flecha_UP = 1;
		Escape = 0;
		Flecha_ESC = 0;
		break;
		
		case 'B':
		case 'C':
		Flecha_LOW = 1;
		Escape = 0;
		Flecha_ESC = 0;
		
		break;
		
		case 13:
		case 32:
		C_Enter = 1;
		break;
		
		case 27:
		Escape = 1;
		Flecha_ESC = 1;
		break;
	}
	if(Ingresar_Valor == 1){
		Enter_2 = 1;
		if(dato == 8 || dato == 127){
			Cursor_Fil_Col(20,48+c);
			enviar_frase(vacio_2);
			c--;
		}
		else{
			if(c < 9){
				if(c == 0){
					enviar_frase(vacio_3);
				}
				Dato_RX[c++] = dato;
				Cursor_Fil_Col(20,48+c);
				UART_enviar_char(dato);
			}
		}
	}
}

ISR(TIMER1_COMPA_vect) {
//USART_SendString("timer1\r\n");//**************************************

//----------leer los ads1115----------------
	Habilitar_LeerTemperatura = 1;
	Canal_Temp = 0;
	
for (char i = 0; i < 8; i++){
//	sprintf(buffer, "Temp: %u\r\n", Vector_Temperaturas[i]);	// Convertir el valor numérico a una cadena de texto
//	USART_SendString(buffer);							// Enviar el texto por el puerto serie
}	
//------------------------------------------



/*------------------dacs----------------------------
	contador += 10;
	if (contador > 4096){
		contador = 0;
	}
	
	Escribir_MAX5822 (DAC1, canalA, contador);
	Escribir_MAX5822 (DAC1, canalB, contador);
	
	Escribir_MAX5822 (DAC2, canalA, contador);
	Escribir_MAX5822 (DAC2, canalB, contador);
	
	Escribir_MAX5822 (DAC3, canalA, contador);
	Escribir_MAX5822 (DAC3, canalB, contador);
---------------------------------------------------*/

	if(Cont_Tiempo != 65535){
		Cont_Tiempo++;
	}
	
	Flag_adecuacion_sensores = 1;
	tiempo++;
	if(Estado_Horno == 2){
		
		if(Led_Rojo == 0){
			
			switch(led_Ambar){
				case 1:
				PORTD |= (1 << PD6);
				PORTD |= (1 << PD7);
				break;
				case 2:
				PORTD ^= (1 << PD6);
				PORTD ^= (1 << PD7);
				break;
			}
			switch(Flag_Aviso_CW){
				case 1:
				PORTD |= (1 << PD6);
				PORTD |= (1 << PD7);
				break;
				case 2:
				PORTD ^= (1 << PD6);
				PORTD ^= (1 << PD7);
				break;
			}
		}
		switch(Led_Rojo){
			case 1:
			// rojo en alto PD7
			PORTD |= (1 << PD7);
			break;
			case 2:
			PORTD ^= (1 << PD7);
			break;
		}	
	}
	if(led_Ambar == 0 && Flag_Aviso_CW == 0 && Led_Rojo == 0){
		switch(Led_Verde){
			case 1:
			// verde en alto PD6
			PORTD |= (1 << PD6);
			break;
			case 2:
			// cambio de estado PD6
			PORTD ^= (1 << PD6);
			break;
		}
	}
	if(Maquina_Estado > 20 && Enter_2 == 0 && Maquina_Estado != 26){
		
		if(tiempo == 2){
			Cursor_Fil_Col(20,49);
			enviar_frase(guion);
		}
		else{
			Cursor_Fil_Col(20,49);
			enviar_frase(vacio);
		}
	}
	
	if(tiempo == 2){
		tiempo = 0;
	}
}

ISR(PCINT0_vect)
{
	uint8_t estado_actual = PINB;
	uint8_t mask = 0x3F;
	
	// Detectar cambios
	uint8_t cambios = (estado_actual ^ estado_anterior_B) & mask;

	// Detectar los que bajaron a 0 (fallas)
	uint8_t bajaron_a_0 = cambios & (~estado_actual);

	if (bajaron_a_0)
	{
		Flag_Parada_Emergencia = 1;

		if (bajaron_a_0 & (1 << PB0)) Vec_PIDs[0] = 1;
		if (bajaron_a_0 & (1 << PB1)) Vec_PIDs[1] = 1;
		if (bajaron_a_0 & (1 << PB2)) Vec_PIDs[2] = 1;
		if (bajaron_a_0 & (1 << PB3)) Vec_PIDs[3] = 1;
		if (bajaron_a_0 & (1 << PB4)) Vec_PIDs[4] = 1;
		if (bajaron_a_0 & (1 << PB5)) Vec_PIDs[5] = 1;
	}

	estado_anterior_B = estado_actual;
	
}

ISR(PCINT1_vect)
{
	uint8_t estado_PC2_actual;

	// Leer estado actual de PC2
	estado_PC2_actual = (PINC & (1 << PC2)) ? 1 : 0;

	// Detectar cambio
	if (estado_PC2_actual != estado_anterior_PC2)
	{
		if (estado_PC2_actual == 0)
		{
			// ?? FLANCO DESCENDENTE (botón presionado)
			Hab_POSterior = 1;// ---- TU CÓDIGO ACÁ ----
			PORTC |= (1 << PC3);  // PC3 en 1
		}
		else
		{
			// ?? FLANCO ASCENDENTE (botón liberado)
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

		if(cambio)
		{
			if( (estado_actual_D & (1 << PD4)) == 0 )
			{
				Flag_Secuencia_Arranque = 1;
				//bajo a 0
			}
			else
			{
				Flag_Parada_Controlada = 1;
				//subio a 1
			}
		}
		estado_anterior_D = estado_actual_D;
}

ISR(INT0_vect) {
	//USART_SendString("int0\r\n");
	Flag_Parada_Emergencia = 1;
}

ISR(INT1_vect){
	ADCSRA |= (1<<ADSC);   	// Iniciar conversión
}

ISR(ADC_vect) {
	valor_adc = ADC;  // lee ADCL + ADCH automáticamente
	Habilitar_Teclado = 1;
	
//	sprintf(imprimir, "ADC: %u\r\n", valor_adc);	// Convertir el valor numérico a una cadena de texto
//	USART_SendString(imprimir);						// Enviar el texto por el puerto serie	
}	

/*-------------------------------------------Pantallas------------------------------------------*/	
/*
if (Actualizar_Menu == 1){
	Actualizar_Menu = 0;

		if (maq_estado_pantalla == 100 || maq_estado_pantalla == 99){
				maq_estado_pantalla=100;
			if (no_repetir == 0){
				no_repetir = 1;
				Pantalla_Principal_1();
			}
		
			if (cursor == 0)
				cursor = Opcion1;
			if (cursor == 5)
				cursor = Opcion4;	
				
			Escribir_Comando_LCD(Linea1_);		
			if (cursor == Opcion1){
				Escribir_Caracter_LCD(Right_Arrow);			
			}else
				Escribir_FraseFlash_LCD(espacio);
				
			Escribir_Comando_LCD(Linea2_);
			if (cursor == Opcion2){	
				Escribir_Caracter_LCD(Right_Arrow);
			}else
				Escribir_FraseFlash_LCD(espacio);	
			
			Escribir_Comando_LCD(Linea3_);		
			if (cursor == Opcion3){
				Escribir_Caracter_LCD(Right_Arrow);
			}else
				Escribir_FraseFlash_LCD(espacio);	
				
			Escribir_Comando_LCD(Linea4_);	
			if (cursor == Opcion4){
				Escribir_Caracter_LCD(Right_Arrow);
			}else
				Escribir_FraseFlash_LCD(espacio);		
			
		}
		
		if (maq_estado_pantalla == 101 || maq_estado_pantalla == 102){
			maq_estado_pantalla=101;
			if (no_repetir == 1){
				no_repetir = 0;
				Pantalla_Principal_2();
			}
		
			if (cursor == 0)
			cursor = Opcion1;
			if (cursor == 4)
			cursor = Opcion3;
			
			Escribir_Comando_LCD(Linea1_);
			if (cursor == Opcion1){
				Escribir_Caracter_LCD(Right_Arrow);
			}else
				Escribir_FraseFlash_LCD(espacio);
			
			Escribir_Comando_LCD(Linea2_);	
			if (cursor == Opcion2){
				Escribir_Caracter_LCD(Right_Arrow);
			}else
				Escribir_FraseFlash_LCD(espacio);
			
			Escribir_Comando_LCD(Linea3_);	
			if (cursor == Opcion3){
				Escribir_Caracter_LCD(Right_Arrow);
			}else
				Escribir_FraseFlash_LCD(espacio);

		
		}
}//actualizar menu
*/
/*---------------------------------------------------------------------------------------------*/

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
	
	// \033 es el ESC (0x1B)
	sprintf(buffer, "\033[%d;%dH", fila, columna);
	
	UART_enviar_string(buffer);
}

void FinFLechas_P0(){
	Flag_Norepetir = 0;
	Flecha_LOW = 0;
	Flecha_UP = 0;
	Cont_Enter = 0;
	Ingresar_Valor = 0;
	Flecha_ESC = 0;
	
}

void Pantalla_0_Uart(){
	
	//codigo de la pantalla_0
	
	if (Flag_Norepetir == 0){

		enviar_frase(ocultar_cursor);
		enviar_frase(Borrar_Pantalla);
		enviar_frase(F_Titulo_Menu_P0);
		
		if(Flecha_P0 == 1){
			enviar_frase(F_arrowLeft);
			enviar_frase(F_SetPoints);
			enviar_frase(F_arrowRigth);
			enviar_frase(Control);
			}else{
			enviar_frase(F_SetPoints);
			enviar_frase(Control);
		}
		if(Flecha_P0 == 2){
			enviar_frase(F_arrowLeft);
			enviar_frase(F_rActuadores);
			enviar_frase(F_arrowRigth);
			enviar_frase(Control);
			}else {
			enviar_frase(F_rActuadores);
			enviar_frase(Control);
		}
		if(Flecha_P0 == 3){
			enviar_frase(F_arrowLeft);
			enviar_frase(F_rSens);
			enviar_frase(F_arrowRigth);
			enviar_frase(Control);
			}else {
			enviar_frase(F_rSens);
			enviar_frase(Control);
		}
		if(Flecha_P0 == 4){
			enviar_frase(F_arrowLeft);
			enviar_frase(F_Tiempos);
			enviar_frase(F_arrowRigth);
			enviar_frase(Control);
			}else {
			enviar_frase(F_Tiempos);
			enviar_frase(Control);
		}
		if(Flecha_P0 == 5){
			enviar_frase(F_arrowLeft);
			enviar_frase(F_Aviso_Alarma);
			enviar_frase(F_arrowRigth);
			enviar_frase(Control);
			}else {
			enviar_frase(F_Aviso_Alarma);
			enviar_frase(Control);
		}
		if(Flecha_P0 == 6){
			enviar_frase(F_arrowLeft);
			enviar_frase(F_Titulo_P6);
			enviar_frase(F_arrowRigth);
			enviar_frase(Control);
			}else {
			enviar_frase(F_Titulo_P6);
			enviar_frase(Control);
		}
		
		Flag_Norepetir = 1;
		Flecha_LOW = 0;
		Flecha_UP = 0;
	}
	
	if (C_Enter == 1){
		C_Enter = 0;
		Flecha_ESC = 0;
		Escape = 0;
		Maquina_Estado = Flecha_P0 + 20;
		Flag_Norepetir = 0;
		Flecha_P1 = 0;
		Flecha_P2 = 0;
		Flecha_P3 = 0;
		Flecha_P4 = 0;
		Flecha_P5 = 0;
	}
	if (Flecha_UP == 1){
		if(Flecha_P0 == 1 || Flecha_P0 == 0 )
		{
			Flecha_P0 = 6;
			FinFLechas_P0();
			}else{
			Flecha_P0--;
			FinFLechas_P0();
		}
	}
	if(Flecha_LOW == 1){
		if(Flecha_P0 == 6)
		{
			Flecha_P0 = 1;
			FinFLechas_P0();
			}else{
			Flecha_P0++;
			FinFLechas_P0();
		}
	}
}

void Pantalla_1_Uart(){
	
	#ifndef Flag_Norepetir_P1
	#define Flag_Norepetir_P1
	
	if (Flag_Norepetir == 0){
		
		enviar_frase(Borrar_Pantalla);
		
		enviar_frase(F_Titulo_P1);
		
		Cursor_Fil_Col(20,33);
		enviar_frase(Dato_Recibido);
		Cursor_Fil_Col(3,0);
		
		if(Flecha_ESC == 1){
			Flecha_P1 = 0;
		}
		if(Flecha_P1 == 1){
			
			enviar_frase(F_arrowLeft);
			enviar_frase(F_spTemp_Z1);
			
			sprintf(Numero, "%d", SP_TempZ1);
			UART_enviar_string(Numero);
			
			enviar_frase(F_Unid_Temp);
			enviar_frase(F_arrowRigth);
			enviar_frase(Control);
			}else{
			enviar_frase(F_spTemp_Z1);
			
			sprintf(Numero, "%d", SP_TempZ1);
			UART_enviar_string(Numero);
			
			enviar_frase(F_Unid_Temp);
			enviar_frase(Control);
		}
		
		if(Flecha_P1 == 2){
			enviar_frase(F_arrowLeft);
			enviar_frase(F_spCV_Z1);
			
			sprintf(Numero, "%d", SP_CaudalZ1);
			UART_enviar_string(Numero);
			
			enviar_frase(F_Unid_caudal);
			enviar_frase(F_arrowRigth);
			enviar_frase(Control);
			enviar_frase(Control);
			}else{
			enviar_frase(F_spCV_Z1);
			
			sprintf(Numero, "%d", SP_CaudalZ1);
			UART_enviar_string(Numero);
			
			enviar_frase(F_Unid_caudal);
			enviar_frase(Control);
			enviar_frase(Control);
		}
		
		
		if(Flecha_P1 == 3){
			enviar_frase(F_arrowLeft);
			enviar_frase(F_spTemp_Z2);
			
			sprintf(Numero, "%d", SP_TempZ2);
			UART_enviar_string(Numero);
			
			enviar_frase(F_Unid_Temp);
			enviar_frase(F_arrowRigth);
			enviar_frase(Control);
			}else{
			enviar_frase(F_spTemp_Z2);
			
			sprintf(Numero, "%d", SP_TempZ2);
			UART_enviar_string(Numero);
			
			enviar_frase(F_Unid_Temp);
			enviar_frase(Control);
		}
		
		if(Flecha_P1 == 4){
			enviar_frase(F_arrowLeft);
			enviar_frase(F_spCV_Z2);
			
			sprintf(Numero, "%d", SP_CaudalZ2);
			UART_enviar_string(Numero);
			
			enviar_frase(F_Unid_caudal);
			enviar_frase(F_arrowRigth);
			enviar_frase(Control);
			enviar_frase(Control);
			}else{
			enviar_frase(F_spCV_Z2);

			sprintf(Numero, "%d", SP_CaudalZ2);
			UART_enviar_string(Numero);
			
			enviar_frase(F_Unid_caudal);
			enviar_frase(Control);
			enviar_frase(Control);
		}
		
		if(Flecha_P1 == 5){
			enviar_frase(F_arrowLeft);
			enviar_frase(F_spCV_Z3);
			
			sprintf(Numero, "%d", SP_CaudalZ3);
			UART_enviar_string(Numero);
			
			enviar_frase(F_Unid_caudal);
			enviar_frase(F_arrowRigth);
			enviar_frase(Control);
			}else{
			enviar_frase(F_spCV_Z3);
			
			sprintf(Numero, "%d", SP_CaudalZ3);
			UART_enviar_string(Numero);
			
			enviar_frase(F_Unid_caudal);
			enviar_frase(Control);
		}
		
		if(Flecha_P1 == 6){
			enviar_frase(F_arrowLeft);
			enviar_frase(F_VelCinta);
			
			sprintf(Numero, "%d", SP_VelocidadZ3);
			UART_enviar_string(Numero);
			
			enviar_frase(F_Unid_cinta);
			enviar_frase(F_arrowRigth);
			enviar_frase(Control);
			}else{
			enviar_frase(F_VelCinta);
			
			sprintf(Numero, "%d", SP_VelocidadZ3);
			UART_enviar_string(Numero);
			
			enviar_frase(F_Unid_cinta);
			enviar_frase(Control);
		}
		enviar_frase(Control_2);
		
		if(Flecha_ESC == 1){
			enviar_frase(F_arrowLeft);
			enviar_frase(F_Exit);
			enviar_frase(F_arrowRigth);
			Flecha_ESC = 0;
			}else{
			enviar_frase(F_Exit);
		}

		
		Flag_Norepetir = 1;
	}
	#endif
	
	Flag_Norepetir_P1

	if(C_Enter == 1){
		
		C_Enter = 0;
		Cont_Enter++;
		
		if(Escape == 1){
			Flag_Norepetir = 0;
			Maquina_Estado = 20;
			Escape = 0;
			Cont_Enter = 0;
			Ingresar_Valor = 0;
			}else{
			
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
	}
	
	if (Flecha_UP == 1){
		if(Flecha_P1 == 1 || Flecha_P1 == 0 ){
			Flecha_P1 = 6;
			FinFLechas_P0();
			}else{
			Flecha_P1--;
			FinFLechas_P0();
		}
	}
	if(Flecha_LOW == 1){
		if(Flecha_P1 == 6){
			Flecha_P1 = 1;
			FinFLechas_P0();
			}else{
			Flecha_P1++;
			FinFLechas_P0();
		}
	}
	
	if(Flecha_ESC == 1){
		Flag_Norepetir = 0;
	}

	if(Flag_Norepetir == 3){
		Flag_Norepetir = 0;
		switch(Flecha_P1){
			
			case 1:
				SP_TempZ1 = valor;
				guardar_eeprom(Temp_deseada_Z1,SP_TempZ1,2);
				PIDs(1);
				Flag_Norepetir = 0;
			break;
			
			case 2:
				SP_CaudalZ1 = valor;
				guardar_eeprom(Caudal_vol_deseado_Z1 ,SP_CaudalZ1,1);
				PIDs(2);
				Flag_Norepetir = 0;
			break;
			
			case 3:
				SP_TempZ2 = valor;
				guardar_eeprom(Temp_deseada_Z2,SP_TempZ2,2);
				PIDs(3);
				Flag_Norepetir = 0;
			break;
			
			case 4:
				SP_CaudalZ2 = valor;
				guardar_eeprom(Caudal_vol_deseado_Z2 ,SP_CaudalZ2,1);
				PIDs(4);
				Flag_Norepetir = 0;
			break;
			
			case 5:
				SP_CaudalZ3 = valor;
				guardar_eeprom(Caudal_vol_deseado_Z3 ,SP_CaudalZ3,2);
				PIDs(5);
				Flag_Norepetir = 0;
			break;
			
			case 6:
				SP_VelocidadZ3 = valor;
				guardar_eeprom(Vel_cinta_transp,SP_VelocidadZ3,1);
				PIDs(6);
				Flag_Norepetir = 0;
			break;
		}
	}
}

void Pantalla_2_Uart(){
	
	#ifndef Flag_Norepetir_P2
	#define Flag_Norepetir_P2
	
	if (Flag_Norepetir == 0){
		
		enviar_frase(Borrar_Pantalla);
		enviar_frase(F_Titulo_P2);
		
		Cursor_Fil_Col(20,33);
		enviar_frase(Dato_Recibido);
		Cursor_Fil_Col(3,0);
		
		if(Flecha_ESC == 1){
			Flecha_P2 = 0;
		}
		
		if(Flecha_P2 == 1){
			enviar_frase(F_arrowLeft);
			
			enviar_frase(F_H1);
			sprintf(Numero, "%4d", R_H1);
			UART_enviar_string(Numero);
			enviar_frase(F_Unid_Temp_MQ3);
			
			enviar_frase(F_arrowRigth);
			enviar_frase(Control);
			}else{
			enviar_frase(F_H1);
			sprintf(Numero, "%4d", R_H1);
			UART_enviar_string(Numero);
			enviar_frase(F_Unid_Temp_MQ3);
			
			enviar_frase(Control);
		}
		
		if(Flecha_P2 == 2){
			enviar_frase(F_arrowLeft);
			enviar_frase(F_B1);
			sprintf(Numero, "%4d", R_B1);
			UART_enviar_string(Numero);
			enviar_frase(F_Unid_caudal_2);
			
			enviar_frase(F_arrowRigth);
			enviar_frase(Control);
			enviar_frase(Control_2);
			}else{
			enviar_frase(F_B1);
			sprintf(Numero, "%4d", R_B1);
			UART_enviar_string(Numero);
			enviar_frase(F_Unid_caudal_2);
			
			enviar_frase(Control);
			enviar_frase(Control_2);
		}
		
		
		if(Flecha_P2 == 3){
			enviar_frase(F_arrowLeft);
			enviar_frase(F_H2);
			sprintf(Numero, "%4d", R_H2);
			UART_enviar_string(Numero);
			enviar_frase(F_Unid_Temp_MQ3);
			
			enviar_frase(F_arrowRigth);
			enviar_frase(Control);
			}else{
			enviar_frase(F_H2);
			sprintf(Numero, "%4d", R_H2);
			UART_enviar_string(Numero);
			enviar_frase(F_Unid_Temp_MQ3);
			
			enviar_frase(Control);
		}
		
		if(Flecha_P2 == 4){
			enviar_frase(F_arrowLeft);
			enviar_frase(F_B2);
			sprintf(Numero, "%4d", R_B2);
			UART_enviar_string(Numero);
			enviar_frase(F_Unid_caudal_2);
			
			enviar_frase(F_arrowRigth);
			enviar_frase(Control);
			enviar_frase(Control_2);
			}else{
			enviar_frase(F_B2);
			sprintf(Numero, "%4d", R_B2);
			UART_enviar_string(Numero);
			enviar_frase(F_Unid_caudal_2);
			
			enviar_frase(Control);
			enviar_frase(Control_2);
		}
		
		if(Flecha_P2 == 5){
			enviar_frase(F_arrowLeft);
			enviar_frase(F_B3);
			sprintf(Numero, "%4d", R_B3);
			UART_enviar_string(Numero);
			enviar_frase(F_Unid_caudal_2);
			
			enviar_frase(F_arrowRigth);
			enviar_frase(Control);
			}else{
			enviar_frase(F_B3);
			sprintf(Numero, "%4d", R_B3);
			UART_enviar_string(Numero);
			enviar_frase(F_Unid_caudal_2);
			
			enviar_frase(Control);
		}
		
		if(Flecha_P2 == 6){
			enviar_frase(F_arrowLeft);
			enviar_frase(F_C1);
			sprintf(Numero, "%4d", R_C1);
			UART_enviar_string(Numero);
			enviar_frase(F_Unid_cinta);
			
			enviar_frase(F_arrowRigth);
			enviar_frase(Control);
			}else{
			enviar_frase(F_C1);
			sprintf(Numero, "%4d", R_C1);
			UART_enviar_string(Numero);
			enviar_frase(F_Unid_cinta);
			
			enviar_frase(Control);
		}
		
		if(Flecha_ESC == 1){
			enviar_frase(F_arrowLeft);
			enviar_frase(F_Exit);
			enviar_frase(F_arrowRigth);
			Flecha_ESC = 0;
			}else{
			enviar_frase(F_Exit);
		}
		Flag_Norepetir = 1;
	}
	#endif
	
	if(C_Enter == 1){
		
		C_Enter = 0;
		Cont_Enter++;
		
		if(Escape == 1){
			Flag_Norepetir = 0;
			Maquina_Estado = 20;
			Escape = 0;
			}else{
			
			if(Cont_Enter == 2){
				Ingresar_Valor = 0;
				Flag_Norepetir = 3;
				Cont_Enter = 0;
				Enter_2 = 0;
				
				valor = atoi((char*)Dato_RX);
				
				}else{
				Ingresar_Valor = 1;
				c = 0;
				Flag_Norepetir = 255;
				
			}
		}
	}
	
	if (Flecha_UP == 1){
		if(Flecha_P2 == 1 || Flecha_P2 == 0 ){
			Flecha_P2 = 6;
			FinFLechas_P0();
			}else{
			Flecha_P2--;
			FinFLechas_P0();
		}
	}
	if(Flecha_LOW == 1){
		if(Flecha_P2 == 6){
			Flecha_P2 = 1;
			FinFLechas_P0();
			}else{
			Flecha_P2++;
			FinFLechas_P0();
		}
	}
	if(Flecha_ESC == 1){
		Flag_Norepetir = 0;
	}
	
	if(Flag_Norepetir == 3){
		
		Flag_Norepetir = 0;
		
		switch(Flecha_P2){
			
			case 1:
				R_H1 = valor;
				guardar_eeprom(Rango_H1,R_H1,2);
			break;
			
			case 2:
				R_B1 = valor;
				guardar_eeprom(Rango_B1,R_B1,1);
			break;
			
			case 3:
				R_H2 = valor;
				guardar_eeprom(Rango_H2,R_H2,2);
			break;
			
			case 4:
				R_B2 = valor;
				guardar_eeprom(Rango_B2,R_B2,1);
			break;
			
			case 5:
				R_B3 = valor;
				guardar_eeprom(Rango_B3,R_B3,2);
			break;

			
			case 6:
				R_C1 = valor;
				guardar_eeprom(Rango_C1,R_C1,1);
			break;
		}
	}
}

void Pantalla_3_Uart(){
	
	#ifndef Flag_Norepetir_P3
	#define Flag_Norepetir_P3

	if (Flag_Norepetir == 0){
		
		enviar_frase(Borrar_Pantalla);
		enviar_frase(F_Titulo_P3);
		
		Cursor_Fil_Col(20,33);
		enviar_frase(Dato_Recibido);
		Cursor_Fil_Col(3,0);
		
		if(Flecha_ESC == 1){
			Flecha_P3 = 0;
		}
		
		if(Flecha_P3 == 1){
			enviar_frase(F_arrowLeft);
			
			enviar_frase(F_TT1);
			sprintf(Numero, "%4d", R_TT1);
			UART_enviar_string(Numero);
			enviar_frase(F_Unid_Temp_MQ3);
			
			enviar_frase(F_arrowRigth);
			enviar_frase(Control);
			}else{
			enviar_frase(F_TT1);
			
			sprintf(Numero, "%4d", R_TT1);
			UART_enviar_string(Numero);
			enviar_frase(F_Unid_Temp_MQ3);
			
			enviar_frase(Control);
		}
		
		if(Flecha_P3 == 2){
			enviar_frase(F_arrowLeft);
			
			enviar_frase(F_TT2);
			sprintf(Numero, "%4d", R_TT2);
			UART_enviar_string(Numero);
			enviar_frase(F_Unid_Temp_MQ3);
			
			enviar_frase(F_arrowRigth);
			enviar_frase(Control);
			}else{
			enviar_frase(F_TT2);
			sprintf(Numero, "%4d", R_TT2);
			UART_enviar_string(Numero);
			enviar_frase(F_Unid_Temp_MQ3);
			
			enviar_frase(Control);
		}
		
		if(Flecha_P3 == 3){
			enviar_frase(F_arrowLeft);
			enviar_frase(F_TT3);
			sprintf(Numero, "%4d", R_TT3);
			UART_enviar_string(Numero);
			enviar_frase(F_Unid_Temp_MQ3);
			
			enviar_frase(F_arrowRigth);
			enviar_frase(Control);
			}else{
			enviar_frase(F_TT3);
			sprintf(Numero, "%4d", R_TT3);
			UART_enviar_string(Numero);
			enviar_frase(F_Unid_Temp_MQ3);
			
			enviar_frase(Control);
		}
		
		if(Flecha_P3 == 4){
			enviar_frase(F_arrowLeft);
			enviar_frase(F_TT4);
			sprintf(Numero, "%4d", R_TT4);
			UART_enviar_string(Numero);
			enviar_frase(F_Unid_Temp_MQ3);
			
			enviar_frase(F_arrowRigth);
			enviar_frase(Control);
			}else{
			enviar_frase(F_TT4);
			sprintf(Numero, "%4d", R_TT4);
			UART_enviar_string(Numero);
			enviar_frase(F_Unid_Temp_MQ3);
			
			enviar_frase(Control);
		}
		
		if(Flecha_P3 == 5){
			enviar_frase(F_arrowLeft);
			enviar_frase(F_TT5);
			sprintf(Numero, "%4d", R_TT5);
			UART_enviar_string(Numero);
			enviar_frase(F_Unid_Temp_MQ3);
			
			enviar_frase(F_arrowRigth);
			enviar_frase(Control);
			}else{
			enviar_frase(F_TT5);
			sprintf(Numero, "%4d", R_TT5);
			UART_enviar_string(Numero);
			enviar_frase(F_Unid_Temp_MQ3);
			
			enviar_frase(Control);
		}
		
		if(Flecha_P3 == 6){
			enviar_frase(F_arrowLeft);
			enviar_frase(F_TT6);
			sprintf(Numero, "%4d", R_TT6);
			UART_enviar_string(Numero);
			enviar_frase(F_Unid_Temp_MQ3);
		
			enviar_frase(F_arrowRigth);
			enviar_frase(Control);
			}else{
			enviar_frase(F_TT6);
			sprintf(Numero, "%4d", R_TT6);
			UART_enviar_string(Numero);
			enviar_frase(F_Unid_Temp_MQ3);
			
			enviar_frase(Control);
		}
		
		if(Flecha_P3 == 7){
			enviar_frase(F_arrowLeft);
			enviar_frase(F_TT7);
			sprintf(Numero, "%4d", R_TT7);
			UART_enviar_string(Numero);
			enviar_frase(F_Unid_Temp_MQ3);
			
			enviar_frase(F_arrowRigth);
			enviar_frase(Control);
			}else{
			enviar_frase(F_TT7);
			sprintf(Numero, "%4d", R_TT7);
			UART_enviar_string(Numero);
			enviar_frase(F_Unid_Temp_MQ3);
			
			enviar_frase(Control);
		}
		
		if(Flecha_P3 == 8){
			enviar_frase(F_arrowLeft);
			enviar_frase(F_TT8);
			sprintf(Numero, "%4d", R_TT8);
			UART_enviar_string(Numero);
			enviar_frase(F_Unid_Temp_MQ3);
			
			enviar_frase(F_arrowRigth);
			enviar_frase(Control);
			}else{
			enviar_frase(F_TT8);
			sprintf(Numero, "%4d", R_TT8);
			UART_enviar_string(Numero);
			enviar_frase(F_Unid_Temp_MQ3);
			
			enviar_frase(Control);
		}
		
		
		if(Flecha_ESC == 1){
			enviar_frase(F_arrowLeft);
			enviar_frase(F_Exit);
			enviar_frase(F_arrowRigth);
			Flecha_ESC = 0;
			}else{
			enviar_frase(F_Exit);
		}
		
		Flag_Norepetir = 1;
	}
	#endif
	
	if(C_Enter == 1){
		
		C_Enter = 0;
		Cont_Enter++;
		
		if(Escape == 1){
			Flag_Norepetir = 0;
			Maquina_Estado = 20;
			Escape = 0;
			}else{
				
			if(Cont_Enter >= 2){
				Ingresar_Valor = 0;
				Flag_Norepetir = 3;
				Cont_Enter = 0;
				Enter_2 = 0;
				valor = atoi((char*)Dato_RX);
				
				}else{
				Ingresar_Valor = 1;
				c = 0;
				Flag_Norepetir = 255;
				Cursor_Fil_Col(20,49);
				enviar_frase(vacio);
			}
		}
	}
	
	if (Flecha_UP == 1){
		if(Flecha_P3 == 1 || Flecha_P3 == 0 ){
			Flecha_P3 = 8;
			FinFLechas_P0();
			}else{
			Flecha_P3--;
			FinFLechas_P0();
		}
	}
	if(Flecha_LOW == 1){
		if(Flecha_P3 == 8){
			Flecha_P3 = 1;
			FinFLechas_P0();
			}else{
			Flecha_P3++;
			FinFLechas_P0();
		}
	}
	
	if(Flecha_ESC == 1){
		Flag_Norepetir = 0;
	}
	if(Flag_Norepetir == 3){
		Flag_Norepetir = 0;
		switch(Flecha_P3){
			
			case 1:
				R_TT1  = valor;
				guardar_eeprom(Rango_TT1,R_TT1,2);
			break;
			
			case 2:
				R_TT2  = valor;
				guardar_eeprom(Rango_TT2,R_TT2,2);
			break;
			
			case 3:
				R_TT3  = valor;
				guardar_eeprom(Rango_TT3,R_TT3,2);
			break;
			
			case 4:
				R_TT4  = valor;
				guardar_eeprom(Rango_TT4,R_TT4,2);
			break;
			
			case 5:
				R_TT5  = valor;
				guardar_eeprom(Rango_TT5,R_TT5,2);
			break;
			
			case 6:
				R_TT6  = valor;
				guardar_eeprom(Rango_TT6,R_TT6,2);
			break;
			
			case 7:
				R_TT7  = valor;
				guardar_eeprom(Rango_TT7,R_TT7,2);
			break;
			
			case 8:
				R_TT8  = valor;
				guardar_eeprom(Rango_TT8,R_TT8,2);
			break;
		}
	}
}

void Pantalla_4_Uart(){
	
	#ifndef Flag_Norepetir_P4
	#define Flag_Norepetir_P4
	
	if (Flag_Norepetir == 0){
		
		enviar_frase(Borrar_Pantalla);
		enviar_frase(F_Titulo_P4);
		
		Cursor_Fil_Col(20,33);
		enviar_frase(Dato_Recibido);
		Cursor_Fil_Col(3,0);
		
		if(Flecha_ESC == 1){
			Flecha_P4 = 0;
		}
		
		if(Flecha_P4 == 1){
			enviar_frase(F_arrowLeft);
			enviar_frase(F_TimePrecal);
			
			sprintf(Numero, "%d", Tiempo_PreCalentamiento);
			UART_enviar_string(Numero);
			
			enviar_frase(F_Unid_Time);
			enviar_frase(F_arrowRigth);
			enviar_frase(Control);
			}else{
			enviar_frase(F_TimePrecal);
			
			sprintf(Numero, "%d", Tiempo_PreCalentamiento);
			UART_enviar_string(Numero);
			
			enviar_frase(F_Unid_Time);
			enviar_frase(Control);
		}
		
		if(Flecha_P4 == 2){
			enviar_frase(F_arrowLeft);
			enviar_frase(F_TimeCal);
			
			sprintf(Numero, "%d", Tiempo_Calentamiento);
			UART_enviar_string(Numero);
			
			enviar_frase(F_Unid_Time);
			enviar_frase(F_arrowRigth);
			enviar_frase(Control);
			}else{
			enviar_frase(F_TimeCal);
			
			sprintf(Numero, "%d", Tiempo_Calentamiento);
			UART_enviar_string(Numero);
			
			enviar_frase(F_Unid_Time);
			enviar_frase(Control);
		}
		
		
		if(Flecha_P4 == 3){
			enviar_frase(F_arrowLeft);
			enviar_frase(F_TimeEnf);
			
			sprintf(Numero, "%d", Tiempo_Enfriamiento);
			UART_enviar_string(Numero);
			
			enviar_frase(F_Unid_Time);
			enviar_frase(F_arrowRigth);
			enviar_frase(Control);
			}else{
			enviar_frase(F_TimeEnf);
			
			sprintf(Numero, "%d", Tiempo_Enfriamiento);
			UART_enviar_string(Numero);
			
			enviar_frase(F_Unid_Time);
			enviar_frase(Control);
		}
		
		
		if(Flecha_ESC == 1){
			enviar_frase(F_arrowLeft);
			enviar_frase(F_Exit);
			enviar_frase(F_arrowRigth);
			Flecha_ESC = 0;
			}else{
			enviar_frase(F_Exit);
		}
		
		Flag_Norepetir = 1;
	}
	#endif
	
	if(C_Enter == 1){
		
		C_Enter = 0;
		Cont_Enter++;
		
		if(Escape == 1){
			Flag_Norepetir = 0;
			Maquina_Estado = 20;
			Escape = 0;
			}else{
			//Set_Point_Mod = Flecha_P1;
			C_Enter = 0;
			if(Cont_Enter >= 2){
				Ingresar_Valor = 0;
				Flag_Norepetir = 3;
				Cont_Enter = 0;
				Enter_2 = 0;
				valor = atoi((char*)Dato_RX);
				}else{
				Ingresar_Valor = 1;
				c = 0;
				Flag_Norepetir = 255;
			}
		}
	}
	
	if (Flecha_UP == 1){
		if(Flecha_P4 == 1 || Flecha_P4 == 0 ){
			Flecha_P4 = 3;
			FinFLechas_P0();
			}else{
			Flecha_P4--;
			FinFLechas_P0();
		}
	}
	if(Flecha_LOW == 1){
		if(Flecha_P4 == 3){
			Flecha_P4 = 1;
			FinFLechas_P0();
			}else{
			Flecha_P4++;
			FinFLechas_P0();
		}
	}
	
	if(Flecha_ESC == 1){
		Flag_Norepetir = 0;
	}
	
	if(Flag_Norepetir == 3){
		
		switch(Flecha_P4){
			
			case 1:
			Tiempo_PreCalentamiento  = valor;
			guardar_eeprom(Tiempo_de_precalentamiento,valor,2);
			break;
			
			case 2:
			Tiempo_Calentamiento  = valor;
			guardar_eeprom(Tiempo_de_calentamiento,valor,2);
			break;
			
			case 3:
			Tiempo_Enfriamiento = valor;
			guardar_eeprom(Tiempo_de_enfriamiento,valor,2);
			break;
		}
		Flag_Norepetir = 0;
	}
}

void Pantalla_5_Uart(){
	
	#ifndef Flag_Norepetir_P5
	#define Flag_Norepetir_P5
	
	if (Flag_Norepetir == 0){
		
		enviar_frase(Borrar_Pantalla);
		enviar_frase(F_Titulo_P5);
		
		Cursor_Fil_Col(20,33);
		enviar_frase(Dato_Recibido);
		Cursor_Fil_Col(3,0);
		
		if(Flecha_P5 == 1){
			enviar_frase(F_arrowLeft);
			enviar_frase(F_WZ1);
			
			sprintf(Numero, "%d", Aviso_TZ1);
			UART_enviar_string(Numero);
			
			enviar_frase(F_Unid_Porc);
			enviar_frase(F_arrowRigth);
			enviar_frase(Control);
			}else{
			enviar_frase(F_WZ1);
			
			sprintf(Numero, "%d", Aviso_TZ1);
			UART_enviar_string(Numero);
			
			enviar_frase(F_Unid_Porc);
			enviar_frase(Control);
		}
		
		if(Flecha_P5 == 2){
			enviar_frase(F_arrowLeft);
			enviar_frase(F_WZ2);
			
			sprintf(Numero, "%d", Aviso_TZ2);
			UART_enviar_string(Numero);
			
			enviar_frase(F_Unid_Porc);
			enviar_frase(F_arrowRigth);
			enviar_frase(Control);
			}else{
			enviar_frase(F_WZ2);
			
			sprintf(Numero, "%d", Aviso_TZ2);
			UART_enviar_string(Numero);
			
			enviar_frase(F_Unid_Porc);
			enviar_frase(Control);
		}
		
		
		if(Flecha_P5 == 3){
			enviar_frase(F_arrowLeft);
			enviar_frase(F_WZ3);
			
			sprintf(Numero, "%d", Aviso_TZ3);
			UART_enviar_string(Numero);
			
			enviar_frase(F_Unid_Temp);
			enviar_frase(F_arrowRigth);
			enviar_frase(Control);
			}else{
			enviar_frase(F_WZ3);
			
			sprintf(Numero, "%d", Aviso_TZ3);
			UART_enviar_string(Numero);
			
			enviar_frase(F_Unid_Temp);
			enviar_frase(Control);
			
		}
		enviar_frase(Control);
		enviar_frase(F_Titulo_P5_2);
		
		if(Flecha_P5 == 4){
			enviar_frase(F_arrowLeft);
			enviar_frase(F_WZ1);
			
			sprintf(Numero, "%d", Alarma_TZ1);
			UART_enviar_string(Numero);
			
			enviar_frase(F_Unid_Porc);
			enviar_frase(F_arrowRigth);
			enviar_frase(Control);
			}else{
			enviar_frase(F_WZ1);
			
			sprintf(Numero, "%d", Alarma_TZ1);
			UART_enviar_string(Numero);
			
			enviar_frase(F_Unid_Porc);
			enviar_frase(Control);
		}
		
		if(Flecha_P5 == 5){
			enviar_frase(F_arrowLeft);
			enviar_frase(F_WZ2);
			
			sprintf(Numero, "%d", Alarma_TZ2);
			UART_enviar_string(Numero);
			
			enviar_frase(F_Unid_Porc);
			enviar_frase(F_arrowRigth);
			enviar_frase(Control);
			}else{
			enviar_frase(F_WZ2);
			
			sprintf(Numero, "%d", Alarma_TZ2);
			UART_enviar_string(Numero);
			
			enviar_frase(F_Unid_Porc);
			enviar_frase(Control);
		}
		
		
		if(Flecha_P5 == 6){
			enviar_frase(F_arrowLeft);
			enviar_frase(F_WZ3);
			
			sprintf(Numero, "%d", Alarma_TZ3);
			UART_enviar_string(Numero);
			
			enviar_frase(F_Unid_Temp);
			enviar_frase(F_arrowRigth);
			enviar_frase(Control);
			}else{
			enviar_frase(F_WZ3);
			
			sprintf(Numero, "%d", Alarma_TZ3);
			UART_enviar_string(Numero);
			
			enviar_frase(F_Unid_Temp);
			enviar_frase(Control);
		}	
		Flag_Norepetir = 1;
	}
	#endif
	
	if(C_Enter == 1){
		
		C_Enter = 0;
		Cont_Enter++;
		C_Enter = 0;
			
			if(Cont_Enter >= 2){
				
				Ingresar_Valor = 0;
				Flag_Norepetir = 3;
				Cont_Enter = 0;
				Enter_2 = 0;
				valor = atoi((char*)Dato_RX);
				}else{
				Ingresar_Valor = 1;
				c = 0;
				Flag_Norepetir = 255;
		}
	}
	
	if (Flecha_UP == 1){
		if(Flecha_P5 == 1 || Flecha_P5 == 0 ){
			Flecha_P5 = 6;
			FinFLechas_P0();
			}else{
			Flecha_P5--;
			FinFLechas_P0();
		}
	}
	if(Flecha_LOW == 1){
		if(Flecha_P5 == 6){
			Flecha_P5 = 1;
			FinFLechas_P0();
			}else{
			Flecha_P5++;
			FinFLechas_P0();
		}
	}
	
	if(Flecha_ESC == 1){
		Flag_Norepetir = 0;
	}
	
	
	if(Flag_Norepetir == 3){
		
		switch(Flecha_P5){
			
			case 1:
				valor = (valor*SP_TempZ1)/100+SP_TempZ1;
				Aviso_TZ1 = valor;
				guardar_eeprom(Aviso_temp_z1,valor,1);
				Flag_Norepetir = 0;
			break;
			
			case 2:
				valor = (valor*SP_TempZ2)/100+SP_TempZ2;
				Aviso_TZ2  = valor;
				guardar_eeprom(Aviso_temp_z2,valor,1);
				Flag_Norepetir = 0;
			break;
			
			case 3:
				Aviso_TZ3 = valor;
				guardar_eeprom(Aviso_temp_z3,valor,2);
				Flag_Norepetir = 0;
			break;
			
			case 4:
				valor = (valor*SP_TempZ1)/100+SP_TempZ1;
				Alarma_TZ1 = valor;
				guardar_eeprom(Alarma_temp_z1,valor,1);
				Flag_Norepetir = 0;
			break;
			
			case 5:
			valor = (valor*SP_TempZ2)/100+SP_TempZ2;
				Alarma_TZ2 = valor;
				guardar_eeprom(Alarma_temp_z2,valor,1);
				Flag_Norepetir = 0;
			break;
			
			case 6:
				Alarma_TZ3 = valor;
				guardar_eeprom(Alarma_temp_z3,valor,2);
				Flag_Norepetir = 0;
			break;
		}
	}
}

void Pantalla_6_Uart(){
	
	if (Flag_Norepetir == 0){
		enviar_frase(Borrar_Pantalla);
		enviar_frase(F_Titulo_P6_2);
		

		enviar_frase(F_PromZ1);
		sprintf(Numero, "%4d", PromedioZona_1);
		UART_enviar_string(Numero);
		enviar_frase(F_Unid_Temp_MQ3);
		enviar_frase(Control);

		enviar_frase(F_PromZ2);
		sprintf(Numero, "%4d", PromedioZona_2);
		UART_enviar_string(Numero);
		enviar_frase(F_Unid_Temp_MQ3);
		enviar_frase(Control);

		enviar_frase(F_PromZ3);
		sprintf(Numero, "%4d", PromedioZona_3);
		UART_enviar_string(Numero);
		enviar_frase(F_Unid_Temp_MQ3);
		enviar_frase(Control);
	
		enviar_frase(F_TT1);
		sprintf(Numero, "%4d", Vec_SsT[0]);
		UART_enviar_string(Numero);
		enviar_frase(F_Unid_Temp_MQ3);
		enviar_frase(Control);
		
		enviar_frase(F_TT2);
		sprintf(Numero, "%4d", Vec_SsT[1]);
		UART_enviar_string(Numero);
		enviar_frase(F_Unid_Temp_MQ3);
		enviar_frase(Control);
		
		enviar_frase(F_TT3);
		sprintf(Numero, "%4d", Vec_SsT[2]);
		UART_enviar_string(Numero);
		enviar_frase(F_Unid_Temp_MQ3);
		enviar_frase(Control);
		
		enviar_frase(F_TT4);
		sprintf(Numero, "%4d", Vec_SsT[3]);
		UART_enviar_string(Numero);
		enviar_frase(F_Unid_Temp_MQ3);
		enviar_frase(Control);

		enviar_frase(F_TT5);
		sprintf(Numero, "%4d", Vec_SsT[4]);
		UART_enviar_string(Numero);
		enviar_frase(F_Unid_Temp_MQ3);
		enviar_frase(Control);

		enviar_frase(F_TT6);
		sprintf(Numero, "%4d", Vec_SsT[5]);
		UART_enviar_string(Numero);
		enviar_frase(F_Unid_Temp_MQ3);
		enviar_frase(Control);

		enviar_frase(F_TT7);
		sprintf(Numero, "%4d", Vec_SsT[6]);
		UART_enviar_string(Numero);
		enviar_frase(F_Unid_Temp_MQ3);
		enviar_frase(Control);

		enviar_frase(F_TT8);
		sprintf(Numero, "%4d", Vec_SsT[7]);
		UART_enviar_string(Numero);
		enviar_frase(F_Unid_Temp_MQ3);
		
		
		switch(Estado_Actual[0]){
			
			case 1:
			Cursor_Fil_Col(3,40);
			enviar_frase(F_Z1W);
			break;
			
			case 2:
			Cursor_Fil_Col(3,40);
			enviar_frase(F_Z1A);
			break;
			
			case 0:
			Cursor_Fil_Col(3,40);
			enviar_frase(F_Normal);
			break;
		}
		
		switch(Estado_Actual[1]){
			case 1:
			Cursor_Fil_Col(5,40);
			enviar_frase(F_Z2W);
			break;
			
			case 2:
			Cursor_Fil_Col(5,40);
			enviar_frase(F_Z2A);
			break;
			
			case 0:
			Cursor_Fil_Col(5,40);
			enviar_frase(F_Normal);
			break;
		}
		switch(Estado_Actual[2]){
			case 1:
			Cursor_Fil_Col(7,40);
			enviar_frase(F_Z3W);
			break;
			
			case 2:
			Cursor_Fil_Col(7,40);
			enviar_frase(F_Z3A);
			break;
			
			case 0:
			Cursor_Fil_Col(7,40);
			enviar_frase(F_Normal);
			break;
		}
		
		if(Flag_Parada_Emergencia == 2){
			Cursor_Fil_Col(9,40);
			enviar_frase(F_Emergencia);
		}
		
		if(Flag_Parada_Controlada  == 1){
			Cursor_Fil_Col(9,40);
			enviar_frase(F_Controlada);
			
			Cursor_Fil_Col(11,40);
			enviar_frase(F_Fase);
			sprintf(Numero, "%d", Fase);
			UART_enviar_string(Numero);
			
			Cursor_Fil_Col(13,40);
			enviar_frase(F_Tiempo);
			sprintf(Numero, "%d", Cont_Tiempo);
			UART_enviar_string(Numero);
		}
		
		if(Flag_Secuencia_Arranque  == 1){
			Cursor_Fil_Col(9,40);
			enviar_frase(F_Arranque);
			
			Cursor_Fil_Col(11,40);
			enviar_frase(F_Fase);
			sprintf(Numero, "%d", Fase);
			UART_enviar_string(Numero);
			
			Cursor_Fil_Col(13,40);
			enviar_frase(F_Tiempo);
			sprintf(Numero, "%d", Cont_Tiempo);
			UART_enviar_string(Numero);
		}
		
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
		
		Flag_Norepetir = 1;
	}
	
		if(Escape == 1){
			Flag_Norepetir = 0;
			Maquina_Estado = 20;
			Escape = 0;
			Cont_Enter = 0;
			Ingresar_Valor = 0;
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
			}else{
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
	}
	
	switch(PromedioZona_1){
		
		case 0:
		Flag_Parada_Emergencia = 1;
		Falla_Sens[0] = 3;
		break;
		
		case 1:
		Flag_Parada_Controlada = 1;
		Falla_Sens[0] = 2;
		break;
		
		case 2:
		Flag_Aviso_CW = 2;
		Falla_Sens[0] = 1;
		break;
		
		case 3:
		Falla_Sens[0] = 0;
	}
	
	switch(PromedioZona_2){
		
		case 0:
		Flag_Parada_Emergencia = 1;
		Falla_Sens[1] = 3;
		break;
		
		case 1:
		Flag_Parada_Controlada = 1;
		Falla_Sens[0] = 2;
		break;
		
		case 2:
		Flag_Aviso_CW = 2;
		Falla_Sens[0] = 1;
		break;
		
		case 3:
		Falla_Sens[1] = 0;
	}
	
	switch(PromedioZona_3){
		
		case 0:
		Falla_Sens[2] = 2;
		Flag_Aviso_CW = 2;
		break;
		
		case 1:
		Falla_Sens[2] = 1;
		Flag_Aviso_CW = 2;
		
		case 2:
		Falla_Sens[2] = 0;
	}
	
	PromedioZona_1 = (Temp_Z1)/PromedioZona_1;
	
	if(Flag_Secuencia_Arranque != 1 && Estado_Horno != 0){
		comparaciones(SP_TempZ1, PromedioZona_1, Aviso_TZ1, Alarma_TZ1,1);
	}
	PromedioZona_2 = Temp_Z2/PromedioZona_2;
	
	if(Flag_Secuencia_Arranque != 1 && Estado_Horno != 0){
		comparaciones(SP_TempZ2, PromedioZona_2, Aviso_TZ2, Alarma_TZ2,2);
	}
	
	PromedioZona_3 = Temp_Z3/PromedioZona_3;
	
	if(Flag_Secuencia_Arranque != 1 && Estado_Horno != 0){
		comparaciones(0, PromedioZona_3, Aviso_TZ3, Alarma_TZ3,3);
	}
}

void comparaciones(uint16_t SP_Temp, uint16_t Prom, uint16_t Aviso, uint16_t Alarma, uint8_t Zona){
	
	if(Zona == 3){
		if(Prom >= Aviso){
			
			if(Prom >= Alarma){
				
				Led_Rojo = 2;
				Contador_Alarma = 0;
				Contador_Aviso++;
				Led_Verde = 0;
				Estado_Actual[Zona-1] = 2;
				}else{
				
				led_Ambar = 2;
				Contador_Aviso = 0;
				Contador_Alarma++;
				Led_Verde = 0;
				Estado_Actual[Zona-1] = 1;
			}
			}else{
			Contador_Aviso++;
			Contador_Alarma++;
			Estado_Actual[Zona-1] = 0;
			Led_Verde = 1;
		}
		}else{
		if( Prom > (SP_Temp + Aviso*SP_Temp/100) || Prom < (SP_Temp - Aviso*SP_Temp/100)){
			
			if(Prom > (SP_Temp + Alarma*SP_Temp/100) || Prom < (SP_Temp - Alarma*SP_Temp/100)){
				
				Led_Rojo = 2;
				Contador_Alarma = 0;
				Contador_Aviso++;
				Led_Verde = 0;
				
				Estado_Actual[Zona-1] = 2;
				
				
				}else{
				led_Ambar = 2;
				Contador_Aviso = 0;
				Contador_Alarma++;
				Led_Verde = 0;
				Estado_Actual[Zona-1] = 1;
			}
			}else{
			Contador_Aviso++;
			Contador_Alarma++;
			Estado_Actual[Zona-1] = 0;
			Led_Verde = 1;
		}
	}

	if(Contador_Aviso == 3){
		Contador_Aviso = 0;
		led_Ambar = 0;
		if(Flag_Aviso_CW == 0 && Led_Verde == 0 && Led_Rojo == 0){
			PORTD &= ~(1 << PD7);
			PORTD &= ~(1 << PD6);
		}
	}

	if(Contador_Alarma == 3){
		Contador_Alarma = 0;
		Led_Rojo = 0;
		if(led_Ambar == 0 && Flag_Aviso_CW == 0){
			// apagar led rojo
			PORTD &= ~(1 << PD7);
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

		}
		Actuador = 0;
	}
}

void Chequear_PIDs_Fallas(void)
{
	uint8_t estado_actual = PINB;
	uint8_t mask = 0x3F;

	// 1 = falla (porque invertimos)
	uint8_t fallas = (~estado_actual) & mask;

	if (fallas)
	{
		// Hay alguna falla
		Flag_Parada_Emergencia = 1;

		// Acumular fallas
		if (fallas & (1 << PB0)) Vec_PIDs[0] = 1;
		if (fallas & (1 << PB1)) Vec_PIDs[1] = 1;
		if (fallas & (1 << PB2)) Vec_PIDs[2] = 1;
		if (fallas & (1 << PB3)) Vec_PIDs[3] = 1;
		if (fallas & (1 << PB4)) Vec_PIDs[4] = 1;
		if (fallas & (1 << PB5)) Vec_PIDs[5] = 1;
	}
	else
	{
		// Todo OK ? resetear
		for (uint8_t i = 0; i < 6; i++)
		{
			Vec_PIDs[i] = 0;
		}

		Flag_Parada_Emergencia = 0;
	}
}

void ParadaEmergencia(){
	
	Flag_Parada_Emergencia = 2;
	Estado_Horno = 5;
	
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
	
	Led_Verde = 0;
	Led_Rojo = 2;
	Actuador = 0;
}

void ParadaControlada(){
	
	
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
	
	switch(Fase){
		
		case 0:
		Led_Rojo = 0;
		led_Ambar = 0;
		Flag_Aviso_CW = 0;
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
			Cont_Tiempo = 65535;
			Led_Verde = 1;
			Estado_Horno = 2;
		}
		break;
	}
}

void leer_eeprom(){
	
	R_C1 = EEPROM_read_byte(Rango_C1);
	R_H1 = EEPROM_read_uint16(Rango_H1);
	R_H2 = EEPROM_read_uint16(Rango_H2);
	R_B1 = EEPROM_read_byte(Rango_B1);
	R_B2 = EEPROM_read_byte(Rango_B2);
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
	SP_CaudalZ1 = EEPROM_read_byte(Caudal_vol_deseado_Z1);
	SP_CaudalZ2 = EEPROM_read_byte(Caudal_vol_deseado_Z2);
	SP_CaudalZ3 = EEPROM_read_uint16(Caudal_vol_deseado_Z3);
	SP_VelocidadZ3 = EEPROM_read_byte(Vel_cinta_transp);
	
	
	Aviso_TZ1 = EEPROM_read_byte(Aviso_temp_z1);
	Aviso_TZ2 = EEPROM_read_byte(Aviso_temp_z2);
	Aviso_TZ3 = EEPROM_read_uint16(Aviso_temp_z3);
	
	Alarma_TZ1 = EEPROM_read_byte(Alarma_temp_z1);
	Alarma_TZ2 = EEPROM_read_byte(Alarma_temp_z2);
	Alarma_TZ3 = EEPROM_read_uint16(Alarma_temp_z3);
	
}

void guardar_eeprom(uint8_t direccion,uint16_t valor,uint8_t bytes){
	
	uint8_t leer_8b;
	uint8_t valor1;
	uint16_t leer_16b;
	
	if(bytes == 1){
		valor1 = valor;
		leer_8b = EEPROM_read_byte(direccion);
		if(leer_8b != valor1){
			EEPROM_write_byte(direccion, valor);
		}
	}
	
	if(bytes == 2){
		leer_16b = EEPROM_read_uint16(direccion);
		if(leer_16b != valor){
			EEPROM_write_uint16(direccion, valor);
		}
	}
}

// eeprom ************ eeprom ************* eeprom
static void EEPROM_write(uint16_t address, uint8_t data)
{
	while (EECR & (1 << EEPE));   // Espera escritura previa

	EEAR = address;
	EEDR = data;

	EECR |= (1 << EEMPE);         // Habilita escritura
	EECR |= (1 << EEPE);          // Ejecuta escritura
}

static uint8_t EEPROM_read(uint16_t address)
{
	while (EECR & (1 << EEPE));   // Espera escritura previa

	EEAR = address;
	EECR |= (1 << EERE);          // Ejecuta lectura

	return EEDR;
}

void EEPROM_write_byte(uint16_t address, uint8_t data)
{
	EEPROM_write(address, data);
}

uint8_t EEPROM_read_byte(uint16_t address)
{
	return EEPROM_read(address);
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


