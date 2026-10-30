#ifndef EVENT_LOG_H
#define EVENT_LOG_H

#include <string>
#include <vector>

// Owner: Anthony
struct Event {
    double time;          // simulation time (s)
    std::string message;
};

// Timestamped record of warnings, faults, and charge/discharge events.
class EventLog {
public:
    void add(double time, const std::string& message);
    void print() const;
    void clear();
    int size() const;

private:
    std::vector<Event> events;
};

#endif