#include "server.hpp"

#include <string>
#include <thread>

#include "common.hpp"
#include "drogon/drogon.h"

namespace server {
bool should_run = false;

void start_webserver(const std::string& log_path) {
	// Mark that we received a start request
	should_run = true;

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

	// We're done
	should_run = false;
}

void stop_webserver() {
	drogon::HttpAppFramework& app = drogon::app();

	if (!should_run && !app.isRunning() && !app.getLoop()->isRunning()) [[unlikely]] {
		return;
	}

	bool first_check = true;

	// Wait for the server to be up before we stop it
	while (should_run && !(app.isRunning() && app.getLoop()->isRunning())) [[unlikely]] {
		if (first_check) [[unlikely]] {
			LOG_DEBUG << "Webserver received shutdown command before it was up - Waiting for it to be up before "
			             "shutting it down for real";
			first_check = false;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	if (!first_check) {
		LOG_DEBUG << "Webserver is finally up - Stopping it now.";
	}

	drogon::app().quit();
}

}  // namespace server
