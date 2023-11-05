#include "host_information.hpp"

#include <utility>

#include "boost/algorithm/string.hpp"
#include "common.hpp"

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

pugi::xml_node& operator>>(pugi::xml_node& host_node, host& host) {
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

host_information::host_information(host host_data, const host_information::time_point& first_seen,
                                   const host_information::time_point& last_seen)
    : host_data(std::move(host_data)), first_seen(first_seen), last_seen(last_seen) {}

host_map_t host_map{};

const Json::Value& operator>>(const Json::Value& node, host_map_t& host_map) {
	host_map.clear();

	if (!node.isArray()) {
		LOG_DEBUG << "Expected node to be an array";
		return node;
	}

	for (const Json::Value& host_node : node) {
		;
	}

	return node;
}

Json::Value& operator<<(Json::Value& node, const host_map_t& host_map) {
	if (!node.isArray()) {
		LOG_DEBUG << "Expected node to be an array";
		return node;
	}

	node.clear();
	node.resize(host_map.size());

	return node;
}
