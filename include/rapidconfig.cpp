#include "./rapidconfig.h"
#include <rapidjson/document.h>

namespace rapidconfig {
	std::string to_string(Status status) {
		switch (status) {
		case Status::TypeMismatch: return "TypeMismatch"; break;
		case Status::Ok: return "Ok"; break;
		default: return "Unknown"; break;
		}
	}
	
	namespace Stringify {
		std::string Double(const double& value) {
			bool on_decimal = false;
			size_t last_char = 0;
			std::string tos = std::to_string(value);

			for (size_t i = 0; i < tos.length(); i++) {
				char c = tos[i];

				if (c == '.') {
					on_decimal = true;
				}
				else if (on_decimal) {
					if (c != '0') last_char = i;
				}
				else last_char = i;
			}

			if (on_decimal) tos.resize(last_char + 1);

			return tos;
		}

		std::string Boolean(const bool& value) {
			return value ? "true" : "false";
		}

		std::vector<std::pair<char, const char*>> escaped = {
			{'\\', "\\\\"},
			{'"', "\\\""},
			{'\n', "\\n"},
			{'\t', "\\t"},
			{'\b', "\\b"},
			{'\f', "\\f"},
			{'\r', "\\r"},
		};

		const char hex[] = "0123456789ABCDEF";

		std::string char_to_hex(unsigned char c) {
			std::string hex;

			hex += hex[c >> 4];
			hex += hex[c & 0x0f];

			return hex;
		}

		std::string pad_start(std::string string, int pad, char fill) {
			std::string output;
			size_t missing = pad - string.length();

			if (missing > 0) while (missing--) output += fill;

			output += string;

			return output;
		}

		std::string String(const std::string& value) {
			std::string ret;

			for (int index = 0; index < value.length(); index++) {
				char at = value[index];
				const char* find = nullptr;

				for (const auto& [c, v] : escaped) if (c == at) {
					find = v;
					break;
				}

				if (find) {
					// find is always 2 bytes
					ret += find[0];
					ret += find[1];

					// data = data.slice(0, index) + find + data.slice(index + 1);
					index += 2;
				}
				else if (at >= 0 && at <= 31) {
					// data = data.slice(0, index) + '\\u' + pad_start(char_to_hex(at), 4, '0') + data.slice(index + 1);
					ret += "\\u" + pad_start(char_to_hex(at), 4, '0');
					index += 5;
				}
				else ret += at;
			}

			return '"' + ret + '"';
		}
	};

	Section::~Section() {
		// free memory
		for (auto [key, value] : members) {
			delete value;
		}
	}

	bool Section::modified() {
		for (auto [key, value] : members) {
			if (value->modified()) return true;
		}

		return false;
	}

	std::string Section::dumps() {
		std::string ret = "{";

		bool end_comma = false;

		for (const auto [key, value] : members) {
			ret += Stringify::String(key);
			ret += ":";
			ret += value->dumps();
			ret += ",";
			end_comma = true;
		}

		if (end_comma) ret.pop_back();

		ret += "}";

		return ret;
	}

	Status Section::load(rapidjson::Value& json) {
		if (!json.IsObject()) return Status::TypeMismatch;

		for (auto& [_jkey, value] : json.GetObject()) {
			const char* jkey = _jkey.GetString();

			for (auto& [mkey, member] : members) {
				if (strcmp(mkey, jkey) != 0) continue;
				member->load(value);
				break;
			}
		}

		return Status::Ok;
	}

	bool string::modified() {
		if (value == last_value) return false;
		last_value = value;
		return true;
	}

	std::string string::dumps() {
		return Stringify::String(value);
	}

	Status string::load(rapidjson::Value& json) {
		if (!json.IsString()) return Status::TypeMismatch;
		value = std::string(json.GetString(), json.GetStringLength());
		return Status::Ok;
	}

	bool number::modified() {
		if (value == last_value) return false;
		last_value = value;
		return true;
	}

	std::string number::dumps() {
		return Stringify::Double(value);
	}

	Status number::load(rapidjson::Value& json) {
		if (!json.IsNumber()) return Status::TypeMismatch;
		value = json.GetDouble();
		return Status::Ok;
	}

	bool numberarray::modified() {
		if (last_value.size() == value.size() && std::memcmp(value.data(), last_value.data(), value.size() * sizeof(double)) == 0) return false;
		last_value = value;
		return true;
	}

	std::string numberarray::dumps() {
		std::string ret;
		for (double v : value) ret += Stringify::Double(v) + ",";
		if (ret[ret.length() - 1] == ',') ret.resize(ret.length() - 1);
		return "[" + ret + "]";
	}

	Status numberarray::load(rapidjson::Value& json) {
		if (!json.IsArray()) return Status::TypeMismatch;

		for (size_t i = 0; i < json.Size(); i++) {
			rapidjson::Value& v = json[(rapidjson::SizeType)i];
			if (!v.IsNumber()) return Status::TypeMismatch;
			value[i] = v.GetDouble();
		}

		return Status::Ok;
	}

	bool boolean::modified() {
		if (value == last_value) return false;
		last_value = value;
		return true;
	}

	std::string boolean::dumps() {
		return Stringify::Boolean(value);
	}

	Status boolean::load(rapidjson::Value& json) {
		if (!json.IsBool()) return Status::TypeMismatch;
		value = json.GetBool();
		return Status::Ok;
	}
}