#pragma once

#include "GraphicsCommandList.h"

#include "CommonUtilities/Matrix4x4.hpp"

class TextureAsset;
class GCmdDrawBillboard : public GraphicsCommandBase
{
public:
	GCmdDrawBillboard(std::shared_ptr<TextureAsset> aTexture, const CU::Matrix4x4f& aTransform);
	~GCmdDrawBillboard() override;

	void Execute() override;
	void Destroy() override;
private:
	std::shared_ptr<TextureAsset> myTexture;
	CU::Matrix4x4f myTransform;
};
