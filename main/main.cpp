#include <stdio.h>
#include "DendoStepper.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
 
DendoStepper motor;
 
extern "C" void app_main(void)
{
    int p=0;
    motor.init(16, 17, 18, TIMER_GROUP_0, TIMER_0, MICROSTEP_1, 200);
    motor.setSpeed(2000, 1000);
 
    while (1)
    {
        motor.runPos(-800);
        while(motor.getState() > IDLE){
            vTaskDelay(1);
        }
        vTaskDelay(200/portTICK_PERIOD_MS);    //caka 200ms po zasune
        p++;
        printf("\nHand inside: %d times\n",p);
        
        motor.runPos(800);
        while(motor.getState() > IDLE){
            vTaskDelay(1);
        }
        vTaskDelay(10/portTICK_PERIOD_MS);  //caka 10ms kym znovu zasune
    }
}