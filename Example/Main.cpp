#include "./Config.h"
#include <iostream>
#include <thread>

MainConfig config;
ConfigManager manager(config);

const char* booltxt(bool b) {
	return b ? "true" : "false";
}

namespace Window {
	extern bool open;
	int Thread();
	bool Create();
	void Update();
};

int main() {
	manager.file = "Config.json";
	manager.load();

	if (!Window::Create()) return 1;

	std::thread wt(Window::Thread);

	while (Window::open) {
		// TODO: add more examples
		std::cout
			<< "Enter a command ID." << std::endl
			<< "1.Toggle show time[" << booltxt(config.Interface.ShowTime) << "]" << std::endl;
		
		int choice = 0;
		std::cin >> choice;
		
		switch (choice) {
		case 1:
			config.Interface.ShowTime ^= 1;
			break;
		}

		if (config.modified()) manager.save();

		Window::Update();

		std::cout << std::endl;
	}

	wt.join();
	if (config.modified()) manager.save();

	return 0;
}