#include "button.h"


void button_init(button_t* btn, pin_t pin) {
    pinMode(pin, INPUT);
    attach_button_interrupt(btn, pin);
}

void button_set_callback(button_t* btn, void (*cb)(button_t* ctx), void* ctx) {
    btn->callback = cb;
    btn->ctx = ctx;
}

bool button_read(const button_t* btn) {
    // Check if this button has been read
    if (!btn) return false;

    // If using INPUT_PULLUP, a pressed button will read LOW (active-low)
    return digitalRead((uint8_t)btn->pin) == LOW;
}

static void __button_callback(void *ctx) {
    button_t *btn = (button_t *)(ctx);

    // Something has triggered the interrupt, what should happen?
    // Perhaps call the user callback?g

    if (!btn) return;
    Serial.printf("button callback\n");

    // Call user callback if set. Note: this runs in ISR context on some platforms.
    if (btn->callback) {
        btn->callback(btn);
    }
}

void attach_button_interrupt(button_t *button, pin_t pin) {
    attachInterruptArg(digitalPinToInterrupt(pin), __button_callback, button, RISING);
}
