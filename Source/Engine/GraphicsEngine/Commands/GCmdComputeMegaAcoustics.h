#pragma once
#include "GraphicsCommandList.h"
#include "../../AudioEngine/RoomSimulator/AcousticData.h"

#include <vector>
#include <future>


class GCmdComputeMegaAcoustics : public GraphicsCommandBase
{
public:
	explicit GCmdComputeMegaAcoustics(
		std::vector<GPURay> rays,
		std::vector<GPUObstacle> obs,
		std::vector<CU::Vector3f> probes,
		float probeRadius,
		CU::Vector3f lRight,
		std::shared_ptr<std::promise<std::vector<GPUMegaHit>>> promise);
	~GCmdComputeMegaAcoustics() override = default;
	void Execute() override;
	void Destroy() override;

private:
	std::vector<GPURay> myRays;
	std::vector<GPUObstacle> myObstacles;
	std::vector<CU::Vector3f> myProbes;
	CU::Vector3f myListenerRight;
	float myProbeRadius;
	std::shared_ptr<std::promise<std::vector<GPUMegaHit>>> myPromise;
};
