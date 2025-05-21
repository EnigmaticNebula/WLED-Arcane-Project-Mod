// For whoever finds this, you've just lost the game!!!!!! HAHAHAHAHAHHAHAHAHAHAHHAHAAHAHAHAA
// Note: If starting serial communication to get outputs, use Serial.begin(115200) in readFromConfig(). 

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
#define PWM_FREQ 5000 // Up to 40MHz.
#define PWM_RES 8 // Up to 16 bits.
#define AIN1_PWM_CHANNEL 4 // Up to 16 channels, starting from 0. Don't use channels that are already in use
#define AIN2_PWM_CHANNEL 5

// Debounce
#define DEBOUNCE_TIME 50 // Time between button toggles that is considered anomalous


class DRV8833MotorDriver : public Usermod 
{
    private:

    // -------------- CONFIG VALUES -------------- //

    bool resetDefaults = false;

    bool motorEnabled = true;
    bool motorEnabledLastState = true;
    bool motorRecentlyEnabled = false;
    bool motorRecentlyDisabled = false;

    int buttonLastSteadyState = LOW; // Last normal button state
    int buttonLastFlickerState = LOW; // Last anomalous button state
    int buttonCurrentState;
    unsigned long lastDebounceTime = 0; 
    int motorLastState = LOW;

    bool forwards = true;
    bool backwards = false;
    bool slowDecay = true;
    bool fastDecay = false;
    bool motorMode = 2;

    bool forwardsLastState = true;
    bool backwardsLastState = false;
    bool fastDecayLastState = true;
    bool slowDecayLastState = false;
    bool motorLastMode = 2;
    bool motorModeRecentlyChanged = false;
    
    int motorSpeed = 255;
    int motorLastSpeed = 255;
    int transitionDuration = 1;

    static const int defMotorSpeed = 255;
    static const bool defForwards = true;
    static const bool defBackwards = false;
    static const bool defSlowDecay = true;
    static const bool defFastDecay = false;
    static const bool defTransitionDuration = 1; // Measured in seconds

    bool initDone = false;

    // Strings used to reduce flash memory usage

    static const char _name[];
    static const char _modes[];
    static const char _speed[];

    static const char _resetDefaults[];
    static const char _motorEnabled[];

    static const char _motorSpeed[];
    static const char _transitionDuration[];

    static const char _forwards[];
    static const char _backwards[];
    static const char _fastDecay[];
    static const char _slowDecay[];

    public:

    // Runs once on startup. Used for initialisations
    void setup() 
    {
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

    int assignMotorSpeed(int motorSpeed)
    {
        if (motorSpeed <= 255) return motorSpeed;
        else return 255;
    }

    bool turnOffDisabledMotor(bool motorEnabled, bool motorRecentlyDisabled, int speed, int mode, int transitionDuration)
    {
        if (motorEnabled == false && motorRecentlyDisabled == true && motorLastState == HIGH && (mode == 1 || mode == 4))
        {
            adjustMotorSpeed(speed, 0, transitionDuration, AIN1_PWM_CHANNEL);
        }

        else if (motorEnabled == false && motorRecentlyDisabled == true && motorLastState == HIGH && (mode == 2 || mode == 3))
        {
            adjustMotorSpeed(speed, 0, transitionDuration, AIN2_PWM_CHANNEL);
        }

        return false;
    }

    bool turnOnEnabledMotor(bool motorEnabled, bool motorRecentlyEnabled, int speed, int mode, int transitionDuration)
    {
        if (motorEnabled == true && motorRecentlyEnabled == true && motorLastState == HIGH && (mode == 1 || mode == 4))
        {
            adjustMotorSpeed(0, speed, transitionDuration, AIN1_PWM_CHANNEL);
        }

        else if (motorEnabled == true && motorRecentlyEnabled == true && motorLastState == HIGH && (mode == 2 || mode == 3))
        {
            adjustMotorSpeed(0, speed, transitionDuration, AIN2_PWM_CHANNEL);
        }
        return false;
    }


    // Gives an identifier to each decay, direction setting combination
    int getModeIdentifier(bool forwards, bool backwards, bool fastDecay, bool slowDecay)
    {
        if (forwards == true && fastDecay == true && backwards == false && slowDecay == false) return 1;
        else if (forwards == true && slowDecay == true && backwards == false && fastDecay == false) return 2;
        else if (backwards == true && fastDecay == true && forwards == false && slowDecay == false) return 3;
        else if (backwards == true && slowDecay == true && forwards == false && fastDecay == false) return 4;
    }

    int getTimeBetweenIncrement(int brightness, int transitionDuration)
    {
        return ((transitionDuration*1000)/brightness);
    }

    void adjustMotorSpeed(int currentSpeed, int targetSpeed, int transitionDuration, int pwmChannel)
    {   
        unsigned long timeOfLastIncrement = millis();
        int dutyCycle = currentSpeed;
        if (targetSpeed > currentSpeed)
        {   
            // This can be done with a for loop if using delay(). However, delay() has to be avoided for long delays as it halts all operations for the specified time. This interferes with WLED. 
            // This issue can be fixed by using millis() instead. However, implementing it in a for loop isn't possible as dutyCycle has to increment on a condition, which can't be done with a for loop
            // This is why a while loop is used instead. 
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

    bool modeRecentlyChanged(int mode, int lastMode)
    {
        if (mode != lastMode) return true;
        else return false;
    }

    bool recentlyDisabled(bool motorEnabled, bool motorEnabledLastState)
    {
        if (motorEnabled == false && motorEnabledLastState == true) return true;
        else return false;
    }

    bool recentlyEnabled(bool motorEnabled, bool motorEnabledLastState)
    {
        if (motorEnabled == true && motorEnabledLastState == false) return true;
        else return false;
    }

    bool validMode(bool forwards, bool backwards, bool fastDecay, bool slowDecay)
    {
        if (forwards == true && fastDecay == true && backwards == false && slowDecay == false) return true;
        else if (forwards == true && slowDecay == true && backwards == false && fastDecay == false) return true;
        else if (backwards == true && fastDecay == true && forwards == false && slowDecay == false) return true;
        else if (backwards == true && slowDecay == true && forwards == false && fastDecay == false) return true;
        else return false;
    }

    void loop() 
    {   

        motorRecentlyDisabled = turnOffDisabledMotor(motorEnabled, motorRecentlyDisabled, motorSpeed, motorMode, transitionDuration);
        motorRecentlyEnabled = turnOnEnabledMotor(motorEnabled, motorRecentlyEnabled, motorSpeed, motorMode, transitionDuration);

        buttonCurrentState = digitalRead(LED_BUTTON_PIN);

        // Catches anomalous button toggles
        if (buttonCurrentState != buttonLastFlickerState) 
        {
            lastDebounceTime = millis();
            buttonLastFlickerState = buttonCurrentState;
        }

        // Runs on valid button toggle
        if ((millis() - lastDebounceTime) > DEBOUNCE_TIME) 
        {
            // Toggle on
            if (buttonLastSteadyState == HIGH && buttonCurrentState == LOW) 
            {
                if (motorLastState == LOW) 
                {
                    if (motorMode == 1)
                    {
                        ledcWrite(AIN2_PWM_CHANNEL, 0);
                        adjustMotorSpeed(0, motorSpeed, transitionDuration, AIN1_PWM_CHANNEL);
                    }
                    else if (motorMode == 2)
                    {
                        ledcWrite(AIN1_PWM_CHANNEL, 255);
                        adjustMotorSpeed(0, motorSpeed, transitionDuration, AIN2_PWM_CHANNEL);
                    }
                    else if (motorMode == 3)
                    {   
                        ledcWrite(AIN1_PWM_CHANNEL, 0);
                        adjustMotorSpeed(0, motorSpeed, transitionDuration, AIN2_PWM_CHANNEL);
                    }
                    else if (motorMode == 4)
                    {
                        ledcWrite(AIN2_PWM_CHANNEL, 255);
                        adjustMotorSpeed(0, motorSpeed, transitionDuration, AIN1_PWM_CHANNEL);
                    }
                    
                    motorLastState = HIGH;
                }

                else if (motorLastState == HIGH) 
                {
                    
                    if (motorMode == 1)
                    {
                        ledcWrite(AIN2_PWM_CHANNEL, 0);
                        adjustMotorSpeed(motorSpeed, 0, transitionDuration, AIN1_PWM_CHANNEL);
                    }
                    else if (motorMode == 2)
                    {
                        ledcWrite(AIN1_PWM_CHANNEL, 255);
                        adjustMotorSpeed(motorSpeed, 0, transitionDuration, AIN2_PWM_CHANNEL);
                    }
                    else if (motorMode == 3)
                    {   
                        ledcWrite(AIN1_PWM_CHANNEL, 0);
                        adjustMotorSpeed(motorSpeed, 0, transitionDuration, AIN2_PWM_CHANNEL);
                    }
                    else if (motorMode == 4)
                    {
                        ledcWrite(AIN2_PWM_CHANNEL, 255);
                        adjustMotorSpeed(motorSpeed, 0, transitionDuration, AIN1_PWM_CHANNEL);
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

    void addToConfig(JsonObject& root) override
    {   
        JsonObject top = root.createNestedObject(FPSTR(_name));
        top[FPSTR(_resetDefaults)] = resetDefaults;
        top[FPSTR(_motorEnabled)] = motorEnabled;
        
        JsonObject modes = top.createNestedObject(FPSTR(_modes));
        if (validMode(forwards, backwards, fastDecay, slowDecay) == true)
        {
            modes[FPSTR(_forwards)] = forwards;
            modes[FPSTR(_backwards)] = backwards;
            modes[FPSTR(_fastDecay)] = fastDecay;
            modes[FPSTR(_slowDecay)] = slowDecay;
        }

        else
        {
            modes[FPSTR(_forwards)] = forwardsLastState;
            modes[FPSTR(_backwards)] = backwardsLastState;
            modes[FPSTR(_fastDecay)] = fastDecayLastState;
            modes[FPSTR(_slowDecay)] = slowDecayLastState;
        }


        JsonObject speed = top.createNestedObject(FPSTR(_speed));
        speed[FPSTR(_motorSpeed)] = assignMotorSpeed(motorSpeed);
        speed[FPSTR(_transitionDuration)] = transitionDuration;

        if (resetDefaults == true)
        {
            top[FPSTR(_resetDefaults)] = false;
            top[FPSTR(_motorEnabled)] = true;
            modes[FPSTR(_forwards)] = defForwards;
            modes[FPSTR(_backwards)] = defBackwards;
            modes[FPSTR(_fastDecay)] = defFastDecay;
            modes[FPSTR(_slowDecay)] = defSlowDecay;
            speed[FPSTR(_motorSpeed)] = motorSpeed;
            speed[FPSTR(_transitionDuration)] = defTransitionDuration;
        };
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

    bool readFromConfig(JsonObject& root) override
    {   
        //Serial.begin(115200);

        JsonObject top = root[FPSTR(_name)];
        JsonObject modes = top[FPSTR(_modes)];
        JsonObject speed = top[FPSTR(_speed)];

        bool configComplete = !top,isNull();

        if (validMode(forwards, backwards, fastDecay, slowDecay) == true)
        {
            if (forwardsLastState != forwards) forwardsLastState = forwards;
            if (backwardsLastState != backwards) backwardsLastState = backwards;
            if (fastDecayLastState != fastDecay) fastDecayLastState = fastDecay;
            if (slowDecayLastState != slowDecay) slowDecayLastState = slowDecay;
            motorLastMode = getModeIdentifier(forwardsLastState, backwardsLastState, fastDecayLastState, slowDecayLastState);
        }

        motorEnabledLastState = motorEnabled;

        //Assigns the saved values from the cfg.json file to its corresponding variable
        configComplete &= getJsonValue(top[FPSTR(_resetDefaults)], resetDefaults);
        configComplete &= getJsonValue(top[FPSTR(_motorEnabled)], motorEnabled);

        configComplete &= getJsonValue(modes[FPSTR(_forwards)], forwards);
        configComplete &= getJsonValue(modes[FPSTR(_backwards)], backwards);
        configComplete &= getJsonValue(modes[FPSTR(_fastDecay)], fastDecay);
        configComplete &= getJsonValue(modes[FPSTR(_backwards)], slowDecay);

        configComplete &= getJsonValue(speed[FPSTR(_motorSpeed)], motorSpeed);
        configComplete &= getJsonValue(speed[FPSTR(_transitionDuration)], transitionDuration);

        motorMode = getModeIdentifier(forwards, backwards, fastDecay, slowDecay);
        motorModeRecentlyChanged = modeRecentlyChanged(motorMode, motorLastMode);

        motorRecentlyDisabled = recentlyDisabled(motorEnabled, motorEnabledLastState);
        motorRecentlyEnabled = recentlyEnabled(motorEnabled, motorEnabledLastState);
        
        if (initDone == true)
        {   
            // Note: It would make sense to put the motorLastState condition in the first if statement, however this breaks the program.
            // also these if statements are disgusting but oh well. i might fix them in the future
            if (motorEnabled == true && (motorLastSpeed != motorSpeed) && motorLastState == HIGH && ((forwards == true && fastDecay == true) || (backwards == true && slowDecay == true))) adjustMotorSpeed(motorLastSpeed, motorSpeed, transitionDuration, AIN1_PWM_CHANNEL);
            if (motorEnabled == true && (motorLastSpeed != motorSpeed) && motorLastState == HIGH && ((backwards == true && fastDecay == true) || (forwards == true && slowDecay == true))) adjustMotorSpeed(motorLastSpeed, motorSpeed, transitionDuration, AIN2_PWM_CHANNEL);
        }

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
        uiScript.print(F("addInfo('MotorDriver:reset-to-defaults',1,'<br /><i style=\"color:#FF2e2e;\">WILL RESET WITHOUT CONFIRMATION!</i>');"));
        uiScript.print(F("addInfo('MotorDriver:motor-speed:speed',1,'<br /><i style=\"color:#7AB6FF;\">Input integer values up to 255</i><br /><i style=\"color:#FFAA00;\">Values greater than 255 will be interpreted as 255</i><br /><i style=\"color:#FFAA00;\">Decimal values will be truncated to just their integer components</i>');"));
        uiScript.print(F("addInfo('MotorDirver:motor-modes:slow-decay',1,'<br /><i style=\"color:#7AB6FF;\">Fast Decay: Motor stops slower. Higher minimum RPM<br />Slow Decay: Active braking used, so motor stops faster. Lower minimum RPM</i>');"));
    }
};

// -------------------- Strings used to reduce flash memory usage -------------------- //

const char DRV8833MotorDriver::_name[]                 PROGMEM = "MotorDriver";
const char DRV8833MotorDriver::_modes[]                PROGMEM = "motor-modes";
const char DRV8833MotorDriver::_speed[]                PROGMEM = "motor-speed";

const char DRV8833MotorDriver::_resetDefaults[]        PROGMEM = "reset-to-defaults";
const char DRV8833MotorDriver::_motorEnabled[]         PROGMEM = "motorEnabled";

const char DRV8833MotorDriver::_forwards[]             PROGMEM = "forwards";
const char DRV8833MotorDriver::_backwards[]            PROGMEM = "backwards";
const char DRV8833MotorDriver::_fastDecay[]            PROGMEM = "fast-decay";
const char DRV8833MotorDriver::_slowDecay[]            PROGMEM = "slow-decay";

const char DRV8833MotorDriver::_motorSpeed[]           PROGMEM = "speed";
const char DRV8833MotorDriver::_transitionDuration[]   PROGMEM = "transition-duration";