#ifndef LOAD_PROFILE_H
#define LOAD_PROFILE_H

#include <string>
#include <vector>

// Owner: Anthony
enum class ProfileType { CONSTANT, PULSED, CSV };

// Discharge current as a function of time. Currents are positive (discharge).
class LoadProfile {
public:
    LoadProfile();

    void setConstant(double current);
    void setPulsed(double highCurrent, double lowCurrent, double period);
    bool loadCSV(const std::string& filename);   // returns false if the file is invalid
    double getCurrent(double time) const;
    ProfileType getType() const;

private:
    ProfileType type;
    double constantCurrent;
    double pulseHigh;
    double pulseLow;
    double pulsePeriod;          // s, full period (half high, half low)
    std::vector<double> csvTimes;
    std::vector<double> csvCurrents;
};

#endif