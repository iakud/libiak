#include "LogConfig.h"

using namespace iak;

std::string LogConfig::s_destination_("./");
bool LogConfig::s_hostNameInLogFileName_(true);
bool LogConfig::s_pidInLogFileName_(true);