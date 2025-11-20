#ifndef __MOTOR_H__
#define __MOTOR_H__

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>

/**
 * @brief Motor control data structure sent TO the motor
 * 
 * This structure matches the ReceivedData structure in the Python code
 * for UDP communication with the motor controller hardware.
 */
typedef struct motor_command {
    float target;              // Target position (radians)
    float target_vel;          // Target velocity (rad/s)
    float kp;                  // Proportional gain
    float kd;                  // Derivative gain
    int32_t enable_filter;     // Enable filtering
    int32_t switch_;           // Motor enable switch (0=off, 1=on)
    int32_t calibrate;         // Calibration flag
    int32_t restart;           // Restart flag
    float timestamp;           // Timestamp
} motor_command_t;

/**
 * @brief Motor feedback data received FROM the motor
 * 
 * This structure matches the SentData structure in the Python code
 * for receiving motor status from the hardware.
 */
typedef struct motor_feedback {
    // Header
    int32_t module_id;
    int32_t receive_dt;
    int32_t timestamp;
    int32_t switch_off;
    float last_rcv_timestamp;
    int32_t info;
    
    // Motor data
    float motor_pos;
    float motor_large_pos;
    float motor_vel;
    float motor_torque;
    float motor_voltage;
    float motor_current;
    int32_t motor_temperature;
    int32_t motor_error0;
    int32_t motor_error1;
    
    // IMU data (13 floats - placeholder)
    float imu_data[13];
    
    // Error data
    int32_t reset_reason0;
    int32_t reset_reason1;
} motor_feedback_t;

/**
 * @brief Motor controller class - ESP32 acts as motor controller
 * Sends commands via UDP to motor hardware and receives feedback
 */
class MotorController {
public:
    /**
     * @brief Initialize motor controller with WiFi and motor IP
     * 
     * @param ssid WiFi network name
     * @param password WiFi password
     * @param motor_ip IP address of motor controller hardware
     * @param motor_port UDP port for motor communication (default 6666)
     */
    void init(const char* ssid, const char* password, const char* motor_ip, uint16_t motor_port = 6666);
    
    /**
     * @brief Update motor controller (call in loop)
     * Handles sending commands and receiving feedback
     */
    void update();
    
    /**
     * @brief Set target velocity for the motor
     * 
     * @param velocity Target velocity in rad/s
     */
    void setVelocity(float velocity);
    
    /**
     * @brief Stop the motor immediately (zero velocity)
     */
    void stop();
    
    /**
     * @brief Emergency stop - disable motor and zero all commands
     */
    void emergencyStop();
    
    /**
     * @brief Enable the motor
     */
    void enable();
    
    /**
     * @brief Disable the motor
     */
    void disable();
    
    /**
     * @brief Check if WiFi is connected
     * 
     * @return true if connected, false otherwise
     */
    bool isConnected();
    
    /**
     * @brief Print network information (IP, MAC address)
     */
    void printNetworkInfo();
    
    /**
     * @brief Get current motor command
     * 
     * @return Reference to motor command structure
     */
    motor_command_t& getCommand();
    
    /**
     * @brief Get current motor feedback
     * 
     * @return Reference to motor feedback structure
     */
    motor_feedback_t& getFeedback();

private:
    WiFiUDP udp;
    motor_command_t command;
    motor_feedback_t feedback;
    
    const char* motorIP;
    uint16_t motorPort;
    uint16_t localPort;
    
    unsigned long lastSendTime;
    unsigned long sendInterval;
    unsigned long lastFeedbackTime;
    
    void sendCommand();
    void receiveFeedback();
    void connectWiFi(const char* ssid, const char* password);
};

#endif // __MOTOR_H__
