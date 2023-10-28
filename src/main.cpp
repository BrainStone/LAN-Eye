#include "drogon/drogon.h"

using namespace drogon;

int main() {
	app().setLogPath("./log").setLogLevel(trantor::Logger::kDebug).addListener("0.0.0.0", 8080).setThreadNum(16).run();
}
