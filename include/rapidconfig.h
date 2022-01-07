#pragma once
#include <string>
#include <vector>
#include <rapidjson/fwd.h>

namespace rapidconfig {
	enum class Status {
		Ok,
		TypeMismatch,
	};

	std::string to_string(Status status);

	class ValueBase {
	public:
		virtual std::string dumps() = 0;
		virtual Status load(rapidjson::Value&) = 0;
		virtual bool modified() = 0;
	};

	template<class T>
	class TypeBase : public ValueBase {
	protected:
		T value;
		T last_value;
	public:
		using Type = T;
		inline TypeBase(const T& data) : value(data), last_value(data) {}
		inline operator T& () {
			return value;
		}
	};

	struct string : public TypeBase<std::string> {
		using TypeBase::TypeBase;
		bool modified() override;
		std::string dumps() override;
		Status load(rapidjson::Value&) override;
	};

	struct boolean : public TypeBase<bool> {
		using TypeBase::TypeBase;
		bool modified() override;
		std::string dumps() override;
		Status load(rapidjson::Value&) override;
	};

	struct number : public TypeBase<double> {
		using TypeBase::TypeBase;
		bool modified() override;
		std::string dumps() override;
		Status load(rapidjson::Value&) override;
	};

	struct numberarray : public TypeBase<std::vector<double>> {
		using TypeBase::TypeBase;
		bool modified() override;
		std::string dumps() override;
		Status load(rapidjson::Value&) override;
	};

	class Section : public ValueBase {
	private:
		std::vector<std::pair<const char*, ValueBase*>> members;
	public:
		std::string dumps() override;
		bool modified() override;
		Status load(rapidjson::Value&) override;
		template<class S, typename T>
		inline S& add_member(const char* name, T def) {
			S& data = *new S(def);
			members.push_back({ name, &data });
			return data;
		}
		template<class ST>
		inline ST& add_section(const char* name) {
			ST& section = *new ST();
			members.push_back({ name, &section });
			return section;
		}
		~Section();
	};
};

// All rapidconfig macros are prefixed with RC

// Allocate a string value within a rapidconfig::Section.
#define RCAddString(NAME, DEFAULT) std::string& NAME = add_member<::rapidconfig::string>(#NAME, DEFAULT)

// Allocate a boolean value within a rapidconfig::Section.
#define RCAddBoolean(NAME, DEFAULT) bool& NAME = add_member<::rapidconfig::boolean>(#NAME, DEFAULT)

// Allocate a bumber value within a rapidconfig::Section.
#define RCAddNumber(NAME, DEFAULT) double& NAME = add_member<::rapidconfig::number>(#NAME, DEFAULT)

// Allocate an array of numbers within a rapidconfig::Section.
#define RCAddNumberArray(NAME, ...) std::vector<double>& NAME = add_member<::rapidconfig::numberarray>(#NAME, std::vector<double>(__VA_ARGS__))

// Allocate a structure and member within a rapidconfig::Section.
#define RCAddSection(NAME, STRUCT) struct section_##NAME## : public ::rapidconfig::Section STRUCT; section_##NAME##& NAME = add_section<section_##NAME##>(#NAME);

// Allocate a member within a rapidconfig::Section.
#define RCAddTSection(NAME, STRUCT) STRUCT& NAME = add_section<##STRUCT##>(#NAME);

// Create a Config::section extension.
#define RCCreateSection(NAME, STRUCT) struct NAME : public ::rapidconfig::Section STRUCT;