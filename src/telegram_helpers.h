#ifndef BOTA_TELEGRAM_HELPERS_H
#define BOTA_TELEGRAM_HELPERS_H

#include <vector>

#include "tgbot/tgbot.h"

TgBot::InlineKeyboardMarkup::Ptr createChoices(
        std::initializer_list<std::initializer_list<std::pair<std::string, std::string>>> choices
                                               );
#endif
