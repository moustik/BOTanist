#include "journal.h"

#include <string>
#include "telegram_helpers.h"

void handleLog(TgBot::Message::Ptr message, TgBot::Bot &bot){
    std::string response = "__Aujourd'hui__ j'ai __planter__ des __carottes__";
    auto keyboard = createChoices({{{"Ce n'était pas *aujourd'hui*", "date_today"}},
                                   {{"Je n'ai pas *planter*", "action_planter"}},
                                   {{"Ce ne sont pas des *carottes*", "item_carottes"}},
                                   {{"C'est tout bon !", "valid"}}
      });
    bot.getApi().sendMessage(message->chat->id, response, false, 0, keyboard, "MarkdownV2");
}

void handleCheck(TgBot::Message::Ptr message, TgBot::Bot &bot){
    TgBot::InlineKeyboardMarkup::Ptr keyboard = createChoices({{ {"check", "check"}, {"new plant", "new_plant"}, {"log", "log"} }});
    std::string response = "ok";
    bot.getApi().sendMessage(message->chat->id, response, false, 0, keyboard, "Markdown");
}

void checkCallbackQuery(TgBot::CallbackQuery::Ptr query, TgBot::Bot &bot){
    bot.getApi().editMessageText("YES!", query->message->chat->id, query->message->messageId, "",
                                 "Markdown", false,
                                 createChoices({{ {"checked", "check"}, {"new plant", "new_plant"}, {"log", "log"} }}));
}
