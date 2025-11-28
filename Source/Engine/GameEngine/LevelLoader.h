#pragma once
#include <filesystem>

struct Level;

class LevelLoader
{
public:
	LevelLoader() = default;
	~LevelLoader() = default;

	bool LoadLevelFromJSON(const std::filesystem::path& aLevelPath, Level& inOutLevel);

private:
	

};
