#include "GraphicsEngine.pch.h"
#include "GCmdSetDebugBuffer.h"
#include "GraphicsEngine.h"

GCmdSetDebugBuffer::GCmdSetDebugBuffer(std::shared_ptr<DebugBuffer> aBuffer)
{
	myBuffer = aBuffer;
}

void GCmdSetDebugBuffer::Execute()
{
	GraphicsEngine::Get().UpdateAndSetConstantBuffer<DebugBuffer>(ConstantBufferType::DebugBuffer, *myBuffer);
}

void GCmdSetDebugBuffer::Destroy()
{
	myBuffer = nullptr;
}
