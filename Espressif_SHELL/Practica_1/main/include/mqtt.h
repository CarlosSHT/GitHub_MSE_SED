/** \file	mqtt.h
 *  Mar 2022
 *  Maestría en Sistemas Embebidos / Sistemas embebidos distribuidos
  */

#ifndef MQTT_H
#define MQTT_H

typedef void (*extTopic)(const char * , const char * );

/* Prototipos */
void MQTT_init(void);
void MQTT_processTopic(const char * , const char *);
void MQTT_subscribe(const char *, extTopic);
void MQTT_publish(const char * , const char * , int );
/* Definiciones */
#define MAX_TOPIC_LENGTH  100
#define MAX_MSG_LENGTH  100

#endif  /* MQTT_H_ */
