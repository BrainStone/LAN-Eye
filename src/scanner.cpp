#include "scanner.hpp"

#include <chrono>
#include <condition_variable>
#include <thread>

#include "common.hpp"

namespace scanner {

bool should_run = false;
std::condition_variable cv;
std::mutex mtx;

void start() {
	should_run = true;

	while (should_run) [[likely]] {
		LOG_INFO << "Scanner";

		// Wait 5 seconds but allow to be interrupted
		std::unique_lock<std::mutex> lock(mtx);
		cv.wait_for(lock, std::chrono::seconds(5), [] { return !should_run; });
	}
}

void stop() {
	if (!should_run) [[unlikely]]
		return;

	cv.notify_all();
	should_run = false;
}

}  // namespace scanner
