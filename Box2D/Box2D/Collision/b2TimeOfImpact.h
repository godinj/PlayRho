/*
* Copyright (c) 2006-2009 Erin Catto http://www.box2d.org
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#ifndef B2_TIME_OF_IMPACT_H
#define B2_TIME_OF_IMPACT_H

#include <Box2D/Common/b2Math.h>
#include <Box2D/Collision/b2Distance.h>

/// Input parameters for b2TimeOfImpact
struct b2TOIInput
{
	b2DistanceProxy proxyA;
	b2DistanceProxy proxyB;
	b2Sweep sweepA;
	b2Sweep sweepB;
	b2Float tMax; ///< Maximum sweep interval time fraction (in the range of [0, 1]).
};

/// Output parameters for b2TimeOfImpact.
class b2TOIOutput
{
public:
	enum State
	{
		e_unknown,
		e_failed,
		e_overlapped,
		e_touching,
		e_separated
	};

	b2TOIOutput() = default;
	
	constexpr b2TOIOutput(State _state, b2Float _t): state(_state), t(_t)
	{
		b2Assert(t >= 0);
		b2Assert(t <= 1);
	}

	/// Gets the state at time factor.
	State get_state() const noexcept { return state; }

	/// Gets time factor at which state occurs.
	/// @return Time factor in range of [0,1] into the future.
	b2Float get_t() const noexcept { return t; }

private:
	State state; ///< State at time factor.
	b2Float t; 	///< Time factor in range of [0,1] into the future.
};

/// Computes the upper bound on time before two shapes penetrate.
/// Time is represented as a fraction between [0,b2TOIInput::tMax].
/// This uses a swept separating axis and may miss some intermediate,
/// non-tunneling collision. If you change the time interval, you should call this function
/// again.
/// @note Use b2Distance to compute the contact point and normal at the time of impact.
b2TOIOutput b2TimeOfImpact(const b2TOIInput& input);

#endif
