// Reimplement GDExtension math.hpp to point where engine defines
// same functions.
#include "core/math/math_defs.h"
#include "core/math/math_funcs.h"
#include "core/typedefs.h"

#define Math_PI Math::PI
#define Math_TAU Math::TAU

namespace Math{

	template <typename T>
	inline T clamp(T x, T minv, T maxv) {
		return CLAMP<T,T,T>(x, minv, maxv);
	}

}
