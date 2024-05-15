
#include "main.h"
#include "config.h"

#define MQTT_QOS0	0
#define MQTT_QOS1	1
#define MQTT_QOS2	2

const char *TAG = "MAIN";

/* Topics */
const char *topic_rssi = "practica1/rssi";		// Publisher Topic RSSI
const char *topic_state = "practica1/state";	// Publisher Topic State System
const char *topic_control = "practica1/control";	// Subscriber Topic

const char *on_state ="ON";
const char *off_state ="OFF";

typedef enum
{
	state_Init = 0,
	state_On,
	state_Off,
	state_Updt,
	state_Idle,
}os_state;

os_state fsm;
int8_t pot;
char msg[20];
char msg_state[20];

bool working;
bool updt_state;
/*******************************************************************************
 Programa principal
******************************************************************************/


void app_subscribeEvent(const char* topic, const char* data);

void app_main(void)
{
	fsm = state_Init;
  /* Inicializaciones */

  while(1){

	  switch (fsm) {
		case state_Init:
			ESP_LOGI(TAG, "state_Init");
			working = false;
			updt_state = false;
			IO_gpioInit();
			WIFI_init();
			MQTT_init();
			/* Subscribe Control */
			MQTT_subscribe(topic_control, app_subscribeEvent);
			fsm = state_Off;
			break;

		case state_Updt:
			if (working)
			{
				fsm = state_On;
				sprintf(msg_state, "%s", on_state);
				MQTT_publish(topic_state, msg_state, MQTT_QOS2);
			}
			else
			{
				fsm = state_Off;
				sprintf(msg_state, "%s", off_state);
				MQTT_publish(topic_state, msg_state, MQTT_QOS2);
			}
			ESP_LOGI(TAG, "topic: %s, mensaje: %s", topic_state, msg_state);
			break;

		case state_On:


			pot = WIFI_getRSSI();
			sprintf(msg, "%hhd", pot);
			MQTT_publish(topic_rssi, msg, MQTT_QOS0);
			ESP_LOGI(TAG, "topic: %s, mensaje: %s", topic_rssi, msg);

			CRONO_delayMs(500);

			if (updt_state)
			{
				fsm = state_Updt;
				updt_state = false;
			}
			break;

		case state_Off:
			CRONO_delayMs(10);

			if (updt_state)
			{
				fsm = state_Updt;
				updt_state = false;
			}

			break;

		case state_Idle:
			ESP_LOGI(TAG, "state_Idle");
			break;

		default:
			ESP_LOGI(TAG, "default");
			break;
	}

  }

}

void app_subscribeEvent(const char *topic, const char *data) {

	ESP_LOGI(TAG, "MQTT: Mensaje recibido: %s %s\n", topic, data);

//	IO_toggleLed();

	if (strcmp(topic_control, topic) == 0) {
		if (strcmp(data, on_state) == 0) {
			if (working == false)	updt_state=true;
			working = true;
		}else if (strcmp(data, off_state) == 0) {
			if (working == true)	updt_state=true;
			working = false;
		}
	}

}
