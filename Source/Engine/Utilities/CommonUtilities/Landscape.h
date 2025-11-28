#pragma once
#include "Sprite.h"
#include "PerlinNoise.h"

class Landscape
{
public:
	Landscape();
	~Landscape();

private:
	CommonUtilities::PerlinNoise myPerlinNoise;
};

Landscape::Landscape()
{
}

Landscape::~Landscape()
{
}
