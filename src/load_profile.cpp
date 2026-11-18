#include "load_profile.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

using namespace std;


// Default load profile
LoadProfile::LoadProfile()
    : type(ProfileType::CONSTANT),
      constantCurrent(3.0),
      pulseHigh(6.0),
      pulseLow(1.0),
      pulsePeriod(20.0)
{
}


// Set constant discharge current
void LoadProfile::setConstant(double current)
{
    type = ProfileType::CONSTANT;
    constantCurrent = current;
}


// Set pulsed discharge current
void LoadProfile::setPulsed(double highCurrent, double lowCurrent, double period)
{
    type = ProfileType::PULSED;
    pulseHigh = highCurrent;
    pulseLow = lowCurrent;
    pulsePeriod = period;
}


// Load current values from CSV file
bool LoadProfile::loadCSV(const string& filename)
{
    ifstream file(filename);

    if (!file.is_open())
    {
        cout << "Unable to open CSV file." << endl;
        return false;
    }

    vector<double> times;
    vector<double> currents;
    string line;

    while (getline(file, line))
    {
        if (line.empty())
        {
            continue;
        }

        stringstream ss(line);
        string timeText, currentText;

        getline(ss, timeText, ',');
        getline(ss, currentText, ',');

        double t, current;

        stringstream timeStream(timeText);
        stringstream currentStream(currentText);

        if (!(timeStream >> t) || !(currentStream >> current))
        {
            // Skip CSV column header
            if (timeText == "time")
            {
                continue;
            }

            cout << "Invalid CSV data." << endl;
            return false;
        }

        if (!isfinite(t) || !isfinite(current) ||
            t < 0 || current < 0)
        {
            cout << "Invalid CSV values." << endl;
            return false;
        }

        if (!times.empty() && t <= times.back())
        {
            cout << "CSV times must be increasing." << endl;
            return false;
        }

        times.push_back(t);
        currents.push_back(current);
    }

    file.close();

    if (times.empty() || times[0] != 0)
    {
        cout << "CSV must start at time 0." << endl;
        return false;
    }

    csvTimes = times;
    csvCurrents = currents;

    type = ProfileType::CSV;

    cout << "CSV load profile loaded." << endl;
    return true;
}


// Get current based on selected profile
double LoadProfile::getCurrent(double time) const
{
    if (type == ProfileType::CONSTANT)
    {
        return constantCurrent;
    }

    else if (type == ProfileType::PULSED)
    {
        if (pulsePeriod <= 0)
        {
            return 0;
        }

        double phase = fmod(time, pulsePeriod);

        if (phase < pulsePeriod / 2.0)
        {
            return pulseHigh;
        }
        else
        {
            return pulseLow;
        }
    }

    else if (type == ProfileType::CSV)
    {
        if (csvTimes.empty())
        {
            return 0;
        }

        if (time < csvTimes[0] || time > csvTimes.back())
        {
            return 0;
        }

        for (int i = static_cast<int>(csvTimes.size()) - 1; i >= 0; i--)
        {
            if (time >= csvTimes[i])
            {
                return csvCurrents[i];
            }
        }
    }

    return 0;
}


// Return the selected profile type
ProfileType LoadProfile::getType() const
{
    return type;
}
