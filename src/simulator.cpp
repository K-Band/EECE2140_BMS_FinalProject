#include "simulator.h"
#include <iostream>
#include <iomanip>
#include <cmath>

using namespace std;


Simulator::Simulator(double timestep)
    : time(0.0), timestep(timestep), lastCurrent(0.0)
{
}



void Simulator::runCharge()
{
    cout << "\n===== Running Charge Cycle =====" << endl;

    if (protection.isFaulted())
    {
        cout << "Cannot charge. A fault is active." << endl;
        return;
    }

    if (!contactor.isClosed())
    {
        cout << "Cannot charge. Contactor is open." << endl;
        return;
    }

    if (timestep <= 0)
    {
        cout << "Invalid simulation timestep." << endl;
        return;
    }

    double startTime = time;
    double maxTime = 14400.0;
    double nextPrint = time + 60.0;

    eventLog.add(time, "Charge cycle started");

    cout << "\nInitial battery status:" << endl;
    printStatusLine();

    while (time - startTime < maxTime)
    {
        if (protection.isFaulted() || !contactor.isClosed())
        {
            break;
        }

        // Get charging current from CC-CV charger
        double current = charger.getChargeCurrent(pack);

        if (charger.isDone())
        {
            cout << "\nCharging completed." << endl;
            break;
        }

        if (!isfinite(current) || current >= 0)
        {
            cout << "Invalid charging current." << endl;
            break;
        }

        // Update battery and other systems
        step(current);

        // Print status every 60 seconds
        if (time >= nextPrint)
        {
            cout << "\nCharger Mode: "
                 << charger.getModeName() << endl;

            printStatusLine();

            nextPrint = time + 60.0;
        }

        if (protection.isFaulted())
        {
            cout << "\nCharging stopped due to a fault." << endl;
            cout << protection.getFaultReason() << endl;
            break;
        }
    }

    if (time - startTime >= maxTime)
    {
        cout << "Maximum charging time reached." << endl;
    }

    eventLog.add(time, "Charge cycle ended");

    cout << "\n===== Charge Finished =====" << endl;

    cout << "Final charger mode: "
         << charger.getModeName() << endl;

    printStatusLine();
}



// Run discharge cycle
void Simulator::runDischarge()
{
    cout << "\n===== Running Discharge =====" << endl;

    if (protection.isFaulted())
    {
        cout << "Cannot discharge. A fault is active." << endl;
        return;
    }

    if (!contactor.isClosed())
    {
        cout << "Cannot discharge. Contactor is open." << endl;
        return;
    }

    if (timestep <= 0)
    {
        cout << "Invalid simulation timestep." << endl;
        return;
    }

    // Maximum simulation time is 4 hours
    double startTime = time;
    double maxTime = 14400.0;

    eventLog.add(time, "Discharge cycle started");

    while (time - startTime < maxTime)
    {
        if (protection.isFaulted() || !contactor.isClosed())
        {
            break;
        }

       double socLimit = protection.getLimit(Protection::LOW_SOC, Protection::WARNING);
        if (pack.getSOC() <= socLimit)
        {
            cout << "\nDischarge completed." << endl;
            cout << "Battery reached the low-SOC limit (" << socLimit * 100.0 << "%)." << endl;
            break;
        }
        
        double current = loadProfile.getCurrent(time);

        if (!isfinite(current) || current < 0)
        {
            cout << "Invalid discharge current." << endl;
            break;
        }

        if (current == 0)
        {
            cout << "Discharge stopped. Current is 0 A." << endl;
            break;
        }

        step(current);

        if (protection.isFaulted())
        {
            cout << "\nDischarge stopped due to a fault." << endl;
            cout << protection.getFaultReason() << endl;
            break;
        }
    }

    if (time - startTime >= maxTime)
    {
        cout << "Maximum simulation time reached." << endl;
    }

    eventLog.add(time, "Discharge cycle ended");

    cout << "\n===== Discharge Finished =====" << endl;
    printStatusLine();
}


// Update the battery for one time step
void Simulator::step(double current)
{
    if (timestep <= 0 || !isfinite(current))
    {
        cout << "Invalid simulation step." << endl;
        return;
    }

    if (!contactor.isClosed() || protection.isFaulted())
    {
        lastCurrent = 0.0;
        return;
    }

    // Check protection before applying current
    Protection::Level level = protection.check(pack, current);

    if (level == Protection::FAULT)
    {
        contactor.open();
        lastCurrent = 0.0;

        eventLog.add(time, protection.getFaultReason());
        cout << "Fault detected. Contactor opened." << endl;
        return;
    }

    // Update battery
    pack.applyCurrent(current, timestep);

    // Update SOC estimate
    socEstimator.update(current, timestep);

    // Update energy usage
    energyTracker.update(pack.getVoltage(), current, timestep);

    lastCurrent = current;
    time += timestep;

    // Check protection after updating battery
    level = protection.check(pack, current);

    if (level == Protection::FAULT)
    {
        contactor.open();
        eventLog.add(time, protection.getFaultReason());
    }

    // Save data if CSV logging is enabled
    if (logger.isEnabled())
    {
        logger.logRow(time,
                      pack.getVoltage(),
                      lastCurrent,
                      pack.getSOC(),
                      socEstimator.getEstimate(),
                      pack.getMaxTemperature(),
                      protection.isFaulted());
    }
}


// Print battery status
void Simulator::printStatusLine() const
{
    cout << fixed << setprecision(1);

    cout << "Time: " << time << " s" << endl;
    cout << "Voltage: " << pack.getVoltage() << " V" << endl;
    cout << "Current: " << lastCurrent << " A" << endl;
    cout << "SOC: " << pack.getSOC() * 100.0 << "%" << endl;
    cout << "Temperature: " << pack.getMaxTemperature() << " C" << endl;

    cout << "SOC: [";

    int bars = static_cast<int>(pack.getSOC() * 20);

    for (int i = 0; i < 20; i++)
    {
        if (i < bars)
        {
            cout << "#";
        }
        else
        {
            cout << "-";
        }
    }

    cout << "]" << endl;

    if (protection.isFaulted())
    {
        cout << "Status: FAULT" << endl;
    }
    else if (!protection.getActiveWarnings().empty())
    {
        cout << "Status: WARNING" << endl;
    }
    else
    {
        cout << "Status: OK" << endl;
    }
}


void Simulator::resetTime()
{
    time = 0.0;
}

double Simulator::getTime() const
{
    return time;
}

double Simulator::getTimestep() const
{
    return timestep;
}

Pack& Simulator::getPack()
{
    return pack;
}

Charger& Simulator::getCharger()
{
    return charger;
}

SocEstimator& Simulator::getSocEstimator()
{
    return socEstimator;
}

EnergyTracker& Simulator::getEnergyTracker()
{
    return energyTracker;
}

Protection& Simulator::getProtection()
{
    return protection;
}

Contactor& Simulator::getContactor()
{
    return contactor;
}

LoadProfile& Simulator::getLoadProfile()
{
    return loadProfile;
}

Logger& Simulator::getLogger()
{
    return logger;
}

EventLog& Simulator::getEventLog()
{
    return eventLog;
}
