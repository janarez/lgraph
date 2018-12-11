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
