#pragma once
#include "GraphicsCommandList.h"

class GCmdDrawQuad : public GraphicsCommandBase
{
public:
	GCmdDrawQuad(const CU::Vector2f& aSize = { 1904, 1041 });
	~GCmdDrawQuad() override = default;

	void Execute() override;
	void Destroy() override;
private:
	CU::Vector2f mySize;
};
