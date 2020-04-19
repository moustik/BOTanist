#include "journal.h"

#include <string>
#include <regex>

#include <nlohmann/json.hpp>

#include "telegram_helpers.h"
#include "web.h"
#include "log.h"

#include "storage.h"

/** Return a list of word with given POS-tag
 * @param word_tags std::vector<nlohmann::json>
 * @param pos_tag VERB NOUN ADV ...
 */
std::vector<std::string> get_word_of_type(std::vector<nlohmann::json> word_tags, std::string pos_tag)
{
    std::vector<nlohmann::json> tagged_type;
    std::vector<std::string> typed_words;
    std::copy_if(word_tags.begin(), word_tags.end(), std::back_inserter(tagged_type),
                 [pos_tag](auto tag){return tag.value("tag", "").substr(0, tag.value("tag", "").find('_')) == pos_tag; });
    std::transform(tagged_type.begin(), tagged_type.end(), std::back_inserter(typed_words), [](auto tag){ return tag.value("text", "");});

    return typed_words;
}

log_type spacyapi_extract(std::string input_log)
{
    nlohmann::json spacy_request = {
      {"model", "fr"},
      {"collapse_phrases", 0},
      {"text", input_log}
    };
    auto spacy_response = nlohmann::json::parse(curl_get("10.8.0.1:8000/dep", spacy_request.dump()));

    auto word_tags = spacy_response.at("words");
    for( auto pos: {"VERB", "NOUN", "ADV"})
    {
        LOG(debug) << pos;
        for( auto word: get_word_of_type(word_tags, pos) )
        {
            LOG(debug) << "\t" <<word;
        }
    }

    log_type description;
    description[action] = get_word_of_type(word_tags, "VERB").front();
    description[plant] = get_word_of_type(word_tags, "NOUN").back();

    return description;
}


void handleLog(TgBot::Message::Ptr message, TgBot::Bot &bot){
    if(message->text.length() < std::string("log").length() + 2)
        return;
    std::string input_log = message->text.substr(std::string("log").length() + 2);

    std::string response;
    log_type extract = spacyapi_extract(input_log);

    response = _format("(action: {}, plant: {})", extract.at(action), extract.at(plant));

    // escape for Markdown
    for(auto escape: {'_', '*', '[', ']', '(', ')', '~', '`', '>', '#', '+', '-', '=', '|', '{', '}', '.', '!'}){
        response = std::regex_replace(response, std::regex(_format("[{}]", escape)), _format("\\{}", escape));
    }

    auto keyboard = createChoices({
        //{{"Ce n'était pas *aujourd'hui*", "date_today"}},
        {{_format("Je n'ai pas *{}*", extract[action]), _format("fix_action_{}_{}", extract[action], message->messageId) }},
        {{_format("Ce ne sont pas des *{}*", extract[plant]), _format("fix_plant_{}_{}", extract[plant], message->messageId) }},
        {{"C'est tout bon !", _format("valid_{}", message->messageId) }}
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
