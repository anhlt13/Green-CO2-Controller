//
// Created by baotr on 9/25/2026.
//

#include "encoder.h"
#include "system_state.h"

static const int8_t QUAD_TABLE[16] = {
    0, -1,  1,  0,
    1,  0,  0, -1,
   -1,  0,  0,  1,
    0,  1, -1,  0
};

static volatile uint8_t lastEncoded = 0;
static volatile int8_t  accumulator = 0;

static void encoderIrqHandler(uint gpio, uint32_t events)
{
    (void)gpio;
    (void)events;

    uint8_t a = gpio_get(ROT_A_PIN);
    uint8_t b = gpio_get(ROT_B_PIN);
    uint8_t encoded = (a << 1) | b;
    uint8_t index = (lastEncoded << 2) | encoded;
    lastEncoded = encoded;

    accumulator += QUAD_TABLE[index];

    if (accumulator >= 4) {
        accumulator = 0;
        EncoderEvent_t event = { .direction = 1 };
        BaseType_t higherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(encoderQueue, &event, &higherPriorityTaskWoken);
        portYIELD_FROM_ISR(higherPriorityTaskWoken);
    } else if (accumulator <= -4) {
        accumulator = 0;
        EncoderEvent_t event = { .direction = -1 };
        BaseType_t higherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(encoderQueue, &event, &higherPriorityTaskWoken);
        portYIELD_FROM_ISR(higherPriorityTaskWoken);
    }
}

void encoderInit(void)
{
    gpio_init(ROT_A_PIN);
    gpio_init(ROT_B_PIN);
    gpio_set_dir(ROT_A_PIN, GPIO_IN);
    gpio_set_dir(ROT_B_PIN, GPIO_IN);
    gpio_pull_up(ROT_A_PIN);
    gpio_pull_up(ROT_B_PIN);

    lastEncoded = (gpio_get(ROT_A_PIN) << 1) | gpio_get(ROT_B_PIN);

    gpio_set_irq_enabled_with_callback(ROT_A_PIN,
        GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &encoderIrqHandler);
    gpio_set_irq_enabled(ROT_B_PIN,
        GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);

    gpio_init(ROT_SW_PIN);
    gpio_set_dir(ROT_SW_PIN, GPIO_IN);
    gpio_pull_up(ROT_SW_PIN);
}