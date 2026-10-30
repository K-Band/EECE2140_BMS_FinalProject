#include "load_profile.h"

LoadProfile::LoadProfile()
    : type(ProfileType::CONSTANT), constantCurrent(3.0),
      pulseHigh(6.0), pulseLow(1.0), pulsePeriod(20.0) {}

void LoadProfile::setConstant(double current) {
    type = ProfileType::CONSTANT;
    constantCurrent = current;
}

void LoadProfile::setPulsed(double highCurrent, double lowCurrent, double period) {
    type = ProfileType::PULSED;
    pulseHigh = highCurrent;
    pulseLow = lowCurrent;
    pulsePeriod = period;
}

bool LoadProfile::loadCSV(const std::string& /*filename*/) {
    // TODO (Anthony): read "time,current" rows into csvTimes/csvCurrents, validate
    return false;
}

double LoadProfile::getCurrent(double /*time*/) const {
    // TODO (Anthony): PULSED and CSV profiles
    if (type == ProfileType::CONSTANT) return constantCurrent;
    return 0.0;
}

ProfileType LoadProfile::getType() const { return type; }
