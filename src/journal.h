#include "tgbot/tgbot.h"

void handleLog(TgBot::Message::Ptr message, TgBot::Bot &bot);
void logCallbackQuery(TgBot::CallbackQuery::Ptr query, TgBot::Bot &bot);
void handleViewLog(TgBot::Message::Ptr message, TgBot::Bot &bot);

void handleCheck(TgBot::Message::Ptr message, TgBot::Bot &bot);
void checkCallbackQuery(TgBot::CallbackQuery::Ptr query, TgBot::Bot &bot);

void dump_log_data(std::string filename);
