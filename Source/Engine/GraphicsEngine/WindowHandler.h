#pragma once

class SplashWindow;


class WindowHandler
{
public:
	WindowHandler();
	~WindowHandler();

	bool Init(SIZE aWindowSize, WNDPROC aWindowProcess, LPCWSTR aWindowTitle);

	HWND GetWindowHandle() const;
	SIZE GetWindowSize() const;

	void ShowSplashWindow();
	void HideSplashWindow();

private:
	SIZE myWindowSize{ 0,0 };
	HWND myWindowHandle{};
	HINSTANCE myModuleHandle = nullptr;
	SplashWindow* mySplashWindow;

};