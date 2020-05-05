#include "botanist.h"

#include "log.h"

#include "journal.h"

Botanist::Botanist(std::string token) : TgBot::Bot(token) {
    this->getEvents().onAnyMessage([this](TgBot::Message::Ptr message)             { this->handleMessage(message, *this); });
    this->getEvents().onUnknownCommand([this](TgBot::Message::Ptr message)         { this->handleComplexCommand(message, *this); });
    this->getEvents().onCallbackQuery([this](TgBot::CallbackQuery::Ptr query)      { this->handleCallbackQueries(query, *this); });
}

void Botanist::handleMessage(TgBot::Message::Ptr message, Bot &bot){
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

void Botanist::handleComplexCommand(TgBot::Message::Ptr message, Bot &bot){
    if(StringTools::startsWith(message->text, "/tirage")) {
        int max = std::stoi(message->text.substr(8));
        bot.getApi().sendMessage(message->chat->id, _format("{}", rand() % max + 1));
    }
}

void Botanist::handleCallbackQueries(TgBot::CallbackQuery::Ptr query, Bot &bot){
     if (StringTools::startsWith(query->data, "check")) {
         checkCallbackQuery(query, bot);
     }
     else if (StringTools::startsWith(query->data, "log")) {
         logCallbackQuery(query, bot);
     }

     bot.getApi().sendMessage(query->message->chat->id, query->data);
}

void Botanist::registerCommand(std::string command, std::function<void(TgBot::Message::Ptr, TgBot::Bot &)> callback){
    this->getEvents().onCommand(command,
                                [this, callback](TgBot::Message::Ptr message) {callback(message, *this); });
}
