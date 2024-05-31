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
	state_Idle,
} os_state;

#define JSON_TOPIC	"{ \"miliseconds\": %d, \"val_ldr\": %d }"
/* Topics */
const char *topic_LEDctrl = "practica2/brillo_led";			// Subs birllo de LED
const char *topic_updtLDR = "practica2/updt_LDR";			// Subs control lectura LDR
const char *topic_state = "practica2/Estado_muestreo";		// Pub estado Muestreo

const char *topic_LDR_data = "practica2/LDR_data"; 			// Pub valor sensor LDR

const char *on_state = "ON";
const char *off_state = "OFF";

const char *TAG = "MAIN";

os_state prev_fsm;
os_state fsm;

uint16_t ldr_data;
float duty_pwm;
bool_t updt_pwm;

bool_t sampling;
bool_t prevstate_ldr;
bool_t updtstate_ldr;

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

void app_main(void) {
	fsm = state_Init;

	while (1) {
		if (updt_pwm) {
			IO_pwmSet(duty_pwm);
			updt_pwm = false;
		}

		switch (fsm) {
		case state_Init:
			/* Inicializaciones */
			ESP_LOGI(TAG, "Inicializacion del sistema");
			/* Inicialización del ADC */
			IO_adcInit();

//			/* Inicialización del GPIO */
//			IO_gpioInit();

			/* Inicialización del PWM */
			IO_pwmInit();

			/* Inicialización del Timer */
			CRONO_timerInit(app_TimerIRQ);

			WIFI_init();
			MQTT_init();

			/* Subscribe Control */
			MQTT_subscribe(topic_LEDctrl, app_subsEventMQTT);
			MQTT_subscribe(topic_updtLDR, app_subsEventMQTT);

			sampling = false;
			prevstate_ldr = false;
			updtstate_ldr = false;
			irq_timer = false;
			fsm = state_Idle;
			break;

		case state_UpdtSampler:
			printf(" UpdtSampler state \n");
			if (updtstate_ldr) {
				CRONO_timerStart(100);
				n = 0;
				sprintf(msg_state, "%s", on_state);
				fsm = state_Working;
			} else {
				CRONO_timerStop();
				n = 0;
				sprintf(msg_state, "%s", off_state);
				fsm = state_Idle;
			}

			MQTT_publish(topic_state, msg_state, MQTT_QOS2);

			break;

		case state_Working:
			if (irq_timer) {
				irq_timer = false;

				samples[n] = ldr_data;
				IO_monitorStem(samples[n]);

				n++;
				sprintf(msg, JSON_TOPIC, n * 100, samples[n - 1]);
//				printf("%s\n", msg);

				MQTT_publish(topic_LDR_data, msg, MQTT_QOS0);

				if (n >= 200) {
					n = 0;
					updtstate_ldr = false;
					fsm = state_UpdtSampler;
				}

			} else {
				CRONO_delayMs(10);
			}

			break;

		case state_Idle:
			CRONO_delayMs(10);
			break;

		default:
			break;
		}

	}
}

void app_subsEventMQTT(const char *topic, const char *data) {
	if (strcmp(topic_LEDctrl, topic) == 0) {
		duty_pwm = atof(data);
		updt_pwm = true;
	} else if (strcmp(topic_updtLDR, topic) == 0 && strcmp(data, "updt") == 0) {
		updtstate_ldr = updtstate_ldr ^ 1;
		fsm = state_UpdtSampler;
	}
}

void app_TimerIRQ(void) {
	irq_timer = true;
	ldr_data = IO_readAdc();
}
