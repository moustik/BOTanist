#include <iostream>

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <string>

#include <nlohmann/json.hpp>

#include <tgbot/tgbot.h>
using namespace std;
using namespace TgBot;

#include "log.h"
#include "storage.h"


InlineKeyboardMarkup::Ptr createChoices(std::initializer_list<string> choices){
    InlineKeyboardMarkup::Ptr keyboard(new InlineKeyboardMarkup);
    vector<InlineKeyboardButton::Ptr> row0;

    for (const auto &choice: choices) {
        InlineKeyboardButton::Ptr checkButton(new InlineKeyboardButton);
        checkButton->text = choice;
        checkButton->callbackData = choice;
        row0.push_back(checkButton);
    }
    keyboard->inlineKeyboard.push_back(row0);

    return keyboard;
}

// -------------------------
#include <curl/curl.h>

size_t writeFunction(void *ptr, size_t size, size_t nmemb, std::string* data) {
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

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
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

int main() {
    string token(getenv("TOKEN"));
    LOG(debug) << "Token: " << token;

    InlineKeyboardMarkup::Ptr keyboard = createChoices({ "check", "new_plant", "log" });

    Bot bot(token);
    bot.getEvents().onCommand("start", [&bot, &keyboard](Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Hi!", false, 0, keyboard);
    });
    bot.getEvents().onAnyMessage([&bot](Message::Ptr message) {
        LOG(debug) << _format("User wrote {}", message->text.c_str());
        if (StringTools::startsWith(message->text, "/start")) {
            return;
        }

        if (message->chat->type != TgBot::Chat::Type::Private) {
            return;
        }
        //bot.getApi().sendMessage(message->chat->id, "Your message is: " + message->text);
    });

    bot.getEvents().onCommand("time", [&bot, &keyboard](Message::Ptr message) {
                                        //  "http://worldtimeapi.org/api/timezone/Europe/Paris.txt"
                                        //  "http://worldtimeapi.org/api/timezone/Pacific/Noumea"
        std::string http_response = curl_get("http://worldtimeapi.org/api/timezone/Europe/Paris");
        auto json_data = nlohmann::json::parse(http_response);
        std::string datetime_FR= std::string(json_data["datetime"]).substr(0,16);

        http_response = curl_get("http://worldtimeapi.org/api/timezone/Pacific/Noumea");
        json_data = nlohmann::json::parse(http_response);
        std::string datetime_NC = std::string(json_data["datetime"]).substr(0,16);
        std::string response = _format("Il est\n {} le {}/{} en France\n {} le {}/{} en Nouvelle Calédonie",
                                       datetime_FR.substr(11), datetime_FR.substr(8, 2), datetime_FR.substr(5, 2),
                                       datetime_NC.substr(11), datetime_NC.substr(8, 2), datetime_NC.substr(5, 2));
        bot.getApi().sendMessage(message->chat->id, response);
    });

    bot.getEvents().onCommand("check", [&bot, &keyboard](Message::Ptr message) {
        string response = "ok";
        bot.getApi().sendMessage(message->chat->id, response, false, 0, keyboard, "Markdown");
    });

    bot.getEvents().onCommand("log", [&bot, &keyboard](Message::Ptr message) {
        string response = "ok";
        bot.getApi().sendMessage(message->chat->id, response, false, 0, keyboard, "Markdown");
    });

    bot.getEvents().onCallbackQuery([&bot](CallbackQuery::Ptr query) {
        if (StringTools::startsWith(query->data, "check")) {
            bot.getApi().editMessageText("YES!", query->message->chat->id, query->message->messageId, "",
                                         "Markdown", false, createChoices({ "checked", "new_plant", "log" }));
        }
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
