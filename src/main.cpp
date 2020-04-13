#include <iostream>

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>

#include <tgbot/tgbot.h>
using namespace std;
using namespace TgBot;

#include "log.h"
#include "storage.h"
#include "telegram_helpers.h"

#include "simple_commands.h"


void handleLog(Message::Ptr message, Bot &bot){
    string response = "__Aujourd'hui__ j'ai __planter__ des __carottes__";
    auto keyboard = createChoices({{{"Ce n'était pas *aujourd'hui*", "date_today"}},
                                   {{"Je n'ai pas *planter*", "action_planter"}},
                                   {{"Ce ne sont pas des *carottes*", "item_carottes"}},
                                   {{"C'est tout bon !", "valid"}}
      });
    bot.getApi().sendMessage(message->chat->id, response, false, 0, keyboard, "MarkdownV2");
}

void handleMessage(Message::Ptr message, Bot &bot){
    LOG(debug) << _format("User wrote {}", message->text.c_str());
    if (StringTools::startsWith(message->text, "/start")) {
        return;
    }

    if (message->chat->type != TgBot::Chat::Type::Private) {
        LOG(debug) << _format("Chat id {}", message->chat->id);
        return;
    }
    //bot.getApi().sendMessage(message->chat->id, "Your message is: " + message->text);
}

void handleComplexCommand(Message::Ptr message, Bot &bot){
    if(StringTools::startsWith(message->text, "/tirage")) {
        int max = std::stoi(message->text.substr(8));
        bot.getApi().sendMessage(message->chat->id, _format("{}", rand() % max + 1));
    }
}


int main() {
    string token(getenv("TOKEN"));
    LOG(debug) << "Token: " << token;
    srand(time(NULL));

    Bot bot(token);


    bot.getEvents().onAnyMessage([&bot](Message::Ptr message) { handleMessage(message, bot); });
    bot.getEvents().onUnknownCommand([&bot](Message::Ptr message) { handleComplexCommand(message, bot); });

    bot.getEvents().onCommand("start", [&bot](Message::Ptr message) { handleStart(message, bot); });

    bot.getEvents().onCommand("time", [&bot](Message::Ptr message) { handleTime(message, bot); });

    bot.getEvents().onCommand("lancer_loto", [&bot](Message::Ptr message) { initLoto(message, bot); });
    bot.getEvents().onCommand("loto", [&bot](Message::Ptr message) { handleLoto(message, bot); });
    bot.getEvents().onCommand("tirage", [&bot](Message::Ptr message) { handleLoto(message, bot); });

    bot.getEvents().onCommand("log", [&bot](Message::Ptr message) { handleLog(message, bot); });

    bot.getEvents().onCommand("check", [&bot](Message::Ptr message) {
        InlineKeyboardMarkup::Ptr keyboard = createChoices({{ {"check", "check"}, {"new plant", "new_plant"}, {"log", "log"} }});
        string response = "ok";
        bot.getApi().sendMessage(message->chat->id, response, false, 0, keyboard, "Markdown");
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
