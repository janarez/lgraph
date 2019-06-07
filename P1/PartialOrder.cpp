#include "PartialOrder.h"

void PartialOrder::initializePartialOrder(size_t max)
{
	this->max = max;
	// initialize the 2d [max]*[max] structure to zeroes
	cum.resize(max + 1);
	for (size_t i = 1; i <= max; ++i)
	{
		cum[i].resize(max + 1);
	}
	// dummy one to make createOrdering function work
	// otherwise the dummy zeroth row would always be selected in that function
	cum[0].push_back(1);
	return;
}

void PartialOrder::zeroMatrix(void)
{
	cum[0].assign({ 1 });
	for (size_t i = 1; i <= max; ++i)
	{
		std::fill(cum[i].begin(), cum[i].end(), 0);
	}
}

bool PartialOrder::transitivity(size_t higher, size_t lower)
{
	// continue up
	for (size_t i = 1; i <= max; ++i)
	{
		if (higher != i && cum[higher][i] == 1)
		{
			if (cum[lower][i] == -1)
				return false;
			else if (lower != i)
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
			else if (higher != i)
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
		cum[higher][lower] = -1;
		cum[lower][higher] = 1;
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
	// to register highest vertex
	size_t highest;
	size_t counter = 0;
	size_t sum;

	for (size_t j = 0; j < max; ++j)
	{
		// get row sums
		for (auto i : cum)
		{
			sum = std::count(i.begin(), i.end(), 1);
			if (sum == 0)
			{
				highest = counter;
				counter = 0;
				break;
			}
			++counter;
		}
		// note that there is a cycle in cum: no highest element
		if (counter == max)
		{
			std::vector<size_t> empty;
			return empty;
		}

		// register highest vertex
		ordering.push_back(highest);

		// subtract values pertaining to highest element
		for (size_t i = 1; i <= max; ++i)
		{
			cum[i][highest] += cum[highest][i];
		}
		// put all 1s in row pertaining to highest element (no need to subtract 1 as indexing starts at 1)
		// will never be reselected since it will have 1 even at index zero
		cum[highest].assign(max+1,1);
	}
	return ordering;
}