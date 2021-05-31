#ifndef PHYSIC_TYPES_H
#define PHYSIC_TYPES_H

#include <vector>

namespace radar 
{
	using meter_t = double; // Type for ranges
	using physic_t = double; // For physic calculations
	using geo_coord_t = double; // latitude longitude height
	using meter_vec_t = std::vector<meter_t>;
}

#endif // PHYSIC_TYPES_H