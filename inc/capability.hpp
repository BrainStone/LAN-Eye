#pragma once

#include <filesystem>
#include <memory>

#include "sys/capability.h"

namespace capability {

using safe_cap_t = std::unique_ptr<std::remove_pointer_t<cap_t>, decltype(&cap_free)>;

bool executable_contains(const std::filesystem::path& executable, cap_value_t cap);
bool process_contains(const pid_t& pid, cap_value_t cap);
bool proc_contains(cap_value_t cap);

bool contains(const safe_cap_t& caps, cap_value_t cap);
bool contains(const cap_t& caps, cap_value_t cap);

}  // namespace capability
