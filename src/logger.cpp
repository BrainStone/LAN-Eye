#include "logger.hpp"

#include <filesystem>

#include "common.hpp"
#include "trantor/utils/AsyncFileLogger.h"
#include "trantor/utils/Logger.h"

void setup_logging(const std::string& log_dir) {
	// File logging
	std::filesystem::path log_path{log_dir};

	if (!exists(log_path)) {
		LOG_ERROR << "log file path not exist";
		abort();
	}

	std::shared_ptr async_file_logger = std::make_shared<trantor::AsyncFileLogger>();
	async_file_logger->setFileName("lan_eye", ".log", log_path);
	async_file_logger->startLogging();
	async_file_logger->setFileSizeLimit(max_logsize);
	async_file_logger->setMaxFiles(max_logs);
	trantor::Logger::setOutputFunction(
	    [async_file_logger](const char* msg, const uint64_t len) {
		    // Write to stdout first (copied from the default logger of trantor)
		    fwrite(msg, 1, static_cast<size_t>(len), stdout);
		    // Then to the file
		    async_file_logger->output(msg, len);
	    },
	    [async_file_logger]() {
		    // Flush stdout first (copied from the default logger of trantor)
		    fflush(stdout);
		    // Then flush the file
		    async_file_logger->flush();
	    });

	// Log level
	trantor::Logger::setLogLevel(trantor::Logger::kDebug);

	// Log format
	trantor::Logger::setDisplayLocalTime(true);
}
