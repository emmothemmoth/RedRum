#pragma once

#include "GraphicsCommandList.h"
#include "../Objects/DebugColor.h"
#include "../Objects/Vertex.h"

#include "../../Utilities/CommonUtilities/Matrix4x4.hpp"

class DebugLineObject;

class GCmdDrawDebugLines : public GraphicsCommandBase
{
public:
	GCmdDrawDebugLines(std::shared_ptr<DebugLineObject> someDebugLines, const CU::Matrix4x4f& aTransform);
	~GCmdDrawDebugLines() override;


	void Execute() override;
	void Destroy() override;
private:
	std::shared_ptr<DebugLineObject> myDebugObject;
	CU::Matrix4x4f myTransform;
	std::string myDebugName;
	bool myHasBones = false;
};
