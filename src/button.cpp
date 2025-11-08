#include "button.h"

// Small registry to map pin numbers to button_t pointers so the ISR
// can find the correct button context. Size chosen to cover typical
// GPIO numbers used in this project (0..39).
static button_t* s_button_map[40] = { 0 };

<<<<<<< Updated upstream
    // (Hint), you'll want to set up an interrupt!
} 
=======
<<<<<<< HEAD
void button_init(button_t* btn, pin_t pin) {
    // Store pin and initialize callback/context
    btn->pin = pin;
    btn->callback = NULL;
    btn->ctx = NULL;

    // Configure hardware pin as input with internal pull-up (buttons are active-low)
    pinMode(pin, INPUT_PULLUP);

    // Register button pointer for the given pin so the interrupt handler
    // can look it up and call the correct callback.
    if ((int)pin >= 0 && (int)pin < (int)(sizeof(s_button_map)/sizeof(s_button_map[0]))) {
        s_button_map[(int)pin] = btn;
    }

    // Attach an interrupt for this pin (attach_button_interrupt will look up the context)
    attach_button_interrupt(pin);
}
=======
    // (Hint), you'll want to set up an interrupt!
} 
>>>>>>> 8979fd71596f837d25d17399f833fd20f03ddea0
>>>>>>> Stashed changes

void button_set_callback(button_t* btn, void (*cb)(button_t* ctx), void* ctx) {
    // Update the button struct to set the user callback
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
    // Perhaps call the user callback?
    if (!btn) return;

    // Call user callback if set. Note: this runs in ISR context on some platforms.
    if (btn->callback) {
        btn->callback(btn);
    }
}

void attach_button_interrupt(pin_t pin) {
    // Look up registered button for this pin (may be NULL)
    void* ctx = NULL;
    if ((int)pin >= 0 && (int)pin < (int)(sizeof(s_button_map)/sizeof(s_button_map[0]))) {
        ctx = (void*)s_button_map[(int)pin];
    }

    // Attach the argument-taking interrupt handler. Use FALLING so press (active-low)
    attachInterruptArg(digitalPinToInterrupt(pin), __button_callback, ctx, FALLING);
}
