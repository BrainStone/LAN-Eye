#include "host_information.hpp"

#include "boost/algorithm/string.hpp"
#include "common.hpp"

using double_duration_t = std::chrono::duration<double>;
using double_time_point_t = std::chrono::time_point<host_information::time_point_t::clock, double_duration_t>;

void host::find_mac() {
	// If mac is not set, try to get it a different way
	if (!mac) {
		// TODO
	}

	// Ensure MAC addresses are always lowercase, if set
	if (mac) {
		boost::algorithm::to_lower(*mac);
	}
}

const pugi::xml_node& operator>>(const pugi::xml_node& host_node, host& host) {
	using namespace std::string_literals;

	if (host_node.name() != "host"s) {
		LOG_DEBUG << "Invalid node \"" << host_node.name() << "\" attempted to pe parsed as a host object";
		return host_node;
	}

	for (pugi::xml_node address : host_node.children("address")) {
		if (std::string(address.attribute("addrtype").value()) == "ipv4") {
			host.ip = boost::asio::ip::make_address(address.attribute("addr").value());
		} else if (std::string(address.attribute("addrtype").value()) == "mac") {
			host.mac = address.attribute("addr").value();
		}
	}
	for (pugi::xml_node hostname : host_node.child("hostnames").children("hostname")) {
		host.hostnames.emplace_back(hostname.attribute("name").value());
	}

	host.find_mac();

	return host_node;
}

const Json::Value& operator>>(const Json::Value& node, host& host) {
	if (!node.isObject()) {
		LOG_DEBUG << "Expected node to be an object";
		return node;
	}

	Json::Value ip, mac, hostnames;

	if (!map_json_object(node, {{"ip", Json::ValueType::stringValue, true, ip},
	                            {"mac", Json::ValueType::stringValue, false, mac},
	                            {"hostnames", Json::ValueType::arrayValue, true, hostnames}})) {
		return node;
	}

	for (const Json::Value& hostname : hostnames) {
		if (!hostname.isString()) {
			LOG_DEBUG << "Expected hostname node to be a string";
			return node;
		}
	}

	host.ip = boost::asio::ip::make_address(ip.asString());
	if (!mac.isNull()) host.mac = mac.asString();
	host.hostnames.clear();
	host.hostnames.reserve(hostnames.size());

	for (const Json::Value& hostname : hostnames) {
		host.hostnames.push_back(hostname.asString());
	}

	return node;
}

Json::Value& operator<<(Json::Value& node, const host& host) {
	if (!node.isObject() && !node.isNull()) {
		LOG_DEBUG << "Expected node to be an object";
		return node;
	}

	node["ip"] = host.ip.to_string();
	if (host.mac) node["mac"] = *host.mac;
	Json::Value& hostnames = node["hostnames"];
	hostnames.resize(host.hostnames.size());
	Json::ArrayIndex i = 0;

	for (const std::string& hostname : host.hostnames) {
		hostnames[i++] = hostname;
	}

	return node;
}

const Json::Value& operator>>(const Json::Value& node, host_information& host_information) {
	using time_point_t = host_information::time_point_t;

	if (!node.isObject()) {
		LOG_DEBUG << "Expected node to be an object";
		return node;
	}

	Json::Value host, first_seen, last_seen;

	if (!map_json_object(node, {{"host", Json::ValueType::objectValue, true, host},
	                            {"first_seen", Json::ValueType::realValue, true, first_seen},
	                            {"last_seen", Json::ValueType::realValue, true, last_seen}})) {
		return node;
	}

	host >> host_information.host_data;
	host_information.first_seen = time_point_cast<time_point_t, double_time_point_t>(first_seen.asDouble());
	host_information.last_seen = time_point_cast<time_point_t, double_time_point_t>(last_seen.asDouble());

	return node;
}

Json::Value& operator<<(Json::Value& node, const host_information& host_information) {
	if (!node.isObject() && !node.isNull()) {
		LOG_DEBUG << "Expected node to be an object";
		return node;
	}

	node["host"] << host_information.host_data;
	node["first_seen"] = time_point_cast<double_time_point_t>(host_information.first_seen).time_since_epoch().count();
	node["last_seen"] = time_point_cast<double_time_point_t>(host_information.last_seen).time_since_epoch().count();

	return node;
}

constexpr bool host_information::first_seen_comparator::operator()(const host_information& lhs,
                                                                   const host_information& rhs) const {
	return lhs.first_seen > rhs.first_seen;
}

host_list_t current_hosts_list{};

const Json::Value& operator>>(const Json::Value& node, host_list_t& host_list) {
	host_list.clear();

	if (!node.isArray()) {
		LOG_DEBUG << "Expected node to be an array";
		return node;
	}

	for (const Json::Value& host_node : node) {
		host_information hi;
		host_node >> hi;

		host_list.insert(hi);
	}

	return node;
}

Json::Value& operator<<(Json::Value& node, const host_list_t& host_list) {
	if (!node.isArray() && !node.isNull()) {
		LOG_DEBUG << "Expected node to be an array";
		return node;
	}

	node.clear();
	node.resize(host_list.size());
	Json::ArrayIndex i = 0;

	for (const host_information& host : host_list) {
		node[i++] << host;
	}

	return node;
}

bool map_json_object(const Json::Value& node, const std::vector<json_mapping_t>& mapping) {
	for (const json_mapping_t& key : mapping) {
		bool is_required = std::get<2>(key);

		if (is_required && !node.isMember(std::get<0>(key))) {
			LOG_DEBUG << "Expected node to be an object that contains the member " << std::get<0>(key);
			return false;
		}

		const Json::Value& child_node = node[std::get<0>(key)];
		std::get<3>(key) = child_node;

		if (!child_node.isConvertibleTo(std::get<1>(key)) && !(!is_required && child_node.isNull())) {
			LOG_DEBUG << "Expected the member " << std::get<0>(key) << " to be convertible to the appropriate type";
			return false;
		}
	}

	return true;
}
