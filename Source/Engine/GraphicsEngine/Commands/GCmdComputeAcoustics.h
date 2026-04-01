#pragma once
#include "GraphicsCommandList.h"
#include "../../AudioEngine/RoomSimulator/AcousticData.h"

#include <vector>
#include <future>

class GCmdComputeAcoustics : public GraphicsCommandBase
{
public:
	explicit GCmdComputeAcoustics(std::vector<GPURay> rays, std::vector<GPUObstacle> obs,
		CU::Vector3f lPos, CU::Vector3f lRight, float lRad,
		std::shared_ptr<std::promise<std::vector<GPURayResult>>> promise);
	~GCmdComputeAcoustics() override = default;
	void Execute() override;
	void Destroy() override;

private:
	std::vector<GPURay> myRays;
	std::vector<GPUObstacle> myObstacles;
	CU::Vector3f myListenerPos;
	CU::Vector3f myListenerRight;
	float myListenerRadius;
	std::shared_ptr<std::promise<std::vector<GPURayResult>>> myPromise;
};
