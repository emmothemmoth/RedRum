#include "GraphicsEngine.pch.h"
#include "GCmdSetFrameBuffer.h"

#include "Buffers\ConstantBuffer.h"
#include "Buffers\FrameBuffer.h"
#include "GraphicsEngine.h"


GCmdSetFrameBuffer::GCmdSetFrameBuffer(const std::shared_ptr<FrameBufferData> aFrameBuffer)
{
	myFrameBuffer = aFrameBuffer;
}

void GCmdSetFrameBuffer::Execute()
{
	GraphicsEngine::Get().UpdateAndSetConstantBuffer(ConstantBufferType::FrameBuffer, *myFrameBuffer);
}

void GCmdSetFrameBuffer::Destroy()
{
	myFrameBuffer = nullptr;
}
