#include <iostream>
#include <limits>
#include "simulator.h"
#include "tasks.h"

void printMenu() {
    std::cout << "\n===== BMS SIMULATOR =====\n"
              << " 1. Configure pack\n"
              << " 2. Show thermal state\n"
              << " 3. Run charge cycle\n"
              << " 4. Run discharge (CC/pulsed/CSV)\n"
              << " 5. Show SOC estimate vs. true\n"
              << " 6. Show energy report\n"
              << " 7. Set protection thresholds\n"
              << " 8. View event log\n"
              << " 9. Toggle CSV logging\n"
              << "10. Run demo scenario\n"
              << " 0. Exit\n"
              << "Select an option: ";
}

int main() {
    Simulator sim;
    int choice = -1;

    while (choice != 0) {
        printMenu();
        if (!(std::cin >> choice)) {
            if (std::cin.eof()) break;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Please enter a number.\n";
            choice = -1;
            continue;
        }

        switch (choice) {
            case 1:  configurePack(sim);  break;
            case 2:  showThermal(sim);    break;
            case 3:  chargeCycle(sim);    break;
            case 4:  dischargeCycle(sim); break;
            case 5:  showSOC(sim);        break;
            case 6:  reportEnergy(sim);   break;
            case 7:  setThresholds(sim);  break;
            case 8:  viewEvents(sim);     break;
            case 9:  toggleLog(sim);      break;
            case 10: runScenario(sim);    break;
            case 0:  exitProgram(sim);    break;
            default: std::cout << "Invalid option. Choose 0-10.\n";
        }
    }
    return 0;
}
