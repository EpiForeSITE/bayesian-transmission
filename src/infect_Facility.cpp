#include "infect/infect.h"

namespace infect {

Facility::~Facility()
{
    for (auto& pair : unit) {
        delete pair.second;
    }
}

void Facility::write(ostream &os) const
{
    os << "Facility ID: " << number << "\nUnits:\n";
    for (const auto& pair : unit) {
        os << "\t" << pair.first << " -> " << pair.second << "\n";
    }
}

} // namespace infect
