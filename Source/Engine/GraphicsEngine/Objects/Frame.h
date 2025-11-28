#pragma once
#include <string>

#include "CommonUtilities\Matrix4x4.hpp"
#include <unordered_map>


struct Frame
{

	std::unordered_map<std::string, CommonUtilities::Matrix4x4<float>> GlobalTransforms;

	std::unordered_map<std::string, CommonUtilities::Matrix4x4<float>> LocalTransforms;

	std::unordered_map<std::string, bool> TriggeredEvents;

	std::unordered_map<std::string, CommonUtilities::Matrix4x4<float>> SocketTransforms;
};
