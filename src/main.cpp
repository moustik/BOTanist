#include <iostream>

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>

#include <nlohmann/json.hpp>

#include <tgbot/tgbot.h>
using namespace std;
using namespace TgBot;

#include "log.h"
#include "storage.h"


InlineKeyboardMarkup::Ptr createChoices(
        std::initializer_list<std::initializer_list<std::pair<std::string, std::string>>> choices
                                        ){
    InlineKeyboardMarkup::Ptr keyboard(new InlineKeyboardMarkup);
    for (const auto &row: choices) {
        vector<InlineKeyboardButton::Ptr> row0;

        for (const auto &choice: row) {
          InlineKeyboardButton::Ptr checkButton(new InlineKeyboardButton);
          checkButton->text = choice.first;
          checkButton->callbackData = choice.second;
          row0.push_back(checkButton);
        }
        keyboard->inlineKeyboard.push_back(row0);
    }
    return keyboard;
}


// ------------------------- web.h
#include <curl/curl.h>


/** @fn cURLWriteFunction
 *  @brief fonction de callback permettant a cURL d'ecrire dans une std:string
 *
 */
size_t cURLWriteFunction(void *ptr, size_t size, size_t nmemb, std::string* data);

/** @fn curl_get
 *  @brief recupere la reponse d'une requete GET http
 *  @param request_url requette HTTP formattee pour le web
 *
 */
std::string curl_get(std::string request_url);


// ------------------------- web.cpp
size_t cURLWriteFunction(void *ptr, size_t size, size_t nmemb, std::string* data) {
    data->append((char*) ptr, size * nmemb);
    return size * nmemb;
}

std::string curl_get(std::string request_url){
    // from https://gist.github.com/whoshuu/2dc858b8730079602044
    auto curl = curl_easy_init();

    std::string response_string;
    std::string header_string;

    if (curl) {
        LOG(debug) << _format("fetching {}", request_url.c_str());
        curl_easy_setopt(curl, CURLOPT_URL, request_url.c_str());
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
        //curl_easy_setopt(curl, CURLOPT_USERPWD, "user:pass");
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.42.0");
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
        curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cURLWriteFunction);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);

        char* url;
        long response_code;
        double elapsed;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &elapsed);
        curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url);

        curl_easy_perform(curl);
        LOG(debug) << response_string;
        LOG(debug) << _format("got {} in {} ms", response_code, elapsed);
        curl_easy_cleanup(curl);
        curl = NULL;
    }

    return response_string;
}
// -------------------------

void handleTime(Message::Ptr message, Bot &bot){
     std::string http_response = curl_get("http://worldtimeapi.org/api/timezone/Europe/Paris");
     auto json_data = nlohmann::json::parse(http_response);
     std::string datetime_FR = std::string(json_data.value("datetime", "")).substr(0,16);

     http_response = curl_get("http://worldtimeapi.org/api/timezone/Pacific/Noumea");
     json_data = nlohmann::json::parse(http_response);
     std::string datetime_NC = std::string(json_data.value("datetime", "")).substr(0,16);
     std::string response = _format("Il est\n {} le {}/{} en France\n {} le {}/{} en Nouvelle Calédonie",
                                    datetime_FR.substr(11), datetime_FR.substr(8, 2), datetime_FR.substr(5, 2),
                                    datetime_NC.substr(11), datetime_NC.substr(8, 2), datetime_NC.substr(5, 2));
     bot.getApi().sendMessage(message->chat->id, response);
}

std::vector<int> lotoNumbers;
void initLoto(Message::Ptr message, Bot &bot){
    lotoNumbers.clear();
    for( int i=0; i<90; ++i ){
        lotoNumbers.push_back(i);
    }
    std::random_shuffle(lotoNumbers.begin(), lotoNumbers.end());

    bot.getApi().sendMessage(message->chat->id, "Prêt à jouer ! Utiliser /loto pour tirer les numéros");
}
void handleLoto(Message::Ptr message, Bot &bot){
    if(lotoNumbers.empty()){
        bot.getApi().sendMessage(message->chat->id, "Le loto n'est pas prêt utiliser /lancer_loto pour commencer");
    }
    else{
        bot.getApi().sendMessage(message->chat->id, _format("{}", lotoNumbers.back()));
        lotoNumbers.pop_back();
    }
}

int main() {
    string token(getenv("TOKEN"));
    LOG(debug) << "Token: " << token;
    srand(time(NULL));

    InlineKeyboardMarkup::Ptr keyboard = createChoices({{ {"check", "check"}, {"new plant", "new_plant"}, {"log", "log"} }});

    Bot bot(token);
    bot.getEvents().onCommand("start", [&bot, &keyboard](Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Hi!", false, 0, keyboard);
    });
    bot.getEvents().onAnyMessage([](Message::Ptr message) {
        LOG(debug) << _format("User wrote {}", message->text.c_str());
        if (StringTools::startsWith(message->text, "/start")) {
            return;
        }

        if (message->chat->type != TgBot::Chat::Type::Private) {
            LOG(debug) << _format("Chat id {}", message->chat->id);
            return;
        }
        //bot.getApi().sendMessage(message->chat->id, "Your message is: " + message->text);
    });

    bot.getEvents().onCommand("time", [&bot](Message::Ptr message) { handleTime(message, bot); });

    bot.getEvents().onCommand("lancer_loto", [&bot](Message::Ptr message) { initLoto(message, bot); });
    bot.getEvents().onCommand("loto", [&bot](Message::Ptr message) { handleLoto(message, bot); });
    bot.getEvents().onCommand("tirage", [&bot](Message::Ptr message) { handleLoto(message, bot); });


    bot.getEvents().onCommand("check", [&bot, &keyboard](Message::Ptr message) {
        string response = "ok";
        bot.getApi().sendMessage(message->chat->id, response, false, 0, keyboard, "Markdown");
    });

    bot.getEvents().onCommand("log", [&bot](Message::Ptr message) {
        string response = "__Aujourd'hui__ j'ai __planter__ des __carottes__";
        auto keyboard = createChoices({{{"Ce n'était pas *aujourd'hui*", "date_today"}},
                                       {{"Je n'ai pas *planter*", "action_planter"}},
                                       {{"Ce ne sont pas des *carottes*", "item_carottes"}},
                                       {{"C'est tout bon !", "valid"}}
          });
        bot.getApi().sendMessage(message->chat->id, response, false, 0, keyboard, "MarkdownV2");
    });

    bot.getEvents().onCallbackQuery([&bot](CallbackQuery::Ptr query) {
        if (StringTools::startsWith(query->data, "check")) {
            bot.getApi().editMessageText("YES!", query->message->chat->id, query->message->messageId, "",
                                         "Markdown", false,
                                         createChoices({{ {"checked", "check"}, {"new plant", "new_plant"}, {"log", "log"} }}));
        }

        bot.getApi().sendMessage(query->message->chat->id, query->data);

    });


    signal(SIGINT, [](int s) {
        LOG(debug) << "got SIGINT\nExiting ...";
        exit(0);
    });

    try {
        LOG(info) << _format("Bot username: {}", bot.getApi().getMe()->username.c_str());
        bot.getApi().deleteWebhook();

        TgLongPoll longPoll(bot);
        while (true) {
            // LOG(debug) << "Long poll started";
            longPoll.start();
        }
    } catch (exception& e) {
        LOG(fatal) << e.what();
    }

    return 0;

}
