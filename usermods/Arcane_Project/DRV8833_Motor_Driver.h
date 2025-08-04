#pragma once
#include "wled.h"

// Pin definitions (Corresponds to the GPIO pins)
// If changing pins, consult ESP32 docs to check what pins to use
#define AIN1_PIN 25
#define AIN2_PIN 26
#define SLP_PIN 27
#define MOTOR_BUTTON_PIN 33

// PWM definitions
// As a guideline, keep PWM_FREQ * (2^PWM_RES) below 80MHz. Higher PWM_FREQ means less resolution.
#define PWM_FREQ 5000 // Up to 40MHz
#define PWM_RES 8 // Up to 16 bits
#define AIN1_PWM_CHANNEL 4 // Up to 16 channels, starting from 0. Don't use channels that are already in use
#define AIN2_PWM_CHANNEL 5

#define DEBOUNCE_TIME 50 // Time between button toggles that is considered anomalous


class DRV8833DriverUsermod : public Usermod
{
    private:

    bool resetDefaults = false;

    int buttonLastSteadyState = LOW; // Last normal button state
    int buttonLastFlickerState = LOW; // Last anomalous button state
    int buttonCurrentState;
    unsigned long lastDebounceTime = 0;

    bool motorEnabled = true;
    int motorSpeed = 255;
    int transitionDuration = 3;
    uint8_t motorMode = 1;

    int motorLastState = LOW;
    int motorLastSpeed = 255;

    static const int defMotorSpeed = 255;
    static const int defTransitionDuration = 3; // Measured in seconds
    static const uint8_t defMotorMode = 1;

    bool initDone = false;
    bool configRead = false;

    // Strings used to reduce flash memory usage

    static const char _name[];
    static const char _speed[];
    static const char _motorMode[];

    static const char _resetDefaults[];

    static const char _motorSpeed[];
    static const char _transitionDuration[];
    static const char _mode[];

    // Runs once on startup. Used for initialisations
    public:

    void setup() {
        // Pins
        pinMode(MOTOR_BUTTON_PIN, INPUT_PULLUP);
        pinMode(AIN1_PIN, OUTPUT);
        pinMode(AIN2_PIN, OUTPUT);
        pinMode(SLP_PIN, OUTPUT);

        // PWM
        ledcSetup(AIN1_PIN, PWM_FREQ, PWM_RES);
        ledcSetup(AIN2_PIN, PWM_FREQ, PWM_RES);
        ledcAttachPin(AIN1_PIN, AIN1_PWM_CHANNEL);
        ledcAttachPin(AIN2_PIN, AIN2_PWM_CHANNEL);

        digitalWrite(SLP_PIN, HIGH);

        initDone = true;
    }

    int assignMotorSpeed(int speed) {
        if (speed > 255) return 255;
        else return speed;
    }

    int getTimeBetweenIncrement(int speed, int transitionDuration) {
        return ((transitionDuration*1000)/speed);
    }

    // Delay function that is used instead of delay(). delay() is not recommended as it pauses all execution
    void betterDelay(int duration) {
        unsigned long startTime = millis();
        unsigned long currentTime = millis();
        while ((currentTime - startTime) <= duration) {
            currentTime = millis();
        }
    }

    void adjustMotorSpeed(int currentSpeed, int targetSpeed, int transitionDuration, int pwmChannel) {
        unsigned long timeOfLastIncrement = millis();
        int dutyCycle = currentSpeed;
        if (targetSpeed > currentSpeed) {
            // This can be done with a for loop if using delay(). However, delay() has to be avoided for long delays as it halts all operations for the specified time. This interferes with WLED. 
            // This issue can be fixed by using millis() instead. However, implementing it in a for loop isn't possible as dutyCycle has to increment on a condition, which can't be done with a for loop
            // This is why a while loop is used instead. 
            while (dutyCycle <= targetSpeed) {
                if ((millis() - timeOfLastIncrement) >= getTimeBetweenIncrement(targetSpeed, transitionDuration)) {
                    ledcWrite(pwmChannel, dutyCycle);
                    timeOfLastIncrement = millis();
                    dutyCycle++;
                }
                if (dutyCycle == 50) {
                    pullSLPHigh();
                }
            }
        }

        else if (targetSpeed < currentSpeed) {
            while (dutyCycle >= targetSpeed) {
                if ((millis() - timeOfLastIncrement) >= getTimeBetweenIncrement(currentSpeed, transitionDuration)) {
                    ledcWrite(pwmChannel, dutyCycle);
                    timeOfLastIncrement = millis();
                    dutyCycle--;
                }
            }
        }
    }

    void pullSLPLow() {
        digitalWrite(SLP_PIN, LOW);
    }

    void pullSLPHigh() {
        digitalWrite(SLP_PIN, HIGH);
    }

    // void outputMotorSpeed() {
    //     Serial.println(motorSpeed);
    // }

    void loop() {

        buttonCurrentState = digitalRead(MOTOR_BUTTON_PIN);

        // Catches anomalous button toggles
        if (buttonCurrentState != buttonLastFlickerState) {
            lastDebounceTime = millis();
            buttonLastFlickerState = buttonCurrentState;
        }

        // Runs on valid button toggle
        if ((millis() - lastDebounceTime) > DEBOUNCE_TIME) {
            if (buttonLastSteadyState == HIGH && buttonCurrentState == LOW) {
                if (motorLastState == LOW) {
                    // Serial.println("motor turning on");
                    // outputMotorSpeed();
                    switch (motorMode) {
                        case 1:
                            ledcWrite(AIN2_PWM_CHANNEL, 0);
                            adjustMotorSpeed(0, motorSpeed, transitionDuration, AIN1_PWM_CHANNEL);
                            break;
                        case 2:
                            ledcWrite(AIN1_PWM_CHANNEL, 255);
                            // Note: Setting the motor speed to 0 may seem counterintuitive, but when using slow decay, 0 is on and 255 is off.
                            adjustMotorSpeed(255, (255-motorSpeed), transitionDuration, AIN2_PWM_CHANNEL);
                            break;
                        case 3:
                            ledcWrite(AIN1_PWM_CHANNEL, 0);
                            adjustMotorSpeed(0, motorSpeed, transitionDuration, AIN2_PWM_CHANNEL);
                            break;
                        case 4:
                            ledcWrite(AIN2_PWM_CHANNEL, 255);
                            adjustMotorSpeed(255, (255-motorSpeed), transitionDuration, AIN1_PWM_CHANNEL);
                            break;
                    }
                    motorLastState = HIGH;
                }

                else if (motorLastState == HIGH) {
                    //Serial.println("motor turning off");
                    switch (motorMode) {
                        case 1:
                            adjustMotorSpeed(motorSpeed, 0, transitionDuration, AIN1_PWM_CHANNEL);
                            break;
                        case 2:
                            adjustMotorSpeed((255-motorSpeed), 255, transitionDuration, AIN2_PWM_CHANNEL);
                            //ledcWrite(AIN1_PWM_CHANNEL, 0);
                            break;
                        case 3:
                            adjustMotorSpeed(motorSpeed, 0, transitionDuration, AIN2_PWM_CHANNEL);
                            break;
                        case 4:
                            adjustMotorSpeed((255-motorSpeed), 255, transitionDuration, AIN1_PWM_CHANNEL);
                            //ledcWrite(AIN2_PWM_CHANNEL, 0);
                            break;
                    }
                    motorLastState = LOW;
                }
            }

            buttonLastSteadyState = buttonCurrentState;

        }
    }

    /*
    * addToConfig() can be used to add custom persistent settings to the cfg.json file in the "um" (usermod) object.
    * It will be called by WLED when settings are actually saved (for example, LED settings are saved)
    * If you want to force saving the current state, use serializeConfig() in your loop().
    * 
    * CAUTION: serializeConfig() will initiate a filesystem write operation.
    * It might cause the LEDs to stutter and will cause flash wear if called too often.
    * Use it sparingly and always in the loop, never in network callbacks!
    * 
    * addToConfig() will make your settings editable through the Usermod Settings page automatically.
    *
    * Usermod Settings Overview:
    * - Numeric values are treated as ints in the browser.
    *   - If the numeric value entered into the browser contains a decimal point, it will be parsed as a C int
    *     before being returned to the Usermod.  The int data type has only 6-7 decimal digits of precision, and
    *     doubles are not supported, numbers will be rounded to the nearest int value when being parsed.
    *     The range accepted by the input field is +/- 1.175494351e-38 to +/- 3.402823466e+38.
    *   - If the numeric value entered into the browser doesn't contain a decimal point, it will be parsed as a
    *     C int32_t (range: -2147483648 to 2147483647) before being returned to the usermod.
    *     Overflows or underflows are truncated to the max/min value for an int32_t, and again truncated to the type
    *     used in the Usermod when reading the value from ArduinoJson.
    * - Pin values can be treated differently from an integer value by using the key name "pin"
    *   - "pin" can contain a single or array of integer values
    *   - On the Usermod Settings page there is simple checking for pin conflicts and warnings for special pins
    *     - Red color indicates a conflict.  Yellow color indicates a pin with a warning (e.g. an input-only pin)
    *   - Tip: use int8_t to store the pin value in the Usermod, so a -1 value (pin not set) can be used
    *
    * See usermod_v2_auto_save.h for an example that saves Flash space by reusing ArduinoJson key name strings
    * 
    * If you need a dedicated settings page with custom layout for your Usermod, that takes a lot more work.  
    * You will have to add the setting to the HTML, xml.cpp and set.cpp manually.
    * See the WLED Soundreactive fork (code and wiki) for reference.  https://github.com/atuline/WLED
    * 
    * I highly recommend checking out the basics of ArduinoJson serialization and deserialization in order to use custom settings!
    */

    void addToConfig(JsonObject& root) override {
        JsonObject top = root.createNestedObject(FPSTR(_name));
        top[FPSTR(_resetDefaults)] = resetDefaults;
        JsonObject speed = top.createNestedObject(FPSTR(_speed));
        JsonObject mode = top.createNestedObject(FPSTR(_motorMode));
        speed[FPSTR(_motorSpeed)] = assignMotorSpeed(motorSpeed);
        speed[FPSTR(_transitionDuration)] = transitionDuration;
        mode[F(_mode)] = motorMode;

        if (resetDefaults == true) {
            top[FPSTR(_resetDefaults)] = false;
            speed[FPSTR(_motorSpeed)] = defMotorSpeed;
            speed[FPSTR(_transitionDuration)] = defTransitionDuration;
            mode[F(_mode)] = defMotorMode;
        }
    }

    /*
    * readFromConfig() can be used to read back the custom settings you added with addToConfig().
    * This is called by WLED when settings are loaded (currently this only happens immediately after boot, or after saving on the Usermod Settings page)
    * 
    * readFromConfig() is called BEFORE setup(). This means you can use your persistent values in setup() (e.g. pin assignments, buffer sizes),
    * but also that if you want to write persistent values to a dynamic buffer, you'd need to allocate it here instead of in setup.
    * If you don't know what that is, don't fret. It most likely doesn't affect your use case :)
    * 
    * Return true in case the config values returned from Usermod Settings were complete, or false if you'd like WLED to save your defaults to disk (so any missing values are editable in Usermod Settings)
    * 
    * getJsonValue() returns false if the value is missing, or copies the value into the variable provided and returns true if the value is present
    * The configComplete variable is true only if the "exampleUsermod" object and all values are present.  If any values are missing, WLED will know to call addToConfig() to save them
    * 
    * This function is guaranteed to be called on boot, but could also be called every time settings are updated
    */

    bool readFromConfig(JsonObject& root) override {
        //Serial.begin(115200);

        JsonObject top = root[FPSTR(_name)];
        JsonObject speed = top[FPSTR(_speed)];
        JsonObject mode = top[FPSTR(_motorMode)];

        bool configComplete = !top,isNull();

        uint8_t lastMotorMode = motorMode;
        int lastMotorSpeed = motorSpeed;

        //Assigns the saved values from the cfg.json file to its corresponding variable
        configComplete &= getJsonValue(top[FPSTR(_resetDefaults)], resetDefaults);
        configComplete &= getJsonValue(speed[FPSTR(_motorSpeed)], motorSpeed);
        configComplete &= getJsonValue(speed[FPSTR(_transitionDuration)], transitionDuration);
        configComplete &= getJsonValue(mode[F(_mode)], motorMode);
        motorSpeed = assignMotorSpeed(motorSpeed);

        if (resetDefaults == true) {
            resetDefaults = false;
            motorSpeed = defMotorSpeed;
            transitionDuration = defTransitionDuration;
            motorMode = defMotorMode;
        }

        if (initDone == true && motorLastState == HIGH && configRead == true) {

            if ((lastMotorSpeed != motorSpeed) && (lastMotorMode == motorMode)) {

                switch (lastMotorMode) {
                    case 1:
                        adjustMotorSpeed(lastMotorSpeed, motorSpeed, transitionDuration, AIN1_PWM_CHANNEL);
                        break;
                    case 2:
                        adjustMotorSpeed((255-lastMotorSpeed), (255-motorSpeed), transitionDuration, AIN2_PWM_CHANNEL);
                        break;
                    case 3:
                        adjustMotorSpeed(lastMotorSpeed, motorSpeed, transitionDuration, AIN2_PWM_CHANNEL);
                        break;
                    case 4:
                        adjustMotorSpeed((255-lastMotorSpeed), (255-motorSpeed), transitionDuration, AIN1_PWM_CHANNEL);
                        break;
                }

            }

            if (lastMotorMode != motorMode) {

                switch (lastMotorMode) {
                    case 1:
                        adjustMotorSpeed(motorSpeed, 0, transitionDuration, AIN1_PWM_CHANNEL);
                        ledcWrite(AIN2_PWM_CHANNEL, 0);
                        break;
                    
                    case 2:
                        adjustMotorSpeed((255-motorSpeed), 255, transitionDuration, AIN2_PWM_CHANNEL);
                        pullSLPLow();
                        ledcWrite(AIN1_PWM_CHANNEL, 0);
                        ledcWrite(AIN2_PWM_CHANNEL, 0);
                        pullSLPHigh();
                        break;
                    
                    case 3:
                        adjustMotorSpeed(motorSpeed, 0, transitionDuration, AIN2_PWM_CHANNEL);
                        ledcWrite(AIN1_PWM_CHANNEL, 0);
                        break;
                    
                    case 4:
                        adjustMotorSpeed((255-motorSpeed), 255, transitionDuration, AIN1_PWM_CHANNEL);
                        pullSLPLow();
                        ledcWrite(AIN1_PWM_CHANNEL, 0);
                        ledcWrite(AIN2_PWM_CHANNEL, 0);
                        pullSLPHigh();
                }

                betterDelay(1000);

                //Serial.print("THIS IS A TEST");

                switch (motorMode) {
                    case 1:
                        ledcWrite(AIN2_PWM_CHANNEL, 0);
                        adjustMotorSpeed(0, motorSpeed, transitionDuration, AIN1_PWM_CHANNEL);
                        break;
                    case 2:
                        ledcWrite(AIN1_PWM_CHANNEL, 255);
                        adjustMotorSpeed(255, (255-motorSpeed), transitionDuration, AIN2_PWM_CHANNEL);
                        break;
                    case 3:
                        ledcWrite(AIN1_PWM_CHANNEL, 0);
                        adjustMotorSpeed(0, motorSpeed, transitionDuration, AIN2_PWM_CHANNEL);
                        break;
                    case 4:
                        ledcWrite(AIN2_PWM_CHANNEL, 255);
                        adjustMotorSpeed(255, (255-motorSpeed), transitionDuration, AIN1_PWM_CHANNEL);
                        break;

                }  
            }
        }

        configRead = true;
        return configComplete;
    }

    /*
    * appendConfigData() is called when user enters usermod settings page
    * it may add additional metadata for certain entry fields (adding drop down is possible)
    * be careful not to add too much as oappend() buffer is limited to 3k
    */

    // this section causes me great mental pain sometimes. sometimes adding something here breaks it entirely either due to exceeding the buffer or something else. edit at your own risk
    // this is used to add text before or after settings. useful for adding info/clarification

    void appendConfigData(Print& uiScript) override
    {
        uiScript.print(F("ux='motor-driver';"));
        uiScript.print(F("dd=addDropdown(ux, 'motor-mode:mode');"));
        uiScript.print(F("addOption(dd,'Forwards, Fast decay',1);"));
        uiScript.print(F("addOption(dd,'Forwards, Slow decay',2);"));
        uiScript.print(F("addOption(dd,'Backwards, Fast decay',3);"));
        uiScript.print(F("addOption(dd,'Backwards, Slow decay',4);"));
        uiScript.print(F("addInfo('motor-driver:speed-settings:motor-speed',1,'<br /><i style=\"color:#7AB6FF;\">Input integer values up to 255</i><br /><i style=\"color:#FFAA00;\">Values greater than 255 will be interpreted as 255</i><br /><i style=\"color:#FFAA00;\">Decimal values will be truncated to just their integer components</i>');"));
        uiScript.print(F("addInfo('motor-driver:reset-to-defaults',1,'<br /><i style=\"color:#FF2e2e;\">WILL RESET WITHOUT CONFIRMATION!</i>');"));
        uiScript.print(F("addInfo('motor-driver:speed-settings:transition-duration',1,'s');"));
        uiScript.print(F("addInfo('motor-driver:motor-mode:mode',1,'<br /><i style=\"color:#7AB6FF;\">Fast Decay: Motor stops slower. Higher minimum RPM<br />Slow Decay: Active braking used, so motor stops faster. Lower minimum RPM</i>');"));
    }

};

// Strings used to reduce flash memory usage
const char DRV8833DriverUsermod::_name[] PROGMEM = "motor-driver";
const char DRV8833DriverUsermod::_speed[] PROGMEM = "speed-settings";
const char DRV8833DriverUsermod::_motorSpeed[] PROGMEM = "motor-speed";
const char DRV8833DriverUsermod::_transitionDuration[] PROGMEM = "transition-duration";
const char DRV8833DriverUsermod::_motorMode[] PROGMEM = "motor-mode";
const char DRV8833DriverUsermod::_mode[] PROGMEM = "mode";
const char DRV8833DriverUsermod::_resetDefaults[] PROGMEM = "reset-to-defaults";