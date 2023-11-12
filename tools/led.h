#ifndef LED_H
#define LED_H
/**
 * @brief ?
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
		digitalWrite(M5_LED, LOW);
		delay(10);
		digitalWrite(M5_LED, HIGH);
    }

	void flash(int times) {
		for (int i = 0; i < times; i++) {
			digitalWrite(M5_LED, LOW);
			delay(10);
			digitalWrite(M5_LED, HIGH);
			delay(100);
		}
    }
};

#endif