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

class DRV8833DriverUsermod : public Usermod 
{

    private:

    int buttonLastSteadyState = LOW;
    int buttonLastFlickerState = LOW;
    int buttonCurrentState;
    unsigned long lastDebounceTime = 0;

    bool slowDecay = true;
    bool fastDecay = false;
    bool forwards = true;
    bool backwards = false;
    bool motorLastState = LOW;
    bool enabled = true;
    bool resetDefaults = false;
    int motorSpeed = 255;
    int transitionDuration = 1;

    static const bool defSlowDecay = true;
    static const bool defFastDecay = false;
    static const bool defForwards = true;
    static const bool defBackwards = false;
    static const bool defEnabled = true;
    static const int defMotorSpeed = 255;
    static const int defTransitionDuration = 1;

    static const char _name[];
    static const char _modes[];
    static const char _speed[];

    static const char _resetDefaults[];
    static const char _enabled[];

    static const char _motorSpeed[];
    static const char _slowDecay[];
    static const char _fastDecay[];
    static const char _forwards[];
    static const char _backwards[];
    static const char _transitionDuration[];

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
        //Serial.begin(115200);
    }

    int getTimeBetweenIncrement(int speed, int transitionDuration)
    {
        return ((transitionDuration*1000)/speed);
    }

    void adjustMotorSpeed(int currentSpeed, int targetSpeed, int transitionDuration, int pwmChannel)
    {
        unsigned long timeOfLastIncrement = millis();
        int dutyCycle = currentSpeed;
        if (targetSpeed > currentSpeed)
        {
            while (dutyCycle <= targetSpeed)
            {
                if ((millis() - timeOfLastIncrement) >= getTimeBetweenIncrement(targetSpeed, transitionDuration))
                {
                    ledcWrite(pwmChannel, dutyCycle);
                    timeOfLastIncrement = millis();
                    dutyCycle++;
                }
            }
        }

        else if (targetSpeed < currentSpeed)
        {
            while (dutyCycle >= targetSpeed)
            {   
                if ((millis() - timeOfLastIncrement) >= getTimeBetweenIncrement(currentSpeed, transitionDuration))
                {   
                    ledcWrite(pwmChannel, dutyCycle);
                    timeOfLastIncrement = millis();
                    dutyCycle--;
                }
            }
        }
    }

    void motorToggle(bool forwards, bool backwards, bool slowDecay, bool fastDecay, int currentSpeed, int targetSpeed, int transitionDuration)
    {
        if (forwards == true && fastDecay == true)
        {
            //Serial.println("Forwards, fast decay");
            ledcWrite(AIN2_PWM_CHANNEL, 0);
            adjustMotorSpeed(currentSpeed, targetSpeed, transitionDuration, AIN1_PWM_CHANNEL);
        }

        else if (forwards == true && slowDecay == true)
        {   
            //Serial.println("Forwards, slow decay");
            ledcWrite(AIN1_PWM_CHANNEL, 255);
            adjustMotorSpeed(currentSpeed, targetSpeed, transitionDuration, AIN2_PWM_CHANNEL);
        }

        else if (backwards == true && fastDecay == true)
        {   
            //Serial.println("Reverse, fast decay");
            ledcWrite(AIN1_PWM_CHANNEL, 0);
            adjustMotorSpeed(currentSpeed, targetSpeed, transitionDuration, AIN2_PWM_CHANNEL);
        }

        else if (backwards == true && slowDecay == true)
        {   
            //Serial.println("Reverse, slow decay");
            ledcWrite(AIN2_PWM_CHANNEL, 255);
            adjustMotorSpeed(currentSpeed, targetSpeed, transitionDuration, AIN1_PWM_CHANNEL);
        }
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

        buttonCurrentState = digitalRead(MOTOR_BUTTON_PIN);

        if (buttonCurrentState != buttonLastFlickerState)
        {
            lastDebounceTime = millis();
            buttonLastFlickerState = buttonCurrentState;
        }

        if ((millis() - lastDebounceTime) > DEBOUNCE_TIME)
        {
            //Serial.println("VALID BUTTON PRESS");
            if (buttonLastSteadyState == HIGH && buttonCurrentState == LOW)
            {   
                Serial.println("pls work");
                if (motorLastState == LOW && enabled == true)
                {
                    motorToggle(forwards, backwards, slowDecay, fastDecay, 0, motorSpeed, transitionDuration);
                    Serial.println(forwards);
                    Serial.println(backwards);
                    Serial.println(fastDecay);
                    Serial.println(slowDecay);
                    motorLastState = HIGH;
                }

                else if (motorLastState == HIGH && enabled == true)
                {
                    motorToggle(forwards, backwards, slowDecay, fastDecay, motorSpeed, 1, transitionDuration);
                    motorLastState = LOW;
                }

            }

            buttonLastSteadyState = buttonCurrentState;
        }
    }

    void addToConfig(JsonObject& root) override
    {
        JsonObject top = root.createNestedObject(FPSTR(_name));
        top[FPSTR(_enabled)] = enabled;
        top[FPSTR(_resetDefaults)] = resetDefaults;

        JsonObject motorModes = top.createNestedObject(FPSTR(_modes));
        motorModes[FPSTR(_fastDecay)] = fastDecay;
        motorModes[FPSTR(_slowDecay)] = slowDecay;
        motorModes[FPSTR(_forwards)] = forwards;
        motorModes[FPSTR(_backwards)] = backwards;

        JsonObject speed = top.createNestedObject(FPSTR(_speed));
        speed[FPSTR(_motorSpeed)] = motorSpeed;
        speed[FPSTR(_transitionDuration)] = transitionDuration;

        if (resetDefaults == true)
        {
            top[FPSTR(_enabled)] = defEnabled;
            top[FPSTR(_resetDefaults)] = false;
            motorModes[FPSTR(_fastDecay)] = defFastDecay;
            motorModes[FPSTR(_slowDecay)] = defSlowDecay;
            motorModes[FPSTR(_forwards)] = defForwards;
            motorModes[FPSTR(_backwards)] = defBackwards;
            speed[FPSTR(_motorSpeed)] = defMotorSpeed;
            speed[FPSTR(_transitionDuration)] = defTransitionDuration;
        }
    }

    bool readFromConfig(JsonObject& root) override
    {   
        Serial.begin(115200);
        JsonObject top = root[FPSTR(_name)];
        JsonObject motorModes = top[FPSTR(_modes)];
        JsonObject speed = top[FPSTR(_speed)];

        bool configComplete = !top,isNull();

        configComplete &= getJsonValue(top[FPSTR(_enabled)], enabled);
        configComplete &= getJsonValue(top[FPSTR(_resetDefaults)], resetDefaults);

        configComplete &= getJsonValue(motorModes[FPSTR(_fastDecay)], fastDecay);
        configComplete &= getJsonValue(motorModes[FPSTR(_slowDecay)], slowDecay);
        configComplete &= getJsonValue(motorModes[FPSTR(_forwards)], forwards);
        configComplete &= getJsonValue(motorModes[FPSTR(_backwards)], backwards);

        configComplete &= getJsonValue(speed[FPSTR(_motorSpeed)], motorSpeed);
        configComplete &= getJsonValue(speed[FPSTR(_transitionDuration)], transitionDuration);

        return configComplete;
    }

    void appendConfigData(Print& uiScript) override
    {   
        uiScript.print(F("addInfo('DRV8833-motor-driver:modes:slow-decay',1,'<br /><i style=\"color:#7AB6FF;\">Slow Decay: Stops the motor quicker by providing active braking from the power generated from the motor</i><br /><i style=\"color:#FFAA00;\">Has a lower minimum RPM. RPM increase is more linear</i>');"));
        uiScript.print(F("addInfo('DRV8833-motor-driver:modes:fast-decay',1,'<br /><i style=\"color:#7AB6FF;\">Fast Decay: Stops the motor slower by letting it coast</i><br /><i style=\"color:#FFAA00;\">Has a higher minimum RPM. RPM increase is less linear)"));
    }
};


const char DRV8833DriverUsermod::_name[]                PROGMEM = "DRV8833-motor-driver";
const char DRV8833DriverUsermod::_modes[]               PROGMEM = "modes";
const char DRV8833DriverUsermod::_speed[]               PROGMEM = "speed-settings";

const char DRV8833DriverUsermod::_resetDefaults[]       PROGMEM = "reset-to-defaults";
const char DRV8833DriverUsermod::_enabled[]             PROGMEM = "enabled";

const char DRV8833DriverUsermod::_motorSpeed[]          PROGMEM = "motor-speed";
const char DRV8833DriverUsermod::_slowDecay[]           PROGMEM = "slow-decay";
const char DRV8833DriverUsermod::_fastDecay[]           PROGMEM = "fast-decay";
const char DRV8833DriverUsermod::_forwards[]            PROGMEM = "forwards";
const char DRV8833DriverUsermod::_backwards[]           PROGMEM = "backwards";
const char DRV8833DriverUsermod::_transitionDuration[]  PROGMEM = "motor-transition-time";