#pragma once

#include <vector>
#include <algorithm>
#include <numeric>

// PartialOrder is partially ordered set with some defined operations

class PartialOrder
{
	// saves order: 1 means row entry is below column entry, -1 above, 0 undetermined
	std::vector<std::vector<int>> cum;
	size_t max;

public:
	// adds relation to cum, returns false if relation violates ordering 
	bool add(size_t higher, size_t lower);
	// fills transitive relations
	bool transitivity(size_t higher, size_t lower);

	// returns valid ordering of according to cum
	std::vector<size_t> createOrdering(void);

	void initializePartialOrder(size_t max);

	// sets cum to all zeroes appart from first row that is 1
	void zeroMatrix(void);
};
