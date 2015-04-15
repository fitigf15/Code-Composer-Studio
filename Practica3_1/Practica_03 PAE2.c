/******************************
 * 
 * PRÁCTICA 3_PIH Creació de Funcions de Retard i un Rellotge ens Temps Real mitjançant l’ús de Timers i Interrupcions

 * 
 *****************************/

#include <msp430x54xA.h>
#include <stdio.h>
#include "hal_lcd.h"

char saludo[17]= "PRACT 3";
char cadena[17];
char borrado[] = "                 ";  
unsigned char contraste   = 0x64;
unsigned char iluminacion  = 30;
unsigned char linea=1;

unsigned char config_alarma=0; //servira per saber si configurem o no la alarma

char estado=0;

unsigned int tiempo = 0;  //guardara el temps
unsigned int alarma = 0;  //guardara el temps de l'alarma

char estado_anterior=8;
long int i=0;
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
	   retraso++;
   }while(retraso<temps);
   

   	 /***********************************************
   	 * HASTA AQUI BLOQUE WHILE
   	 ***********************************************/	
	
}


/**************************************************************************
 * INICIALIZACIÓN DE LOS BOTONES.
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
 * CONFIGURACIÓN DEL PUERTO 4, PARA ILUMINAR LOS LEDS. A REALIZAR POR EL ALUMNO
 * 
 * Sin datos de entrada
 * 
 * Sin datos de salida
 *  
 ****************************************************************************/
 
void config_P4_LEDS (void)
{
	P4DIR |= 0xFF;	//Posem tots els bits de P4 com a sortida
	P4OUT = 0x01;	//Inicialitzem tots els bits de P4 a 0 per tal de tenir els leds
					//apagats
}


/**************************************************************************
 * INICIALIZACIÓN DE TIMERS.
 *
 **************************************************************************/

void config_A1_timer(void)
{
	TA1CTL = TASSEL_1+MC_1; //TASSEL_1 - Mode ACLK, MC_1 Mode UP
	TA1CCTL0=CCIE; //Habilitem interrupcions

	TA1CCR0 = 33; //ACLK treballa a uns 32kHz, per tant fer-lo arribar a 33 equivaldria a aproximadament 1ms

}

/**************************************************************************
 * INICIALIZACIÓN DE PANTALLA. 
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
  halLcdBackLightInit(); // Inicio de Iluminación posterior de la pantalla 
  halLcdSetBackLight(iluminacion); // Determinación de la Iluminación posterior de la pantalla
  halLcdSetContrast(contraste);//Establecimiento del contraste
  halLcdClearScreen();  //Limpiar (borrar) la pantalla
}



void main(void)
{	

  	WDTCTL = WDTPW+WDTHOLD;       	// Paramos el watchdog timer
  
  	init_botons();					// Iniciamos los botones y Leds.
    _enable_interrupt(); 				// Activamos las interrupciones a nivel global del chip
    init_LCD();						// Inicializamos la pantalla
    config_P4_LEDS();				// Inicialitzem els LEDs
    config_A1_timer();				// Inicialitzem el timer

  
  	escribir(saludo,linea); 			//escribimos saludo en la primera linea
  	linea++; 					//Aumentamos el valor de linea y con ello pasamos a la linea siguiente
  	i=0;
  	do
   	{
   		
   		
		if (estado_anterior != estado)			// Dependiendo el valor del estado se encenderá un LED externo u otro.
		{
		   sprintf(cadena," estado %d", estado); 	// Guardamos en cadena lo siguiente frase: estado "valor del estado"
		   escribir(cadena,linea); 			// Escribimos cadena
		   estado_anterior=estado; 			// Actualizamos el valor de estado_anterior, para que no esté siempre escribiendo.
		}

		//P1OUT^= 0x03;					// Encender los LEDS con intermitencia

		if (i>=1000){	//Quan s'arriba 1000 cops a TA1CCR0
			if (P4OUT == 0x80){//Comprovem la posicio de l'1 al registre (LED que esta ences)
				P4OUT = 0x01;
			}else{//Tornem a encendre el primer LED
				P4OUT <<= 1;
			}

			i=0;
			linea++; //linea del temps

			if(tiempo == 86399){  //si la hora era 23:59:59 llavors tornarem a 0
				tiempo=0;
			}else{
				tiempo++;
			}
			sprintf(cadena, "%d:%d:%d", (tiempo/3600)%24,(tiempo/60)%60,tiempo%60); //posem la hora
			linea++; // linea de la alarma
			if(config_alarma==0){ //si la alarma NO esta configurada

				sprintf(cadena, "alarma no configurada");
				escribir(cadena,linea); 			// Escribimos cadena

			}else{
				if(tiempo==alarma && config_alarma==2){ //si la alarma esta configurada i coincideix amb el temps
					sprintf(cadena, "ALARMA!: %d:%d:%d", (alarma/3600)%24,(alarma/60)%60,alarma%60); //la alarma ha sonat
					escribir(cadena,linea); 			// Escribimos cadena
				}else{ //s'esta configurant l'alarma o no coincideix amb el temps
					 sprintf(cadena, "alarma: %d:%d:%d", (alarma/3600)%24,(alarma/60)%60,alarma%60); //posem la nova hora de la alarma per pantalla
					 escribir(cadena,linea); 			// Escribimos cadena
				}
			}
			linea--; //linea del temps

			linea--; //linea inicial

		}
	
   	}
    while(1);
}


/**************************************************************************
 * MINIPROGRAMA DE LOS BOTONES:
 * Mediante este programa, se detectará que botón se ha pulsado
 * 		 
 * Sin Datos de entrada
 * 
 * Sin datos de salida
 * 
 * Actualiza el valor estado
 * 
 **************************************************************************/
 
#pragma vector=PORT2_VECTOR  //interrupción de los botones. Actualiza el valor de la variable global estado.
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
	 case 0x10:

		 //joystick up
		 //estado = 6;
		 if(TA1CCR0<330){//incrementem TA1CCR0
			 TA1CCR0 += 33;
		 }
		 estado = TA1CCR0;
		 break;
	 case 0x20:
		 //joystick down

		 //estado = 7;
		 if(TA1CCR0>33){ //decrementem TA1CCR0
			 TA1CCR0 -= 33;
		 }
		 estado = TA1CCR0;
		 break;
	 case 0x08:
		 //joystick center
		 if(config_alarma==0){
			 config_alarma=1; //configurem l'alarma
		 }else{
			 config_alarma=2; //no configurem l'alarma
		 }
	 case 0x40:
		 //boto s1
		 if(config_alarma==1){ //si estem configurant l'alarma
			 if(alarma==86399){ //si la alarma era 23:59:59 sera llavors 00:00:00
				 alarma = 0;
			 }else{
				 alarma++;   //augmentem la alarma
			 }


		 }


	 case 0x80:
		 //boto s2
		 if(config_alarma==1){ //si estem configurant l'alarma
			 if(alarma>0){  // si la larma era 00:00:00 seguira sent 00:00:00
				alarma--;
			 }
		 }

	 }



	
	/***********************************************
   	 * HASTA AQUI BLOQUE CASE
   	 ***********************************************/	
	
	P2IFG = 0;		//limpiamos todas las interrupciones
	P2IE |= 0xC0; 	//interrupciones botones S1 y S2 (P2.6 y P2.7) reactivadas
	P2IE |= 0x3E;  //interrupciones joystick (2.1-2.5) reactivadas
 return;
}


/**************************************************************************
 * MINIPROGRAMA DE TIMER:
 * Mediante este programa, se detectará el timer
 *
 * Sin Datos de entrada
 *
 * Sin datos de salida
 *
 * Aumenta nuestra variable i
 *
 **************************************************************************/
#pragma vector= TIMER1_A0_VECTOR
__interrupt void Timer1_ISR(void)
{
	i++; //Quan s'arriba a TA1CCR0 s'incrementa i (Depen de la frequencia si es ACLK o SMCLK)
}


