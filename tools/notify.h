#ifndef NOTIFY_H
#define NOTIFY_H


class Beeper {	
public:
	Beeper() { }

	void beep() {
		if (!is_enabled) return;
		#if !defined(PLUS2)
			BUZZER.tone(4500);
			delay(50);
			BUZZER.mute();
		#else
			BUZZER.tone(4500, 50);
		#endif
	}

	void beep(int times) {
		if (!is_enabled) return;
		for (int i = 0; i < times; i++) {
			beep();
			delay(50);
		}
	}
public:
	bool is_enabled = true;
};

/**
 * @note ?
 * When LOW the LED is on, when HIGH the LED is off.
 */
class Led {
public:
    Led() {
		pinMode(M5_LED, OUTPUT);
		digitalWrite(M5_LED, HIGH);
    }

    void on() {
		digitalWrite(M5_LED, LOW);
    }

	void off() {
		digitalWrite(M5_LED, HIGH);
    }

	void flash() {
		if (!is_enabled) return;
		digitalWrite(M5_LED, LOW);
		delay(10);
		digitalWrite(M5_LED, HIGH);
    }

	void flash(int times) {
		if (!is_enabled) return;
		for (int i = 0; i < times; i++) {
			flash();
			delay(50);
		}
    }

public:
	bool is_enabled = true;
};

class Notify {
public:
	Notify() { 
		beeper = Beeper();
		led = Led();
	}

	void send() {
		beeper.beep();
		led.flash();
	}

	void send(int times) {
		beeper.beep(times);
		led.flash(times);
	}
public:
	Beeper beeper;
	Led led;
};

#endif // ! NOTIFY_H