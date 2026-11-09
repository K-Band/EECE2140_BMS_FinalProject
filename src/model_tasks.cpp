#include <iostream>
#include <limits>
#include <string>
#include "tasks.h"
#include "simulator.h"


using namespace std;

//Helper function: asks for a number until user enters valid number between minVal and maxVal
static double readNumber(const string& prompt, double minVal, double maxVal) {
    double value;
    while (true) {
        cout << prompt;
        if (cin >> value && value >= minVal && value <= maxVal) return value;
        cout << "Please enter a number from " << minVal << " to " << maxVal << ".\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

//Sets cell count, capacity, resistance, and starting SOC of the pack
void configurePack(Simulator& sim)
{
    std::cout << "\n--- Configure Pack ---\n"
              << "(LG HG2 defaults: 4 cells, 3.0 Ah, 20 mOhm, 100% SOC)\n";

    int numCells      = static_cast<int>(readNumber("Cells in series (1-20): ", 1, 20));
    double capacityAh = readNumber("Cell capacity in Ah (0.5-10): ", 0.5, 10.0);
    double resistance = readNumber("Cell resistance in mOhm (1-500): ", 1, 500) / 1000.0;
    double startSOC   = readNumber("Starting SOC in % (0-100): ", 0, 100) / 100.0;

    sim.getPack().configure(numCells, capacityAh, resistance, startSOC);

    cout << "\nPack configured:\n";
    sim.getPack().printCellStatus();
}

//Shows voltage, SOC, and temperature of every cell
void showThermal(Simulator& sim)
{
    std::cout << "\n--- Thermal State ---\n";
    sim.getPack().printCellStatus();
}