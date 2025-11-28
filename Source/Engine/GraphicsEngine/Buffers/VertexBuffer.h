#pragma once
#include "wrl.h"

//remove
#include <d3d11.h>

#include <string>

struct VertexBuffer
{
	Microsoft::WRL::ComPtr<ID3D11Buffer> Buffer;
	std::string Name;
	size_t NumVertices = 0;
	size_t DataSize = 0;
	size_t VertexStride = 0;
};