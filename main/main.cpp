#include <stdio.h>
#include "DendoStepper.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/adc.h"
 
#define MAX_SPEED 2000
#define MIN_SPEED 200

static uint16_t speed=200;

DendoStepper motor;

static long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

static void potTask(void*p){
    //measures voltage level on GPIO36 and changes the speed accordingly
    while(1){
        uint16_t adc = adc1_get_raw(ADC1_CHANNEL_0);
        ESP_LOGI("ADC","%d",adc);
        speed = map(adc,0,4095,MIN_SPEED,MAX_SPEED);

        
        vTaskDelay(10);
    }
}
 
extern "C" void app_main(void)
{
    //init adc
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_0,ADC_ATTEN_DB_2_5);

    gpio_config_t gp_cfg = {
        .pin_bit_mask = 1ULL << GPIO_NUM_32,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&gp_cfg);
    //init on/off button

    int p=0;
    xTaskCreate(potTask,"potTask",4096,NULL,5,NULL);
    motor.init(14, 13, 18, TIMER_GROUP_0, TIMER_0, MICROSTEP_1, 200);
    
 
    while (1)
    {
        motor.setSpeed(speed, 1000);
        motor.runPos(-800);
        while(motor.getState() > IDLE){
            vTaskDelay(1);
        }
        vTaskDelay(200/portTICK_PERIOD_MS);    //caka 200ms po zasune
        p++;
        printf("\nHand inside: %d times\n",p);
        
        motor.setSpeed(speed, 1000);
        motor.runPos(800);
        while(motor.getState() > IDLE){
            vTaskDelay(1);
        }
        while(!gpio_get_level(GPIO_NUM_32)){
            //OFF tlacidlo je stlacene nerob nist
            ESP_LOGE("ruka","OFF");
            vTaskDelay(10);
        }
        vTaskDelay(10/portTICK_PERIOD_MS);  //caka 10ms kym znovu zasune
    }
}