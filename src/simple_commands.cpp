#include "simple_commands.h"

#include <random>
#include <algorithm>

#include <nlohmann/json.hpp>

#include "version.h"
#include "telegram_helpers.h"
#include "web.h"
#include "log.h"

// return an integral random number in the range 0 - (n - 1)
int Rand(int n)
{
    return rand() % n ;
}


void handleStart(Message::Ptr message, Bot &bot){
    InlineKeyboardMarkup::Ptr keyboard = createChoices({{ {"check", "check"}, {"new plant", "new_plant"}, {"log", "log"} }});
    bot.getApi().sendMessage(message->chat->id, "Hi!", false, 0, keyboard);
}

void handleVersion(Message::Ptr message, Bot &bot){
    bot.getApi().sendMessage(message->chat->id,
                             _format("Running {} from {}\n{}",
                                     botanist::version::GIT_SHA1,
                                     botanist::version::GIT_DATE,
                                     botanist::version::GIT_COMMIT_SUBJECT));
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

void handleMeteo(Message::Ptr message, Bot &bot){
	std::string response;
	const float MV_eau = 997; // masse volumique de l'eau
	std::time_t t = std::time(0);   // get time now
	struct std::tm* now = std::localtime(&t);
	char date[26];
	strftime(date, sizeof(date), "%Y-%m-%dT%H:00:00+00:00", now);

	//std::string http_response = curl_get("https://samples.openweathermap.org/data/2.5/forecast?q=Saint-Cyr-sur-Loire,fr&units=metric&appid=439d4b804bc8187953eb36d2a8c26a02");
	 std::string http_response = curl_get("https://public.opendatasoft.com/api/records/1.0/search/?dataset=arome-0025-enriched&lang=fr&rows=80&sort=forecast&facet=commune&refine.commune=Saint-Cyr-sur-Loire");
	 if (http_response.size() > 0)
	 {
		 auto json_data = nlohmann::json::parse(http_response);
		 auto records = json_data["records"];
		 int i;
		 for (i = 0; (i < records.size()); i++)
		 {
			 if (records[i]["fields"].value("forecast", "").compare(date) == 0)
			 {
				 break;
			 }
		 }
		 if (i >= records.size())
		 {
			 response = "error data";
		 }
		 else
		 {
			 float val1 = records[i]["fields"]["2_metre_temperature"];			// Saint Cyr Nord
			 float val2 = records[i+1]["fields"]["2_metre_temperature"];		// Saint Cyr Sud
			 float temperature = floor(val1 + val2) * 0.5;

			 val1 = records[i]["fields"]["relative_humidity"];
			 val2 = records[i + 1]["fields"]["relative_humidity"];
			 float humidite = floor(val1 + val2) * 0.5;

			 val1 = records[i]["fields"]["total_water_precipitation"];
			 val2 = records[i + 1]["fields"]["total_water_precipitation"];
			 float precipitation = floor(val1 + val2) * 0.5 / MV_eau * 100;			// kg.m-2 => cm

			 val1 = records[i]["fields"]["wind_speed"];
			 val2 = records[i + 1]["fields"]["wind_speed"];
			 float vent = floor(val1 + val2) * 0.5 * 3.6;							// m.s-1 => km.h-1
			 response = _format("Il fait {}°C avec une humidité de {}%, un vent de {} km/h et {} cm de précipitation prévu dans l'heure ", temperature, humidite, vent, precipitation);
		 }
	 }
	 else
	 {
		 response = "error acces";
	 }
     bot.getApi().sendMessage(message->chat->id, response);
}


std::vector<int> lotoNumbers;

void initLoto(Message::Ptr message, Bot &bot){
    lotoNumbers.clear();
    for( int i=0; i<90; ++i ){
        lotoNumbers.push_back(i);
    }

    std::random_device rd;
    std::mt19937 g(rd());

    std::shuffle(lotoNumbers.begin(), lotoNumbers.end(), g);

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
