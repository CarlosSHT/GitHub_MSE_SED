
#include "main.h"
#include "config.h"
const char *TAG = "MAIN";

int8_t pot;
char msg[20];
/*******************************************************************************
 Programa principal
******************************************************************************/
void app_main(void)
{
  /* Inicializaciones */
	IO_gpioInit();
	WIFI_init();

	MQTT_init();
	MQTT_subscribe("practica1/led");
  while(1){
      /* Loop */
	  pot = WIFI_getRSSI();
	  sprintf(msg, "%hhd", pot);
	  MQTT_publish("practica1/rssi", msg);
	  ESP_LOGI(TAG, "practica1/rssi, mensaje %s", msg);
	  CRONO_delayMs(1000);
  }

}
