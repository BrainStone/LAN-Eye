#include <condition_variable>
#include <csignal>
#include <cstring>
#include <mutex>
#include <queue>
#include <thread>

#include "common.hpp"
#include "logger.hpp"
#include "server.hpp"

std::mutex mtx;

std::condition_variable finished_setup_cv;
bool finished_setup = false;

std::condition_variable should_run_cv;
bool should_run = true;

std::queue<std::thread> threads;

void shutdown(int signal) {
	const char* singal_name = sigabbrev_np(signal);

	if (!finished_setup) {
		LOG_WARN << "Received SIG" << singal_name
		         << " before the application has started properly... - Waiting for it to finish";

		std::unique_lock<std::mutex> lock{mtx};
		finished_setup_cv.wait(lock, [] { return finished_setup; });

		LOG_INFO << "Finished waiting - commencing shutdown";
	} else {
		LOG_INFO << "Received SIG" << singal_name << " - Terminating program";
	}

	should_run = false;
	should_run_cv.notify_all();

	stop_webserver();
}

void handle_terminate_request(int signal) {
	// Immediately return from the signal handler so other threads can continue. The shutdown logic can run into a lock!
	threads.emplace(shutdown, signal);
}

int main() {
	// Register signal handlers
	std::signal(SIGINT, handle_terminate_request);
	std::signal(SIGTERM, handle_terminate_request);

	// Setup logging
	const std::string log_path = "log";

	setup_logging(log_path);

	// Start all threads
	threads.emplace(start_webserver, log_path);

	LOG_INFO << "Startup complete";
	finished_setup = true;
	finished_setup_cv.notify_all();

	{
		std::unique_lock<std::mutex> lock{mtx};
		should_run_cv.wait(lock, [] { return !should_run; });
	}

	LOG_INFO << "Waiting for all threads to stop";
	while (!threads.empty()) {
		threads.front().join();
		threads.pop();
	}

	LOG_INFO << "Goodbye! - Keep your eyes open ;)";
}
