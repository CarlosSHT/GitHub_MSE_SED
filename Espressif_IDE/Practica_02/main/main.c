
#include "config.h"
#include "main.h"
#include <stdint.h>


const char *TAG = "MAIN";
uint16_t data;

volatile int samples[SAMPLES_SIZE];
volatile int n = 0;

/*******************************************************************************
 Programa principal
******************************************************************************/

void app_main(void)
{
  /* Inicializaciones */
	printf("Inicializacion del sistema\n");
	/* Inicialización del ADC */
	IO_adcInit();

	/* Inicialización del GPIO */
	IO_gpioInit();

	/* Inicialización del PWM */
	IO_pwmInit();

	/* Inicialización del Timer */
	CRONO_timerInit();

	// Muestreo
	CRONO_timerStart(20);
	printf("GRABACIÓN INICIADA\n");
	while(n < SAMPLES_SIZE){
	    IO_toggleLed();
	    CRONO_delayMs(500);
	}
	CRONO_timerStop();
	printf("GRABACIÓN FINALIZADA\n");

	// Intervalo de espera...
	IO_monitorPause("Presione Enter para continuar...\n");

	// Reproducción
	printf("REPRODUCCIÓN INICIADA\n");
	for(n = 0; n < SAMPLES_SIZE ; n++){
	    IO_pwmSet(samples[n] / 4096.0 * 100);
	    IO_monitorStem(samples[n]);
	    CRONO_delayMs(20);
	}
	IO_pwmSet(0);
	printf("REPRODUCCIÓN FINALIZADA\n");
	n = 0;

	while(1){
	    // loop
	    IO_toggleLed();
	    CRONO_delayMs(250);
	}
}
