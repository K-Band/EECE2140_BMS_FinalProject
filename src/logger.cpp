#include "logger.h"

Logger::Logger() : enabled(false), fileName("bms_log.csv") {}

Logger::~Logger()
{
    disable();
}

bool Logger::enable(const std::string& filename)
{
    disable();

    fileName = filename;
    file.open(fileName);

    if (!file.is_open())
    {
        return false;
    }

    file << "Time,Voltage,Current,SOC,EstimatedSOC,Temperature,Faulted\n";

    enabled = true;
    return true;
}

void Logger::disable()
{
    if (file.is_open())
    {
        file.close();
    }

    enabled = false;
}

bool Logger::isEnabled() const
{
    return enabled;
}

std::string Logger::getFileName() const
{
    return fileName;
}

void Logger::logRow(double time, double voltage, double current, double soc,
                    double estimatedSoc, double temperature, bool faulted)
{
    if (!enabled || !file.is_open())
    {
        return;
    }

    file << time << ","
         << voltage << ","
         << current << ","
         << soc << ","
         << estimatedSoc << ","
         << temperature << ","
         << faulted << "\n";
}