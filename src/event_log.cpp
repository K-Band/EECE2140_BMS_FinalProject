#include "event_log.h"
#include <iostream>

void EventLog::add(double time, const std::string& message) {
    events.push_back({time, message});
}

void EventLog::print() const {
    // TODO (Anthony): format timestamps
    if (events.empty()) {
        std::cout << "Event log is empty.\n";
        return;
    }
    for (const Event& e : events) std::cout << e.time << " s: " << e.message << "\n";
}

void EventLog::clear() { events.clear(); }
int EventLog::size() const { return static_cast<int>(events.size()); }
