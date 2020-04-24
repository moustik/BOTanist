#include "storage.h"

std::vector<log_type> logs = {
    {{plant, "carotte"}, {action, "planter"}},
    {{plant, "banane"}, {action, "arroser"}},
    {{plant, "carotte"}, {action, "arroser"}}
};

std::vector<std::string> tag_type_str = { "plant", "action", "value", "unit", "misc" };

std::vector<std::string> plant_list = std::vector<std::string>({"carotte", "banane"});
std::vector<std::string> action_list = std::vector<std::string>({"planter", "arroser", "ramasser"});

std::vector<raw_datum> raw_data;
std::map<int, raw_datum> raw_data_index;
