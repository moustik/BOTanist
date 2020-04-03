#include <cstdlib>
#include <string>

enum tag_type { plant, action, value, unit, misc };

std::vector<std::string> plant_list {"carotte", "banane"};
std::vector<std::string> action_list {"planter", "arroser", "ramasser"};

std::vector<std::map<tag_type, std::string>> logs {
    {{plant, "banane"}, {action, "arroser"}},
    {{plant, "carotte"}, {action, "planter"}},
    {{plant, "carotte"}, {action, "arroser"}},
};

