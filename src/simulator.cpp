#include "logger.h"

Logger::Logger() : enabled(false), fileName("bms_log.csv") {}

Logger::~Logger() { disable(); }

bool Logger::enable(const std::string& filename) {
    // TODO (Anthony): open file and write header row
    fileName = filename;
    enabled = false;
    return false;
}

void Logger::disable() {
    if (file.is_open()) file.close();
    enabled = false;
}

bool Logger::isEnabled() const { return enabled; }
std::string Logger::getFileName() const { return fileName; }

void Logger::logRow(double /*time*/, double /*voltage*/, double /*current*/, double /*soc*/,
                    double /*estimatedSoc*/, double /*temperature*/, bool /*faulted*/) {
    // TODO (Anthony): write one CSV row if enabled
}
