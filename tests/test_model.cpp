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
    
    //OCV TESTS
    // Exact table points return the table value
    assert(std::fabs(table.lookup(0.50) - 3.72) < 1e-9);

    // Halfway between 30% (3.61) and 40% (3.66) is 3.635
    assert(std::fabs(table.lookup(0.35) - 3.635) < 1e-9);

    // Values outside 0-1 clamp to the ends
    assert(std::fabs(table.lookup(-0.2) - 3.00) < 1e-9);
    assert(std::fabs(table.lookup(1.5) - 4.20) < 1e-9);

    // OCV always increases with SOC
    for (double s = 0.0; s < 1.0; s += 0.01)
        assert(table.lookup(s + 0.01) > table.lookup(s));

    //Cell tests

    // SOC: 3 A for 360 s on a 3 Ah cell removes 0.1 (10%)
    Cell cell(3.0, 0.02, 1.0);
    for (int i = 0; i < 360; i++) cell.update(3.0, 1.0);
    assert(std::fabs(cell.getSOC() - 0.9) < 1e-6);

    // Terminal voltage sags below OCV while discharging
    assert(cell.getTerminalVoltage() < cell.getOCV());

    // SOC never goes below 0
    for (int i = 0; i < 10000; i++) cell.update(3.0, 1.0);
    assert(cell.getSOC() == 0.0);

    // Charging (negative current) raises SOC and never goes above 1
    Cell charging(3.0, 0.02, 0.5);
    charging.update(-3.0, 1.0);
    assert(charging.getSOC() > 0.5);
    for (int i = 0; i < 10000; i++) charging.update(-3.0, 1.0);
    assert(charging.getSOC() == 1.0);

    // Terminal voltage rises above OCV while charging
    Cell charging2(3.0, 0.02, 0.5);
    charging2.update(-3.0, 1.0);
    assert(charging2.getTerminalVoltage() > charging2.getOCV());

    // Temperature rises above ambient under load
    Cell hot(3.0, 0.02, 1.0);
    for (int i = 0; i < 600; i++) hot.update(10.0, 1.0);
    assert(hot.getTemperature() > 25.0);

    //ThermalModel steady-state test
    
    // At steady state heating = cooling: I^2 R = k (T - T_amb)
    // T = 25 + (3^2 * 0.02) / 0.03 = 31 deg C
    ThermalModel thermalModel(0.03, 46.0, 25.0);
    double temp = 25.0;
    for (int i = 0; i < 20000; i++) temp = thermalModel.step(temp, 3.0, 0.02, 1.0);
    assert(std::fabs(temp - 31.0) < 0.01);

    //Pack tests
    Pack p;
    p.configure(4, 3.0, 0.02, 0.5);
    assert(p.getNumCells() == 4);

    // At rest, pack voltage = 4 x OCV at 50% = 4 x 3.72 = 14.88 V
    assert(std::fabs(p.getVoltage() - 14.88) < 1e-6);

    // Average SOC starts at 50%
    assert(std::fabs(p.getSOC() - 0.5) < 1e-9);

    // 3 A for 360 s removes 10% from every cell
    for (int i = 0; i < 360; i++) p.applyCurrent(3.0, 1.0);
    assert(std::fabs(p.getSOC() - 0.4) < 1e-6);

    // Identical cells: max = min, and pack voltage = 4 x one cell
    assert(std::fabs(p.getMaxCellVoltage() - p.getMinCellVoltage()) < 1e-9);
    assert(std::fabs(p.getVoltage() - 4.0 * p.getMaxCellVoltage()) < 1e-9);

    // Pack heats up under load
    assert(p.getMaxTemperature() > 25.0);

    // configure() rejects a 0-cell pack
    p.configure(0, 3.0, 0.02, 1.0);
    assert(p.getNumCells() == 1);

    std::cout << "test_model: all tests passed\n";
    return 0;
}