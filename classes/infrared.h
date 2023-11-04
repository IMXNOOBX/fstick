#include <M5StickCPlus.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

class InfraredSender {
public:
    InfraredSender() {
        irsend.begin(IRLED);
        endingEarly = false;
    }

    void sendAllCodes() {
        for (uint16_t i = 0; i < powerCodesCount; i++) {
            powerCode = region == powerCodes[i];
            const uint8_t numpairs = powerCode->numpairs;

            for (uint8_t k = 0; k < numpairs; k++) {
                uint16_t ti = (read_bits(powerCode->bitcompression)) * 2;
                uint16_t ontime, offtime;

                #if defined(PLUS)
                    offtime = powerCode->times[ti];  // read word 1 - ontime
                    ontime = powerCode->times[ti + 1]; // read word 2 - offtime
                #else
                    ontime = powerCode->times[ti];  // read word 1 - ontime
                    offtime = powerCode->times[ti + 1]; // read word 2 - offtime
                #endif

                rawData[k * 2] = offtime * 10;
                rawData[(k * 2) + 1] = ontime * 10;
                yield();
            }

            irsend.sendRaw(rawData, (numpairs * 2), freq_to_timerval(powerCode->timer_val));
            yield();

            bitsleft_r = 0;
            delay_ten_us(20500);

            if (M5.Axp.GetBtnPress()) {
                endingEarly = true;
                current_proc = 1;
                isSwitching = true;
                rstOverride = false;
                break;
            }

            if (digitalRead(TRIGGER) == BUTTON_PRESSED) {
                while (digitalRead(TRIGGER) == BUTTON_PRESSED) {
                    yield();
                }
                endingEarly = true;
                quickflashLEDx(4);
                break;
            }
        }

        if (endingEarly == false) {
            delay_ten_us(MAX_WAIT_TIME);
            delay_ten_us(MAX_WAIT_TIME);
            quickflashLEDx(8);
        }
    }

private:
    IRsend irsend;
    const uint16_t MAX_WAIT_TIME = 65535;
    const uint16_t BUTTON_PRESSED = 0;
    uint8_t bitsleft_r = 0;
    uint8_t bits_r = 0;
    uint8_t code_ptr;
    volatile const IrCode *powerCode;
    uint8_t num_codes;
    uint8_t region;
    bool endingEarly;

    // Methods
    void delay_ten_us(uint16_t us) {
        uint8_t timer;
        while (us != 0) {
            for (timer = 0; timer <= DELAY_CNT; timer++) {
                NOP;
                NOP;
            }
            NOP;
            us--;
        }
    }

    void quickflashLED( void ) {
        digitalWrite(LED, LOW);
        delay_ten_us(3000);   // 30 ms ON-time delay
        digitalWrite(LED, HIGH);
    }

    void quickflashLEDx( uint8_t x ) {
        quickflashLED();
        while (--x) {
            delay_ten_us(25000);     // 250 ms OFF-time delay between flashes
            quickflashLED();
        }
    }

    uint8_t read_bits(uint8_t count) {
        uint8_t i;
        uint8_t tmp = 0;
        for (i = 0; i < count; i++) {
            if (bitsleft_r == 0) {
                bits_r = powerCode->codes[code_ptr++];
                bitsleft_r = 8;
            }
            bitsleft_r--;
            tmp |= (((bits_r >> bitsleft_r) & 1) << (count - 1 - i));
        }
        return tmp;
    }

    // Rest of the variables and constants remain the same

    // Define other variables and functions as needed
};
