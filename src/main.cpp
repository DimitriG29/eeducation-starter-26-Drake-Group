#include <Arduino.h>
#include <button.h>
#include <encoder.h>
#include <Adafruit_NeoPixel.h>

#define NEOPIXEL_PIN  26
#define NEOPIXEL_COUNT 1

// Create a button
button_t button;
encoder_t encoder;

Adafruit_NeoPixel strip(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
volatile int brightness = 50;

static void on_spin(encoder_t *encoder, int32_t change)
{
    brightness += (int)change * 5;
    if (brightness < 0)   brightness = 0;
    if (brightness > 255) brightness = 255;
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

    strip.begin();
    strip.show();                 
    strip.setBrightness(brightness);

    strip.setPixelColor(0, strip.Color(255, 255, 255));
    strip.show();

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
    static int last_brightness = -1;
    int current_brightness = brightness;

    if (current_brightness != last_brightness) {
        last_brightness = current_brightness;
        strip.setBrightness(current_brightness);
        strip.setPixelColor(0, strip.Color(255, 255, 255)); // white LED
        strip.show();
        Serial.print("LED updated via color, level = ");
        Serial.println(current_brightness);
    }
    delay(200);
    Serial.printf("Button status : %d\n", down);
}