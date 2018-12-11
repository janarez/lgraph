#include "LIntersect.h"
#include "GraphLoader.h"

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

	combination.assign(max, 1);
	permutation.resize(max);
}

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

void LIntersectionGraph::guessDirections(size_t counter)
{
	// marks valid directions configuration
	bool dirOK = true;
	for (size_t i = counter+1; i <= max; ++i)
	{
		if (i == 0)
		{
			auto it = gl.neighbors[i].upper_bound(i);
			
			directions[i] = -1;
			while (it != gl.neighbors[i].end())
			{
				if (directions[*it] == 1)
					dirOK = false;
				else
					directions[*it] = -1;
				++it;
			}
			if (dirOK)
				guessDirections(i);

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
				guessDirections(i);
		}
	}
	bool found = tryToFind();
	return;
}

bool LIntersectionGraph::tryToFind(void)
{
	// all directions must be set
	if (std::find(directions.begin(), directions.end(), 0) == directions.end())	
		return false;

	// try to find the l graph
	deduceStopIntervals();
	// do CUM with known data
	doPartialOrder();
	return false;
}

void LIntersectionGraph::deduceStopIntervals(void)
{
	for (size_t left = 1; left <= max; ++left)
	{
		for (auto right : gl.neighbors[left])
		{
			// right must reach at least to left
			if (left == -1 && right == -1)
			{
				stops[right] = left;
				// reach to min vertex
				break;
			}
			else if (left == 1 && right == 1)
			{
				stops[left] = right;
				// reach to max vertex
			}
		}
	}
	return;
}

void LIntersectionGraph::doPartialOrder(void)
{
	std::vector<int> allN(max);
	std::iota(allN.begin(), allN.end(), 1);

	cum = PartialOrder();

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
					cum.add(right, left);
				}
				else if (directions[left] == 1 && directions[right] == 1)
				{
					// right is lower than left
					cum.add(left, right);
				}
			}
			// no edge
			else
			{
				if (directions[left] == -1 && directions[right] == -1 && stops[right] <= left)
				{
					// right is lower than left
					cum.add(left, right);
				}
				else if (directions[left] == 1 && directions[right] == 1 && stops[left] >= right)
				{
					// right is higher than left
					cum.add(right, left);
				}
			}
		}
	}
	return;
}


bool LIntersectionGraph::testEachPair(void)
{
	std::vector<size_t> toerase;
	for (auto& i : totest)
	{
		for (size_t j = 0; j < shapes.size(); ++j)
		{
			if (i == j + 1) {}
			else if (gl.neighbors[i].find(j + 1) == gl.neighbors[i].end())
			{
				if (ls.doIntersect(shapes[i - 1], shapes[j]))
				{
					for (auto k : toerase)
						totest.erase(k);
					return false;
				}
			}
			else
			{
				if (!ls.doIntersect(shapes[i - 1], shapes[j]))
				{
					for (auto k : toerase)
						totest.erase(k);
					return false;
				}
			}
		}
		toerase.push_back(i);
	}
	totest.clear();
	return true;
}

bool LIntersectionGraph::updateCombination()
{
	auto& iter = combination.begin();
	for (size_t i = 0; iter != combination.end(); ++i)
	{
		if (*iter == max)
		{
			*iter = 1;
			shapes[i].setSide(1);
			totest.insert(i + 1);
		}
		else
		{
			++(*iter);
			shapes[i].setSide(*iter);
			//shapes[i].setBend(permutation[i]);
			totest.insert(i + 1);
			return true;
		}
		++iter;
	}
	return false;
}

void LIntersectionGraph::insertPointsIntoL(void)
{
	for (size_t i = 0; i < max; ++i)
	{
		shapes[i].setBend(permutation[i]);
		shapes[i].setUp(i + 1);
		shapes[i].setSide(combination[i]);
	}
	return;
}

bool LIntersectionGraph::updatePermutation()
{
	std::vector<size_t> lastpermutation = permutation;
	bool updated = std::next_permutation(permutation.begin(), permutation.end());
	for (size_t i = 0; i < max; ++i)
	{
		if (permutation[i] != lastpermutation[i])
		{
			//shapes[i].setUp(i + 1);
			shapes[i].setBend(permutation[i]);
			//shapes[i].setSide(combination[i]);
		}
	}
	// test all pairs when permutation changes
	totest.insert(std::begin(permutation), std::end(permutation));
	return updated;
}

bool LIntersectionGraph::createLGraph(void)
{
	// every possible order of ends in y direction
	std::iota(std::begin(permutation), std::end(permutation), 1);
	// at begining test all pairs
	totest.insert(std::begin(permutation), std::end(permutation));
	bool solution = false;
	insertPointsIntoL();
	if (forbiddenEdgeCrossing())
		return false;
	/* for every y permutation O(nn!)
	{	for every x permutation O(n^(n+1))
	{ check if intersections are correct O(n^2log n)} }
	*/
	do
	{
		// iterates through combinations of x coordinates
		do
		{
			// here intersections are tested
			solution = testEachPair();
			if (solution)
				break;
		} while (updateCombination());
		if (solution)
			break;
	} while (updatePermutation());
	return solution;
}

// blbosti
bool LIntersectionGraph::forbiddenEdgeCrossing()
{
	bool crossing = false;
	// 3 vertices make up a crossing (reason for -2)
	for (size_t i = 1; i <= max - 2; ++i)
	{
		for (auto k : gl.neighbors[i])
		{
			if (k < i)
				continue;
			for (size_t j = i + 1; j < k; ++j)
			{
				for (auto m : gl.neighbors[j])
				{
					if (m == i || m == k)
					{
						crossing = false; break;
					}
					if (m < i && m > k)
						crossing = true;
				}
				if (crossing)
					return true;
			}
		}
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
