#include "storage.h"

std::vector<log_type> logs = {
    {{plant, "carotte"}, {action, "planter"}},
    {{plant, "banane"}, {action, "arroser"}},
    {{plant, "carotte"}, {action, "arroser"}}
};

std::vector<std::string> plant_list = std::vector<std::string>({"carotte", "banane"});
std::vector<std::string> action_list = std::vector<std::string>({"planter", "arroser", "ramasser"});
