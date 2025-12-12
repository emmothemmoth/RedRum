#pragma once

class IAsset
{
public:
	IAsset() = default;
	virtual ~IAsset() = default;

	virtual bool Load() = 0;
};
