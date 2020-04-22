#include <cstdlib>
#include <string>
#include <vector>
#include <map>

enum tag_type { plant, action, value, unit, misc };

extern std::vector<std::string> plant_list;
extern std::vector<std::string> action_list;

typedef std::map<tag_type, std::string> log_type;
extern std::vector<log_type> logs;
