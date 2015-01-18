// 17.01.15 cmoellinger
#ifndef __UTILITIES_H_
#define __UTILITIES_H_

#include "debug.h"
#include "assert.h"

namespace Utilities
{
	class Math
	{
	public:
		template <class T> static T Clamp(T &rValue, T minValue, T maxValue)
		{
			assert(minValue <= maxValue);

			if (rValue < minValue) rValue = minValue;
			if (rValue > maxValue) rValue = maxValue;
		}
	};
}

#endif