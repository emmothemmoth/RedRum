#include "AssetManager.pch.h"
#include "TextureAssetHandler.h"
#include "TextureAsset.h"
#include <filesystem>

#include "..\GraphicsEngine\GraphicsEngine.h"

bool TextureAssetHandler::LoadTextureFromFile(const std::wstring& aPath, TextureAsset& inOutAsset) const
{
    std::wstring contentPath = L"Content\\" + aPath;
    const std::filesystem::path texturePath = contentPath;
    if (GraphicsEngine::Get().LoadTexture(texturePath, inOutAsset))
    {
        return true;
    }
    return false;
}
