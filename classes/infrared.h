#include "globals.h"

extern Logger l;
extern Led led;

class IrBlaster {
public:
    IrBlaster() {}

	bool init() { 
		try {
			irsend = new IRsend(kIrSendPin);
			return true;
		} catch(...) {
			return false; 
		}
	}

    void turnOnOff() {
        sendIRCode(TV_ON_OFF_POWER_CODE); // Define your TV's power-on IR code
		l.log(Logger::INFO, "turnOnOff() called, sending code: " + String(TV_ON_OFF_POWER_CODE));
    }

    void turnOnOffLoop() {
		// for tv codes
		for (int i = 0; i < tvCodesSize; i++) {
			sendIRCode(tvCodes[i]); // Define your TV's power-on IR code
		}

		l.log(Logger::INFO, "turnOnOffLoop() called, sending code: " + String(TV_ON_OFF_POWER_CODE));
    }

	void sendAllPowerCodes() {
        for (int i = 0; i < powerCodesCount; i++) {
            sendCustomIRCode(powerCodes[i]->times, powerCodes[i]->codes, powerCodes[i]->numpairs);
            delay(500); // Adjust delay as needed
        }
    }

	void loop() {

	}

private:
    int irLed;
    IRsend* irsend;
	uint16_t kIrSendPin = 9; // IR Emitter Pin - M5 IR Unit
    const uint16_t TV_ON_OFF_POWER_CODE = 0x1234; // Replace with your TV's power IR code

    void sendIRCode(uint16_t code) {
        irsend->sendNEC(code, 32);
		led.flash();
    }

	void sendCustomIRCode(const uint16_t customCode[], const uint8_t customCodes[], int codeLength) {
        // irsend->sendRaw(customCode, customCodes, codeLength, 38); // Adjust the modulation frequency as needed (38 kHz in this case)
        led.flash(); // You can remove this line if not needed
    }

	uint64_t tvCodes[12] = {
		0x20DF10EF,  // TV Model 1
		0x20DF609F,  // TV Model 2
		0x20DF40BF,  // Samsung UN55NU7100
		0x20DF30CF,  // LG 55UH8509
		0x20DF50AF,  // Sony XBR65X850C
		0x20DF708F,  // Vizio E55-C2
		0x20DF00FF,  // TCL 55P715
		0x20DF906F,  // Hisense 55H8C
		0x20DF807F,  // Panasonic TX65FXW784
		0x20DFD02F,  // Toshiba 55LF621U19
		0x20DFC03F,  // Sharp LC-55N8000U
		0x20DFB04F,  // Insignia NS-55DF710NA19
	};

	int tvCodesSize = sizeof(tvCodes) / sizeof(tvCodes[0]);
};
