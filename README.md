BMS Simulator
A text-based C++ simulation of a lithium-ion battery management system (BMS). The reference cell is a LG 18650HG2. The program models a series battery pack and tracks charging, energy use, state of charge (SOC), temperature, and warning/fault conditions through a menu-driven console interface.

EECE 2140 Final Project, Group 07: Drew Bancroft, Kavi Bandara, Anthony Wang

Building and Running

Requirements: `g++` with C++17 support and `make`.
```bash
make          # builds the ./bms executable
./bms         # runs the simulator
make test     # builds and runs all tests in tests/
make clean    # removes build files
```

Menu
Option	Task	Description	Owner
1	T1	Configure pack (cell count, capacity, resistance, starting SOC)	Kavi
2	T2	Show thermal state of each cell	Kavi
3	T3	Run CC-CV charge cycle	Drew
4	T4	Run discharge (constant, pulsed, or CSV load)	Anthony
5	T5	Show SOC estimate vs. true SOC	Drew
6	T6	Show energy report (Wh in/out, efficiency, cycles)	Drew
7	T7	Set protection thresholds and reset faults	Drew
8	T8	View event log	Anthony
9	T9	Toggle CSV logging	Anthony
10	T10	Run demo scenario	Anthony
0	T0	Exit	Anthony
Project Structure
```
bms/
├── Makefile
├── README.md
├── include/            class headers + tasks.h
├── src/
│   ├── main.cpp            menu loop (switch statement)
│   ├── model_tasks.cpp     T1–T2 (Kavi)
│   ├── control_tasks.cpp   T3, T5–T7 (Drew)
│   ├── sim_tasks.cpp       T4, T8–T10, T0 (Anthony)
│   └── <class>.cpp         one source file per class
├── tests/              assert-based tests, one file per owner
├── data/               input files (sample_load.csv)
└── docs/               class diagram, iteration reports, Excel plan
```
