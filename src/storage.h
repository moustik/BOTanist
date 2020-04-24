#include <cstdlib>
#include <string>
#include <vector>
#include <map>

enum tag_type { plant, action, value, unit, misc };
extern std::vector<std::string> tag_type_str;

extern std::vector<std::string> plant_list;
extern std::vector<std::string> action_list;

typedef std::map<tag_type, std::string> log_type;
extern std::vector<log_type> logs;

typedef std::tuple<int, std::string, log_type> raw_datum;
extern std::vector<raw_datum> raw_data;
extern std::map<int, raw_datum> raw_data_index;
