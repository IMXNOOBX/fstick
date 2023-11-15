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
        return version;
    }

    void setVersion(const String& newVersion) {
        version = newVersion;
        this->save(); 
    }

    bool getLed() const {
        return led_enable;
    }

    void setLed(bool enable) {
		led.off();
        led_enable = enable;
        this->save();
    }

    bool getSound() const {
        return sound_enable;
    }

    void setSound(bool enable) {
        sound_enable = enable;
        this->save();
    }

    bool getBattSaver() const {
        return battery_saver;
    }

    void setBattSaver(bool enable) {
        battery_saver = enable;
        battery.setBT(enable);
        this->save();
    }

    bool getRotation() const {
        return rotation;
    }

    void setRotation(int pos) {
	    M5.Lcd.setRotation(pos);
        rotation = pos;
        this->save();
    }
public:
    bool led_enable = true;
    bool battery_saver = true;
    bool sound_enable = false;
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
		setLed(led_enable);
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
