#pragma once
#include "GraphicsCommandList.h"
struct Shader;

class GCmdSetPixelShader : GraphicsCommandBase
{
public:
	GCmdSetPixelShader(const std::string_view& aShaderName);
	~GCmdSetPixelShader() override = default;
	void Execute() override;
	void Destroy() override;
private:
	std::string_view myShaderName;
};

