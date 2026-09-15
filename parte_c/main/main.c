#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

// Semáforo 1 (Tte. Cantaluppi)
#define S1_ROJO     GPIO_NUM_25
#define S1_AMARILLO GPIO_NUM_26
#define S1_VERDE    GPIO_NUM_27

// Semáforo 2 (Gral. Genes)
#define S2_ROJO     GPIO_NUM_12
#define S2_AMARILLO GPIO_NUM_13
#define S2_VERDE    GPIO_NUM_14

// Pulsador Peatonal
#define PIN_BOTON   GPIO_NUM_4

// Función auxiliar para esperar de forma fraccionada y reaccionar al botón en < 500ms
int esperar_con_pulsador(int ms_totales) {
    int transcurrido = 0;
    while (transcurrido < ms_totales) {
        if (gpio_get_level(PIN_BOTON) == 0) { // Pulsador presionado (Pull-up lee 0)
            return 1; 
        }
        vTaskDelay(pdMS_TO_TICKS(100)); // Revisa cada 100 ms
        transcurrido += 100;
    }
    return 0;
}

// Rutina de emergencia peatonal (ambos en rojo por 8 segundos)
void ejecutar_emergencia_peatonal(void) {
    // Forzar ambos semáforos a rojo
    gpio_set_level(S1_VERDE, 0);   gpio_set_level(S1_AMARILLO, 0);   gpio_set_level(S1_ROJO, 1);
    gpio_set_level(S2_VERDE, 0);   gpio_set_level(S2_AMARILLO, 0);   gpio_set_level(S2_ROJO, 1);
    
    // Mantener 8 segundos según la consigna de la Parte C
    vTaskDelay(pdMS_TO_TICKS(8000));
}

void app_main(void) {
    // Configuración de pines de salida para Semáforo 1
    gpio_reset_pin(S1_ROJO);
    gpio_set_direction(S1_ROJO, GPIO_MODE_OUTPUT);
    gpio_reset_pin(S1_AMARILLO);
    gpio_set_direction(S1_AMARILLO, GPIO_MODE_OUTPUT);
    gpio_reset_pin(S1_VERDE);
    gpio_set_direction(S1_VERDE, GPIO_MODE_OUTPUT);

    // Configuración de pines de salida para Semáforo 2
    gpio_reset_pin(S2_ROJO);
    gpio_set_direction(S2_ROJO, GPIO_MODE_OUTPUT);
    gpio_reset_pin(S2_AMARILLO);
    gpio_set_direction(S2_AMARILLO, GPIO_MODE_OUTPUT);
    gpio_reset_pin(S2_VERDE);
    gpio_set_direction(S2_VERDE, GPIO_MODE_OUTPUT);

    // Configuración del pulsador con resistencia Pull-Up interna
    gpio_reset_pin(PIN_BOTON);
    gpio_set_direction(PIN_BOTON, GPIO_MODE_INPUT);
    gpio_set_pull_mode(PIN_BOTON, GPIO_PULLUP_ONLY);

    while (1) {
        // ==========================================
        // FASE 1: S1 en VERDE / S2 en ROJO
        // ==========================================
        gpio_set_level(S1_ROJO, 0); gpio_set_level(S1_AMARILLO, 0); gpio_set_level(S1_VERDE, 1);
        gpio_set_level(S2_ROJO, 1); gpio_set_level(S2_AMARILLO, 0); gpio_set_level(S2_VERDE, 0);
        
        if (esperar_con_pulsador(4000)) { 
            ejecutar_emergencia_peatonal(); 
            continue; 
        }

        // ==========================================
        // FASE 2: S1 en AMARILLO / S2 en ROJO
        // ==========================================
        gpio_set_level(S1_VERDE, 0); gpio_set_level(S1_AMARILLO, 1);
        
        if (esperar_con_pulsador(1500)) { 
            ejecutar_emergencia_peatonal(); 
            continue; 
        }

        // ==========================================
        // FASE 3: Transición de seguridad (1 segundo)
        // S1 pasa a ROJO, S2 espera 1s antes de su verde
        // ==========================================
        gpio_set_level(S1_AMARILLO, 0); gpio_set_level(S1_ROJO, 1);
        
        if (esperar_con_pulsador(1000)) { // 1 segundo exacto de espera de seguridad[cite: 1]
            ejecutar_emergencia_peatonal(); 
            continue; 
        }

        // ==========================================
        // FASE 4: S1 en ROJO / S2 en VERDE
        // ==========================================
        gpio_set_level(S1_ROJO, 1);
        gpio_set_level(S2_ROJO, 0); gpio_set_level(S2_VERDE, 1);
        
        if (esperar_con_pulsador(4000)) { 
            ejecutar_emergencia_peatonal(); 
            continue; 
        }

        // ==========================================
        // FASE 5: S1 en ROJO / S2 en AMARILLO
        // ==========================================
        gpio_set_level(S2_VERDE, 0); gpio_set_level(S2_AMARILLO, 1);
        
        if (esperar_con_pulsador(1500)) { 
            ejecutar_emergencia_peatonal(); 
            continue; 
        }

        // ==========================================
        // FASE 6: Transición de seguridad inversa (1 segundo)
        // S2 pasa a ROJO antes de liberar S1 nuevamente
        // ==========================================
        gpio_set_level(S2_AMARILLO, 0); gpio_set_level(S2_ROJO, 1);
        
        if (esperar_con_pulsador(1000)) { // 1 segundo de seguridad[cite: 1]
            ejecutar_emergencia_peatonal(); 
            continue; 
        }
    }
}