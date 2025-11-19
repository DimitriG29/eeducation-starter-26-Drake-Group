#include "encoder.h"

static void __encoder_isr_a(void* ctx) 
{
    encoder_t* enc = (encoder_t*)ctx;
}

static void __encoder_isr_b(void* ctx) 
{
    //intialize enc
    encoder_t* enc = (encoder_t*)ctx;
    //read new state + store old to compare
    int new_state = (digitalRead(enc->pin_a) << 1) | digitalRead(enc->pin_b);
    int old = enc->last_state;
    //no change
    if (new_state == old) return;
    //else find change
    int change = ((old & 1) == ((new_state >> 1) & 1)) ? -1 : +1;
    enc->position += change;
    enc->last_state = new_state;
    //call funct
    if (enc->spin_cb) 
    {
        enc->spin_cb(enc, change);
    }
}

static void __encoder_isr_btn(void* ctx) 
{
    encoder_t* enc = (encoder_t*)ctx;
    if (!enc) return;
    // how to call the callback
    if (enc->button_cb) enc->button_cb(enc);
}

void encoder_init(encoder_t* enc, pin_t pin_a, pin_t pin_b, pin_t pin_btn) 
{
    if (!enc) return;
    //connect enc to pins
    enc->pin_a = pin_a;
    enc->pin_b = pin_b;
    enc->pin_btn = pin_btn;

    //set pins to take input
    pinMode(pin_a, INPUT);
    pinMode(pin_b, INPUT);
    pinMode(pin_btn, INPUT);

    //initialize enc vals
    enc->position = 0;
    enc->last_state = (digitalRead(pin_a) << 1) | digitalRead(pin_b);
    enc->spin_cb = NULL;
    enc->button_cb = NULL;
    attach_encoder_interrupts(enc);

    // Initialize the encoder struct by giving it reasonable values
    // And initialize the hardware if needed
}

void encoder_set_spin_callback(encoder_t* enc, void (*cb)(encoder_t* enc, int32_t delta)) {
    if (!enc) return;
    enc->spin_cb = cb;
}

void encoder_set_button_callback(encoder_t* enc, void (*cb)(encoder_t* enc)) {
    if (!enc) return;
    enc->button_cb = cb;
}

int32_t encoder_get_position(const encoder_t* enc) {
    if (!enc) return 0;
    return enc->position;
}

void encoder_set_position(encoder_t* enc, int32_t pos) {
    if (!enc) return;
    enc->position = pos;
}

void attach_encoder_interrupts(encoder_t* enc) {
    attachInterruptArg(digitalPinToInterrupt(enc->pin_a), __encoder_isr_a, enc, CHANGE);
    attachInterruptArg(digitalPinToInterrupt(enc->pin_b), __encoder_isr_b, enc, CHANGE);
    attachInterruptArg(digitalPinToInterrupt(enc->pin_btn), __encoder_isr_btn, enc, RISING);
}
