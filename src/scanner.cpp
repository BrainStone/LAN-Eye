#include "scanner.hpp"

#include <chrono>
#include <condition_variable>
#include <string>
#include <thread>

#include "boost/asio.hpp"
#include "boost/process.hpp"
#include "capability.hpp"
#include "common.hpp"

namespace scanner {

bool should_run = false;
std::shared_ptr<boost::process::child> nmap;
std::condition_variable cv;
std::mutex mtx;

void start(bool sudo) {
	should_run = true;

	std::vector<std::string> argv{};

	if (sudo) {
		argv.insert(argv.end(),
		            {
		                boost::process::search_path("sudo").string(),  // sudo executable
		                "-n",                                          // Non-interactive, never prompt for anything
		                "--",  // End sudo argument parsing. Everything after it is just the actual command
		            });
	}

	const std::filesystem::path nmap_path = boost::process::search_path("nmap").string();
	const bool have_cap_net_raw = capability::proc_contains(CAP_NET_RAW);

	argv.insert(argv.end(),
	            {
	                nmap_path.string(),  // nmap executable
	                "-oX=-",             // Print output as XML
	                "-PR",               // Try to do an ARP Ping if the host to scan is in the same network
	                "-sn",               // Ping scan, only check if the host is up
	                "-T4",               // I am speed
	            });

	// If we're not running as sudo, we still have the chance to execute with elevated permissions. If we have these
	// permissions let nmap know.
	if (!sudo && (have_cap_net_raw || capability::executable_contains(nmap_path, CAP_NET_RAW))) {
		argv.emplace_back("--privileged");
	}

	// Actual net to scan
	argv.emplace_back("10.69.42.0/24");

	for (const auto& arg : argv) LOG_INFO << arg;

	while (should_run) [[likely]] {
		boost::asio::io_service ios;
		std::future<std::string> data, error;
		std::error_code ec;

		nmap = std::make_shared<boost::process::child>(argv,                            // Pass executable and arguments
		                                               boost::process::std_in.close(),  // No stdin
		                                               boost::process::std_out > data,  // Store stdout in data
		                                               boost::process::std_err > error,  // Store stderr in error
		                                               ios  // io_service needed for async output processing
		);

		ios.run();  // this will actually block until the program is finished
		nmap->wait(ec);
		int rc = nmap->exit_code();
		nmap = nullptr;

		// Check for errors while actually waiting
		if (ec) [[unlikely]] {
			LOG_WARN << "Error while waiting for the program to end: " << ec.message();

			continue;
		}
		// Check for errors during nmap execution
		else if (rc && (should_run || (rc != SIGINT && rc != SIGTERM))) [[unlikely]] {
			LOG_WARN << "An error occured (RC " << rc << "):\n" << error.get();

			continue;
		}
		// Don't process the data if we're supposed to exit
		else if (!should_run) [[unlikely]] {
			break;
		}

		LOG_INFO << data.get();

		// Wait 5 seconds but allow to be interrupted
		std::unique_lock<std::mutex> lock(mtx);
		cv.wait_for(lock, std::chrono::seconds(5), [] { return !should_run; });
	}
}

void stop() {
	if (!should_run) [[unlikely]]
		return;

	// Mark that we're done running
	should_run = false;

	// Stop running nmap processes
	if (nmap) {
		pid_t pid = nmap->id();

		LOG_DEBUG << "nmap still running - sending SIGTERM to " << pid;
		if (kill(pid, SIGTERM) == -1) {
			LOG_WARN << "Error while executing kill: " << strerror(errno);
		}
	}

	// Skip waiting
	cv.notify_all();
}

}  // namespace scanner
