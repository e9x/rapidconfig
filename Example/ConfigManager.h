#include <filesystem>
#include <rapidconfig.h>

class ConfigManager {
private:
	rapidconfig::Section& section;
public:
	std::filesystem::path file;
	ConfigManager(rapidconfig::Section& section);
	bool load();
	bool save();
};