// Never gonna give you up. Never gonna let you down. Never gonna run around and desert you. Never gonna make you cry. Never gonna say goodbye. Never gonna tell a lie and hurt you
#pragma once
#include "wled.h"

//------ IN1 & IN2 COMBINATIONS: ------
// IN1: LOW IN2: LOW - Motor off       
// IN1: HIGH IN2: HIGH - Motor off        
// IN1: HIGH IN2: LOW - Clockwise       
// IN1: LOW IN2: HIGH - Anti-clockwise  
//-------------------------------------


// Pin definitions (Corresponds to the GPIO pins)
// If changing pins, consult ESP32 documentation to check what pins to use

#define IN1_PIN 25
#define IN2_PIN 26 
#define ENA_PIN 27 // Carries PWM signal
#define MOTOR_BUTTON_PIN 33

// PWM definitions
// As a guideline, keep PWM_FREQ * (2^PWM_RES) below 80MHz. Higher PWM_FREQ means less resolution.

#define MOTOR_PWM_FREQ 5000 // Up to 40MHz
#define MOTOR_PWM_RES 8 // Up to 16 bits
#define MOTOR_PWM_CHANNEL 1 // Up to 16 channels, starting from 0. Don't use channels that are already in use
#define MOTOR_PWM_DELAY_MS 25 // Delay between PWM signal increments
#define MOTOR_SPEED 100 // Up to 2^PWM_RES
#define MOTOR_SPEED_STEP 1 // Size of PWM signal increments

#define DEBOUNCE_TIME 50 // Time between button toggles that is considered anomalous

class L298NDriverUsermod : public Usermod {

    private:

    // Variables
    int buttonLastSteadyState = HIGH; // Last normal button state
    int buttonLastFlickerState = LOW; // Last anomalous button state 
    int motorLastDirectionState = LOW; // LOW: Anti-clockwise HIGH: Clockwise
    int buttonCurrentState;
    int motorLastState = LOW;
    unsigned long lastDebounceTime = 0; // Last time between flicker states
  
    public:

    // Initialisations
    void setup() {

        // Pins
        pinMode(MOTOR_BUTTON_PIN, INPUT_PULLUP);
        pinMode(IN1_PIN, OUTPUT);
        pinMode(IN2_PIN, OUTPUT);
        pinMode(ENA_PIN, OUTPUT);

        // PWM
        ledcSetup(MOTOR_PWM_CHANNEL, MOTOR_PWM_FREQ, MOTOR_PWM_RES);
        ledcAttachPin(ENA_PIN, MOTOR_PWM_CHANNEL);

        // Serial
        //Serial.begin(115200); // Starts communication between device and ESP32 at 115200 bits/s. Changing this value may cause unwanted results
    }

    void loop() {

        buttonCurrentState = digitalRead(MOTOR_BUTTON_PIN); 

        // Catches anomalous button toggles
        if (buttonCurrentState != buttonLastFlickerState) {
            lastDebounceTime = millis();
            buttonLastFlickerState = buttonCurrentState;
        }
        // Runs on valid button toggle
        if ((millis() - lastDebounceTime) > DEBOUNCE_TIME) {

            // Toggle on
            if (buttonLastSteadyState == HIGH && buttonCurrentState == LOW) {

                // Motor on
                if (motorLastState == LOW) {
                    //Serial.println("MOTOR ON");

                    // Motor clockwise
                    if (motorLastDirectionState == LOW) {
                        //Serial.println("Motor clockwise");
                        digitalWrite(IN1_PIN, HIGH);
                        digitalWrite(IN2_PIN, LOW);

                        motorLastDirectionState = HIGH;
                    }

                    // Motor anti-clockwise
                    else if (motorLastDirectionState == HIGH) {
                        //Serial.println("Motor anti-clockwise");
                        digitalWrite(IN1_PIN, LOW);
                        digitalWrite(IN2_PIN, HIGH);
                        
                        motorLastDirectionState = LOW;
                    }


                    // Sends PWM signal to cycle motor from off to on
                    for (int dutyCycle = 0; dutyCycle <= MOTOR_SPEED; dutyCycle += MOTOR_SPEED_STEP) {
                        ledcWrite(MOTOR_PWM_CHANNEL, dutyCycle);
                        delay(MOTOR_PWM_DELAY_MS);
                    }

                    motorLastState = HIGH;
                }

                // Motor off
                else if (motorLastState == HIGH) {
                    //Serial.println("MOTOR OFF");
                    digitalWrite(IN1_PIN, LOW);
                    digitalWrite(IN2_PIN, LOW);

                    // Sends PWM signal to cycle LED from on to off
                    for (int dutyCycle = MOTOR_SPEED; dutyCycle >= 0; dutyCycle -= MOTOR_SPEED_STEP) {
                        ledcWrite(MOTOR_PWM_CHANNEL, dutyCycle);
                        delay(MOTOR_PWM_DELAY_MS);
                    }

                    motorLastState = LOW;
                }
            }

            buttonLastSteadyState = buttonCurrentState;
        }
    }
};

