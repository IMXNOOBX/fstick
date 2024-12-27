#include "globals.h"

extern Logger logger;
extern Notify notify;
extern Battery battery;

class CSettings
{
public:
	CSettings() {}

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
		#if defined(PLUS2)
			auto dt = M5.Rtc.getDateTime();
			dt.time.hours = hour;
			dt.time.minutes = minute;
			dt.time.seconds = second;
			M5.Rtc.setDateTime(dt);
		#else
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

    void set_hour(bool next = false, bool prev = false, bool click = false) {
        if (b_is_selecting_hours) {
            if (next) 
                if (u8t_hour == 23)
                    u8t_hour = 0;
                else
                    u8t_hour++;

            if (prev)
                if (u8t_hour == 0)
                    u8t_hour = 23;
                else
                    u8t_hour--;
        }
        else if (b_is_selecting_minutes) {
            if (next)
                if (u8t_minute == 59)
                    u8t_minute = 0;
                else
                    u8t_minute++;
            if (prev)
                if (u8t_minute == 0)
                    u8t_minute = 59;
                else
                    u8t_minute--;
        }
        else if (click) {
            if (b_is_selecting_hours) {
                b_is_selecting_hours = false;
                b_is_selecting_minutes = true;
            }
            else if (b_is_selecting_minutes) {
                b_is_selecting_minutes = false;
                b_is_selecting_hours = true;
                b_is_selecting_time = false;
            }
            else {
                b_is_selecting_hours = true;
                b_is_selecting_time = false;
            }
        }
        else {
            b_is_selecting_time = !b_is_selecting_time;
            b_is_selecting_hours = true;
            b_is_selecting_minutes = false;
        }

        notify.send();
    }

public:
    bool led_enable = true;
    bool battery_saver = true;
    bool sound_enable = false;
    uint8_t rotation = 1;

    bool secret_mode = false;
    bool b_is_selecting_time = false;
    bool b_is_selecting_hours = false;
    bool b_is_selecting_minutes = false;

    uint8_t u8t_hour = 0;
    uint8_t u8t_minute = 0;

    bool had_fs_before = true;
    size_t fs_offset = 0x75;
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
        
        if (had_fs_before)
		    setRotation(rotation);
    }

    bool read() {
        size_t offset = 0;

        // had_fs_before
        had_fs_before = static_cast<bool>(EEPROM.read(fs_offset));
        logger.log(Logger::INFO, "Was fs installed before? " + String(had_fs_before ? "yes" : "no"));

        if (!had_fs_before)
            return false;

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

        // had_fs_before
        EEPROM.write(fs_offset, true);

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
