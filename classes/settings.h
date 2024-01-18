#include "globals.h"

extern Logger logger;
extern Notify notify;
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
        #if !defined(PLUS2)
            RTC_TimeTypeDef TimeStruct;
            TimeStruct.Hours = hour;
            TimeStruct.Minutes = minute;
            TimeStruct.Seconds = second;
            M5.Rtc.SetTime(&TimeStruct);
        #endif
		this->save();
	}

    bool toggleScretMode() {
        this->secret_mode = true;
        return this->secret_mode;
    }

    bool getSecretMode() {
        return this->secret_mode;
    }

    bool getLed() const {
        return this->led_enable;
    }

    void toggleLed() {
        this->led_enable = !this->led_enable;
        notify.led.is_enabled = this->led_enable;

        notify.send();
        this->save();
        logger.log(Logger::INFO, "Setting changed, led is now " + String(this->led_enable ? "enabled" : "disabled"));
    }

    bool getSound() const {
        return this->sound_enable;
    }

    void toggleSound() {
        this->sound_enable = !this->sound_enable;
        notify.beeper.is_enabled = this->sound_enable;

        notify.send();
        this->save();
        logger.log(Logger::INFO, "Setting changed, sound is now " + String(this->sound_enable ? "enabled" : "disabled"));
    }

    bool getBattSaver() const {
        return this->battery_saver;
    }

    void toggleBattSaver() {
        this->battery_saver = !this->battery_saver;
        battery.setBT(this->battery_saver);

        notify.send();
        this->save();
        logger.log(Logger::INFO, "Setting changed, battery saver is now " + String(this->battery_saver ? "enabled" : "disabled"));
    }

    bool getRotation() const {
        return rotation;
    }

    void switchRotation() {
        this->rotation = (this->rotation > 2 ? 0 : this->rotation + 1);
        this->setRotation(this->rotation);
    }

    void setRotation(int pos) {
        this->rotation = pos;
	    SCREEN.setRotation(pos); // 0 to 3 rotate clockwise, 4 to 7 rotate counterclockwise (default is 1)

        notify.send();
        this->save();
        logger.log(Logger::INFO, "Setting changed, rotation is now " + String(this->rotation));
    }

public:
    bool led_enable = true;
    bool battery_saver = true;
    bool sound_enable = false;
    uint8_t rotation = 1;

    bool secret_mode = false;
private:

    void load() {
		if(!this->read()) {
            this->save();
            logger.log(Logger::INFO, "Settings have been resetted to default values");
            return;
        }

        battery.setBT(this->battery_saver);
        notify.led.is_enabled = this->led_enable;
        notify.beeper.is_enabled = this->sound_enable;
		setRotation(rotation);
    }

    bool read() {
        size_t offset = 0;

        // led_enable
        led_enable = static_cast<bool>(EEPROM.read(offset++));
        logger.log(Logger::INFO, "Led is " + String(led_enable ? "enabled" : "disabled"));

        // sound_enable
        sound_enable = static_cast<bool>(EEPROM.read(offset++));
        logger.log(Logger::INFO, "Sound is " + String(sound_enable ? "enabled" : "disabled"));

        // battery_saver
        battery_saver = static_cast<bool>(EEPROM.read(offset++));
        logger.log(Logger::INFO, "Battery saver is " + String(battery_saver ? "enabled" : "disabled"));

        // rotation
        rotation = EEPROM.read(offset++);
        logger.log(Logger::INFO, "Rotation is " + String(rotation));
        
        if (rotation < 0 || rotation > 3)
            return false;

        return true;
    }

    void save() {
        size_t offset = 0;

        // led_enable
        EEPROM.write(offset++, led_enable);

        // sound_enable
        EEPROM.write(offset++, sound_enable);

        // battery_saver
        EEPROM.write(offset++, battery_saver);

        // rotation
        EEPROM.write(offset++, rotation);

        EEPROM.commit(); // Commit the changes to EEPROM
    }
};
