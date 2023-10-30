#include <csignal>
#include <cstring>
#include <queue>
#include <thread>

#include "common.hpp"
#include "logger.hpp"
#include "server.hpp"

void handle_terminate_request(int signal) {
	LOG_INFO << "Received SIG" << sigabbrev_np(signal) << " - Terminating program";

	stop_webserver();
}

std::queue<std::thread> threads;

int main() {
	const std::string log_path = "log";

	setup_logging(log_path);

	// Register signal handlers
	std::signal(SIGINT, handle_terminate_request);
	std::signal(SIGTERM, handle_terminate_request);

	// Start all threads
	threads.emplace(start_webserver, log_path);

	LOG_INFO << "Waiting for all threads to stop";
	while (!threads.empty()) {
		threads.front().join();
		threads.pop();
	}

	LOG_INFO << "Goodbye! - Keep your eyes open ;)";
}
