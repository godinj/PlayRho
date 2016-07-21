/*
 * Original work Copyright (c) 2006-2009 Erin Catto http://www.box2d.org
 * Modified work Copyright (c) 2016 Louis Langholtz https://github.com/louis-langholtz/Box2D
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

#ifndef AABB_hpp
#define AABB_hpp

#include <Box2D/Common/Math.h>

namespace box2d
{
	/// An axis aligned bounding box.
	class AABB
	{
	public:
		AABB() = default;
		
		constexpr AABB(Vec2 a, Vec2 b) noexcept:
			lowerBound{Vec2{Min(a.x, b.x), Min(a.y, b.y)}}, upperBound{Vec2{Max(a.x, b.x), Max(a.y, b.y)}} {}
		
		/// Get the center of the AABB.
		constexpr Vec2 GetCenter() const noexcept
		{
			return (lowerBound + upperBound) / float_t(2);
		}
		
		/// Get the extents of the AABB (half-widths).
		constexpr Vec2 GetExtents() const noexcept
		{
			return (upperBound - lowerBound) / float_t(2);
		}
		
		/// Gets the perimeter length.
		/// @return Twice the sum of the width and height.
		constexpr float_t GetPerimeter() const noexcept
		{
			const auto wx = upperBound.x - lowerBound.x;
			const auto wy = upperBound.y - lowerBound.y;
			return (wx + wy) * 2;
		}
		
		/// Combine an AABB into this one.
		constexpr AABB& operator += (const AABB& aabb)
		{
			lowerBound = Min(lowerBound, aabb.lowerBound);
			upperBound = Max(upperBound, aabb.upperBound);
			return *this;
		}
		
		/// Does this aabb contain the provided AABB.
		constexpr bool Contains(const AABB& aabb) const noexcept
		{
			return
			(lowerBound.x <= aabb.lowerBound.x) && (lowerBound.y <= aabb.lowerBound.y) &&
			(aabb.upperBound.x <= upperBound.x) && (aabb.upperBound.y <= upperBound.y);
		}
				
		Vec2 GetLowerBound() const noexcept { return lowerBound; }
		Vec2 GetUpperBound() const noexcept { return upperBound; }
		
		AABB& Move(Vec2 value) noexcept
		{
			lowerBound += value;
			upperBound += value;
			return *this;
		}
		
	private:
		Vec2 lowerBound;	///< the lower vertex
		Vec2 upperBound;	///< the upper vertex
	};
	
	constexpr inline AABB operator + (const AABB& aabb1, const AABB& aabb2)
	{
		return AABB{Min(aabb1.GetLowerBound(), aabb2.GetLowerBound()), Max(aabb1.GetUpperBound(), aabb2.GetUpperBound())};
	}
	
	constexpr inline AABB operator + (Vec2 lhs, const AABB& rhs)
	{
		return AABB{rhs.GetLowerBound() - lhs, rhs.GetUpperBound() + lhs};
	}
	
	constexpr inline AABB operator + (const AABB& lhs, Vec2 rhs)
	{
		return AABB{lhs.GetLowerBound() - rhs, lhs.GetUpperBound() + rhs};
	}

	inline bool TestOverlap(const AABB& a, const AABB& b) noexcept
	{
		const auto d1 = b.GetLowerBound() - a.GetUpperBound();
		const auto d2 = a.GetLowerBound() - b.GetUpperBound();

		return (d1.x <= float_t{0}) && (d1.y <= float_t{0}) && (d2.x <= float_t{0}) && (d2.y <= float_t{0});
	}
	
	/// Ray-cast input data.
	/// @detail The ray extends from p1 to p1 + maxFraction * (p2 - p1).
	struct RayCastInput
	{
		Vec2 p1; ///< Point 1.
		Vec2 p2; ///< Point 2.
		float_t maxFraction; ///< Max fraction.
	};
	
	/// Ray-cast output data.
	/// @detail The ray hits at p1 + fraction * (p2 - p1), where p1 and p2 come from RayCastInput.
	struct RayCastOutput
	{
		Vec2 normal;
		float_t fraction;
	};

	bool RayCast(const AABB& aabb, RayCastOutput* output, const RayCastInput& input);

} // namespace box2d

#endif /* AABB_hpp */