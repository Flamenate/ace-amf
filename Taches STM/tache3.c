#define BAUD_RATE 115200

int main(void)
{
    while (1)
    {
        float valeur = 1234f; //on remplace par la valeur lue de l'ADC

        char message[50];
        sprintf(message, "Valeur = %u\r\n", valeur);



        Delay_ms(1000);
    }

    return 0;
}
