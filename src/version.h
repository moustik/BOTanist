#include <string>

namespace botanist
{
  struct version
  {
    static const std::string GIT_SHA1;
    static const std::string GIT_DATE;
    static const std::string GIT_COMMIT_SUBJECT;
  };
}
