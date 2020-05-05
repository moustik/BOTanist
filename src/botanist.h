#include <string>
#include <functional>

#include <tgbot/tgbot.h>

class Botanist : public TgBot::Bot {
  public:
    Botanist(std::string);

    void handleMessage(TgBot::Message::Ptr message, Bot &bot);
    void handleComplexCommand(TgBot::Message::Ptr message, Bot &bot);
    void handleCallbackQueries(TgBot::CallbackQuery::Ptr query, Bot &bot);

    void registerCommand(std::string command, std::function<void(TgBot::Message::Ptr, TgBot::Bot &)> callback);

};
