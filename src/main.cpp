#include <Arduino.h>
#include <button.h>
#include <encoder.h>
#include <motor.h>

// ============================================================================
// CONFIGURATION - Update these values for your setup
// ============================================================================
const char* WIFI_SSID = "Device-Northwestern";     // Your WiFi network name
const char* WIFI_PASSWORD = "";                    // Your WiFi password (empty for open network)
const char* MOTOR_IP = "10.106.4.140";             // Motor controller hardware IP address
const uint16_t MOTOR_PORT = 6666;                  // Motor UDP port

// Motor velocity when encoder spins (rad/s)
const float MOTOR_VELOCITY = 20.0f;

// ============================================================================
// Hardware Interfaces
// ============================================================================
button_t button;
encoder_t encoder;
MotorController motor;

// Motor control state
volatile bool motorEnabled = true;  // Start enabled (controlled by button)

/**
 * @brief Encoder spin callback - control motor based on direction
 * 
 * @param encoder Pointer to encoder structure
 * @param change Direction of spin (+1 = clockwise, -1 = counterclockwise)
 */
static void on_spin(encoder_t *encoder, int32_t change)
{
    Serial.printf("Encoder spin: %d\n", change);
    
    if (!motorEnabled) {
        return;  // Don't control motor if disabled
    }
    
    if (change > 0) {
        // Clockwise - spin motor right at 20 rad/s
        motor.setVelocity(MOTOR_VELOCITY);
        Serial.println("Motor: RIGHT at 20 rad/s");
    } else if (change < 0) {
        // Counterclockwise - spin motor left at -20 rad/s
        motor.setVelocity(-MOTOR_VELOCITY);
        Serial.println("Motor: LEFT at -20 rad/s");
    }
}

/**
 * @brief Button push callback - emergency stop motor
 * 
 * @param btn Pointer to button structure
 */
static void on_push_button(button_t *btn) {
    Serial.println("Button pushed! EMERGENCY STOP");
    
    // Emergency stop - disable motor and zero all commands
    motor.emergencyStop();
    motorEnabled = false;
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n================================================");
    Serial.println("  ESP32 Motor Controller");
    Serial.println("  Encoder + Button Input -> Motor Control");
    Serial.println("================================================\n");

    // Initialize motor controller - connects to WiFi and motor hardware
    Serial.println("[Setup] Initializing motor controller...");
    motor.init(WIFI_SSID, WIFI_PASSWORD, MOTOR_IP, MOTOR_PORT);
    
    // Set up the button
    Serial.println("[Setup] Initializing button...");
    button_init(&button, BTN_0);
    
    // Set up the rotary encoder
    Serial.println("[Setup] Initializing encoder...");
    encoder_init(&encoder, RE_CW, RE_CCW, RE_BTN);

    // Attach callback functions
    button_set_callback(&button, on_push_button, NULL);
    encoder_set_spin_callback(&encoder, on_spin);
    
    // Enable motor by default
    motor.enable();
    
    Serial.println("\n[Setup] Complete!");
    Serial.println("\n=== CONTROLS ===");
    Serial.println("  Encoder LEFT:  Motor spins LEFT at -20 rad/s");
    Serial.println("  Encoder RIGHT: Motor spins RIGHT at +20 rad/s");
    Serial.println("  Encoder STOP:  Motor stops automatically");
    Serial.println("  Button PUSH:   Emergency stop & disable motor");
    Serial.println("\nMotor is ENABLED and ready.\n");
}

void loop() {
    // Update motor controller - sends commands and receives feedback via UDP
    motor.update();
    
    // Track encoder position to detect when it stops moving
    static int32_t lastEncoderPos = 0;
    static unsigned long lastEncoderChange = 0;
    int32_t currentPos = encoder.position;
    
    // Detect encoder movement
    if (currentPos != lastEncoderPos) {
        lastEncoderPos = currentPos;
        lastEncoderChange = millis();
    }
    
    // If encoder hasn't moved for 200ms, stop motor
    if (motorEnabled && (millis() - lastEncoderChange > 200)) {
        if (motor.getCommand().target_vel != 0.0f) {
            motor.stop();
            Serial.println("[Control] Encoder stopped - Motor stopping");
        }
    }
    
    // Small delay to prevent overwhelming the system
    delay(10);
}

