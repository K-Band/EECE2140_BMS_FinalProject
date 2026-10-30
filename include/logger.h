#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <string>

// Owner: Anthony
// Writes one CSV row per simulation time step when enabled.
class Logger {
public:
    Logger();
    ~Logger();

    bool enable(const std::string& filename);   // opens file and writes header row
    void disable();                              // closes file
    bool isEnabled() const;
    std::string getFileName() const;
    void logRow(double time, double voltage, double current, double soc,
                double estimatedSoc, double temperature, bool faulted);

private:
    std::ofstream file;
    bool enabled;
    std::string fileName;
};

#endif