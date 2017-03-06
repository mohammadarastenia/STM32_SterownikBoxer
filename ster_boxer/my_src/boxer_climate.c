/*
 * boxer_climate.c
 *
 *  Created on: 29 lip 2015
 *      Author: Doles
 */

#include "boxer_climate.h"
#include "boxer_timers.h"

static systime_t speedTimer = 0;
static uint8_t lastFanPull = 0;
static uint8_t lastFanPush = 0;

static systime_t measureOwireTimer = 0;
static systime_t measureI2cTimer = 0;
static systime_t shtInitTimer = 0;
static systime_t oWireInitTimer = 0;
static ErrorStatus error = SUCCESS;

/////////////////////////////////////////////////////////////////////////////
void Climate_SensorsHandler(void)
{
	if (systimeTimeoutControl(&oWireInitTimer, 2000))
	{
#ifndef OWIRE_OFF_MODE
		initializeConversion(&ds18b20_1);
    	initializeConversion(&ds18b20_2);
#endif
	}

	if (systimeTimeoutControl(&measureOwireTimer, 3000))
	{
#ifndef OWIRE_OFF_MODE
		readTemperature(&ds18b20_1);
		displayData.tempDS18B20_1_t = ds18b20_1.fTemp;
		readTemperature(&ds18b20_2);
		displayData.tempDS18B20_2_t = ds18b20_2.fTemp;
#endif
	}

	if (systimeTimeoutControl(&shtInitTimer, 2500))
	{
#ifndef I2C_OFF_MODE
		error = SHT21_SoftReset(I2C2, SHT21_ADDR);
#endif
	}

	if (systimeTimeoutControl(&measureI2cTimer, 5000))
	{
#ifndef I2C_OFF_MODE
		displayData.lux = TSL2561_ReadLux(&error);

        uint16_t tempWord = 0;
        uint16_t humWord = 0;

    	tempWord = SHT21_MeasureTempCommand(I2C2, SHT21_ADDR, &error);
    	humWord = SHT21_MeasureHumCommand(I2C2, SHT21_ADDR, &error);

    	humWord = ((uint16_t)(SHT_HumData.msb_lsb[0]) << 8) | SHT_HumData.msb_lsb[1];
    	tempWord = ((uint16_t)(SHT_TempData.msb_lsb[0]) << 8) | SHT_TempData.msb_lsb[1];

    	displayData.tempSHT2x = SHT21_CalcTemp(tempWord);
    	displayData.humiditySHT2x = SHT21_CalcRH(humWord);
#endif
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Climate_TempCtrl_Handler(void)
{
	if (systimeTimeoutControl(&speedTimer, 200))
	{
		flagsGlobal.increaseSpeedFlag = TRUE;
	}

    if ((tempControl.tempCtrlMode == TEMP_AUTO) && (gFansSoftStartFlag == FALSE)) //sterowanie temperatura maksymalna
	{
//    	_printParam(UC"userParam.tempControl", userParam.tempControl);
		if (xLightControl.lightingState == LIGHT_ON)
		{
//			_printParam(UC"LightControl.LightingState", LightControl.LightingState);
			if (flagsGlobal.increaseSpeedFlag == TRUE) //powoduje zwiekszanie PWM'a co sekunde
			{
				if (ds18b20_1.fTemp > (float)tempControl.userTemp)
				{
					//USARTx_SendString(USART_COMM, UC"fTemp > userTemp\n\r");

					PWM_IncPercentTo(PWM_FAN_PULL_AIR, 95); //wyciagajacy
					PWM_IncPercentTo(PWM_FAN_PUSH_AIR, 70); //wciagajacy
				}
				else
				{
					//USARTx_SendString(USART_COMM, UC"fTemp < userTemp\n\r");
					PWM_DecPercentTo(PWM_FAN_PULL_AIR, 60);
					PWM_DecPercentTo(PWM_FAN_PUSH_AIR, 40);
				}

				flagsGlobal.increaseSpeedFlag = FALSE;
			}
		}
		else
		{
//			_printParam(UC"LightControl.LightingState", LightControl.LightingState);
			PWM_SetPercent(PWM_FAN_PULL_AIR, 50);
			PWM_SetPercent(PWM_FAN_PUSH_AIR, 25);
		}
	}
    else if (tempControl.tempCtrlMode == TEMP_MANUAL)
    {
    	if (lastFanPull != tempControl.fanPull)
		{
			PWM_SetPercent(PWM_FAN_PULL_AIR, tempControl.fanPull);
		}

		if (lastFanPush != tempControl.fanPush)
		{
			PWM_SetPercent(PWM_FAN_PUSH_AIR, tempControl.fanPush);
		}

    	lastFanPull = tempControl.fanPull;
    	lastFanPush = tempControl.fanPush;
    }
}