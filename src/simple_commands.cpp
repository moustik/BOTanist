#include "simple_commands.h"

#include <nlohmann/json.hpp>

#include "telegram_helpers.h"
#include "web.h"
#include "log.h"

void handleStart(Message::Ptr message, Bot &bot){
    InlineKeyboardMarkup::Ptr keyboard = createChoices({{ {"check", "check"}, {"new plant", "new_plant"}, {"log", "log"} }});
    bot.getApi().sendMessage(message->chat->id, "Hi!", false, 0, keyboard);
}

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
