#include <string>
#include <utility>

#include "gtest/gtest.h"
#include "host_information.hpp"
#include "json/reader.h"
#include "json/writer.h"

const std::pair<host, std::string> host_test_cases[]{
    {host{.ip = boost::asio::ip::make_address("1.2.3.4")}, R"({"hostnames":[],"ip":"1.2.3.4"})"},
    {host{.ip = boost::asio::ip::make_address("1.2.3.4"), .mac = "01:23:45:67:89:ab"},
     R"({"hostnames":[],"ip":"1.2.3.4","mac":"01:23:45:67:89:ab"})"},
    {host{.ip = boost::asio::ip::make_address("1.2.3.4"), .hostnames = {"example.com"}},
     R"({"hostnames":["example.com"],"ip":"1.2.3.4"})"},
    {host{.ip = boost::asio::ip::make_address("1.2.3.4"), .hostnames = {"example.com", "example.local", "aaaaaaaaa"}},
     R"({"hostnames":["example.com","example.local","aaaaaaaaa"],"ip":"1.2.3.4"})"},
};

TEST(TestChronoHelpers, MakeTimePoint) {
	using system_tp_seconds_t = std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>;
	using steady_tp_seconds_t = std::chrono::time_point<std::chrono::steady_clock, std::chrono::seconds>;
	using system_tp_milliseconds_t = std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>;
	using steady_tp_milliseconds_t = std::chrono::time_point<std::chrono::steady_clock, std::chrono::milliseconds>;

	for (typename std::chrono::seconds::rep num : {0, 10000, -10000}) {
		EXPECT_EQ(system_tp_seconds_t{std::chrono::seconds{num}}, make_time_point<system_tp_seconds_t>(num));
		EXPECT_EQ(steady_tp_seconds_t{std::chrono::seconds{num}}, make_time_point<steady_tp_seconds_t>(num));
		EXPECT_EQ(system_tp_milliseconds_t{std::chrono::milliseconds{num}},
		          make_time_point<system_tp_milliseconds_t>(num));
		EXPECT_EQ(steady_tp_milliseconds_t{std::chrono::milliseconds{num}},
		          make_time_point<steady_tp_milliseconds_t>(num));
	}

	auto system_now = std::chrono::system_clock::now();
	auto steady_now = std::chrono::steady_clock::now();

	EXPECT_EQ(system_now,
	          make_time_point<std::chrono::system_clock::time_point>(system_now.time_since_epoch().count()));
	EXPECT_EQ(steady_now,
	          make_time_point<std::chrono::steady_clock::time_point>(steady_now.time_since_epoch().count()));
}

TEST(TestChronoHelpers, TimePointCast) {
	using double_tp_t = std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<double>>;
	using int_milli_tp_t = std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>;

	for (typename int_milli_tp_t::duration::rep num : {0, 10000, -10000, 123456}) {
		EXPECT_EQ(make_time_point<int_milli_tp_t>(num),
		          time_point_cast<int_milli_tp_t>(make_time_point<double_tp_t>(num / 1000.0)));
		EXPECT_EQ(make_time_point<int_milli_tp_t>(num), (time_point_cast<int_milli_tp_t, double_tp_t>(num / 1000.0)));
	}
}

TEST(TestHost, JsonDeserialization) {
	Json::StreamWriterBuilder builder{};
	builder["indentation"] = "";

	for (const std::pair<host, std::string>& test : host_test_cases) {
		host host;
		Json::Value root;
		Json::Reader reader;
		EXPECT_TRUE(reader.parse(test.second, root));

		root >> host;

		EXPECT_EQ(test.first, host);
	}
}

TEST(TestHost, JsonSerialization) {
	Json::StreamWriterBuilder builder{};
	builder["indentation"] = "";

	for (const std::pair<host, std::string>& test : host_test_cases) {
		Json::Value root;
		root << test.first;

		EXPECT_EQ(test.second, Json::writeString(builder, root));
	}
}
