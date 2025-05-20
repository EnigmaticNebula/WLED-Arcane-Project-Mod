// Note: If starting serial communication to get outputs, use Serial.begin(115200) in readFromConfig()

#pragma once
#include "wled.h"
#include "tones.h"

// Pin definitions (Corresponds to the GPIO pins)
// If changing pins, consult ESP32 documentation to check what pins to use
#define BUZZER_PIN 22

// PWM definitions
// As a guideline, keep PWM_FREQ * (2^PWM_RES) below 80MHz. Higher PWM_FREQ means less resolution.
#define BUZZER_PWM_FREQ 2000 // Up to 40MHz
#define BUZZER_PWM_RES 13    // Up to 16 bits
#define BUZZER_PWM_CHANNEL 3 // Up to 16 channels, starting from 0. Don't use channels that are already in use


// -------------------------------------------------- SONG DEFINITIONS ------------------------------------------------- //

// Rick roll
int rickRollMelody[] = {NOTE_CS4, NOTE_DS4, NOTE_F4, NOTE_DS4, NOTE_AS4, NOTE_AS4, NOTE_GS4, 0,                                                             // Never gonna give you up
                        NOTE_CS4, NOTE_DS4, NOTE_F4, NOTE_DS4, NOTE_GS4, NOTE_GS4, NOTE_FS4, NOTE_F4, NOTE_DS4,                                             // Never gonna let you down
                        NOTE_CS4, NOTE_DS4, NOTE_FS4, NOTE_DS4, NOTE_FS4, NOTE_GS4, NOTE_F4, NOTE_DS4, NOTE_CS4, 0, NOTE_CS4, NOTE_GS4, NOTE_FS4, 0,        // Never gonna run around and desert you
                        NOTE_CS4, NOTE_DS4, NOTE_FS4, NOTE_DS4, NOTE_AS4, NOTE_AS4, NOTE_GS4, 0,                                                            // Never gonna make you cry
                        NOTE_CS4, NOTE_DS4, NOTE_F4, NOTE_DS4, NOTE_CS5, NOTE_F4, NOTE_FS4, NOTE_F4, NOTE_DS4,                                              // Never gonna say goodbye
                        NOTE_CS4, NOTE_DS4, NOTE_F4, NOTE_DS4, NOTE_FS4, NOTE_GS4, NOTE_F4, NOTE_DS4, NOTE_CS4, 0, NOTE_CS4, NOTE_GS4, NOTE_FS4, NOTE_FS4}; // Never gonna tell a lie and hurt you

int rickRollNoteDurations[] = {133, 133, 133, 133, 398, 398, 500, 281,
                               133, 133, 133, 133, 398, 398, 266, 133, 398,
                               133, 133, 133, 133, 398, 398, 133, 398, 239, 292, 239, 531, 531, 531,
                               133, 133, 133, 133, 398, 398, 664, 175,
                               133, 133, 133, 133, 398, 398, 239, 133, 398,
                               133, 133, 133, 133, 398, 398, 398, 133, 239, 292, 239, 239, 239, 531};


// Lava chicken
int lavaChickenMelody[] = {
    NOTE_C5, NOTE_C5, NOTE_C5, NOTE_DS5, NOTE_C5, 0, NOTE_C5, NOTE_C5, NOTE_C5, NOTE_DS5, NOTE_C5, 0,                             // la la la lava chi chi chi chicken
    NOTE_F5, NOTE_F5, NOTE_DS5, NOTE_F5, NOTE_DS5, NOTE_C5, NOTE_AS4, NOTE_C5, NOTE_C5, NOTE_AS4, NOTE_DS5, 0,                    // Steve's lava chicken yeah it's tasty as hell
    NOTE_F5, NOTE_F5, NOTE_DS5, NOTE_F5, NOTE_DS5, NOTE_C5, NOTE_AS4, NOTE_C5, NOTE_C5, NOTE_DS5, NOTE_C5, NOTE_AS4, NOTE_C5, 0,  // ooh mama cita now you're ringing the bell
    NOTE_F5, NOTE_F5, NOTE_DS5, NOTE_F5, NOTE_DS5, NOTE_C5, NOTE_AS4, NOTE_C5, NOTE_C5, NOTE_AS4, NOTE_DS5, NOTE_F5, NOTE_DS5, 0, // crispy and juicy now you're having a snack
    NOTE_F5, NOTE_F5, NOTE_DS5, NOTE_F5, NOTE_DS5, NOTE_C5, NOTE_AS4, NOTE_C6, NOTE_C6, NOTE_C6, NOTE_C6                          // ooh super spicy it's a lava attack
};

int lavaChickenNoteDurations[] = {193, 193, 193, 193, 578, 193, 193, 193, 193, 193, 578, 578,
                                        578, 193, 193, 193, 193, 193, 193, 193, 193, 193, 578, 193,
                                        578, 193, 193, 193, 193, 193, 193, 193, 193, 193, 97, 97, 385, 193,
                                        578, 193, 193, 193, 193, 193, 193, 193, 193, 193, 97, 97, 385, 193,
                                        578, 193, 193, 193, 193, 193, 193, 193, 193, 193, 1733};


// Enemy
int enemyMelody[] = {NOTE_E4, NOTE_FS4, NOTE_E4, NOTE_D4, NOTE_B3, 0, NOTE_E4, NOTE_D4, NOTE_E4, NOTE_D4, NOTE_E4, NOTE_D4, NOTE_E4, NOTE_D4, NOTE_E4, NOTE_D4, NOTE_B4, NOTE_D5, 0, // oh the misery. Everybody wants to be my enemy
                     NOTE_E4, NOTE_FS4, NOTE_E4, NOTE_D4, NOTE_B3, 0, NOTE_E4, NOTE_D4, NOTE_E4, NOTE_D4, NOTE_E4, NOTE_D4, NOTE_B4, 0, NOTE_B3, NOTE_B3, NOTE_B3, NOTE_D4, 0,       // spare the sympathy. everybody wants to be. my enemy
                     NOTE_CS4, 0, NOTE_B3, 0, NOTE_FS3, NOTE_E3, NOTE_FS3, 0, NOTE_B4, NOTE_B4, NOTE_B4, NOTE_D4, 0,                                                                 // e e e e e, my enemy
                     NOTE_CS4, 0, NOTE_B3, 0, NOTE_FS3, NOTE_E3, NOTE_FS3, 0, NOTE_B2, NOTE_B2, NOTE_B2, NOTE_D4};                                                                   // e e e e e, but i'm ready
                                                    
int enemyNoteDurations[] = {587, 196, 196, 196, 391, 782, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 782, 196, 978,
                            587, 196, 196, 196, 391, 782, 196, 196, 196, 196, 196, 196, 391, 587, 196, 196, 196, 782, 391,
                            391, 391, 391, 391, 196, 587, 391, 1760, 196, 196, 196, 782, 391,
                            391, 391, 391, 391, 196, 587, 391, 1564, 196, 196, 196, 391};

// Beep
int beepMelody[] = {NOTE_A7};
int beepNoteDurations[] = {200};


class StartupJingleUsermod : public Usermod
{
    private:

    // -------------- CONFIG VALUES -------------- //

    bool enabled = true;
    bool resetDefaults = false;

    bool rickRollJingleEnabled = true;
    bool lavaChickenJingleEnabled = true;
    bool enemyJingleEnabled = true;
    bool beepJingleEnabled = true;

    int rickRollPerc = 10;
    int lavaChickenPerc = 10;
    int enemyPerc = 10;
    int beepPerc = 70;

    static const int defRickRollPerc = 10;
    static const int defLavaChickenPerc = 10;
    static const int defEnemyPerc = 10;
    static const int defBeepPerc = 70;
    static const bool defRickRollJingleEnabled = true;
    static const bool defLavaChickenJingleEnabled = true;
    static const bool defEnemyJingleEnabled = true;
    static const bool defBeepJingleEnabled = true;
    
    int prevRickRollPerc = 10;
    int prevLavaChickenPerc = 10;
    int prevEnemyPerc = 10;
    int prevBeepPerc = 70;

    int songRepeats = 0;
    int randomNum;


    // Strings used to reduce flash memory usage

    static const char _name[];
    static const char _toggles[];
    static const char _percentages[];

    static const char _enabled[];
    static const char _resetDefaults[];

    static const char _rickRollJingleEnabled[];
    static const char _lavaChickenJingleEnabled[];
    static const char _enemyJingleEnabled[];
    static const char _beepJingleEnabled[];

    static const char _rickRollPerc[];
    static const char _lavaChickenPerc[];
    static const char _enemyPerc[];
    static const char _beepPerc[];


    public:

    // Runs once on startup. Used for initialisations
    void setup() 
    {
        // Pins
        pinMode(BUZZER_PIN, OUTPUT);

        // PWM
        ledcSetup(BUZZER_PWM_CHANNEL, BUZZER_PWM_FREQ, BUZZER_PWM_RES);
        ledcAttachPin(BUZZER_PIN, BUZZER_PWM_CHANNEL);
    }


    void playJingle(bool jingleEnabled, int melody[], int noteDurations[], int noteCount)
    {   
        if (jingleEnabled == true)
        {
            for (int i = 0; i < noteCount; i++)
            {
                ledcWriteTone(BUZZER_PWM_CHANNEL, melody[i]);
                delay(noteDurations[i]);
                ledcWriteTone(BUZZER_PWM_CHANNEL, 0);
            }
            songRepeats += 1;
        }
    }

    int getRandNum()
    {
        randomNum = (esp_random() & 0b1111111)+1;
        while (randomNum > 100)
        {
            randomNum = (esp_random() & 0b1111111)+1;
        }

        return randomNum;
    }

    void loop() {

        if (songRepeats == 0) // Makes sure that the song is only played once on startup
        {   
            if (enabled == true && (enemyJingleEnabled == true || rickRollJingleEnabled == true || lavaChickenJingleEnabled == true || beepJingleEnabled == true))
            {
                randomNum = getRandNum();

                // ------------------------- SONGS ------------------------- //

                // Never Gonna Give You Up
                if (randomNum >= 1 && randomNum <= rickRollPerc) 
                {
                    playJingle(rickRollJingleEnabled, rickRollMelody, rickRollNoteDurations, 62);
                }

                // Steve's Lava Chicken
                else if (randomNum >= rickRollPerc+1 && randomNum <= (rickRollPerc+lavaChickenPerc)) 
                {
                    playJingle(lavaChickenJingleEnabled, lavaChickenMelody, lavaChickenNoteDurations, 63);
                }

                // Enemy
                else if (randomNum >= (rickRollPerc+lavaChickenPerc+1) && randomNum <= rickRollPerc+lavaChickenPerc+enemyPerc)
                {
                    playJingle(enemyJingleEnabled, enemyMelody, enemyNoteDurations, 63);
                }

                // Startup beep
                else if (randomNum >= (rickRollPerc+lavaChickenPerc+enemyPerc) && randomNum <= rickRollPerc+lavaChickenPerc+enemyPerc+beepPerc)
                {
                    playJingle(beepJingleEnabled, beepMelody, beepNoteDurations, 1);
                }
            }

            // Runs if the jingle mod is enabled but no songs are
            else
            {
                songRepeats = songRepeats+1;
            }
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
        top[FPSTR(_enabled)] = enabled;
        top[FPSTR(_resetDefaults)] = resetDefaults;
        
        JsonObject jingleToggles = top.createNestedObject(FPSTR(_toggles));
        jingleToggles[FPSTR(_rickRollJingleEnabled)] = rickRollJingleEnabled;
        jingleToggles[FPSTR(_lavaChickenJingleEnabled)] = lavaChickenJingleEnabled;
        jingleToggles[FPSTR(_enemyJingleEnabled)] = enemyJingleEnabled;
        jingleToggles[FPSTR(_beepJingleEnabled)] = beepJingleEnabled;

        JsonObject jinglePerc = top.createNestedObject(FPSTR(_percentages));

        if ((rickRollPerc + beepPerc + lavaChickenPerc + enemyPerc) == 100)
        {
            if (rickRollPerc == 0) jingleToggles[FPSTR(_rickRollJingleEnabled)] = false;
            if (lavaChickenPerc == 0) jingleToggles[FPSTR(_lavaChickenJingleEnabled)] = false;
            if (enemyPerc == 0) jingleToggles[FPSTR(_enemyJingleEnabled)] = false;
            if (beepPerc == 0) jingleToggles[FPSTR(_beepJingleEnabled)] = false;

            jinglePerc[FPSTR(_rickRollPerc)] = rickRollPerc;
            jinglePerc[FPSTR(_lavaChickenPerc)] = lavaChickenPerc;
            jinglePerc[FPSTR(_enemyPerc)] = enemyPerc;
            jinglePerc[FPSTR(_beepPerc)] = beepPerc;

        }

        else 
        {    
            jinglePerc[FPSTR(_rickRollPerc)] = prevRickRollPerc;
            jinglePerc[FPSTR(_lavaChickenPerc)] = prevLavaChickenPerc;
            jinglePerc[FPSTR(_enemyPerc)] = prevEnemyPerc;
            jinglePerc[FPSTR(_beepPerc)] = prevBeepPerc;
        }

        if (resetDefaults == true)
        {
            top[FPSTR(_enabled)] = true;
            top[FPSTR(_resetDefaults)] = false;

            jingleToggles[FPSTR(_rickRollJingleEnabled)] = defRickRollJingleEnabled;
            jingleToggles[FPSTR(_lavaChickenJingleEnabled)] = defLavaChickenJingleEnabled;
            jingleToggles[FPSTR(_enemyJingleEnabled)] = defEnemyJingleEnabled;
            jingleToggles[FPSTR(_beepJingleEnabled)] = defBeepJingleEnabled;

            prevRickRollPerc = defRickRollPerc;
            prevLavaChickenPerc = defLavaChickenPerc;
            prevEnemyPerc = defEnemyPerc;
            prevBeepPerc = defBeepPerc;

            jinglePerc[FPSTR(_rickRollPerc)] = defRickRollPerc;
            jinglePerc[FPSTR(_lavaChickenPerc)] = defLavaChickenPerc;
            jinglePerc[FPSTR(_enemyPerc)] = defEnemyPerc;
            jinglePerc[FPSTR(_beepPerc)] = defBeepPerc;
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
        JsonObject top = root[FPSTR(_name)];
        JsonObject jingleToggles = top[FPSTR(_toggles)];
        JsonObject perc = top[FPSTR(_percentages)];

        bool configComplete = !top.isNull();

        // This if statement prevents invalid percentages being saved as previous values. This fixes the case where invalid values are saved if the user enters in decimal values, and the integer components don't add to 100
        if ((rickRollPerc + lavaChickenPerc + enemyPerc + beepPerc) == 100)
        {
            if (prevRickRollPerc != rickRollPerc) prevRickRollPerc = rickRollPerc;
            if (prevLavaChickenPerc != lavaChickenPerc) prevLavaChickenPerc = lavaChickenPerc;
            if (prevEnemyPerc != enemyPerc) prevEnemyPerc = enemyPerc;
            if (prevBeepPerc != beepPerc) prevBeepPerc = beepPerc;
        }

        configComplete &= getJsonValue(top[FPSTR(_enabled)], enabled);
        configComplete &= getJsonValue(top[FPSTR(_resetDefaults)], resetDefaults);

        configComplete &= getJsonValue(jingleToggles[FPSTR(_rickRollJingleEnabled)], rickRollJingleEnabled);
        configComplete &= getJsonValue(jingleToggles[FPSTR(_lavaChickenJingleEnabled)], lavaChickenJingleEnabled);
        configComplete &= getJsonValue(jingleToggles[FPSTR(_enemyJingleEnabled)], enemyJingleEnabled);
        configComplete &= getJsonValue(jingleToggles[FPSTR(_beepJingleEnabled)], beepJingleEnabled);

        configComplete &= getJsonValue(perc[FPSTR(_rickRollPerc)], rickRollPerc);
        configComplete &= getJsonValue(perc[FPSTR(_lavaChickenPerc)], lavaChickenPerc);
        configComplete &= getJsonValue(perc[FPSTR(_enemyPerc)], enemyPerc);
        configComplete &= getJsonValue(perc[FPSTR(_beepPerc)], beepPerc);

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
        uiScript.print(F("addInfo('StartupJingle:jingle-percentages:startup-beep-percentage',1,'<br /><i style=\"color:#7AB6FF;\">Integer values only</i><br /><i style=\"color:#FFAA00;\">Values must add to 100%</i>');"));
        uiScript.print(F("addInfo('StartupJingle:reset-to-defaults',1,'<br /><i style=\"color:#FF2e2e\">WILL RESET WITHOUT CONFIRMATION!</i>');"));
    }
};

    
// -------------------- Strings used to reduce flash memory usage -------------------- //

const char StartupJingleUsermod::_name[]                      PROGMEM = "StartupJingle";
const char StartupJingleUsermod::_toggles[]                   PROGMEM = "jingle-toggles";
const char StartupJingleUsermod::_percentages[]               PROGMEM = "jingle-percentages";

const char StartupJingleUsermod::_enabled[]                   PROGMEM = "enabled";
const char StartupJingleUsermod::_resetDefaults[]             PROGMEM = "reset-to-defaults";

const char StartupJingleUsermod::_rickRollJingleEnabled[]     PROGMEM = "rickroll";
const char StartupJingleUsermod::_lavaChickenJingleEnabled[]  PROGMEM = "steves-lava-chicken";
const char StartupJingleUsermod::_enemyJingleEnabled[]        PROGMEM = "enemy";
const char StartupJingleUsermod::_beepJingleEnabled[]         PROGMEM = "startup-beep";

const char StartupJingleUsermod::_rickRollPerc[]              PROGMEM = "rickroll-percentage";
const char StartupJingleUsermod::_lavaChickenPerc[]           PROGMEM = "steves-lava-chicken-percentage";
const char StartupJingleUsermod::_enemyPerc[]                 PROGMEM = "enemy-percentage";
const char StartupJingleUsermod::_beepPerc[]                  PROGMEM = "startup-beep-percentage";
