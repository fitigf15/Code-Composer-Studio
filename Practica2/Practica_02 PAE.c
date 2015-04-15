/******************************
 * 
 * PR�CTICA 2_PIH Programaci� de Ports i pr�ctica de: "do-while" "case" "if" i "for"
 * 
 *****************************/

#include <msp430x54xA.h>
#include <stdio.h>
#include "hal_lcd.h"

char saludo[17]= "PRACT 2";
char cadena[17];
char borrado[] = "                 ";  
unsigned char contraste   = 0x64;
unsigned char iluminacion  = 30;
unsigned char linea=1;
char estado=0;
char estado_anterior=8;
long int i;
long int temps_delay = 25000;


/**************************************************************************
 * BORRAR LINEA
 * 
 * Datos de entrada: Linea, indica la linea a borrar
 * 
 * Sin datos de salida
 * 
 **************************************************************************/

void borrar(unsigned char Linea)
{
	halLcdPrintLine(borrado, Linea, OVERWRITE_TEXT); //incluimos una linea en blanco
}


/**************************************************************************
 * ESCRIBIR LINEA
 * 
 * Datos de entrada: Linea, indica la linea a escribir
 * 					 String, cadena de caracteres que vamos a introducir
 * 
 * Sin datos de salida
 * 
 **************************************************************************/
  
void escribir(char String[], unsigned char Linea)  

{
	halLcdPrintLine(String, Linea, OVERWRITE_TEXT); //superponemos la nueva palabra introducida, haya o no algo.
}

/**************************************************************************
 * DELAY - A CONFIGURAR POR EL ALUMNO_ con bucle while
 * 
 * Datos de entrada: Tiempo de retraso. 1 segundo equivale a variable retraso 25000 (aprox)
 * 
 * Sin datos de salida
 * 
 **************************************************************************/


void delay (long int temps)
{
   volatile long int retraso=0;
	   	/***********************************************
   	 * A RELLENAR POR EL ALUMNO BLOQUE WHILE
   	 ***********************************************/
   
   
   do{
	   retraso++;    //segons se'ns ha explicat a classe anirem sumant de 1 en 1 fins arribar al valor demanat. 25000 equival a 1 segon
   }while(retraso<temps);
   

   	 /***********************************************
   	 * HASTA AQUI BLOQUE WHILE
   	 ***********************************************/	
	
}


/**************************************************************************
 * INICIALIZACI�N DE LOS BOTONES.
 * Configuramos botones y leds: 
 * 
 * Sin datos de entrada
 * 
 * Sin datos de salida
 * 
 **************************************************************************/

void init_botons(void)
{
  //Configuramos botones y leds:
  P1DIR |= 0x03;	//Puertos P1.0 y P1.1 como salidas (Leds)
  P1OUT |= 0x01;	//Inicializamos puerto P1.0 a 1, 
  P1OUT &= 0xFD;	// y P1.1 a 0, para leds en alternancia


  P2DIR &= ~0xC0;	//Puertos P2.6 y P2.7 como entradas (botones S1 y S2)
  P2SEL &= ~0xC0;	//Puertos P2.6 y P2.7 como I/O digitales,
  P2REN |= 0xC0;	//con resistencia activada
  P2OUT |= 0xC0;	// de pull-up
  P2IE |= 0xC0; 	//Interrupciones activadas en P2.6 y P2.7,
  P2IES &= ~0xC0;	// con transicion L->H
  
  
  //Configuramos el joystick:
  P2DIR &= ~0x3E;	//Puertos P2.1 a P2.5 como entradas (joystick)
  P2SEL &= ~0x3E;	//Puertos P2.1 y P2.5 como I/O digitales,
  P2REN |= 0x3E;	//con resistencia activada
  P2OUT |= 0x3E;	// de pull-up
  P2IE |= 0x3E; 	//Interrupciones activadas en P2.1 a P2.5,
  P2IES &= ~0x3E;	// con transicion L->H
  
}

/*****************************************************************************
 * CONFIGURACI�N DEL PUERTO 4, PARA ILUMINAR LOS LEDS. A REALIZAR POR EL ALUMNO
 * 
 * Sin datos de entrada
 * 
 * Sin datos de salida
 *  
 ****************************************************************************/
 
void config_P4_LEDS (void)
{
	P4DIR |= 0xFF;	//Posem tots els bits de P4 com a sortida
	P4OUT &= 0x00;	//Inicialitzem tots els bits de P4 a 0 per tal de tenir els leds
					//apagats
}


/**************************************************************************
 * INICIALIZACI�N DE PANTALLA. 
 * Inicializamos la pantallita LCD:
 * 
 * Sin datos de entrada
 * 
 * Sin datos de salida
 * 
 **************************************************************************/

void init_LCD(void)
{
      
  halLcdInit();  //Programa interno para iniciar la pantalla     
  halLcdBackLightInit(); // Inicio de Iluminaci�n posterior de la pantalla 
  halLcdSetBackLight(iluminacion); // Determinaci�n de la Iluminaci�n posterior de la pantalla
  halLcdSetContrast(contraste);//Establecimiento del contraste
  halLcdClearScreen();  //Limpiar (borrar) la pantalla
}



void main(void)
{	

  	WDTCTL = WDTPW+WDTHOLD;       	// Paramos el watchdog timer
  
  	init_botons();					// Iniciamos los botones y Leds.
    _enable_interrupt(); 				// Activamos las interrupciones a nivel global del chip
    init_LCD();						// Inicializamos la pantalla
    config_P4_LEDS();

  
  	escribir(saludo,linea); 			//escribimos saludo en la primera linea
  	linea++; 					//Aumentamos el valor de linea y con ello pasamos a la linea siguiente
  	
  	do
   	{
   		
   		
		if (estado_anterior != estado)			// Dependiendo el valor del estado se encender� un LED externo u otro.
		{
		   sprintf(cadena," estado %d", estado); 	// Guardamos en cadena lo siguiente frase: estado "valor del estado"
		   escribir(cadena,linea); 			// Escribimos cadena
		   estado_anterior=estado; 			// Actualizamos el valor de estado_anterior, para que no est� siempre escribiendo.
		}
		//P1OUT^= 0x03;					// Encender los LEDS con intermitencia
		delay(25000);					// retraso de aprox 1 segundo

		if(estado == 4){
			P4OUT &= 0x00;
			for (i=0;i<8;i++){
				P4OUT = (1<<i);
				delay(temps_delay);
			}
		}

		if(estado == 3){
			P4OUT &= 0x00;
			for (i=7;i>=0;i--){
				P4OUT = (1<<i);
				delay(temps_delay);
			}
		}

		if (estado == 6){
			temps_delay = temps_delay/1.75;
		}

		if (estado == 7){
			temps_delay = temps_delay*1.75;
		}

	
   	}
    while(1);
}


/**************************************************************************
 * MINIPROGRAMA DE LOS BOTONES:
 * Mediante este programa, se detectar� que bot�n se ha pulsado
 * 		 
 * Sin Datos de entrada
 * 
 * Sin datos de salida
 * 
 * Actualiza el valor estado
 * 
 **************************************************************************/
 
#pragma vector=PORT2_VECTOR  //interrupci�n de los botones. Actualiza el valor de la variable global estado.
__interrupt void Port2_ISR(void)
{
	P2IE &= 0xC0; 	//interrupciones botones S1 y S2 (P2.6 y P2.7) desactivadas
	P2IE &= 0x3E;   //interrupciones joystick (2.1-2.5) desactivadas
	
	/**********************************************************+
		A RELLENAR POR EL ALUMNO BLOQUE CASE 
	 
	Boton S1, estado =1 y leds ON
	Boton S2, estado =2 y leds uno ON y otro OFF 
	Joystick left, estado =3 y leds off
	Joystick right, estado =4 y leds off
	Joystick center, estado = 5 y leds uno OFF y otro ON
	Joystick up, estado =6 y leds off
	Joystick down, estado =7 y leds off
	 * *********************************************************/
	 switch(P2IFG){
	 case 0x40:
		 //boto s1
		 estado = 1;
		 P1OUT |= 0x03; //leds ON
		 break;
	 case 0x80:
		 //boto s2
		 estado = 2;
		 P1OUT |= 0x01;
		 P1OUT &= 0xFD; //led1 ON led2 OFF
		 break;
	 case 0x02:
		 //joystick left
		 estado = 3;
		 P1OUT &= 0xFC; //leds OFF
		 break;
	 case 0x04:
		 //joystick right
		 estado = 4;
		 P1OUT &= 0xFC; //leds OFF
		 break;
	 case 0x08:
		 //joystick center
		 estado = 5;
		 P1OUT |= 0x02;
		 P1OUT &= 0xFE; //led1 OFF led2 ON
		 break;
	 case 0x10:
		 //joystick up
		 estado = 6;
		 P1OUT &= 0xFC; //leds OFF
		 break;
	 case 0x20:
		 //joystick down
		 estado = 7;
		 P1OUT &= 0xFC; //leds OFF
		 break;
	 }



	
	/***********************************************
   	 * HASTA AQUI BLOQUE CASE
   	 ***********************************************/	
	
	P2IFG = 0;		//limpiamos todas las interrupciones
	P2IE |= 0xC0; 	//interrupciones botones S1 y S2 (P2.6 y P2.7) reactivadas
	P2IE |= 0x3E;  //interrupciones joystick (2.1-2.5) reactivadas
 return;
}
