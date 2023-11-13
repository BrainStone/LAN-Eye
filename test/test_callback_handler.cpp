#include "callback_handler.hpp"
#include "gtest/gtest.h"

using test_callback_handler_t = callback_handler<int>;

TEST(TestCallbackHandler, DummyCallback) {
	test_callback_handler_t handler;

	EXPECT_NO_THROW({
		// Just make sure this doesn't throw anything
		test_callback_handler_t::callback_handle handle = handler.register_callback([](int) {});
		handler.call_callbacks(0);
		handler.unregister_callback(handle);
		handler.call_callbacks(0);
	});
}

TEST(TestCallbackHandler, SimpleCallback) {
	test_callback_handler_t handler;
	int val = 0;

	// Just make sure this doesn't throw anything
	test_callback_handler_t::callback_handle handle = handler.register_callback([&val](int new_val) { val = new_val; });
	EXPECT_EQ(val, 0);

	handler.call_callbacks(1);
	EXPECT_EQ(val, 1);

	handler.unregister_callback(handle);
	handler.call_callbacks(2);
	EXPECT_EQ(val, 1);
}

TEST(TestCallbackHandler, MultipleCallbacks) {
	test_callback_handler_t handler;
	int val = 0;

	// Just make sure this doesn't throw anything
	test_callback_handler_t::callback_handle handle1 =
	    handler.register_callback([&val](int new_val) { val += new_val; });
	EXPECT_EQ(val, 0);

	handler.call_callbacks(1);
	EXPECT_EQ(val, 1);

	test_callback_handler_t::callback_handle handle2 =
	    handler.register_callback([&val](int new_val) { val += new_val * 2; });
	EXPECT_EQ(val, 1);

	handler.call_callbacks(1);
	EXPECT_EQ(val, 4);

	handler.unregister_callback(handle1);
	handler.call_callbacks(1);
	EXPECT_EQ(val, 6);

	handler.unregister_callback(handle2);
	handler.call_callbacks(1);
	EXPECT_EQ(val, 6);
}

TEST(TestCallbackHandler, MultipleRegistering) {
	test_callback_handler_t handler;
	test_callback_handler_t::callback_t func = [](int) {};

	test_callback_handler_t::callback_handle handle1 = handler.register_callback(func);
	test_callback_handler_t::callback_handle handle2 = handler.register_callback(func);

	EXPECT_NE(handle1, handle2);
}

TEST(TestCallbackHandler, MultipleUnregistering) {
	test_callback_handler_t handler;

	test_callback_handler_t::callback_handle handle = handler.register_callback([](int) {});

	handler.unregister_callback(handle);

	EXPECT_THROW(handler.unregister_callback(handle), std::invalid_argument);
}

TEST(TestCallbackHandler, MultipleUnregisteringCopy) {
	test_callback_handler_t handler;

	test_callback_handler_t::callback_handle handle1 = handler.register_callback([](int) {});
	test_callback_handler_t::callback_handle handle2 = handle1;

	// Make sure we actually have two copies
	EXPECT_NE(&handle1, &handle2);

	handler.unregister_callback(handle1);

	EXPECT_THROW(handler.unregister_callback(handle2), std::invalid_argument);
}

TEST(TestCallbackHandler, MultipleParams) {
	callback_handler<int, int> handler;
	int val = 0;

	// Just make sure this doesn't throw anything
	callback_handler<int, int>::callback_handle handle =
	    handler.register_callback([&val](int val1, int val2) { val = 3 * val1 + 5 * val2; });
	EXPECT_EQ(val, 0);

	handler.call_callbacks(1, 2);
	EXPECT_EQ(val, 13);

	handler.unregister_callback(handle);
	handler.call_callbacks(2, 1);
	EXPECT_EQ(val, 13);
}
