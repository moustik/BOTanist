#include "journal.h"

#include <string>
#include <regex>
#include <tuple>

#include <nlohmann/json.hpp>

#include "telegram_helpers.h"
#include "web.h"
#include "log.h"

#include "storage.h"


std::string escape_for_md(std::string to_be_escaped){
    std::string escaped = to_be_escaped;
    for(auto escape: {'_', '*', '(', ')', '~', '`', '>', '#', '+', '-', '=', '|', '{', '}', '.', '!'}){
        escaped = std::regex_replace(escaped, std::regex(_format("[{}]", escape)), _format("\\{}", escape));
    }

    return escaped;
}

/** Return a list of word with given POS-tag
 * @param word_tags std::vector<nlohmann::json>
 * @param property property value to be check against
 * @param property_name will be searched in the word_tag element
 * @param return_field property to be returned
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
    raw_data_index[message->messageId] = std::make_tuple(message->messageId, input_log, extract);

    auto keyboard = createChoices({
        //{{"Ce n'était pas *aujourd'hui*", "date_today"}},
        {{_format("Je n'ai pas *{}*", extract[action]), _format("log_fix_action_{}_{}", extract[action], message->messageId) }},
        {{_format("Ce ne sont pas des *{}*", extract[plant]), _format("log_fix_plant_{}_{}", extract[plant], message->messageId) }},
        {{"C'est tout bon !", _format("log_valid_{}", message->messageId) }}
      });
    bot.getApi().sendMessage(message->chat->id, escape_for_md(response), false, 0, keyboard, "MarkdownV2");
}


void handleViewLog(TgBot::Message::Ptr message, TgBot::Bot &bot){
    std::stringstream response;
    response << std::right << std::setw(10) << "plante" << std::setw(15) << "action"  << std::endl;
    response << std::setfill('-') << std::setw(25) << "" << std::endl << std::setfill(' ');
    for(auto line: logs){
        std::string log_action = line[action];
        std::string log_plant = line[plant];
        response << std::setw(10) << log_plant << std::setw(15) << log_action  << std::endl;
    }

    bot.getApi().sendMessage(message->chat->id, "```" + escape_for_md(response.str()) + "```", false, 0,
                             NULL, "MarkdownV2");
}

void logCallbackQuery(TgBot::CallbackQuery::Ptr query, TgBot::Bot &bot){
//    bot.getApi().editMessageText("YES!", query->message->chat->id, query->message->messageId, "",
//                                 "Markdown", false,
//                                 createChoices({{ {"checked", "check"}, {"new plant", "new_plant"}, {"log", "log"} }}));

    if(StringTools::startsWith(query->data, "log_valid")){
        int messageId = std::stoi(query->data.substr(std::string("log_valid").size()+1));
        LOG(debug) << "message id gotten " << messageId;
        auto data = raw_data_index.at(messageId);
        raw_data.push_back(data);
        logs.push_back(std::get<2>(data));

        bot.getApi().editMessageReplyMarkup(query->message->chat->id, query->message->messageId);
        bot.getApi().sendMessage(query->message->chat->id, "C'est noté !");
    }
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


void dump_log_data(std::string filename){
    std::ofstream o(filename);
    nlohmann::json j;

    for(auto datum: raw_data){
        nlohmann::json jlog;
        for( auto const& [key, val] : std::get<2>(datum) ){
            jlog[tag_type_str[key]] = val;
        }

        j.push_back({
            {"input_string", std::get<1>(datum)},
            {"log_entry", jlog}
        });
    }
    o << std::setw(4) << j << std::endl;
}
