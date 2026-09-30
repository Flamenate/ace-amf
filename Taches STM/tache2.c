int main(void)
{
    uint16_t acquisition_brute;
    float tension;

    while (1)
    {
        acquisition_brute = HAL_ADC_GetValue(&hadc1);
        tension = ((float)acquisition_brute * 3.3) / 4095; 
		//valeurs du datasheet stm32f411
		//https://www.st.com/en/microcontrollers-microprocessors/stm32f411/documentation.html
    }
 return 0
}
