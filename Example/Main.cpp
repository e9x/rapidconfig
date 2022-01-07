#include "./Config.h"
#include <iostream>
#include <thread>

MainConfig config;
ConfigManager manager(config);

const char* booltxt(bool b) {
	return b ? "true" : "false";
}

namespace Window {
	extern bool Open;
	int Thread();
	bool Create();
	void Update();
};

int res_choice = -1;

void resolve_choice() {
	while (Window::Open) {
		std::cin >> res_choice;

		if (std::cin.fail()) {
			std::cin.clear();
			std::cin.ignore();
		}
	}
}

int main() {
	manager.file = "Config.json";
	manager.load();

	Window::Open = true; 
	std::thread(resolve_choice).detach();
	std::thread wt(Window::Thread);
	
	while (Window::Open) {
		// TODO: add more examples
		std::cout
			<< "Enter a command ID." << std::endl
			<< "1.Toggle show time[" << booltxt(config.Interface.ShowTime) << "]" << std::endl;

		while (res_choice == -1 && Window::Open);
		if (!Window::Open) break;;

		int choice = res_choice;
		res_choice = -1;

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