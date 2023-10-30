#include "logger.hpp"

#include "common.hpp"
#include "drogon/drogon.h"

void setup_logging(const std::string& log_path) {
	// Log Path
	drogon::app().setLogPath(log_path, "lan_eye", max_logsize, max_logs);

	// Log level
	drogon::app().setLogLevel(trantor::Logger::kInfo);

	// Log format
	drogon::app().setLogLocalTime(true);

	// Start logger
	drogon::app().setupFileLogger();
}
