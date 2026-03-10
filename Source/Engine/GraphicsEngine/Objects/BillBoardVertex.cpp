#include "GraphicsEngine.pch.h"
#include "BillBoardVertex.h"

const std::vector<VertexElementDesc> BillboardVertex::InputLayoutDefinition =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, false},
	{"TEXCOORDFIRST", 0, DXGI_FORMAT_R32G32_FLOAT, false},
	{"TEXCOORDSECOND", 0, DXGI_FORMAT_R32G32_FLOAT ,false},
};
