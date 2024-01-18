#include "globals.h"

extern Logger logger;
extern Notify notify;

class IrBlaster
{
public:
	IrBlaster() {}

	bool init()
	{
		try
		{
			irsend = new IRsend(kIrSendPin);
			irsend->begin();
			digitalWrite(kIrSendPin, HIGH); // Turn off the light as its turned on by default
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	bool sendAllPowerCodes() {
		send_codes = !send_codes;

		logger.log(Logger::INFO, send_codes ? "Starting send ir code loop" : "Stopping sending ir code loop");
		code_index = 0;

		logger.setShouldDisplayLog(send_codes);
		return send_codes;
	}

	void sendAllPowerCodesRender() {
		SCREEN.setTextColor(WHITE, BLACK);

		int barWidth = 200;
		int progress = map(code_index, 0, powerCodesCount, 0, barWidth);

		SCREEN.drawRect(20, 80, barWidth, 20, WHITE);

		SCREEN.fillRect(20, 80, progress, 20, WHITE);

		SCREEN.setTextSize(2);
		SCREEN.setCursor(20, 50);
		SCREEN.print("Sending: (" + String(code_index) + "/" + String(powerCodesCount) + ")");
	}

	void loop() {
		if (send_codes && (i_last_update < millis())) {
			powerCode = powerCodes[code_index];

			if (code_index >= powerCodesCount) {
				logger.log(Logger::INFO, "Finished sending (" + String(code_index) + "/" + String(powerCodesCount) + ") codes.");
				code_index = 0;
				send_codes = false;
				logger.setShouldDisplayLog(false);
				return;
			}

			const uint8_t freq = powerCode->timer_val;
			const uint8_t numpairs = powerCode->numpairs;
			const uint8_t bitcompression = powerCode->bitcompression;
			code_ptr = 0;

			for (uint8_t k = 0; k < numpairs; k++)
			{
				uint16_t ti = (read_bits(bitcompression)) * 2;
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
			digitalWrite(kIrSendPin, HIGH); // Needed to turn off the light, after each code is sent

			bitsleft_r = 0;
			logger.log(Logger::INFO, "Sending code: (" + String(code_index) + "/" + String(powerCodesCount) + ") freq: " + String(freq) + ", pair: " + String(ontime) + ", " + String(offtime));
			code_index++;
		}

		if (i_last_update < millis())
			i_last_update = millis() + 20;
	}
public:
	bool send_codes = false;
private:
	int irLed;
	IRsend *irsend;
	uint16_t kIrSendPin = 9; // IR Emitter Pin - M5 IR Unit
	uint16_t ontime, offtime;
	int code_index = 0;
	int i_last_update = 0;
	uint16_t rawData[300];

	/**
	 * @brief Variables used for reading the IR code
	 */
	const IrCode *powerCode;
	uint8_t bitsleft_r = 0;
	uint8_t bits_r = 0;
	uint8_t code_ptr;
	uint8_t read_bits(uint8_t count)
	{
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
