// For whoever finds this, you've just lost the game!!!!!! HAHAHAHAHAHHAHAHAHAHAHHAHAAHAHAHAA
// Note: If starting serial communication to get outputs, use Serial.begin(115200) in readFromConfig(). 

#pragma once
#include "wled.h"

// Pin definitions (Corresponds to the GPIO pins)
// If changing pins, consult ESP32 docs to check what pins to use
#define BLUE_LED_PIN 17
#define LED_FIL1_PIN 18
#define LED_FIL2_PIN 19
#define LED_BUTTON_PIN 21

// PWM definitions
// As a guideline, keep PWM_FREQ * (2^PWM_RES) below 80MHz. Higher PWM_FREQ means less resolution.
#define PWM_FREQ 5000 // Up to 40MHz.
#define PWM_RES 8 // Up to 16 bits.
#define BLUE_LED_PWM_CHANNEL 0 // Up to 16 channels, starting from 0. Don't use channels that are already in use
#define LED_FIL1_PWM_CHANNEL 1
#define LED_FIL2_PWM_CHANNEL 2

// Debounce
#define DEBOUNCE_TIME 50 // Time between button toggles that is considered anomalous


class LEDToggleUsermod : public Usermod 
{
    private:

    // -------------- CONFIG VALUES -------------- //

    bool resetDefaults = false;

    int buttonLastSteadyState = LOW; // Last normal button state
    int buttonLastFlickerState = LOW; // Last anomalous button state
    int buttonCurrentState;
    unsigned long lastDebounceTime = 0; 
    int LEDLastState = LOW;
    
    bool blueLedEnabled = true;
    bool ledFil1Enabled = true;
    bool ledFil2Enabled = true;

    bool blueLedLastState = true;
    bool ledFil1LastState = true;
    bool ledFil2LastState = true;

    bool blueLedRecentlyEnabled = false;
    bool ledFil1RecentlyEnabled = false;
    bool ledFil2RecentlyEnabled = false;

    bool blueLedRecentlyDisabled = false;
    bool ledFil1RecentlyDisabled = false;
    bool ledFil2RecentlyDisabled = false;

    int blueLedBrightness = 255;
    int ledFil1Brightness = 255;
    int ledFil2Brightness = 255;

    int blueLedLastBrightness = 255;
    int ledFil1LastBrightness = 255;
    int ledFil2LastBrightness = 255;

    int transitionDuration = 1;

    static const int defBlueLedBrightness = 255;
    static const int defLedFil1Brightness = 255;
    static const int defLedFil2Brightness = 255;
    static const bool defBlueLedEnabled = true;
    static const bool defLedFil1Enabled = true;
    static const bool defLedFil2Enabled = true;
    static const bool defTransitionDuration = 1; // Measured in seconds

    bool initDone = false;

    // Strings used to reduce flash memory usage

    static const char _name[];
    static const char _toggles[];
    static const char _brightness[];

    static const char _resetDefaults[];

    static const char _blueLed[];
    static const char _ledFil1[];
    static const char _ledFil2[];

    static const char _blueLedBrightness[];
    static const char _ledFil1Brightness[];
    static const char _ledFil2Brightness[];
    static const char _transitionTime[];


    public:

    // Runs once on startup. Used for initialisations
    void setup() 
    {
        // Pins
        pinMode(LED_BUTTON_PIN, INPUT_PULLUP);
        pinMode(BLUE_LED_PIN, OUTPUT);
        pinMode(LED_FIL1_PIN, OUTPUT);
        pinMode(LED_FIL2_PIN, OUTPUT);

        // PWM
        ledcSetup(BLUE_LED_PWM_CHANNEL, PWM_FREQ, PWM_RES);
        ledcSetup(LED_FIL1_PWM_CHANNEL, PWM_FREQ, PWM_RES);
        ledcSetup(LED_FIL2_PWM_CHANNEL, PWM_FREQ, PWM_RES);
        ledcAttachPin(BLUE_LED_PIN, BLUE_LED_PWM_CHANNEL);
        ledcAttachPin(LED_FIL1_PIN, LED_FIL1_PWM_CHANNEL);
        ledcAttachPin(LED_FIL2_PIN, LED_FIL2_PWM_CHANNEL);

        initDone = true;
    }

    int assignLedBrightness(const char _ledBrightness[], int ledBrightness)
    {
        if (ledBrightness <= 255)
        {
            return ledBrightness;
        }

        else
        {
            return 255;
        }
    }

    bool turnOffDisabledLed(bool ledState, bool ledRecentlyDisabled, int brightness, int transitionDuration, int pwmChannel)
    {
        if (ledState == false && ledRecentlyDisabled == true && LEDLastState == HIGH)
        {
            adjustLedBrightness(brightness, 0, transitionDuration, pwmChannel);
        }
        return false;
    }

    bool turnOnEnabledLed(bool ledState, bool ledRecentlyEnabled, int brightness, int transitionDuration, int pwmChannel)
    {
        if (ledState == true && ledRecentlyEnabled == true && LEDLastState == HIGH)
        {
            adjustLedBrightness(0, brightness, transitionDuration, pwmChannel);
        }
        return false;
    }

    int getTimeBetweenIncrement(int brightness, int transitionDuration)
    {
        return ((transitionDuration*1000)/brightness);
    }

    void adjustLedBrightness(int currentBrightness, int targetBrightness, int transitionDuration, int pwmChannel)
    {   
        unsigned long timeOfLastIncrement = millis();
        int dutyCycle = currentBrightness;
        if (targetBrightness > currentBrightness)
        {   
            // This can be done with a for loop if using delay(). However, delay() has to be avoided for long delays as it halts all operations for the specified time. This interferes with WLED. 
            // This issue can be fixed by using millis() instead. However, implementing it in a for loop isn't possible as dutyCycle has to increment on a condition, which can't be done with a for loop
            // This is why a while loop is used instead. 
            while (dutyCycle <= targetBrightness)
            {  
                if ((millis() - timeOfLastIncrement) >= getTimeBetweenIncrement(targetBrightness, transitionDuration))
                {
                    ledcWrite(pwmChannel, dutyCycle);
                    timeOfLastIncrement = millis();
                    dutyCycle++;
                }
            }
        }

        else if (targetBrightness < currentBrightness)
        {
            while (dutyCycle >= targetBrightness)
            {
                if ((millis() - timeOfLastIncrement) >= getTimeBetweenIncrement(currentBrightness, transitionDuration))
                {
                    ledcWrite(pwmChannel, dutyCycle);
                    timeOfLastIncrement = millis();
                    dutyCycle--;
                }
            }
        }
    }

    bool recentlyDisabled(bool ledState, bool ledLastState)
    {
        if (ledState == false && ledLastState == true) return true;
        else return false;
    }

    bool recentlyEnabled(bool ledState, bool ledLastState)
    {
        if (ledState == true && ledLastState == false) return true;
        else return false;
    }

    void loop() 
    {   

        blueLedRecentlyDisabled = turnOffDisabledLed(blueLedEnabled, blueLedRecentlyDisabled, blueLedBrightness, transitionDuration, BLUE_LED_PWM_CHANNEL);
        ledFil1RecentlyDisabled = turnOffDisabledLed(ledFil1Enabled, ledFil1RecentlyDisabled, ledFil1Brightness, transitionDuration, LED_FIL1_PWM_CHANNEL);
        ledFil2RecentlyDisabled = turnOffDisabledLed(ledFil2Enabled, ledFil2RecentlyDisabled, ledFil2Brightness, transitionDuration, LED_FIL2_PWM_CHANNEL);

        blueLedRecentlyEnabled = turnOnEnabledLed(blueLedEnabled, blueLedRecentlyEnabled, blueLedBrightness, transitionDuration, BLUE_LED_PWM_CHANNEL);
        ledFil1RecentlyEnabled = turnOnEnabledLed(ledFil1Enabled, ledFil1RecentlyEnabled, ledFil1Brightness, transitionDuration, LED_FIL1_PWM_CHANNEL);
        ledFil2RecentlyEnabled = turnOnEnabledLed(ledFil2Enabled, ledFil2RecentlyEnabled, ledFil2Brightness, transitionDuration, LED_FIL2_PWM_CHANNEL);


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
                if (LEDLastState == LOW) 
                {
                    //Serial.println("LED ON");
                    if (blueLedEnabled == true)
                    {
                        adjustLedBrightness(0, blueLedBrightness, transitionDuration, BLUE_LED_PWM_CHANNEL);
                    }
                    if (ledFil1Enabled == true)
                    {
                        adjustLedBrightness(0, ledFil1Brightness, transitionDuration, LED_FIL1_PWM_CHANNEL);
                    }
                    if (ledFil2Enabled == true)
                    {
                        adjustLedBrightness(0, ledFil2Brightness, transitionDuration, LED_FIL2_PWM_CHANNEL);
                    }
                    
                    LEDLastState = HIGH;
                }

                else if (LEDLastState == HIGH) 
                {
                    
                    if (blueLedEnabled == true)
                    {
                        adjustLedBrightness(blueLedBrightness, 0, transitionDuration, BLUE_LED_PWM_CHANNEL);
                    }
                    if (ledFil1Enabled == true)
                    {
                        adjustLedBrightness(ledFil1Brightness, 0, transitionDuration, LED_FIL1_PWM_CHANNEL);
                    }
                    if (ledFil2Enabled == true)
                    {
                        adjustLedBrightness(ledFil2Brightness, 0, transitionDuration, LED_FIL2_PWM_CHANNEL);
                    }

                    LEDLastState = LOW;

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
        
        JsonObject ledToggles = top.createNestedObject(FPSTR(_toggles));
        ledToggles[FPSTR(_blueLed)] = blueLedEnabled;
        ledToggles[FPSTR(_ledFil1)] = ledFil1Enabled;
        ledToggles[FPSTR(_ledFil2)] = ledFil2Enabled;

        JsonObject bright = top.createNestedObject(FPSTR(_brightness));
        bright[FPSTR(_blueLedBrightness)] = assignLedBrightness(_blueLedBrightness, blueLedBrightness);
        bright[FPSTR(_ledFil1Brightness)] = assignLedBrightness(_ledFil1Brightness, ledFil1Brightness);
        bright[FPSTR(_ledFil2Brightness)] = assignLedBrightness(_ledFil2Brightness, ledFil2Brightness);
        bright[FPSTR(_transitionTime)] = transitionDuration;

        if (resetDefaults == true)
        {
            top[FPSTR(_resetDefaults)] = false;
            ledToggles[FPSTR(_blueLed)] = defBlueLedEnabled;
            ledToggles[FPSTR(_ledFil1)] = defLedFil1Enabled;
            ledToggles[FPSTR(_ledFil2)] = defLedFil2Enabled;
            bright[FPSTR(_blueLedBrightness)] = defBlueLedBrightness;
            bright[FPSTR(_ledFil1Brightness)] = defLedFil1Brightness;
            bright[FPSTR(_ledFil2Brightness)] = defLedFil2Brightness;
            bright[FPSTR(_transitionTime)] = defTransitionDuration;
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

    bool readFromConfig(JsonObject& root) override
    {   
        //Serial.begin(115200);

        JsonObject top = root[FPSTR(_name)];
        JsonObject ledToggles = top[FPSTR(_toggles)];
        JsonObject bright = top[FPSTR(_brightness)];

        bool configComplete = !top,isNull();

        blueLedLastState = blueLedEnabled;
        ledFil1LastState = ledFil1Enabled;
        ledFil2LastState = ledFil2Enabled;
        blueLedLastBrightness = blueLedBrightness;
        ledFil1LastBrightness = ledFil1Brightness;
        ledFil2LastBrightness = ledFil2Brightness;

        //Assigns the saved values from the cfg.json file to its corresponding variable
        configComplete &= getJsonValue(top[FPSTR(_resetDefaults)], resetDefaults);

        configComplete &= getJsonValue(ledToggles[FPSTR(_blueLed)], blueLedEnabled);
        configComplete &= getJsonValue(ledToggles[FPSTR(_ledFil1)], ledFil1Enabled);
        configComplete &= getJsonValue(ledToggles[FPSTR(_ledFil2)], ledFil2Enabled);

        configComplete &= getJsonValue(bright[FPSTR(_blueLedBrightness)], blueLedBrightness);
        configComplete &= getJsonValue(bright[FPSTR(_ledFil1Brightness)], ledFil1Brightness);
        configComplete &= getJsonValue(bright[FPSTR(_ledFil2Brightness)], ledFil2Brightness);
        configComplete &= getJsonValue(bright[FPSTR(_transitionTime)], transitionDuration);


        blueLedRecentlyDisabled = recentlyDisabled(blueLedEnabled, blueLedLastState);
        ledFil1RecentlyDisabled = recentlyDisabled(ledFil1Enabled, ledFil1LastState);
        ledFil2RecentlyDisabled = recentlyDisabled(ledFil2Enabled, ledFil2LastState);
        blueLedRecentlyEnabled = recentlyEnabled(blueLedEnabled, blueLedLastState);
        ledFil1RecentlyEnabled = recentlyEnabled(ledFil1Enabled, ledFil1LastState);
        ledFil2RecentlyEnabled = recentlyEnabled(ledFil2Enabled, ledFil2LastState);
        
        if (initDone == true)
        {   
            // Note: It would make sense to put the LEDLastState condition in the first if statement, however this breaks the program.
            if (blueLedEnabled == true && (blueLedLastBrightness != blueLedBrightness && LEDLastState == HIGH)) adjustLedBrightness(blueLedLastBrightness, blueLedBrightness, transitionDuration, BLUE_LED_PWM_CHANNEL);
            if (ledFil1Enabled == true && (ledFil1LastBrightness != ledFil1Brightness && LEDLastState == HIGH)) adjustLedBrightness(ledFil1LastBrightness, ledFil1Brightness, transitionDuration, LED_FIL1_PWM_CHANNEL);
            if (ledFil2Enabled == true && (ledFil2LastBrightness != ledFil2Brightness && LEDLastState == HIGH)) adjustLedBrightness(ledFil2LastBrightness, ledFil2Brightness, transitionDuration, LED_FIL2_PWM_CHANNEL);
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
        uiScript.print(F("addInfo('LEDToggle:enable-LEDs:LED-filament-2',1,'<br /><i style=\"color:#7AB6FF;\">The on/off state of disabled LEDs will still change with button presses to prevent any desynchronisation</i>');"));
        uiScript.print(F("addInfo('LEDToggle:LED-brightnesses:LED-filament-2-brightness',1,'<br /><i style=\"color:#7AB6FF;\">Input integer values up to 255</i><br /><i style=\"color:#FFAA00;\">Values greater than 255 will be interpreted as 255</i><br /><i style=\"color:#FFAA00;\">Decimal values will be truncated to just their integer components</i>');"));
        uiScript.print(F("addInfo('LEDToggle:reset-to-defaults',1,'<br /><i style=\"color:#FF2e2e;\">WILL RESET WITHOUT CONFIRMATION!</i>');"));
        uiScript.print(F("addInfo('LEDToggle:LED-brightnesses:LED-transition-time',1,'s');"));
    }
};

// -------------------- Strings used to reduce flash memory usage -------------------- //

const char LEDToggleUsermod::_name[]               PROGMEM = "LEDToggle";
const char LEDToggleUsermod::_toggles[]            PROGMEM = "enable-LEDs";
const char LEDToggleUsermod::_brightness[]         PROGMEM = "LED-brightnesses";

const char LEDToggleUsermod::_resetDefaults[]      PROGMEM = "reset-to-defaults";

const char LEDToggleUsermod::_blueLed[]            PROGMEM = "blue-LED";
const char LEDToggleUsermod::_ledFil1[]            PROGMEM = "LED-filament-1";
const char LEDToggleUsermod::_ledFil2[]            PROGMEM = "LED-filament-2";

const char LEDToggleUsermod::_blueLedBrightness[]  PROGMEM = "blue-LED-brightness";
const char LEDToggleUsermod::_ledFil1Brightness[]  PROGMEM = "LED-filament-1-brightness";
const char LEDToggleUsermod::_ledFil2Brightness[]  PROGMEM = "LED-filament-2-brightness";
const char LEDToggleUsermod::_transitionTime[]     PROGMEM = "LED-transition-time";