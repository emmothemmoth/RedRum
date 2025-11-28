#pragma once

class IAsset
{
public:
	IAsset() = default;
	~IAsset() = default;

	virtual bool Load() = 0;
};
