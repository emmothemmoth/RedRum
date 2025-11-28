#include "GraphicsEngine.pch.h"
#include "WindowHandler.h"

#include "Windows\SplashWindow.h"

WindowHandler::WindowHandler()
{
}

WindowHandler::~WindowHandler()
{
}

bool WindowHandler::Init(SIZE aWindowSize, WNDPROC aWindowProcess, LPCWSTR aWindowTitle)
{
    // Redirect stdout and stderr to the console.
    FILE* consoleOut;
    FILE* consoleErr;
    AllocConsole();
    freopen_s(&consoleOut, "CONOUT$", "w", stdout);  // NOLINT(cert-err33-c)
    setvbuf(consoleOut, nullptr, _IONBF, 1024);  // NOLINT(cert-err33-c)
    freopen_s(&consoleErr, "CONOUT$", "w", stderr);  // NOLINT(cert-err33-c)
    setvbuf(consoleErr, nullptr, _IONBF, 1024);  // NOLINT(cert-err33-c)

    /*
     * A note about strings:
     * Strings in C++ are terrible things. They come in a variety of formats which causes issues.
     * Many modern libraries expect UTF-8 (ImGui, FMOD, FBX SDK, etc) but Windows defaults to UTF-16LE
     * which is not compatible 1=1.
     *
     * To avoid weird problems with compatibility, mangled characters and other problems it is highly
     * recommended to store everything as UTF-8 encoded strings whenever possible. This means that
     * when we store i.e. the name of a Model, Level, Sound File, etc we do so in a UTF-8 formatted
     * std::string, and when we need to communicate with the Windows (or DirectX) API we need to use
     * std::wstring which represents a UTF-16LE string.
     *
     * There are functions available for conversion between these formats in the str namespace in
     * StringHelpers.h.
     *
     * The provided Logging library expects UTF-8 format strings which should provide minimal headaches
     * for any involved situation. For anything non-unicode (like non swedish signs, accents, etc) you
     * can just use normal strings as you would anywhere since UTF-8 is backwards compatible with ASCii
     *
     * SetConsoleOutputCP(CP_UTF8) tells the Windows Console that we'll output UTF-8. This DOES NOT
     * affect file output in any way, that's a whole other can of worms. But if you always write and
     * read your strings in the same format, and always treat them as byte blocks, you'll be fine.
     */
    SetConsoleOutputCP(CP_UTF8);

    const HWND consoleWindow = GetConsoleWindow();
    RECT consoleSize;
    GetWindowRect(consoleWindow, &consoleSize);
    MoveWindow(consoleWindow, consoleSize.left, consoleSize.top, 1280, 720, true);

    myModuleHandle = GetModuleHandleW(NULL);
    constexpr LPCWSTR windowClassName = L"MainWindow";

    // First we create our Window Class
    WNDCLASS windowClass = {};
    windowClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
    windowClass.lpfnWndProc = aWindowProcess;
    windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    windowClass.lpszClassName = windowClassName;
    RegisterClass(&windowClass);

    // Then we use the class to create our window
    myWindowHandle = CreateWindow(
        windowClassName,                                // Classname
        aWindowTitle,                                    // Window Title
        WS_OVERLAPPEDWINDOW | WS_POPUP,    // Flags
        (GetSystemMetrics(SM_CXSCREEN) - aWindowSize.cx) / 2,
        (GetSystemMetrics(SM_CYSCREEN) - aWindowSize.cy) / 2,
        aWindowSize.cx,
        aWindowSize.cy,
        nullptr, nullptr, nullptr,
        nullptr
    );

    myWindowSize = aWindowSize;

    return true;
}

HWND WindowHandler::GetWindowHandle() const
{
    return myWindowHandle;
}

SIZE WindowHandler::GetWindowSize() const
{
    return myWindowSize;
}

void WindowHandler::ShowSplashWindow()
{
    if (!mySplashWindow)
    {
        mySplashWindow = new SplashWindow();
        mySplashWindow->Init(myModuleHandle);
    }
}

void WindowHandler::HideSplashWindow()
{
    mySplashWindow->Close();
    delete mySplashWindow;
    ShowWindow(myWindowHandle, SW_SHOW);
    SetForegroundWindow(myWindowHandle);
}

