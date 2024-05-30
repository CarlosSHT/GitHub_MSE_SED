
#include "config.h"
#include "main.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>


#define MQTT_QOS0	0
#define MQTT_QOS1	1
#define MQTT_QOS2	2

typedef bool bool_t;

typedef enum
{
	state_Init = 0,
	state_UpdtSampler,
	state_Working,
	state_Off,
	state_UpdtWork,
	state_Idle,
}os_state;

#define JSON_TOPIC	"{ \"miliseconds\": %d, \"val_ldr\": %d }\n"
/* Topics */
const char *topic_LEDctrl = "practica2/brillo_led";		// Publisher Topic RSSI
const char *topic_updtLDR = "practica2/updt_LDR";	// Publisher Topic State System
const char *topic_control = "practica2/Estado_muestreo";	// Subscriber Topic
const char *topic_state = "practica2/Estado_muestreo";	// Publisher Topic State System

const char *topic_LDR_data = "practica2/LDR_data"; //Subscriber Topic

const char *on_state ="ON";
const char *off_state ="OFF";

const char *TAG = "MAIN";

os_state prev_fsm;
os_state fsm;

uint16_t ldr_data;
float 	duty_pwm;
bool_t 	updt_pwm;

bool_t	sampling;
bool_t	prevstate_ldr;
bool_t	updtstate_ldr;

char msg[45];
char msg_state[20];
bool_t irq_timer;

int samples[SAMPLES_SIZE];
int n = 0;

/*******************************************************************************
 Programa principal
******************************************************************************/

void app_subsEventMQTT(const char *topic, const char *data);
void app_TimerIRQ(void);

void app_main(void)
{
	fsm = state_Init;

	while (1) {
		if (updt_pwm) {
			IO_pwmSet(duty_pwm);
			updt_pwm=false;
		}

		switch (fsm) {
			case state_Init:
			  /* Inicializaciones */
				ESP_LOGI(TAG, "Inicializacion del sistema");
//				/* Inicialización del ADC */
//				IO_adcInit();

//				/* Inicialización del GPIO */
//				IO_gpioInit();

				/* Inicialización del PWM */
				IO_pwmInit();

				/* Inicialización del Timer */
				CRONO_timerInit(app_TimerIRQ);


				WIFI_init();
				MQTT_init();


				/* Subscribe Control */
				MQTT_subscribe(topic_LEDctrl, app_subsEventMQTT);
				MQTT_subscribe(topic_updtLDR, app_subsEventMQTT);

//				updt_pwm = false;
				sampling=false;
				prevstate_ldr=false;
				updtstate_ldr=false;
				irq_timer=false;
				fsm = state_Idle;
				break;

			case state_UpdtSampler:
				printf("state_UpdtSampler\n");
				if (updtstate_ldr)
				{
					CRONO_timerStart(100);
					n=0;
					sprintf(msg_state, "%s", on_state);
					fsm = state_Working;
				}
				else
				{
					CRONO_timerStop();
					n=0;
					sprintf(msg_state, "%s", off_state);
					fsm = state_Idle;
				}

				MQTT_publish(topic_state, msg_state, MQTT_QOS2);

				break;

			case state_Working:
				if (irq_timer) {
					irq_timer = false;

					samples[n] = ldr_data;
					n++;
					sprintf(msg, JSON_TOPIC, n*100, samples[n-1]);
//					sprintf(msg, "%d", ldr_data);

					printf("%s\n",msg);
					MQTT_publish(topic_LDR_data, msg, MQTT_QOS0);

					if(n>=200)
					{
						n=0;
						updtstate_ldr = false;
						fsm = state_UpdtSampler;
					}

				}
				else
				{
					CRONO_delayMs(10);
				}

				break;

			case state_Off:

				break;

			case state_Idle:
				CRONO_delayMs(10);
//				if (updtstate_ldr != prevstate_ldr) {
//					printf("Actualiza  updtstate_ldr %d\n", updtstate_ldr);
//					prevstate_ldr = updtstate_ldr;
//					fsm = state_UpdtSampler;
//				}
//				else
//				{
////					printf("state_Idle\n");
//					CRONO_delayMs(10);
//				}

				break;

			default:
				break;
		}

	}





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


void app_subsEventMQTT(const char *topic, const char *data)
{
	printf("Evento: %s %s\n", topic, data);

	if (strcmp(topic_LEDctrl, topic) == 0) {
		duty_pwm = atof(data);
		updt_pwm=true;
	}
	else if (strcmp(topic_updtLDR, topic) == 0 && strcmp(data, "updt") == 0) {
		updtstate_ldr=updtstate_ldr ^ 1;
		fsm = state_UpdtSampler;
	}


}


void app_TimerIRQ(void)
{
	irq_timer = true;
	ldr_data=IO_readAdc();
}
