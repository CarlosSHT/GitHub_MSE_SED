
#include "config.h"
#include "main.h"
#include <stdint.h>


typedef enum
{
	state_Init = 0,
	state_LDRread,
	state_Off,
	state_Updt,
	state_Idle,
}os_state;


os_state fsm;

const char *TAG = "MAIN";
uint16_t data_LDR;


/*******************************************************************************
 Programa principal
******************************************************************************/

void app_main(void)
{
  /* Inicializaciones */

  while(1){
	  switch (fsm) {
		case state_Init:
			printf("Inicializacion del sistema\n");
			/* Inicialización del ADC */
			IO_adcInit();
			data_LDR=0;
			fsm = state_LDRread;
			break;

		case state_LDRread:
//			fsm = state_LDRread;

			data_LDR = IO_readAdc();
			printf("SENSOR: %i\n", data_LDR);

			CRONO_delayMs(250);

			break;
		default:
			break;
	}

  }

}
