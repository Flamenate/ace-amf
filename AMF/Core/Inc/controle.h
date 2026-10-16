#ifndef INC_CONTROLE_H_
#define INC_CONTROLE_H_

#include "vl53l0x_api.h"

#define VL53L0X_OFFSET 	60 	// [mm]
#define P_pos			1.0f
#define I_pos			1.0f
#define P_cur			1.0f
#define I_cur			1.0f
#define DT_pos			0.01f 	// [s]
#define DT_cur			0.001f 	// [s]
#define VCC				2.85f	// [V]
#define R				1.5f 	// [ohm]
#define I_MAX			1.5f	// [A]
#define P_MAX			I_MAX * I_MAX * R // [W]
#define ADC_MAX			3723.0f	// Pour 1.5A l'ampli donne 3V -> ADC_MAX = 3.0*4095/3.3

void VL53L0X_config(VL53L0X_Dev_t *hvl53l0x, I2C_HandleTypeDef *hi2c, VL53L0X_Error *status);
uint16_t VL53L0X_LeerDistanciaMM(VL53L0X_Dev_t *hvl53l0x, VL53L0X_RangingMeasurementData_t *RangingData, VL53L0X_Error *status);
float pid_position(int16_t erreur);
float pid_courrant(float erreur);
void set_pwm_duty(float duty);
void pid_set_p(float P);
void pid_set_i(float I);

#endif /* INC_CONTROLE_H_ */
