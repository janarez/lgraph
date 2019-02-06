#pragma once

#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <ostream>
#include <set>
#include <unordered_set>

// PartialOrder is partially ordered set with some defined operations

class PartialOrder
{
	// saves order: 1 means row entry is below column entry, -1 above, 0 undetermined
	std::vector<std::vector<int>> cum;
	size_t max;

public:
	bool add(size_t higher, size_t lower);
	bool transitivity(size_t higher, size_t lower);
	bool createCombination(std::vector<size_t>& available,std::vector<size_t>& result);
	bool createCombinationRecursion(void);

	std::vector<size_t> createOrdering(void);

	PartialOrder(size_t max)
	{
		this->max = max;
		// initialize the 2d [max]*[max] structure to zeroes
		cum.resize(max + 1);
		for (size_t i = 1; i <= max; ++i)
		{
			cum[i].resize(max + 1);
		}
	}
};
