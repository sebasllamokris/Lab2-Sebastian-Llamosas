#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define PIN_ROJO     GPIO_NUM_25
#define PIN_AMARILLO GPIO_NUM_26
#define PIN_VERDE    GPIO_NUM_27
#define PIN_BOTON    GPIO_NUM_33

// Función auxiliar para revisar el botón constantemente (reacción < 500ms)
int esperar_con_boton(int milisegundos) {
    int ciclos = milisegundos / 50; 
    for (int i = 0; i < ciclos; i++) {
        if (gpio_get_level(PIN_BOTON) == 0) { // Detecta pulsación a GND
            return 1; 
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
    return 0; 
}

void app_main(void)
{
    // Configurar LEDs como salida
    gpio_set_direction(PIN_ROJO, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_AMARILLO, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_VERDE, GPIO_MODE_OUTPUT);

    // Configurar Botón en GPIO 33 con Pull-Up interno
    gpio_set_direction(PIN_BOTON, GPIO_MODE_INPUT);
    gpio_set_pull_mode(PIN_BOTON, GPIO_PULLUP_ONLY);

    while (1) {
        // 1. Estado: ROJO (5 segundos de ciclo normal)
        gpio_set_level(PIN_ROJO, 1);
        gpio_set_level(PIN_AMARILLO, 0);
        gpio_set_level(PIN_VERDE, 0);
        vTaskDelay(pdMS_TO_TICKS(5000));

        // 2. Estado: VERDE (revisa el botón peatonal)
        gpio_set_level(PIN_ROJO, 0);
        gpio_set_level(PIN_AMARILLO, 0);
        gpio_set_level(PIN_VERDE, 1);
        
        // Si el peatón presiona el botón durante el verde, interrumpe y pasa a rojo
        if (esperar_con_boton(4000)) { 
            goto ir_a_rojo;
        }

        // 3. Estado: AMARILLO (precaución)
        gpio_set_level(PIN_ROJO, 0);
        gpio_set_level(PIN_AMARILLO, 1);
        gpio_set_level(PIN_VERDE, 0);
        if (esperar_con_boton(1500)) {
            goto ir_a_rojo;
        }
        
        continue;

    ir_a_rojo:
        // Fuerza el rojo inmediatamente por el cruce peatonal y cumple los 5000ms
        gpio_set_level(PIN_ROJO, 1);
        gpio_set_level(PIN_AMARILLO, 0);
        gpio_set_level(PIN_VERDE, 0);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}