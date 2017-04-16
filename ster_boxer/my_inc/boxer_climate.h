/*
 * boxer_climate.h
 *
 *  Created on: 29 lip 2015
 *      Author: Doles
 */

#ifndef MY_INC_BOXER_CLIMATE_H_
#define MY_INC_BOXER_CLIMATE_H_

#include "boxer_struct.h"
#include "stm32f0xx_tim.h"
#include "system_gpio.h"
#include "ds18b20.h"
#include "debug.h"
#include "boxer_timers.h"

DS18B20Sensor_t ds18b20_1;
DS18B20Sensor_t ds18b20_2;

void Climate_TempCtrl_Handler(void);
void Climate_SensorsHandler(void);

#endif /* MY_INC_BOXER_CLIMATE_H_ */
