#pragma once

#include <map>
#include <optional>
#include <string>
#include <tuple>
#include <vector>

#include "boost/asio/ip/address.hpp"
#include "json/value.h"
#include "pugixml.hpp"

struct host {
	boost::asio::ip::address ip;
	std::optional<std::string> mac;
	std::vector<std::string> hostnames;

	// Determines the mac address if it's empty
	void find_mac();

	bool operator==(const host& other) const = default;
	bool operator!=(const host& other) const = default;

	friend const pugi::xml_node& operator>>(const pugi::xml_node& node, host& host);
	friend const Json::Value& operator>>(const Json::Value& node, host& host);
	friend Json::Value& operator<<(Json::Value& node, const host& host);
};

class host_information {
public:
	using duration_t = std::chrono::milliseconds;
	using time_point_t = std::chrono::time_point<std::chrono::system_clock, duration_t>;

private:
	host host_data;
	time_point_t first_seen;
	time_point_t last_seen;

public:
	host_information() = default;
	host_information(host host_data, const time_point_t& first_seen, const time_point_t& last_seen);

	bool operator==(const host_information& other) const = default;
	bool operator!=(const host_information& other) const = default;

	friend const Json::Value& operator>>(const Json::Value& node, host_information& host_information);
	friend Json::Value& operator<<(Json::Value& node, const host_information& host_information);
};

using host_map_t = std::map<host_information::time_point_t, host_information>;

extern host_map_t host_map;

const Json::Value& operator>>(const Json::Value& node, host_map_t& host_map);
Json::Value& operator<<(Json::Value& node, const host_map_t& host_map);

// Key name, expected type, required, reference to node
using json_mapping_t = std::tuple<const char*, Json::ValueType, bool, Json::Value&>;

bool map_json_object(const Json::Value& node, const std::vector<json_mapping_t>& mapping);
