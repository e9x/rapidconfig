#include "./Config.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <CommCtrl.h>
#include <iostream>
#include <thread>

namespace Window {
	bool Open = false;

	HWND window = NULL;
	HWND last_open = NULL;
	HWND current_time = NULL;

	void update_dims() {
		RECT rect;
		GetWindowRect(window, &rect);

		config.Window.Dimensions[0] = rect.left;
		config.Window.Dimensions[1] = rect.top;
		config.Window.Dimensions[2] = rect.right - rect.left;
		config.Window.Dimensions[3] = rect.bottom - rect.top;
	}

	LRESULT wndproc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
		switch (msg) {
		case WM_MOVE:
			update_dims();
			break;
		case WM_SIZE:
			update_dims();
			break;
		case WM_CLOSE:
			DestroyWindow(window);
			break;
		case WM_DESTROY:
			Open = false;
			PostQuitMessage(0);
			break;
		}

		return DefWindowProcW(hwnd, msg, wp, lp);
	}

	const wchar_t* register_class(const wchar_t* cls) {
		WNDCLASS wc{};

		wc.lpszClassName = cls;
		wc.lpfnWndProc = wndproc;
		wc.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
		wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
		RegisterClassW(&wc);

		return cls;
	}

	void UpdateOpen() {
		double now = (double)time(0);

		if (config.Window.LastOpen == -1) {
			SetWindowTextW(last_open, L"This window wasn't opened before.");
		}
		else {
			std::wstring text = L"This window was last opened " + std::to_wstring(now - config.Window.LastOpen) + L" seconds ago.";
			SetWindowTextW(last_open, text.c_str());
		}

		config.Window.LastOpen = now;
	}

	double last_time = 0;

	void UpdateTime(bool force = false) {
		double now = (double)time(0);

		if (!force && last_time == now) return;
		last_time = now;

		if (config.Interface.ShowTime) {
			std::wstring text = L"The time is " + std::to_wstring(now);
			SetWindowTextW(current_time, text.c_str());
		}
		else {
			SetWindowTextW(current_time, L"");
		}
	}

	bool Create() {
		std::vector<double>& dims = config.Window.Dimensions;
		window = CreateWindowExW(0, register_class(L"Test Window Class"), L"Test Window", WS_OVERLAPPEDWINDOW, dims[0], dims[1], dims[2], dims[3], NULL, NULL, NULL, NULL);

		if (window == NULL) {
			std::cerr << "window was NULL. GLE was " << GetLastError() << std::endl;
			return false;
		}

		last_open = CreateWindowExW(0, WC_STATICW, NULL, WS_CHILD | WS_VISIBLE, 10, 10, 400, 25, window, NULL, NULL, NULL);
		current_time = CreateWindowExW(0, WC_STATICW, NULL, WS_CHILD | WS_VISIBLE, 10, 40, 400, 25, window, NULL, NULL, NULL);

		UpdateOpen();
		UpdateTime();

		UpdateWindow(window);
		ShowWindow(window, SW_SHOW);

		return true;
	}

	int Thread() {
		Create();
		bool run = true;

		std::thread timet([&run]() {
			while (run) {
				UpdateTime();
			}
		});

		MSG msg;
		while (GetMessageW(&msg, NULL, NULL, NULL)) {
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		run = false;
		timet.join();

		return (int)msg.wParam;
	}
};