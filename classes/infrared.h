#include "globals.h"

extern Logger l;
extern Led led;

class IrBlaster
{
public:
	IrBlaster() {}

	bool init()
	{
		try
		{
			irsend = new IRsend(kIrSendPin);
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	void sendAllPowerCodes()
	{
		l.setShouldDisplayLog(true); // Set log output to screen

		int i = 0;
		const IrCode *powerCode;
		while ((powerCode = powerCodes[i]) != nullptr) {
			i++;
			uint8_t freq = powerCode->timer_val;
			uint8_t numpairs = powerCode->numpairs;
			uint8_t bitcompression = powerCode->bitcompression;
			uint16_t rawData[300];

			for (uint8_t k = 0; k < numpairs; k++)
			{
				uint16_t ti = (read_bits(bitcompression, powerCode)) * 2;
				#if defined(PLUS)
					offtime = powerCode->times[ti];	   // read word 1 - ontime
					ontime = powerCode->times[ti + 1]; // read word 2 - offtime
				#else
					ontime = powerCode->times[ti];		// read word 1 - ontime
					offtime = powerCode->times[ti + 1]; // read word 2 - offtime
				#endif
				rawData[k * 2] = offtime * 10;
				rawData[(k * 2) + 1] = ontime * 10;
				yield();
			}

			irsend->sendRaw(rawData, (numpairs * 2), freq);
			digitalWrite(kIrSendPin, HIGH); // Seems to be needed to turn off the light

			l.log(Logger::INFO, "Sending code: (" + String(i) + ") freq: " + String(freq) + ", pair: " + String(ontime) + ", " + String(offtime));

			delay(20);
			led.flash(); // Here just in case it conflicts with the ir message
		}

		l.log(Logger::INFO, "Finished sending " + String(powerCodesCount) + " codes.");

		l.setShouldDisplayLog(false);
	}

	void loop()
	{
	}

private:
	int irLed;
	IRsend* irsend;
	uint16_t kIrSendPin = 9; // IR Emitter Pin - M5 IR Unit
	uint16_t ontime, offtime;

	uint8_t bitsleft_r = 0;
	uint8_t bits_r = 0;
	uint8_t code_ptr;
	uint8_t read_bits(uint8_t count, const IrCode *powerCode) {
		uint8_t i;
		uint8_t tmp = 0;

		for (i = 0; i < count; i++)
		{
			if (bitsleft_r == 0)
			{
				bits_r = powerCode->codes[code_ptr++];
				bitsleft_r = 8;
			}
			bitsleft_r--;
			tmp |= (((bits_r >> (bitsleft_r)) & 1) << (count - 1 - i));
		}
		return tmp;
	}
};
