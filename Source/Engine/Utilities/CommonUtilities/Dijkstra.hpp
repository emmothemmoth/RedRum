#include <vector>
#include <iostream>
#include <assert.h>



namespace CommonUtilities
{
	static const int MapWidth = 20;
	static const int MapHeight = 20;
	static const int TileCount = MapWidth * MapHeight;

	struct Node
	{
		std::vector<Node*> adjacents;
		int index;
		int weight;
	};

	struct PathData
	{
		unsigned int currDist;
		unsigned int previousIndex;
	};

	std::vector<int> Dijkstra(const std::vector<Tile>& aMap, int aStartIndex, int anEndIndex);
	void CalculateDistance(const std::vector<Node>& aGraph, std::vector<Node>& someVisited, std::vector<bool>& someUnVisited, std::vector<PathData>& someDistances, int aCurrentIndex);
	std::vector<Node> GetGraph(const std::vector<Tile>& aMap);
	std::vector<int> GetPath(const std::vector<PathData>& someDistances, int aStartIndex, int anEndIndex);

	std::vector<int> Dijkstra(const std::vector<Tile>& aMap, int aStartIndex, int anEndIndex)
	{
		assert(aStartIndex < aMap.size() && anEndIndex < aMap.size());
		std::vector<Node> graph = GetGraph(aMap);

		//std::vector<Node> unVisited = graph;
		//unVisited[aStartIndex].weight = 0;
		std::vector<bool> unVisitedNodes;
		unVisitedNodes.resize(graph.size());
		for (bool node : unVisitedNodes)
		{
			node = false;
		}

		std::vector<Node> visited;
		std::vector<PathData> distances;
		distances.resize(graph.size());
		for (int index = 0; index < distances.size(); ++index)
		{
			distances[index].currDist = INT_MAX;
		}
		distances[aStartIndex].currDist = 0;
		distances[aStartIndex].previousIndex = 0;

		CalculateDistance(graph, visited, unVisitedNodes, distances, aStartIndex);

		std::vector<int> path = GetPath(distances, aStartIndex, anEndIndex);
		return path;
	}

	void CalculateDistance(const std::vector<Node>& aGraph, std::vector<Node>& someVisited, std::vector<bool>& someUnVisited, std::vector<PathData>& someDistances, int aCurrentIndex)
	{
		someVisited.push_back(aGraph[aCurrentIndex]);
		someUnVisited[aCurrentIndex] = true;

		//update weights to neighbours
		for (int index = 0; index < aGraph[aCurrentIndex].adjacents.size(); ++index)
		{
			int adjacentIndex = aGraph[aCurrentIndex].adjacents[index]->index;
			if (someDistances[aCurrentIndex].currDist + aGraph[aCurrentIndex].adjacents[index]->weight < someDistances[adjacentIndex].currDist)
			{
				someDistances[adjacentIndex].currDist = someDistances[aCurrentIndex].currDist + aGraph[aCurrentIndex].adjacents[index]->weight;

				someDistances[adjacentIndex].previousIndex = aCurrentIndex;
			}
		}
		for (int index = 0; index < aGraph[aCurrentIndex].adjacents.size(); ++index)
		{
			//int adjacentIndex = aGraph[aCurrentIndex].adjacents[index]->index;
			if (someUnVisited[aGraph[aCurrentIndex].adjacents[index]->index] == false)
			{
				CalculateDistance(aGraph, someVisited, someUnVisited, someDistances, aGraph[aCurrentIndex].adjacents[index]->index);
			}

		}

	}

	std::vector<Node> GetGraph(const std::vector<Tile>& aMap)
	{
		//Create the graph
		std::vector<Node> graph;
		for (int index = 0; index < aMap.size(); ++index)
		{
			Node newNode = {};
			if (aMap[index] == Tile::Passable)
			{
				newNode.weight = 1;
			}
			else
			{
				newNode.weight = 1000;
			}
			newNode.index = index;
			graph.push_back(newNode);
		}

		//Init adjacents
		for (int yIndex = 0; yIndex < MapHeight; ++yIndex)
		{
			for (int xIndex = 0; xIndex < MapWidth; ++xIndex)
			{
				int currentIndex = yIndex * MapHeight + xIndex;

				//If not lowest row ->add below
				if ((MapWidth - 1) < currentIndex)
				{
					graph[currentIndex].adjacents.push_back(&graph[currentIndex - MapWidth]);
				}
				//if not highest row -> add above
				if (currentIndex < graph.size() - MapWidth)
				{
					graph[currentIndex].adjacents.push_back(&graph[currentIndex + MapWidth]);
				}
				//if not leftmost column -> add left
				if (0 < currentIndex % MapWidth)
				{
					graph[currentIndex].adjacents.push_back(&graph[currentIndex - 1]);
				}
				//if not rightmost column -> add right
				if (0 < currentIndex % (MapWidth - 1) || (currentIndex < 1 && -1 < currentIndex))
				{
					graph[currentIndex].adjacents.push_back(&graph[currentIndex + 1]);
				}
			}
		}
		return graph;
	}

	std::vector<int> GetPath(const std::vector<PathData>& someDistances, int aStartIndex, int anEndIndex)
	{
		std::vector<int> path;
		path.push_back(anEndIndex);
		int currentIndex = anEndIndex;
		while ((aStartIndex < currentIndex) || (currentIndex < aStartIndex))
		{
			path.push_back(someDistances[currentIndex].previousIndex);
			currentIndex = someDistances[currentIndex].previousIndex;
		}
		std::vector<int> correctPath;
		for (size_t index = path.size() - 1; index != 0; --index)
		{
			correctPath.push_back(path[index]);
		}
		correctPath.push_back(anEndIndex);
		return correctPath;
	}

}
