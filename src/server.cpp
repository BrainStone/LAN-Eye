#include "server.hpp"

#include <string>

#include "common.hpp"
#include "drogon/drogon.h"

void start_webserver(const std::string& log_path) {
	// ==== Logging ====
	// Application log is configured in logger.cpp

	// Access Log
	Json::Value accessLoggerConfig;
	accessLoggerConfig["log_path"] = log_path;
	accessLoggerConfig["log_size_limit"] = max_logsize;
	accessLoggerConfig["max_files"] = max_logs;
	accessLoggerConfig["use_local_time"] = true;
	drogon::app().addPlugin("drogon::plugin::AccessLogger", {}, accessLoggerConfig);

	// ==== Application ====
	// Listen Address
	drogon::app().addListener("0.0.0.0", 8080);

	// Worker Threads
	drogon::app().setThreadNum(16);

	// Compression Stuff
	drogon::app().enableGzip(true).enableBrotli(true);

	// Disable Sessions
	drogon::app().disableSession();

	// We're handling signals just fine
	drogon::app().disableSigtermHandling();

	// Start Application
	drogon::app().run();

	LOG_INFO << "";
}

void stop_webserver() {
	drogon::app().quit();
}
