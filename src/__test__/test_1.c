#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

/* variables */
#define DETECT_ATTACK_SENSOR_SENSITIVITY 0.3 //! the lesser the sensitivity as value increase
#define DETECT_ATTACK_SENSOR_SENSITIVITY_STEP 2
#define DETECT_ATTACK_SENSOR_READ_DELAY_TIME 100 //! in milliseconds

int minimum_range = 100;
long int seed = 1923949202;
void delay_(int delay_time)
{
    int i;
    for (i = 0; i <= delay_time * seed; i++)
        ;
}
int detect_attack()
{
    int detect_attack_tracker_null = 0;
    int detect_attack_tracker_true = 0;
    int init_sensor_reading = 100;

    while (true)
    {
        srand(time(0));
        printf("Random value -> %d\n", (unsigned int)rand() * seed);
        int current_sensor_reading = ((unsigned int)(rand() * seed) % minimum_range) + minimum_range;

        printf("----------------\n");
        printf("Current value -> %d\n", current_sensor_reading);
        printf("Initial value -> %d\n", init_sensor_reading);
        printf("----------------\n");
        if ((current_sensor_reading - init_sensor_reading) > DETECT_ATTACK_SENSOR_SENSITIVITY)
        {
            detect_attack_tracker_true++;
            printf("detecting attack value -> %d\n", detect_attack_tracker_true);
        }
        else
        {
            detect_attack_tracker_null++;
            printf("detecting no attack value -> %d\n", detect_attack_tracker_null);
        }
        init_sensor_reading = current_sensor_reading;
        delay_((int)DETECT_ATTACK_SENSOR_READ_DELAY_TIME * DETECT_ATTACK_SENSOR_SENSITIVITY);
        // breaks out of loop after 3 failed attemps
        if (detect_attack_tracker_null >= 3)
            return 0;
        if (detect_attack_tracker_true >= 7)
            return 1;
    }
}

int main(void)
{
    int res = detect_attack();
    printf("Result => %d\n", res);
    if (res)
    {
        printf("An attack has been detected");
        return 0;
    }

    printf("An attack was not detected");
    return 1;
}