#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <ctime>
using namespace std;

// --------------------
// 1. Log Level
// --------------------
enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL
};

string levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG:   return "DEBUG";
        case LogLevel::INFO:    return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR:   return "ERROR";
        case LogLevel::FATAL:   return "FATAL";
    }

    return "";
}

// --------------------
// 2. Log Message
// --------------------
class LogMessage {
public:
    LogLevel level;
    string message;
    time_t timestamp;

    LogMessage(LogLevel level, string message) {
        this->level = level;
        this->message = message;
        this->timestamp = time(nullptr);
    }
};

// --------------------
// 3. Formatter
// --------------------
class Formatter {
public:
    string format(LogMessage log) {

        string timeStr = ctime(&log.timestamp);

        // Remove newline from ctime()
        if (!timeStr.empty() && timeStr.back() == '\n')
            timeStr.pop_back();

        return "[" + timeStr + "] [" +
               levelToString(log.level) + "] " +
               log.message;
    }
};

// --------------------
// 4. Handler
// --------------------
class LogHandler {
protected:
    LogLevel minLevel;
    Formatter formatter;

public:
    LogHandler(LogLevel minLevel) {
        this->minLevel = minLevel;
    }

    virtual void log(LogMessage message) = 0;

    bool shouldHandle(LogLevel level) {
        return (int)level >= (int)minLevel;
    }

    virtual ~LogHandler() {}
};

// --------------------
// 5. Console Handler
// --------------------
class ConsoleHandler : public LogHandler {
public:
    ConsoleHandler(LogLevel minLevel)
        : LogHandler(minLevel) {}

    void log(LogMessage message) override {

        if (!shouldHandle(message.level))
            return;

        cout << formatter.format(message) << endl;
    }
};

// --------------------
// 6. File Handler
// --------------------
class FileHandler : public LogHandler {
private:
    ofstream file;

public:
    FileHandler(LogLevel minLevel, string filename)
        : LogHandler(minLevel) {

        file.open(filename, ios::app);
    }

    void log(LogMessage message) override {

        if (!shouldHandle(message.level))
            return;

        file << formatter.format(message) << endl;
    }

    ~FileHandler() {
        file.close();
    }
};

// --------------------
// 7. Logger
// --------------------
class Logger {
private:
    vector<LogHandler*> handlers;

public:

    void addHandler(LogHandler* handler) {
        handlers.push_back(handler);
    }

    void log(LogLevel level, string message) {

        LogMessage logMessage(level, message);

        for (auto handler : handlers) {
            handler->log(logMessage);
        }
    }

    void debug(string message) {
        log(LogLevel::DEBUG, message);
    }

    void info(string message) {
        log(LogLevel::INFO, message);
    }

    void warning(string message) {
        log(LogLevel::WARNING, message);
    }

    void error(string message) {
        log(LogLevel::ERROR, message);
    }

    void fatal(string message) {
        log(LogLevel::FATAL, message);
    }
};

// --------------------
// 8. Main
// --------------------
int main() {

    Logger logger;

    ConsoleHandler console(LogLevel::DEBUG);

    FileHandler file(LogLevel::ERROR, "app.log");

    logger.addHandler(&console);
    logger.addHandler(&file);

    logger.debug("Application started");

    logger.info("User logged in");

    logger.warning("Memory usage is high");

    logger.error("Database connection failed");

    logger.fatal("Application crashed");

    return 0;
}

// Your Output
// [Thu Sep 10 15:07:05 2026] [DEBUG] Application started
// [Thu Sep 10 15:07:05 2026] [INFO] User logged in
// [Thu Sep 10 15:07:05 2026] [WARNING] Memory usage is high
// [Thu Sep 10 15:07:05 2026] [ERROR] Database connection failed
// [Thu Sep 10 15:07:05 2026] [FATAL] Application crashed

