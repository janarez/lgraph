#include "LIntersect.h"
#include "GraphLoader.h"
#include "PartialOrder.h"

#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <ostream>
#include <algorithm>
#include <set>
#include <numeric>
#include <iostream>
#include <exception>
#include <istream>
#include <fstream>
#include <iterator>

// constructor
// takes in graph in the form of map of vertices to its neighbours

LIntersectionGraph::LIntersectionGraph(std::map<size_t, std::set<size_t>>& neighbors)
{
	// initializes all LShapes to zeroes;
	gl.neighbors = neighbors;
	// 0 index unused
	shapes.resize(neighbors.size()+1);
	max = shapes.size()-1;
	deduceDirections();
}

// -1 is left, 1 is right, 0 is undecide
void LIntersectionGraph::deduceDirections(void)
{
	// 0 index unused
	directions.resize(max+1);
	stops.resize(max + 1);
	std::iota(stops.begin(), stops.end(), 1);

	// 1 points to right, max point to left for sure
	for (auto const& i : gl.neighbors)
	{
		auto it = i.second.upper_bound(i.first);
		if (i.second.begin() == it)
		{
			directions[i.first] = 1;
		}
		else if (i.second.end() == it)
		{
			directions[i.first] = -1;
		}
	}
	return;
}

// this is a recursive functions that through direction guessing proceeds to find solution
// all possible directions are guessed and as such the complexity is O(2^N)
// first run with counter = 1 since dir[1] is set already
bool LIntersectionGraph::guessDirections(size_t counter)
{
	// marks valid directions configuration
	bool dirOK = true;
	// do guessing for all nonset directions
	for (size_t i = counter+1; i <= max; ++i)
	{
		if (directions[i] == 0)
		{
			// all preceeding vertices are correct and so only higher vertices are checked
			auto it = gl.neighbors[i].upper_bound(i);

			// first guess left
			directions[i] = -1;
			while (it != gl.neighbors[i].end())
			{
				if (directions[*it] == 1)
					dirOK = false;
				else
					directions[*it] = -1;
				++it;
			}
			// next vertex and check if solution was found
			if (dirOK)
			{
				if (guessDirections(i))
					return true;
			}
			// then guess right
			directions[i] = 1;
			while (it != gl.neighbors[i].begin())
			{
				if (directions[*it] == -1)
					dirOK = false;
				else
					directions[*it] = 1;
				--it;
			}
			if (dirOK)
			{
				if (guessDirections(i))
					return true;
			}
		}
	}
	// look for solution
	return = tryToFind();
}

bool LIntersectionGraph::tryToFind(void)
{
	// all directions must be set
	if (std::find(directions.begin(), directions.end(), 0) == directions.end())
		return false;

	// try to find the l graph
	deduceStopIntervals();
	// do CUM with known data
	return doPartialOrder();
}

// the minimal value for the side coordinate
// correct directions are passed
void LIntersectionGraph::deduceStopIntervals(void)
{
	for (size_t left = 1; left <= max; ++left)
	{
		for (auto right : gl.neighbors[left])
		{
			// right must reach at least to left
			if (directions[left] == -1 && directions[right] == -1)
			{
				stops[right] = left;
				// reach to min vertex
				break;
			}
			else if (directions[left] == 1 && directions[right] == 1)
			{
				stops[left] = right;
				// reach to max vertex
			}
		}
	}
	return;
}

// creates cum from so far known data
// returns if cum exists
bool LIntersectionGraph::doPartialOrder(void)
{
	std::vector<int> allN(max);
	std::iota(allN.begin(), allN.end(), 1);

	bool cumIsOK = true;

	for (size_t left = 1; left <= max; ++left)
	{
		for (auto right : allN)
		{
			if (right == left)
				continue;
			else if (right < left)
			{
				int temp = right;
				right = left;
				left = temp;
			}
			// edge exists
			if (gl.neighbors[left].find(right) != gl.neighbors[left].end())
			{
				if (directions[left] == -1 && directions[right] == -1)
				{
					// right is higher than left
					cumIsOK = cum.add(right, left);
				}
				else if (directions[left] == 1 && directions[right] == 1)
				{
					// right is lower than left
					cumIsOK = cum.add(left, right);
				}
			}
			// no edge
			else
			{
				if (directions[left] == -1 && directions[right] == -1 && stops[right] <= left)
				{
					// right is lower than left
					cumIsOK = cum.add(left, right);
				}
				else if (directions[left] == 1 && directions[right] == 1 && stops[left] >= right)
				{
					// right is higher than left
					cumIsOK = cum.add(right, left);
				}
			}
			if (!cumIsOK)
				return false;
		}
	}
	return cum.createCombination();
}

void LIntersectionGraph::fillShapes(void)
{
	for (size_t i = 0; i < max; ++i)
	{
		shapes[i].up = i + 1;
		shapes[i].bend = i; // actually result from cum
		shapes[i].side = stops[i + 1];

	}
	return;
}

bool LIntersectionGraph::createLGraph(void)
{
	if (!guessDirections(0))
	{
		// wait for correct directions
	}
	else
	{
		// solution exists, fill in lshapes
		fillShapes();
	}
	return false;
}

void LIntersectionGraph::printResult() const
{
	for (size_t i = 0; i < shapes.size(); ++i)
	{
		std::cout << shapes[i].getUp() << "     " << shapes[i].getBend() << "     " << shapes[i].getSide() << std::endl;
	}
	return;
}
