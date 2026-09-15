#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

// Definición de los pines GPIO para los LEDs
#define PIN_ROJO     GPIO_NUM_25
#define PIN_AMARILLO GPIO_NUM_26
#define PIN_VERDE    GPIO_NUM_27

void app_main(void)
{
    // Configuración de los pines como salida digital
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << PIN_ROJO) | (1ULL << PIN_AMARILLO) | (1ULL << PIN_VERDE),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);

    // Bucle infinito del ciclo del semáforo
    while (1) {
        // 1. Estado: Detenido (Rojo ON) - 5000 ms[cite: 1]
        gpio_set_level(PIN_ROJO, 1);
        gpio_set_level(PIN_AMARILLO, 0);
        gpio_set_level(PIN_VERDE, 0);
        vTaskDelay(pdMS_TO_TICKS(5000));

        // 2. Estado: Circulando (Verde ON) - 4000 ms[cite: 1]
        gpio_set_level(PIN_ROJO, 0);
        gpio_set_level(PIN_AMARILLO, 0);
        gpio_set_level(PIN_VERDE, 1);
        vTaskDelay(pdMS_TO_TICKS(4000));

        // 3. Estado: Por detenerse (Amarillo ON) - 1500 ms[cite: 1]
        gpio_set_level(PIN_ROJO, 0);
        gpio_set_level(PIN_AMARILLO, 1);
        gpio_set_level(PIN_VERDE, 0);
        vTaskDelay(pdMS_TO_TICKS(1500));
    }
}