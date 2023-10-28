#include "drogon/drogon.h"

int main() {
	// ==== Logging ====
	const std::string log_path = "log";

	// Application Log
	drogon::app().setLogPath(log_path).setLogLevel(trantor::Logger::kInfo).setLogLocalTime(true);

	// Access Log
	Json::Value accessLoggerConfig;
	accessLoggerConfig["log_path"] = log_path;
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

	// Start Application
	drogon::app().run();
}
