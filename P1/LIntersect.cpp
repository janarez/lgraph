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
#include<iostream>
#include<istream>
#include<fstream>

// constructor
// takes in graph in the form of map of vertices to its neighbours

LIntersectionGraph::LIntersectionGraph(std::map<size_t, std::unordered_set<int>>& neighbors)
{
	// initializes all LShapes to zeroes;
	gl.neighbors = neighbors;
	shapes.resize(neighbors.size());
	max = shapes.size();
	combination.assign(max, 1);
	permutation.resize(max);
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
