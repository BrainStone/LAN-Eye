#include "capability.hpp"

#include <string.h>

#include "common.hpp"

namespace capability {

bool executable_contains(const std::filesystem::path& executable, cap_value_t cap) {
	safe_cap_t caps{cap_get_file(executable.c_str()), cap_free};

	return contains(caps, cap);
}

bool process_contains(const pid_t& pid, cap_value_t cap) {
	safe_cap_t caps{cap_get_pid(pid), cap_free};

	return contains(caps, cap);
}

bool proc_contains(cap_value_t cap) {
	safe_cap_t caps{cap_get_proc(), cap_free};

	return contains(caps, cap);
}

bool contains(const safe_cap_t& caps, cap_value_t cap) {
	return contains(caps.get(), cap);
}

bool contains(const cap_t& caps, cap_value_t cap) {
	if (caps == nullptr) {
		LOG_ERROR << "Could not get capabilities: " << strerror(errno);
		return false;
	}

	cap_flag_value_t cap_flag_val;

	if (cap_get_flag(caps, cap, CAP_EFFECTIVE, &cap_flag_val) == -1) {
		LOG_ERROR << "Could not get cap flag information";
		return false;
	}

	return cap_flag_val == CAP_SET;
}

}  // namespace capability
