#include "drogon/drogon.h"

int main() {
	// ==== Logging ====
	const std::string log_path = "log";

	// Access Log
	Json::Value accessLoggerConfig;
	accessLoggerConfig["log_path"] = log_path;
	drogon::app().addPlugin("drogon::plugin::AccessLogger", {}, accessLoggerConfig);

	// Application Log
	drogon::app().setLogPath(log_path).setLogLevel(trantor::Logger::kInfo);

	// ==== Application ====
	// Listen Address
	drogon::app().addListener("0.0.0.0", 8080);

	// Worker threads
	drogon::app().setThreadNum(16);

	// Start Application
	drogon::app().run();
}
