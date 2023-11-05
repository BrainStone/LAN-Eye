#pragma once

#include <json/value.h>

#include <map>
#include <optional>
#include <string>
#include <vector>

#include "boost/asio/ip/address.hpp"
#include "pugixml.hpp"

struct host {
	boost::asio::ip::address ip;
	std::optional<std::string> mac;
	std::vector<std::string> hostnames;

	// Determines the mac address if it's empty
	void find_mac();

	bool operator==(const host& other) const = default;
	bool operator!=(const host& other) const = default;

	friend pugi::xml_node& operator>>(pugi::xml_node& node, host& host);
};

class host_information {
public:
	using time_point = std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>;

private:
	const host host_data;
	const time_point first_seen;
	const time_point last_seen;

public:
	host_information(host host_data, const time_point& first_seen, const time_point& last_seen);
	host_information(const host_information& other) = default;
	host_information(host_information&& other) = default;

	bool operator==(const host_information& other) const = default;
	bool operator!=(const host_information& other) const = default;
};

using host_map_t = std::map<host_information::time_point, host_information>;

extern host_map_t host_map;

const Json::Value& operator>>(const Json::Value& node, host_map_t& host_map);
Json::Value& operator<<(Json::Value& node, const host_map_t& host_map);
