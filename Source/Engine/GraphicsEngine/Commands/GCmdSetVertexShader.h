#pragma once
#include "GraphicsCommandList.h"
struct Shader;

class GCmdSetVertexShader : GraphicsCommandBase
{
public:
	GCmdSetVertexShader(const std::string_view& aShaderName);
	~GCmdSetVertexShader() override = default;
	void Execute() override;
	void Destroy() override;
private:
	std::string_view myShaderName;
};