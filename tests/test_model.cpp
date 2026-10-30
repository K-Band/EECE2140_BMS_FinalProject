// Owner: Kavi — tests for Cell, Pack, OcvTable, ThermalModel
#include <cassert>
#include <cmath>
#include <iostream>
#include "pack.h"

int main() {
    Pack pack;
    pack.configure(4, 3.0, 0.02, 1.0);
    assert(pack.getNumCells() == 4);

    OcvTable table;
    assert(std::fabs(table.lookup(1.0) - table.getMaxVoltage()) < 1e-9);
    assert(std::fabs(table.lookup(0.0) - table.getMinVoltage()) < 1e-9);

    // TODO: OCV interpolation between table points
    // TODO: steady-state temperature settles at T_amb + I^2 R / k
    // TODO: SOC drops by I * dt / (3600 * capacity) after one step

    std::cout << "test_model: all tests passed\n";
    return 0;
}