#include "GraphicsEngine.pch.h"
#include "GCmdSetVertexShader.h"

#include "GraphicsEngine.h"

GCmdSetVertexShader::GCmdSetVertexShader(const std::string_view& aShaderName)
{
	myShaderName = aShaderName;
}

void GCmdSetVertexShader::Execute()
{
	GraphicsEngine::Get().SetVertexShader(myShaderName);
}

void GCmdSetVertexShader::Destroy()
{
}
