#pragma once

#include <optional>
#include <set>
#include <string>
#include <tuple>
#include <vector>

#include "boost/asio/ip/address.hpp"
#include "json/value.h"
#include "pugixml.hpp"

template <typename TP>
constexpr TP make_time_point(const typename TP::duration::rep& val) {
	return TP{typename TP::duration{val}};
}

template <typename TP_to, typename TP_from>
    requires std::same_as<typename TP_to::clock, typename TP_from::clock>
constexpr TP_to time_point_cast(const TP_from& time_point) {
	return std::chrono::time_point_cast<typename TP_to::duration>(time_point);
}

template <typename TP_to, typename TP_from>
    requires std::same_as<typename TP_to::clock, typename TP_from::clock>
constexpr TP_to time_point_cast(const typename TP_from::duration::rep& val) {
	return time_point_cast<TP_to, TP_from>(make_time_point<TP_from>(val));
}

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

struct host_information {
	using duration_t = std::chrono::milliseconds;
	using time_point_t = std::chrono::time_point<std::chrono::system_clock, duration_t>;

	host host_data;
	time_point_t first_seen;
	time_point_t last_seen;

	bool operator==(const host_information& other) const = default;
	bool operator!=(const host_information& other) const = default;

	friend const Json::Value& operator>>(const Json::Value& node, host_information& host_information);
	friend Json::Value& operator<<(Json::Value& node, const host_information& host_information);

	struct first_seen_comparator {
		constexpr bool operator()(const host_information& lhs, const host_information& rhs) const {
			return lhs.first_seen > rhs.first_seen;
		}
	};
};

using host_list_t = std::multiset<host_information, host_information::first_seen_comparator>;

extern host_list_t current_hosts_list;

const Json::Value& operator>>(const Json::Value& node, host_list_t& host_list);
Json::Value& operator<<(Json::Value& node, const host_list_t& host_list);

// Key name, expected type, required, reference to node
using json_mapping_t = std::tuple<const char*, Json::ValueType, bool, Json::Value&>;

bool map_json_object(const Json::Value& node, const std::vector<json_mapping_t>& mapping);
