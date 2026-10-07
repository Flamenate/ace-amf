#include "controle.h"
#include "main.h"
#include "tim.h"
#include "adc.h"

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
	//HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

	// Convertion lecture ADC a courant en ampere
	// Pour l'instant j'ai fait l'assumision que pour 1.5A (I_MAX) -> 4095 et 0A -> 0

	courant = I_MAX * ((float)adc_raw / ADC_MAX);

	HAL_ADC_Start_DMA(&hadc1, &adc_raw, 1);
	__HAL_DMA_DISABLE_IT(hadc1.DMA_Handle, DMA_IT_HT);

	// POUR TESTING VITESSE DU SEMAPHORE
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	xSemaphoreGiveFromISR(semaphore_courant, &xHigherPriorityTaskWoken);

	portYIELD_FROM_ISR(xHigherPriorityTaskWoken)

}

void VL53L0X_config(VL53L0X_Dev_t *hvl53l0x, I2C_HandleTypeDef *hi2c, VL53L0X_Error *status) {
    hvl53l0x->I2cHandle = hi2c;				// Handle i2c pour communiquer avec senseur
    hvl53l0x->I2cDevAddr = 0x52;         	// Direction i2c par default (0x29 << 1)
    uint8_t vhv = 0;
    uint8_t phase = 0;

    // Reset du senseur
    HAL_GPIO_WritePin(RESET_VL53_GPIO_Port, RESET_VL53_Pin, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(RESET_VL53_GPIO_Port, RESET_VL53_Pin, GPIO_PIN_SET);
    HAL_Delay(10);

    *status = VL53L0X_DataInit(hvl53l0x);
    *status = VL53L0X_StaticInit(hvl53l0x);
    *status = VL53L0X_PerformRefCalibration(hvl53l0x, &vhv, &phase);

    *status = VL53L0X_SetDeviceMode(hvl53l0x, VL53L0X_DEVICEMODE_SINGLE_RANGING);
}

uint16_t VL53L0X_LeerDistanciaMM(VL53L0X_Dev_t *hvl53l0x, VL53L0X_RangingMeasurementData_t *RangingData, VL53L0X_Error *status) {
    *status = VL53L0X_PerformSingleRangingMeasurement(hvl53l0x, RangingData);

    if (RangingData->RangeStatus == 0) {
        return RangingData->RangeMilliMeter - VL53L0X_OFFSET; // Mesure valide, on reste l'offset manuellement. On a pas les materiaux pour calibrer le senseur
    }
    return 0; // Verifier RangingData.RangeStatus pour obtenir l'erreur precis
}

float pid_position(int16_t erreur) {
	static int16_t prev_erreur = 0;
	static float integrale = 0;

	// La masse est plus haute que la consigne -> Le fils doit refroidir
	if (erreur <= 0) {
		return 0.0f;
	}

	float proportionale = P_pos * erreur;
	float temp = I_pos * DT_pos * ((float)erreur + (float)prev_erreur) * 0.5f;
	integrale += temp;

	prev_erreur = erreur;

	float integrale_max;

	// Limits du partie integrale
	if (proportionale > P_MAX) {
		integrale_max = 0;
	} else {
		integrale_max = P_MAX - proportionale;
	}

	if (integrale > integrale_max) {
		integrale = integrale_max;
	}

	// Puissance = P + I
	float consigne_puisance = proportionale + integrale;

	// Saturation de la consigne
	if (consigne_puisance > P_MAX) {
		consigne_puisance = P_MAX;
	}

	return consigne_puisance;
}

float pid_courrant(float erreur) {
	static int16_t prev_erreur = 0;
	static float integrale = 0;

	float proportionale = P_pos * erreur;
	float temp = I_pos * DT_pos * ((float)erreur + (float)prev_erreur) * 0.5f;
	integrale += temp;

	prev_erreur = erreur;

	float integrale_max;

	// Limits du partie integrale
	if (proportionale > P_MAX) {
		integrale_max = 0;
	} else {
		integrale_max = P_MAX - proportionale;
	}

	if (integrale > integrale_max) {
		integrale = integrale_max;
	}

	// Puissance = P + I
	float consigne_puisance = proportionale + integrale;

	// Saturation de la consigne
	if (consigne_puisance > P_MAX) {
		consigne_puisance = P_MAX;
	}

	return consigne_puisance;
}

void set_pwm_duty(float duty) {
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, (float)(duty * __HAL_TIM_GET_AUTORELOAD(&htim3)));
}
