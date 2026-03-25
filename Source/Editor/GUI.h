#pragma once

#include "GuiCmd.h"
#include "EditorInterface/EditorInterface.h"

#include "../Engine/GameEngine/Events/MulticastDelegate.h"
#include "../Engine/External/DearImGui/imgui.h"

#include "Windows.h"
#include <memory>
#include <vector>
#include <array>
#include <filesystem>
#include <unordered_map>
#include <queue>


struct ID3D11Device;
struct ID3D11DeviceContext;
class TextureAsset;

using FOnViewportResize = MulticastDelegate<CU::Vector2U>;

struct GuiFrameData
{
    ImDrawData DrawData{};
    std::vector<ImDrawList*> ClonedLists;

    // The destructor automatically handles cleanup when the Render thread is done!
    ~GuiFrameData()
    {
        for (ImDrawList* list : ClonedLists)
        {
            IM_DELETE(list);
        }
        ClonedLists.clear();
    }
};

class GUI
{
public:
	GUI() = default;
	~GUI() = default;

	void Init(HWND aWindowHandle, ID3D11Device* aDX11Device, ID3D11DeviceContext* aDX11Context, const CU::Vector2f& aResolution);

    void SetRenderTexture(std::shared_ptr<TextureAsset> aTexture) { myViewportTexture = aTexture; }

	void Update(const float aDeltatime);
    void CaptureDrawData(GuiFrameData& outFrame);
			
	void Render(const GuiFrameData& aFrame);

	void ShutDown();

    bool IsViewportActive() const { return myViewportHovered && myViewportFocused; }

    EditorInterface& GetInterface() { return myInterface; }

    FOnViewportResize OnViewportResize;
private:
    void DisplayViewport(const float aDeltaTime);
    void DisplayInspector();
    void DisplayContentBrowser();

private:
    EditorInterface myInterface;
	CU::Vector2f myResolution;
    CU::Vector2<unsigned> myCurrentViewportSize = { 1600, 900 };
    CU::Vector2<unsigned> myPendingViewportSize = { 1600, 781 };
    float myResizeTimer = 0.0f;
    std::shared_ptr<TextureAsset> myViewportTexture;
    bool myViewportHovered = false;
    bool myViewportFocused = false;
};
