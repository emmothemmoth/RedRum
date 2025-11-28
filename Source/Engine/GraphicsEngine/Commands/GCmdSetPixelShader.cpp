#include "GraphicsEngine.pch.h"
#include "GCmdSetPixelShader.h"

#include "GraphicsEngine.h"

GCmdSetPixelShader::GCmdSetPixelShader(const std::string_view& aShaderName)
{
	myShaderName = aShaderName;
}

void GCmdSetPixelShader::Execute()
{
	GraphicsEngine::Get().SetPixelShader(myShaderName);
}

void GCmdSetPixelShader::Destroy()
{
}
