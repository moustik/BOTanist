#include "journal.h"

#include <string>
#include <regex>

#include <nlohmann/json.hpp>

#include "telegram_helpers.h"
#include "web.h"
#include "log.h"


void handleLog(TgBot::Message::Ptr message, TgBot::Bot &bot){
    if(message->text.length() < std::string("log").length() + 2)
        return;
    std::string input_log = message->text.substr(std::string("log").length() + 2);

    nlohmann::json spacy_request = {
      {"model", "fr"},
      {"collapse_phrases", 0},
      {"text", input_log}
    };
    auto spacy_response = nlohmann::json::parse(curl_get("10.8.0.1:8000/dep", spacy_request.dump()));

    std::string response = "";
    for(auto tag: spacy_response.at("words")){
        response.append(_format(" {} , {} \n", tag.value("text", ""), tag.value("tag", "")));
    }

    // escape for Markdown
    for(auto escape: {'_', '*', '[', ']', '(', ')', '~', '`', '>', '#', '+', '-', '=', '|', '{', '}', '.', '!'}){
        response = std::regex_replace(response, std::regex(_format("[{}]", escape)), _format("\\{}", escape));
    }

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
