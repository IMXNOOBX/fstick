#include "globals.h"

extern Logger l;
extern Led led;

class IrBlaster {
public:
    IrBlaster(int irLedPin) : irLed(irLedPin) {
        // Initialize the IR sender with the specified IR LED pin
        irsend = new IRsend(irLedPin);
    }

    void turnOnOff() {
        sendIRCode(TV_ON_OFF_POWER_CODE); // Define your TV's power-on IR code
		led.flash(); // TODO: Fix this
		l.log(Logger::INFO, "turnOnOff() called, sending code: " + String(TV_ON_OFF_POWER_CODE));
    }

private:
    IRsend* irsend;
    const uint16_t TV_ON_OFF_POWER_CODE = 0x1234; // Replace with your TV's power IR code
    int irLed;

    void sendIRCode(uint16_t code) {
        irsend->sendNEC(code, 32);
    }
};
