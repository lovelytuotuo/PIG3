#ifndef __ESP8266_H
#define __ESP8266_H

#include "main.h"

void Net_Things_Init(void);
void Property_Post(uint8_t heartrate,uint8_t spo2);
void PubTopic(uint8_t *msg);

#endif




