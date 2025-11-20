#include "motor.h"

void MotorController::init(const char* ssid, const char* password, const char* motor_ip, uint16_t motor_port) {
    // Initialize command structure with default values
    command.target = 0.0f;
    command.target_vel = 0.0f;
    command.kp = 20.0f;
    command.kd = 0.5f;
    command.enable_filter = 1;
    command.switch_ = 1;  // Start enabled (can be controlled via encoder/button)
    command.calibrate = 0;
    command.restart = 0;
    command.timestamp = 0.0f;
    
    // Initialize feedback structure
    memset(&feedback, 0, sizeof(motor_feedback_t));
    
    // Set motor parameters
    motorIP = motor_ip;
    motorPort = motor_port;
    localPort = 6666;  // Local UDP port to receive feedback
    
    // Communication timing
    lastSendTime = 0;
    sendInterval = 10;  // Send every 10ms (100Hz) for responsive control
    lastFeedbackTime = 0;
    
    // Connect to WiFi
    connectWiFi(ssid, password);
    
    // Start UDP
    udp.begin(localPort);
    
    Serial.printf("[Motor] Controller initialized\n");
    Serial.printf("[Motor] Target: %s:%d\n", motorIP, motorPort);
    Serial.printf("[Motor] Listening on port: %d\n", localPort);
    
    // Print network info after a delay
    delay(2000);
    printNetworkInfo();
}

void MotorController::connectWiFi(const char* ssid, const char* password) {
    Serial.printf("[WiFi] Connecting to '%s'", ssid);
    
    // Handle open networks (no password)
    if (password == NULL || strlen(password) == 0) {
        WiFi.begin(ssid);
    } else {
        WiFi.begin(ssid, password);
    }
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    Serial.println();
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("[WiFi] Connected!");
    } else {
        Serial.println("[WiFi] Connection failed!");
    }
}

void MotorController::printNetworkInfo() {
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n========== Network Info ==========");
        Serial.printf("MAC Address: %s\n", WiFi.macAddress().c_str());
        Serial.printf("IP Address:  %s\n", WiFi.localIP().toString().c_str());
        Serial.printf("Gateway:     %s\n", WiFi.gatewayIP().toString().c_str());
        Serial.printf("Subnet Mask: %s\n", WiFi.subnetMask().toString().c_str());
        Serial.printf("RSSI:        %d dBm\n", WiFi.RSSI());
        Serial.println("==================================\n");
    } else {
        Serial.println("\n[Network] Not connected to WiFi\n");
    }
}

void MotorController::update() {
    unsigned long currentTime = millis();
    
    // Send command at regular intervals
    if (currentTime - lastSendTime >= sendInterval) {
        sendCommand();
        lastSendTime = currentTime;
    }
    
    // Check for incoming feedback
    receiveFeedback();
}

void MotorController::sendCommand() {
    if (WiFi.status() != WL_CONNECTED) {
        return;
    }
    
    // Update timestamp
    command.timestamp = millis() / 1000.0f;
    
    // Pack command structure - matches Python ReceivedData.pack()
    // Format: ffffiiiff (9 values total = 36 bytes)
    uint8_t buffer[36];
    
    memcpy(buffer + 0, &command.target, 4);
    memcpy(buffer + 4, &command.target_vel, 4);
    memcpy(buffer + 8, &command.kp, 4);
    memcpy(buffer + 12, &command.kd, 4);
    memcpy(buffer + 16, &command.enable_filter, 4);
    memcpy(buffer + 20, &command.switch_, 4);
    memcpy(buffer + 24, &command.calibrate, 4);
    memcpy(buffer + 28, &command.restart, 4);
    memcpy(buffer + 32, &command.timestamp, 4);
    
    // Send to motor controller
    udp.beginPacket(motorIP, motorPort);
    udp.write(buffer, sizeof(buffer));
    udp.endPacket();
}

void MotorController::receiveFeedback() {
    int packetSize = udp.parsePacket();
    if (packetSize > 0) {
        uint8_t buffer[256];
        int len = udp.read(buffer, sizeof(buffer));
        
        if (len >= sizeof(motor_feedback_t)) {
            // Parse feedback data - matches Python SentData structure
            memcpy(&feedback, buffer, sizeof(motor_feedback_t));
            lastFeedbackTime = millis();
            
            // Optional: Print feedback periodically for debugging
            static unsigned long lastPrint = 0;
            if (millis() - lastPrint > 2000) {  // Print every 2 seconds
                Serial.printf("[Feedback] Pos: %.3f rad, Vel: %.3f rad/s, V: %.2fV, I: %.2fA, Temp: %d°C\n", 
                    feedback.motor_pos, feedback.motor_vel, feedback.motor_voltage, 
                    feedback.motor_current, feedback.motor_temperature);
                lastPrint = millis();
            }
        }
    }
}

void MotorController::setVelocity(float velocity) {
    command.target_vel = velocity;
}

void MotorController::stop() {
    command.target_vel = 0.0f;
    command.target = 0.0f;
}

void MotorController::emergencyStop() {
    command.switch_ = 0;
    command.target = 0.0f;
    command.target_vel = 0.0f;
    sendCommand();  // Send immediately
    Serial.println("[Motor] EMERGENCY STOP!");
}

void MotorController::enable() {
    command.switch_ = 1;
    Serial.println("[Motor] Enabled");
}

void MotorController::disable() {
    command.switch_ = 0;
    Serial.println("[Motor] Disabled");
}

bool MotorController::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

motor_command_t& MotorController::getCommand() {
    return command;
}

motor_feedback_t& MotorController::getFeedback() {
    return feedback;
}
