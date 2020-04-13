#ifndef BOTA_LOG_H
#define BOTA_LOG_H

#pragma warning(disable: 4596)

#include <boost/log/trivial.hpp>
#define LOG(lvl) BOOST_LOG_TRIVIAL(lvl)

#include <fmt/format.h>
#define _format fmt::format

#endif
