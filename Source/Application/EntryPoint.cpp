#define WIN32_LEAN_AND_MEAN
#include "Windows.h"

#include "resource.h"

#include "..\GraphicsEngine\GraphicsEngine.h"
#include "..\AssetManager\AssetManager.h"
#include "../Editor/Editor.h"

#include "CommonUtilities\InputMapper.h"
#include "CommonUtilities\Timer.h"
#include "Application.h"
#include "StringHelpers.h"

#include "../Engine/External/DearImGui/imgui.h"
#include "../Engine/External/DearImGui/imgui_impl_win32.h"
#include "../Engine/External/DearImGui/imgui_impl_dx11.h"

#include <shellapi.h>
#include <thread>

int GuardedMain();

int main()
{
    GuardedMain();
    return 0;
}

LRESULT CALLBACK WinProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

void GetScreenResolution(LONG& aWidth, LONG& aHeigth)
{
    RECT desktop;
    const HWND hDesktop = GetDesktopWindow();
    GetWindowRect(hDesktop, &desktop);
    aWidth = desktop.right;
    aHeigth = desktop.bottom;
}

bool CheckForCommandLineArgs(std::string& inOutArg)
{
    LPWSTR* szArglist;
    int nArgs;

    szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);

    if (nArgs < 2)
    {
        return false;
    }

    LPWSTR level = GetCommandLineW();
    std::string levelStr;
    UINT codepage = CP_ACP;
    char* p = 0;
    int bsz;

    bsz = WideCharToMultiByte(codepage,
        0,
        level, -1,
        0, 0,
        0, 0);
    if (bsz > 0)
    {
        p = new char[bsz];
        int rc = WideCharToMultiByte(codepage,
            0,
            level, -1,
            p, bsz,
            0, 0);
        if (rc != 0)
        {
            p[bsz - 1] = 0;
            levelStr = p;
        }
    }
    size_t spacePos = levelStr.find(" ");
    inOutArg = std::string(levelStr.begin() + spacePos + 1, levelStr.end());
    delete[] p;
    return true;

}

bool VerifyFile(std::string_view aFile)
{
    std::filesystem::path file = aFile;
    if (aFile.empty())
    {
        return false;
    }
    if (!std::filesystem::exists(aFile))
    {
        return false;
    }
    if (!file.has_extension() || !file.has_filename())
    {
        return false;
    }
    return true;
}

int GuardedMain()
{
    LONG width, heigth;
    GetScreenResolution(width, heigth);
    SIZE windowSize = { width, heigth };
    constexpr LPCWSTR windowTitle = L"Audio Editor";
    GraphicsEngine::Get().InitWindow(windowSize, WinProc, windowTitle);
    GraphicsEngine::Get().ShowSplashScreen();
    GraphicsEngine::Get().Initialize(true);
    CommonUtilities::Input::Init();


    const std::filesystem::path startDir = std::filesystem::current_path();
    const std::filesystem::path contentDir = startDir / "Content";
    AssetManager::Get().Init(contentDir);


    Editor editor;
    editor.Init();

    GraphicsEngine::Get().HideSplashScreen();

    std::thread updateThread(&Editor::UpdateLoop, &editor);
    editor.Run();
    updateThread.join();
    editor.ShutDown();

    return 0;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    try
    {
        return GuardedMain();
    }
    catch (const std::exception& e)
    {

        std::string message = e.what();
        if (!str::is_valid_utf8(message))
        {
            message = str::acp_to_utf8(message);
        }
        //LOG(MVLog, Error, "Exception caught!\n{}", message);
        return -1;
    }
}
LRESULT CALLBACK WinProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
    extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return true;
    CommonUtilities::Input::BuiltInWinProc(hWnd, uMsg, wParam, lParam);
    if (uMsg == WM_DESTROY || uMsg == WM_CLOSE)
    {
        PostQuitMessage(0);
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}