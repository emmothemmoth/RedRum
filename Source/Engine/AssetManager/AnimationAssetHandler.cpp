#include "AssetManager.pch.h"
#include "AnimationAssetHandler.h"

#include "AnimationAsset.h"

#include "CommonUtilities\UtilityFunctions.hpp"

#include "TGAFbx.h"
#include "TgaFbxStructs.h"

bool AnimationAssetHandler::LoadAnimationFromFBX(const std::filesystem::path& aPath, AnimationAsset& inOutAsset)
{
    TGA::FBX::Animation animation;
    std::wstring contentPath = L"Content\\" + aPath.wstring();
    if (TGA::FBX::Importer::LoadAnimationW(contentPath, animation))
    {
        inOutAsset.SetName(animation.Name);
        inOutAsset.SetDuration(animation.Duration);
        inOutAsset.SetFPS(animation.FramesPerSecond);
        inOutAsset.SetLength(animation.Length);

        //Push all events
        for (int eventIndex = 0; eventIndex < animation.EventNames.size(); ++eventIndex)
        {
            inOutAsset.PushEvent(animation.EventNames[eventIndex]);
        }

        //Load all frames
        for (int frameIndex = 0; frameIndex < animation.Frames.size(); ++frameIndex)
        {
            Frame newFrame;
            //Load all global transforms
            for (std::pair<std::string, TGA::FBX::Matrix> pair : animation.Frames[frameIndex].GlobalTransforms)
            {
                CommonUtilities::Matrix4x4<float> globalMatrix;
                CommonUtilities::TGAFBXMatrix4ToCUMatrix4(pair.second, globalMatrix);
                newFrame.GlobalTransforms.insert({ pair.first, globalMatrix });
            }
            //Load all local transforms for bones
            for (std::pair<std::string, TGA::FBX::Matrix> pair : animation.Frames[frameIndex].LocalTransforms)
            {
                CommonUtilities::Matrix4x4<float> localTransform;
                CommonUtilities::TGAFBXMatrix4ToCUMatrix4(pair.second, localTransform);
                newFrame.LocalTransforms.insert({ pair.first, localTransform });
            }
            //Load all triggered events
            for (std::pair<std::string, bool> pair : animation.Frames[frameIndex].TriggeredEvents)
            {
                newFrame.TriggeredEvents.insert({ pair.first, pair.second });
            }
            //load all socket transforms
            for (std::pair<std::string, TGA::FBX::Matrix> pair : animation.Frames[frameIndex].SocketTransforms)
            {
                CommonUtilities::Matrix4x4<float> socketTransform;
                CommonUtilities::TGAFBXMatrix4ToCUMatrix4(pair.second, socketTransform);
                newFrame.SocketTransforms.insert({ pair.first, socketTransform });
            }
            inOutAsset.PushFrame(newFrame);

        }

        return true;
    }
    return false;
}
