#include <limits>
#include <string>
#include <tuple>

#include "gtest/gtest.h"
#include "host_information.hpp"
#include "json/reader.h"
#include "json/writer.h"

const std::tuple<host, std::string, std::string> host_test_cases[]{
    {host{.ip = boost::asio::ip::make_address("1.2.3.4")}, R"({"hostnames":[],"ip":"1.2.3.4"})",
     R"(<host><status state="up" reason="reset" reason_ttl="0"/><address addr="1.2.3.4" addrtype="ipv4"/><hostnames></hostnames><times srtt="123456" rttvar="12345" to="1000000"/></host>)"},
    {host{.ip = boost::asio::ip::make_address("1.2.3.4"), .mac = "01:23:45:67:89:ab"},
     R"({"hostnames":[],"ip":"1.2.3.4","mac":"01:23:45:67:89:ab"})",
     R"(<host><status state="up" reason="arp-response" reason_ttl="0"/><address addr="1.2.3.4" addrtype="ipv4"/><address addr="01:23:45:67:89:AB" addrtype="mac"/><hostnames></hostnames><times srtt="123456" rttvar="12345" to="1000000"/></host>)"},
    {host{.ip = boost::asio::ip::make_address("1.2.3.4"), .hostnames = {"example.com"}},
     R"({"hostnames":["example.com"],"ip":"1.2.3.4"})",
     R"(<host><status state="up" reason="reset" reason_ttl="0"/><address addr="1.2.3.4" addrtype="ipv4"/><hostnames><hostname name="example.com" type="PTR"/></hostnames><times srtt="123456" rttvar="12345" to="1000000"/></host>)"},
    {host{.ip = boost::asio::ip::make_address("1.2.3.4"), .hostnames = {"example.com", "example.local", "aaaaaaaaa"}},
     R"({"hostnames":["example.com","example.local","aaaaaaaaa"],"ip":"1.2.3.4"})",
     R"(<host><status state="up" reason="reset" reason_ttl="0"/><address addr="1.2.3.4" addrtype="ipv4"/><hostnames><hostname name="example.com" type="PTR"/><hostname name="example.local" type="PTR"/><hostname name="aaaaaaaaa" type="PTR"/></hostnames><times srtt="123456" rttvar="12345" to="1000000"/></host>)"},
};

class TestChronoHelpers : public ::testing::TestWithParam<std::intmax_t> {
protected:
	using system_tp_seconds_t = std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>;
	using steady_tp_seconds_t = std::chrono::time_point<std::chrono::steady_clock, std::chrono::seconds>;
	using system_tp_milliseconds_t = std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>;
	using steady_tp_milliseconds_t = std::chrono::time_point<std::chrono::steady_clock, std::chrono::milliseconds>;

	using double_tp_t = std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<double>>;
	using int_milli_tp_t = std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>;
};

TEST_P(TestChronoHelpers, MakeTimePoint) {
	auto num = GetParam();

	EXPECT_EQ(system_tp_seconds_t{std::chrono::seconds{num}}, make_time_point<system_tp_seconds_t>(num));
	EXPECT_EQ(steady_tp_seconds_t{std::chrono::seconds{num}}, make_time_point<steady_tp_seconds_t>(num));
	EXPECT_EQ(system_tp_milliseconds_t{std::chrono::milliseconds{num}}, make_time_point<system_tp_milliseconds_t>(num));
	EXPECT_EQ(steady_tp_milliseconds_t{std::chrono::milliseconds{num}}, make_time_point<steady_tp_milliseconds_t>(num));
}

TEST_P(TestChronoHelpers, TimePointCast) {
	auto num = GetParam();

	EXPECT_EQ(make_time_point<int_milli_tp_t>(num),
	          time_point_cast<int_milli_tp_t>(make_time_point<double_tp_t>(num / 1000.0L)));
	EXPECT_EQ(make_time_point<int_milli_tp_t>(num), (time_point_cast<int_milli_tp_t, double_tp_t>(num / 1000.0L)));
}

INSTANTIATE_TEST_SUITE_P(Simple, TestChronoHelpers, ::testing::Values(0, 10000, -10000, 123456));
INSTANTIATE_TEST_SUITE_P(RealTimestamps, TestChronoHelpers,
                         ::testing::Values(1699493856282, std::numeric_limits<std::int32_t>::max() * 1000L));

TEST(TestChronoHelpers, MakeTimePointNow) {
	auto system_now = std::chrono::system_clock::now();
	auto steady_now = std::chrono::steady_clock::now();

	EXPECT_EQ(system_now,
	          make_time_point<std::chrono::system_clock::time_point>(system_now.time_since_epoch().count()));
	EXPECT_EQ(steady_now,
	          make_time_point<std::chrono::steady_clock::time_point>(steady_now.time_since_epoch().count()));
}

TEST(TestHost, XmlDeserialization) {
	Json::Reader reader;

	for (const std::tuple<host, std::string, std::string>& test : host_test_cases) {
		host host;
		pugi::xml_document doc;

		ASSERT_TRUE(doc.load_string(std::get<2>(test).c_str()));

		doc.child("host") >> host;

		EXPECT_EQ(std::get<0>(test), host);
	}
}

TEST(TestHost, JsonDeserialization) {
	Json::Reader reader;

	for (const std::tuple<host, std::string, std::string>& test : host_test_cases) {
		host host;
		Json::Value root;

		ASSERT_TRUE(reader.parse(std::get<1>(test), root));

		root >> host;

		EXPECT_EQ(std::get<0>(test), host);
	}
}

TEST(TestHost, JsonSerialization) {
	Json::StreamWriterBuilder builder{};
	builder["indentation"] = "";

	for (const std::tuple<host, std::string, std::string>& test : host_test_cases) {
		Json::Value root;
		root << std::get<0>(test);

		EXPECT_EQ(std::get<1>(test), Json::writeString(builder, root));
	}
}
