#pragma once

#include <functional>
#include <list>
#include <memory>

template <typename... T>
class callback_handler {
public:
	using callback_t = std::function<void(T...)>;

	class callback_handle {
	protected:
		using handle_t = std::list<callback_t>::iterator;

	private:
		std::shared_ptr<bool> valid;
		handle_t handle;

	protected:
		explicit callback_handle(handle_t&& handle);

	public:
		[[nodiscard]] inline bool is_valid() const noexcept;
		inline explicit operator bool() const noexcept;

		[[nodiscard]] inline bool operator==(const callback_handle& other) const noexcept;
		[[nodiscard]] inline bool operator!=(const callback_handle& other) const noexcept = default;

	protected:
		[[nodiscard]] handle_t& get(bool invalidate = true);

		friend class callback_handler;
	};

private:
	std::list<callback_t> callbacks;

public:
	callback_handle register_callback(const callback_t& callback) noexcept;
	void unregister_callback(callback_handle& handle);
	void call_callbacks(const T&... args);
};

#include "callback_handler.inc"
