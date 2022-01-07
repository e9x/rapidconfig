#include "./Config.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <CommCtrl.h>
#include <iostream>

namespace Window {
	bool Open = false;

	HWND window = NULL;
	HWND last_open = NULL;
	HWND current_time = NULL;

	LRESULT wndproc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
		switch (msg) {
		case WM_MOVE:
			config.Window.Dimensions[0] = LOWORD(lp);
			config.Window.Dimensions[1] = HIWORD(lp);
			break;
		case WM_SIZE:
			config.Window.Dimensions[2] = LOWORD(lp);
			config.Window.Dimensions[3] = HIWORD(lp);
			break;
		case WM_CLOSE:
			DestroyWindow(window);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		}

		return DefWindowProcW(hwnd, msg, wp, lp);
	}

	const wchar_t* register_class(const wchar_t* cls) {
		WNDCLASS wc{};

		wc.lpszClassName = cls;
		wc.lpfnWndProc = wndproc;

		RegisterClassW(&wc);

		return cls;
	}

	void Update() {
		if (config.Window.LastOpen == -1) {
			SetWindowTextW(last_open, L"This window wasn't opened before.");
		}
		else {
			double now = (double)time(0);
			std::wstring text = L"This window was opened " + std::to_wstring(now - config.Window.LastOpen) + L" seconds ago.";
			SetWindowTextW(last_open, text.c_str());
		}
	}

	bool Create() {
		std::vector<double>& dims = config.Window.Dimensions;
		window = CreateWindowExW(0, register_class(L"Test Window Class"), L"Test Window", WS_OVERLAPPEDWINDOW, dims[0], dims[1], dims[2], dims[3], NULL, NULL, NULL, NULL);

		if (window == NULL) {
			std::cerr << "window was NULL. GLE was " << GetLastError() << std::endl;
			return false;
		}

		last_open = CreateWindowExW(0, WC_STATICW, NULL, WS_CHILD | WS_VISIBLE, 10, 10, 100, 20, NULL, NULL, NULL, NULL);

		if (window == NULL) {
			std::cerr << "last_open was NULL. GLE was " << GetLastError() << std::endl;
			return false;
		}

		Update();

		UpdateWindow(window);
		ShowWindow(window, SW_SHOW);

		return true;
	}

	int Thread() {
		Create();

		MSG msg;
		while (GetMessageW(&msg, NULL, NULL, NULL)) {
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		return (int)msg.wParam;
	}
};