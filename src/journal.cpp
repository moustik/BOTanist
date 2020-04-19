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
std::vector<std::string> get_field_with_property(std::vector<nlohmann::json> word_tags, std::string property, std::string property_name,
                                                std::string return_field)
{
    std::vector<nlohmann::json> tagged_type;
    std::vector<std::string> typed_words;
    std::copy_if(word_tags.begin(), word_tags.end(), std::back_inserter(tagged_type),
                 [property, property_name](auto tag){
                   return tag.value(property_name, "").substr(0, tag.value(property_name, "").find('_')) == property; });
    std::transform(tagged_type.begin(), tagged_type.end(), std::back_inserter(typed_words),
                   [return_field](auto tag){return tag.value(return_field, "");});

    return typed_words;
}

std::vector<std::string> spacyapi_get_word_of_type(std::vector<nlohmann::json> word_tags, std::string tag)
{
    return get_field_with_property(word_tags, tag, "tag", "text");
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

    log_type description;
    description[action] = spacyapi_get_word_of_type(word_tags, "VERB").front();
    description[plant] = spacyapi_get_word_of_type(word_tags, "NOUN").back();

    return description;
}

log_type spacy_server_extract(std::string input_log)
{
    nlohmann::json spacy_request = {
      {"text", input_log}
    };

    auto spacy_response = nlohmann::json::parse(curl_get("10.8.0.1:8001/pos", spacy_request.dump()));

    auto word_tags = spacy_response.at("data").at(0).at("tags");
    LOG(debug) << word_tags;

    log_type description;
    description[action] = get_field_with_property(word_tags, "VERB", "pos", "lemma").front();
    description[plant] = get_field_with_property(word_tags, "NOUN", "pos", "lemma").back();

    return description;
}

void handleLog(TgBot::Message::Ptr message, TgBot::Bot &bot){
    if(message->text.length() < std::string("log").length() + 2)
        return;
    std::string input_log = message->text.substr(std::string("log").length() + 2);

    std::string response;
    //log_type extract = spacyapi_extract(input_log);
    log_type extract = spacy_server_extract(input_log);

    response = _format("(action: {}, plant: {})", extract.at(action), extract.at(plant));
    LOG(debug) << response;

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
