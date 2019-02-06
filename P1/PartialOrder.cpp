#include "PartialOrder.h"

#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <set>
#include <numeric>
#include <exception>

bool PartialOrder::transitivity(size_t higher, size_t lower)
{
	// continue up
	for (size_t i = 1; i <= max; ++i)
	{
		if (higher != i && cum[higher][i] == 1)
		{
			if (cum[lower][i] == -1)
				return false;
			else
			{
				cum[lower][i] = 1;
				cum[i][lower] = -1;
			}
		}
	}
	//continue down
	for (size_t i = 1; i <= max; ++i)
	{
		if (lower != i && cum[i][lower] == 1)
		{
			if (cum[i][higher] == -1)
				return false;
			else
			{
				cum[i][higher] = 1;
				cum[higher][i] = -1;
			}
		}
	}
	return true;
}

bool PartialOrder::add(size_t higher, size_t lower)
{
	// correct is cum[higher][lower] == -1
	if (cum[higher][lower] == 1)
	{
		// error, not partial order anymore
		return false;
	}
	else if (cum[higher][lower] == 0)
	{
		// insert all info
		cum[higher][lower] = 1;
		cum[lower][higher] = -1;
		return transitivity(higher, lower);
	}
	return true;
}

// reconstruct ordering from the cum matrix
// the highest one cannot have any 1's
// second highest one has 1 only with the highest one etc...
// equality -> random choice
std::vector<size_t> PartialOrder::createOrdering(void)
{
	std::vector<size_t> ordering;
	std::vector<size_t> sum;
	for (size_t j = 0; j < max; ++j)
	{
		// get row sums
		for (auto i : cum)
		{
			sum.push_back(std::count(cum[i].begin(), cum[i].end(), 1));
		}
		// register highest vertex
		highest = std::min_element(sum.begin(), sum.end()) + 1;
		ordering.push_back(highest);

		// subtract column pertaining to highest element
		for (size_t i = 1; i <= max; ++i)
		{
			cum[i][i] -= cum[highest][i];
		}
		// delete row pertaining to highest element
		cum.erase(std::find(cum.begin(),cum.end(),i));
	}
	return ordering;
}

/* OBSOLETE CODE

// available holds yet unused numbers, result the combination
// returns flag if the combination is valid
bool PartialOrder::createCombination(std::vector<size_t>& available, std::vector<size_t>& result)
{
	// second number
	for (auto i : available)
	{
		// check move validity
		for (auto j : available))
		{
			if (i != j)
			{
				if (cum[i][j] == -1)
					return false;
			}
		}
		// proceed with next position, mark this one unavailable
		available.erase(std::find(available.begin(),available.end(),i));
		result.pushback(i);
		if(createCombination(available, result))
			return true;
	}
	return false;  // not sure
}

bool PartialOrder::createCombinationRecursion(void)
{
	// first number
	std::vector<size_t> available(max+1);
	std::iota(available.begin(), available.end(), 1);

	for (size_t i = 1; i <= max; ++i)
	{
		available.erase(std::find(available.begin(),available.end(),i));
		std::vector<size_t> result {i};
		if(createCombination(available, result))
			return true;
	}
	return false;
}
*/