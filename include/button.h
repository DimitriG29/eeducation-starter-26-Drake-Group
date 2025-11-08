#ifndef __BUTTON_H__
#define __BUTTON_H__

#include <Arduino.h>
#include <stdint.h>

#include "pin.h"

/**
 * @brief Simple button abstraction with a single callback.
 */
typedef struct button {
    pin_t pin;
    void (*callback)(struct button* ctx);  // Function to call when button is pressed
    void* ctx;                    // User data passed to callback
} button_t;

/**
 * @brief Initializes the button.
 *
 * @param btn Pointer to button instance.
 * @param pin Input pin number.
 */
void button_init(button_t* btn, pin_t pin);

/**
 * @brief Assigns a callback function to the button.
 *
 * @param btn Pointer to button instance.
 * @param cb  Callback function pointer.
 * @param ctx User-provided context pointer.
 */
void button_set_callback(button_t* btn, void (*cb)(button_t* ctx), void* ctx);


/**
 * @brief Reads the current digital state of the button pin.
 *
 * @param btn Pointer to button instance.
 * @return uint8_t 0 or 1 depending on pin state.
 */
bool button_read(const button_t* btn);

/**
 * @brief Attaches an interrupt to a pin and associates a context pointer.
 * 
 * @param pin        The input pin number.
 */
void attach_button_interrupt(button_t *btn, pin_t pin);


#endif  // __BUTTON_H__