#include <iostream>
#include "tasks.h"
#include "simulator.h"

using namespace std;

void dischargeCycle(Simulator& sim)
{
    (void)sim; //placeholder so make runs cleanly remove when code implemented
    cout << "Discharge cycle not implemented yet." << endl;
}

void viewEvents(Simulator& sim)
{
    (void)sim; //placeholder so make runs cleanly remove when code implemented
    cout << "View event log not implemented yet." << endl;
}

void toggleLog(Simulator& sim)
{
    (void)sim; //placeholder so make runs cleanly remove when code implemented 
    cout << "Toggle CSV logging not implemented yet." << endl;
}

void runScenario(Simulator& sim)
{
    (void)sim; //placeholder so make runs cleanly remove when code implemented
    cout << "Demo scenario not implemented yet." << endl;
}

void exitProgram(Simulator& sim)
{
    sim.getLogger().disable();
    cout << "Exiting BMS simulator." << endl;
}