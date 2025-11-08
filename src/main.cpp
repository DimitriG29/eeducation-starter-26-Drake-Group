#include <Arduino.h>
#include <button.h>

// Create a button
button_t button;

static void on_push_button(button_t *btn) {
    // what should happen when you push a button?
    Serial.println("Button pushed!");
}

static void on_push_button_2(button_t *btn) {

}

void setup() {
    Serial.begin(9600);

    // Set up the button 0
    button_init(&button, BTN_0);

    // attach a callback function!
    // it will be ran whenever the user clicks the button
    // this says, whenever we push the button, we run 'on_push_button'
    // and provide no additional arguements for the function (NULL)
    button_set_callback(&button, on_push_button_2, NULL);
}

void loop() {
    // we can read the button using the button read
    bool down = button_read(&button);
    Serial.printf("Button status : %d\n", down);
}