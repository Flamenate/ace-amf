int main(void)
{
    uint16_t acquisition_brute;
    float tension;

    ADC_Init();

    while (1)
    {
        acquisition_brute = ADC_Read();
        tension = ((float)acquisition_brute * 3.3) / 4095; 
		//valeurs du datasheet stm32f411
		//https://www.st.com/en/microcontrollers-microprocessors/stm32f411/documentation.html

        printf("ADC = %u | Tension = %.3f V\n",
               acquisition_brute,
               tension);
    }
 return 0
}