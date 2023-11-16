#ifndef LOGGER_H
#define LOGGER_H

class Logger {
public:
    Logger() {
        Serial.begin(115200);
    }

    enum LogLevel {
        INFO,
        WARNING,
        ERROR
    };

    void log(LogLevel logLevel, String message) {
        String logMessage = getLogTypeString(logLevel) + " | " + message;
        Serial.println(String(getLogCount()) + " | " + logMessage);
        
        if (shouldDisplayLog) {
            int textColor = getLogTextColor(logLevel);
            displayLog(logMessage, textColor);
        }
    }

    void setShouldDisplayLog(bool shouldDisplayLog) {
        this->shouldDisplayLog = shouldDisplayLog;
        if (!shouldDisplayLog)
            this->clearLog();
    }

private:
    bool shouldDisplayLog = false;
    int logCount = 0;

	String getTime() {// TODO: Use RTC
		return "12:34:56";
	}

	int getLogCount() {
		return ++this->logCount;
	}

    String getLogTypeString(LogLevel logLevel) {
        switch (logLevel) {
            case INFO:
                return "INFO";
            case WARNING:
                return "WARNING";
            case ERROR:
                return "ERROR";
            default:
                return "INFO"; // Default to INFO
        }
    }

    int getLogTextColor(LogLevel logLevel) {
        switch (logLevel) {
            case INFO:
                return 0x07FF; // Set the text color for INFO
            case WARNING:
                return 0xFFE0; // Set the text color for WARNING
            case ERROR:
                return 0xF800; // Set the text color for ERROR
            default:
                return 0xFFFF; // Default text color
        }
    }

    void displayLog(const String& logMessage, int textColor) {
        clearLog();
        M5.Lcd.setCursor(0, SCREEN_HEIGHT - 20);
        M5.Lcd.setTextSize(1);
        // M5.Lcd.setCursor(10, M5.Lcd.getCursorY() + 10);
        M5.Lcd.setTextColor(textColor, BLACK);
        M5.Lcd.println(logMessage);
        M5.Lcd.setTextColor(WHITE);
    }

    void clearLog() {
        M5.Lcd.fillRect(0, SCREEN_HEIGHT - 20, SCREEN_WIDTH, 20, BLACK);
    }
};

#endif