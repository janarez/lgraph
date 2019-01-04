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

// available holds yet unused numbers, result the combination
// returns flag if the combination is valid
bool PartialOrder::createCombination(std::vector<size_t>& available, std::vector<size_t>& result)
{
	std::vector<size_t> combination(max+1);

	// first number
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
		available.erase(std::find(avaiable.begin(),available.end(),i));
		result.pushback(i);
		createCombination(available, result)
	}
	return true;
}
