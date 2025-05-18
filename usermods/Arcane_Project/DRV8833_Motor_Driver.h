// Never gonna give you up. Never gonna let you down. Never gonna run around and desert you. Never gonna make you cry. Never gonna say goodbye. Never gonna tell a lie and hurt you
#pragma once
#include "wled.h"

// Pin definitions (Corresponds to the GPIO pins)
// If changing pins, consult ESP32 documentation to check what pins to use

#define AIN1_PIN 25
#define AIN2_PIN 26 
#define MOTOR_BUTTON_PIN 33

// PWM definitions
// As a guideline, keep PWM_FREQ * (2^PWM_RES) below 80MHz. Higher PWM_FREQ means less resolution.

#define PWM_FREQ 5000 // Up to 40MHz
#define PWM_RES 8 // Up to 16 bits
#define AIN1_PWM_CHANNEL 4 // Up to 16 channels, starting from 0. Don't use channels that are already in use
#define AIN2_PWM_CHANNEL 5
#define PWM_DELAY_MS 25 // Delay between PWM signal increments
#define MOTOR_SPEED 255 // Up to 2^PWM_RES
#define MOTOR_SPEED_STEP 1 // Size of PWM signal increments

#define DEBOUNCE_TIME 50 // Time between button toggles that is considered anomalous

class DRV8833DriverUsermod : public Usermod {

    private:

    int buttonLastSteadyState = LOW;
    int buttonLastFlickerState = LOW;
    int buttonCurrentState;
    unsigned long lastDebounceTime = 0;

    public:

    void setup()
    {
        pinMode(MOTOR_BUTTON_PIN, INPUT_PULLUP);
        pinMode(AIN1_PIN, OUTPUT);
        pinMode(AIN2_PIN, OUTPUT);

        ledcSetup(AIN1_PWM_CHANNEL, PWM_FREQ, PWM_RES);
        ledcSetup(AIN2_PWM_CHANNEL, PWM_FREQ, PWM_RES);
        ledcAttachPin(AIN1_PIN, AIN1_PWM_CHANNEL);
        ledcAttachPin(AIN2_PIN, AIN2_PWM_CHANNEL);
    }

    void loop()
    {

        // AIN1: PWM AIN2: 0 FUNCTION: Forward, fast decay
        // AIN1: 1 AIN2: PWM FUNCTION: Forward, slow decay
        // AIN1: 0 AIN2: PWM FUNCTION: Reverse, fast decay
        // AIN1: PWM AIN2: 1 FUNCTION: Reverse, slow decay

        // Slow decay stops the motor faster as it provides active braking by using the motor as a generator
        // Fast decay stops the motor slower and instead lets the motor coast.
        // Slow decay allows for the motor to start at a lower voltage, and the increase of RPM with voltage is more linear
        // Fast decay means the motor starts at a higher voltage and the increase of RPM with voltage is less linear

        if (buttonCurrentState != buttonLastFlickerState)
        {
            lastDebounceTime = millis();
            buttonLastFlickerState = buttonCurrentState;
        }




    }
};