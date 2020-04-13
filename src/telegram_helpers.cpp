#include "telegram_helpers.h"

TgBot::InlineKeyboardMarkup::Ptr createChoices(
        std::initializer_list<std::initializer_list<std::pair<std::string, std::string>>> choices
                                        ){
    TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
    for (const auto &row: choices) {
        std::vector<TgBot::InlineKeyboardButton::Ptr> row0;

        for (const auto &choice: row) {
          TgBot::InlineKeyboardButton::Ptr checkButton(new TgBot::InlineKeyboardButton);
          checkButton->text = choice.first;
          checkButton->callbackData = choice.second;
          row0.push_back(checkButton);
        }
        keyboard->inlineKeyboard.push_back(row0);
    }
    return keyboard;
}
