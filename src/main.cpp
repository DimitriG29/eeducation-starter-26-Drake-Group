#include <Arduino.h>
#include <button.h>
#include <encoder.h>

// Create a button
button_t button;
encoder_t encoder;

static void on_spin(encoder_t *encoder, int32_t change)
{
    Serial.println("spinningggg\n");
}
static void on_push_button(button_t *btn) {
    // what should happen when you push a button?
    Serial.println("Button pushed!");
}

void setup() {
    Serial.begin(115200);

    // Set up the button 0
    button_init(&button, BTN_0);
    encoder_init(&encoder, RE_CW, RE_CCW, RE_BTN);

    // attach a callback function!
    // it will be ran whenever the user clicks the button
    // this says, whenever we push the button, we run 'on_push_button'
    // and provide no additional arguements for the function (NULL)
    button_set_callback(&button, on_push_button, NULL);
    encoder_set_spin_callback(&encoder, on_spin);
}

void loop() {
    // we can read the button using the button read
    bool down = button_read(&button);
    delay(100);
    Serial.printf("Button status : %d\n", down);
}