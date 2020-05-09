#include <iostream>

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#include <ctime>
#include <iomanip>

#include "botanist.h"

#include "log.h"
#include "telegram_helpers.h"

#include "simple_commands.h"
#include "journal.h"


std::vector<std::pair<std::string, std::function<void(TgBot::Message::Ptr, TgBot::Bot &)>>> REGISTER_COMMANDS = {
  {"start", handleStart},
  {"version", handleVersion},

  {"time", handleTime},
  {"meteo", handleMeteo},

  {"lancer_loto", initLoto},
  {"loto", handleLoto},
  {"tirage", handleLoto},

  {"log", handleLog},
  {"view", handleViewLog},
};


int main() {
    srand(time(NULL));

    std::string token(getenv("TOKEN"));
    LOG(debug) << "Token: " << token;

    Botanist bot(token);

    for( auto [command, callback]: REGISTER_COMMANDS){
        bot.registerCommand(command, callback);
    }

    signal(SIGINT, [](int s) {
        LOG(debug) << "got SIGINT\nExiting ...";

        std::string dump_dir(getenv("DUMP_DIR") ? getenv("DUMP_DIR") : ".");
        std::time_t t = std::time(nullptr);
        std::stringstream data_dump_filename;
        data_dump_filename << dump_dir << "/" << "data_dump_" << std::put_time(std::localtime(&t), "%Y%m%d_%H%M%S") << ".json";
        LOG(info) << "dumping learning data to " << data_dump_filename.str();
        dump_log_data(data_dump_filename.str());
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
    } catch (std::exception& e) {
        LOG(fatal) << e.what();
    }

    return 0;

}
