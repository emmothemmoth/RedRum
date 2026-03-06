#pragma once
#include "WorldBounds.h"
#include "Components/GameObject.h"

#include <memory>
#include <vector>

struct Level
{
	std::vector <std::shared_ptr<GameObject>> GameObjects;
	std::shared_ptr<GameObject> Camera;
	WorldBounds Bounds;
};
