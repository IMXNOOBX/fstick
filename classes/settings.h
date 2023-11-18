#include "globals.h"

extern Logger l;
extern Led led;
extern Battery battery;

class Settings
{
public:
	Settings() {}

	bool init()
	{
		try{
			EEPROM.begin(512);
			this->load();
			return true;
		}
		catch (...) {
			return false;
		}
	}

	bool destroy() {
		try {
			EEPROM.end();
			return true;
		}
		catch (...) {
			return false;
		}
	}

	void setClock(uint8_t hour, uint8_t minute, uint8_t second) {
        RTC_TimeTypeDef TimeStruct;
        TimeStruct.Hours = hour;
        TimeStruct.Minutes = minute;
        TimeStruct.Seconds = second;
		M5.Rtc.SetTime(&TimeStruct);
		this->save();
	}

	String getVersion() const {
        return this->version;
    }

    void setVersion(const String& newVersion) {
        this->version = newVersion;
        this->save(); 
    }

    bool toggleScretMode() {
        this->secret_mode = true;
        return this->secret_mode;
    }

    bool getSecretMode() {
        return this->secret_mode;
    }

    int getSecretCount() {
        return ++secret_count;
    }

    void resetSecretCount() {
        this->secret_count = 0;
    }

    bool getLed() const {
        return this->led_enable;
    }

    void toggleLed() {
        led.flash();
        led.is_enabled = !led_enable;
        led_enable = !led_enable;
        this->save();
        l.log(Logger::INFO, "Setting changed, led is now " + String(led_enable ? "enabled" : "disabled"));
    }

    bool getSound() const {
        return this->sound_enable;
    }

    void toggleSound() {
        led.flash();
        this->sound_enable = !this->sound_enable;
        this->save();
        l.log(Logger::INFO, "Setting changed, sound is now " + String(this->sound_enable ? "enabled" : "disabled"));
    }

    bool getBattSaver() const {
        return this->battery_saver;
    }

    void toggleBattSaver() {
        led.flash();
        battery.setBT(this->battery_saver);
        this->battery_saver = !this->battery_saver;
        this->save();
        l.log(Logger::INFO, "Setting changed, battery saver is now " + String(this->battery_saver ? "enabled" : "disabled"));
    }

    bool getRotation() const {
        return rotation;
    }

    void setRotation(int pos) {
        led.flash();
	    M5.Lcd.setRotation(pos);
        this->rotation = pos;
        this->save();
        l.log(Logger::INFO, "Setting changed, rotation is now " + String(this->rotation));
    }

    bool led_enable = true;
public:
    bool battery_saver = true;
    bool sound_enable = false;

    bool secret_mode = false;
    int secret_count = 0;
private:
    String version = VERSION;
    int rotation = 1;

	// Keep track of cumulative size
    size_t sizeOffset = 0;

    void load() {
		if(!this->read()) {
            this->save();
            l.log(Logger::INFO, "Settings have been resetted to default values");
            return;
        }

        battery.setBT(battery_saver);
        led.is_enabled = !this->led_enable;
		setRotation(rotation);
    }

    bool read() {
        EEPROM.get(0, version);
		
		sizeOffset += sizeof(String);
        EEPROM.get(sizeOffset, led_enable); 
		
		sizeOffset += sizeof(bool);
        EEPROM.get(sizeOffset, sound_enable);
		
		sizeOffset += sizeof(bool);
        EEPROM.get(sizeOffset, battery_saver);

		sizeOffset += sizeof(int);
        EEPROM.get(sizeOffset, rotation);

        if (rotation <= 3 || rotation >= 0)
            return false;

		sizeOffset = 0;
        return true;
    }

    void save() {
        EEPROM.put(0, version);

		sizeOffset += sizeof(String);
        EEPROM.put(sizeOffset, led_enable);

		sizeOffset += sizeof(bool);
        EEPROM.put(sizeOffset, sound_enable);

		sizeOffset += sizeof(bool);
        EEPROM.put(sizeOffset, battery_saver);

		sizeOffset += sizeof(int);
        EEPROM.put(sizeOffset, rotation);

        EEPROM.commit(); // Commit the changes to EEPROM
		sizeOffset = 0;
    }
};
