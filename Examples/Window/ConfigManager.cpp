#include "./ConfigManager.h"
#include <rapidjson/Document.h>
#include <rapidjson/error/en.h>
#include <fstream>
#include <iostream>

ConfigManager::ConfigManager(rapidconfig::Section& s) : section(s) {}

bool ConfigManager::load() {
	std::ifstream in(file, std::ios_base::binary);
	if (!in.is_open()) {
		std::cerr << "Error opening file: " << file << std::endl;
		return false;
	}
	std::string data(std::istreambuf_iterator<char>(in), {});
	in.close();

	rapidjson::Document document;
	rapidjson::ParseResult result = document.Parse(data.data(), data.length());

	if (!result) {
		std::cerr << "Error parsing document: " << rapidjson::GetParseError_En(result.Code()) << " (" << result.Offset() << ")" << std::endl;
		return false;
	}

	if (rapidconfig::Status status = section.load(document); status != rapidconfig::Status::Ok) {
		std::cerr << "Error loading document: " << rapidconfig::to_string(status) << std::endl;
		return false;
	}

	// update last_value
	section.modified();

	return true;
}

bool ConfigManager::save() {
	std::ofstream out(file, std::ios_base::binary | std::ios::trunc);
	if (!out.is_open()) {
		std::cerr << "Error opening file: " << file << std::endl;
		return false;
	}

	out << section.dumps();
	out.close();

	return true;
}