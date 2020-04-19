#include <cstdlib>
#include <string>
#include <vector>
#include <map>

enum tag_type { plant, action, value, unit, misc };

std::vector<std::string> plant_list {"carotte", "banane"};
std::vector<std::string> action_list {"planter", "arroser", "ramasser"};

typedef std::map<tag_type, std::string> log_type;
std::vector<log_type> logs {
    {{plant, "banane"}, {action, "arroser"}},
    {{plant, "carotte"}, {action, "planter"}},
    {{plant, "carotte"}, {action, "arroser"}},
};
