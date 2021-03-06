#pragma once
#ifndef BOTA_SIMPLE_COMMANDS_H
#define BOTA_SIMPLE_COMMANDS_H

#include <vector>

#include <tgbot/tgbot.h>
using namespace TgBot;


void handleStart(Message::Ptr message, Bot &bot);
void handleVersion(Message::Ptr message, Bot &bot);

void handleTime(Message::Ptr message, Bot &bot);

void handleMeteo(Message::Ptr message, Bot &bot);
//void handlePluie(Message::Ptr message, Bot &bot);

void initLoto(Message::Ptr message, Bot &bot);
void handleLoto(Message::Ptr message, Bot &bot);

#endif
