#pragma once
#include "GraphicsCommandList.h"
#include "../../AudioEngine/RoomSimulator/AcousticData.h"

#include <vector>
#include <future>

class GCmdComputeAcoustics : public GraphicsCommandBase
{
public:
	explicit GCmdComputeAcoustics(std::vector<GPURay> someRays, std::vector<GPUObstacle> someObstacles, std::shared_ptr<std::promise<std::vector<GPURayResult>>> aPromise);
	~GCmdComputeAcoustics() override = default;
	void Execute() override;
	void Destroy() override;

private:
	std::vector<GPURay> myRays;
	std::vector<GPUObstacle> myObstacles;
	std::shared_ptr<std::promise<std::vector<GPURayResult>>> myPromise;
};
