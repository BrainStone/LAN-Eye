#pragma once

#include <stdexcept>

#include "callback_handler.hpp"

template <typename... T>
callback_handler<T...>::callback_handle::callback_handle(handle_t&& handle)
    : valid{std::make_shared<bool>(true)}, handle{handle} {}

template <typename... T>
void callback_handler<T...>::callback_handle::verify_handle() const {
	if (!is_valid()) {
		throw std::invalid_argument{"handle is invalid"};
	}
}

template <typename... T>
bool callback_handler<T...>::callback_handle::is_valid() const noexcept {
	return *valid;
}

template <typename... T>
callback_handler<T...>::callback_handle::operator bool() const noexcept {
	return is_valid();
}

template <typename... T>
bool callback_handler<T...>::callback_handle::operator==(const callback_handle& other) const {
	verify_handle();

	return handle == other.handle;
}

template <typename... T>
auto callback_handler<T...>::callback_handle::get(bool invalidate) -> handle_t& {
	verify_handle();

	if (invalidate) *valid = false;

	return handle;
}

template <typename... T>
auto callback_handler<T...>::register_callback(const callback_t& callback) noexcept -> callback_handle {
	callbacks.push_back(callback);
	return callback_handle{std::prev(callbacks.end())};
}

template <typename... T>
void callback_handler<T...>::unregister_callback(callback_handle& handle) {
	callbacks.erase(handle.get());
}

template <typename... T>
void callback_handler<T...>::call_callbacks(const T&... args) {
	for (const callback_t& callback : callbacks) {
		callback(args...);
	}
}
