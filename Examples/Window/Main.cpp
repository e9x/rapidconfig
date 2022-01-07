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
	void UpdateTime(bool force = false);
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

void show_examples() {
	std::cout
		<< "Enter a command ID." << std::endl
		<< "1.Toggle show time[" << booltxt(config.Interface.ShowTime) << "]" << std::endl;
}

int main() {
	manager.file = "Config.json";
	manager.load();

	Window::Open = true; 
	std::thread(resolve_choice).detach();
	std::thread wt(Window::Thread);
	
	show_examples();

	while (Window::Open) {
		if (res_choice == -1) continue;
		
		int choice = res_choice;
		res_choice = -1;

		switch (choice) {
		case 1:
			config.Interface.ShowTime ^= 1;
			Window::UpdateTime(true);
			break;
		}

		std::cout << std::endl;
		show_examples();
	}

	wt.join();
	if (config.modified()) manager.save();

	return 0;
}