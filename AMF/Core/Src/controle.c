#include "controle.h"
#include "main.h"

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
	float temp = I_pos * DT_pos * (erreur + prev_erreur) * 0.5f;
	integrale += temp;

	// Limits du partie integrale
	// AJOUTER SATURATION DU SORTIE
	return 0.0f;
}
