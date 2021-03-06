/*
 * Original work Copyright (c) 2006-2009 Erin Catto http://www.box2d.org
 * Modified work Copyright (c) 2017 Louis Langholtz https://github.com/louis-langholtz/PlayRho
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#ifndef PLAYRHO_COLLISION_SHAPES_SHAPE_HPP
#define PLAYRHO_COLLISION_SHAPES_SHAPE_HPP

#include <PlayRho/Common/Math.hpp>
#include <PlayRho/Collision/DistanceProxy.hpp>
#include <PlayRho/Collision/MassData.hpp>
#include <PlayRho/Common/NonNegative.hpp>
#include <memory>
#include <functional>
#include <utility>
#include <typeinfo>

namespace playrho {
namespace d2 {

class Shape;

// Forward declare functions.
// Note that these may be friend functions but that declaring these within the class that
// they're to be friends of, doesn't also insure that they're found within the namespace
// in terms of lookup.

/// @brief Gets the number of child primitives of the shape.
/// @return Non-negative count.
ChildCounter GetChildCount(const Shape& shape) noexcept;

/// @brief Gets the "child" for the given index.
/// @param shape Shape to get "child" shape of.
/// @param index Index to a child element of the shape. Value must be less
///   than the number of child primitives of the shape.
/// @note The shape must remain in scope while the proxy is in use.
/// @throws InvalidArgument if the given index is out of range.
/// @see GetChildCount
DistanceProxy GetChild(const Shape& shape, ChildCounter index);

/// @brief Gets the mass properties of this shape using its dimensions and density.
/// @return Mass data for this shape.
MassData GetMassData(const Shape& shape) noexcept;

/// @brief Gets the coefficient of friction.
/// @return Value of 0 or higher.
Real GetFriction(const Shape& shape) noexcept;

/// @brief Gets the coefficient of restitution value of the given shape.
Real GetRestitution(const Shape& shape) noexcept;

/// @brief Gets the density of the given shape.
/// @return Non-negative density (in mass per area).
NonNegative<AreaDensity> GetDensity(const Shape& shape) noexcept;

/// @brief Gets the vertex radius of the indexed child of the given shape.
///
/// @details This gets the radius from the vertex that the shape's "skin" should
///   extend outward by. While any edges - line segments between multiple vertices -
///   are straight, corners between them (the vertices) are rounded and treated
///   as rounded. Shapes with larger vertex radiuses compared to edge lengths
///   therefore will be more prone to rolling or having other shapes more prone
///   to roll off of them. Here's an image of a shape configured via a
///   <code>PolygonShapeConf</code> with it's skin drawn:
///
/// @param shape Shape to get child's vertex radius for.
/// @param idx Child index to get vertex radius for.
///
/// @image html SkinnedPolygon.png
///
/// @note This must be a non-negative value.
///
/// @see UseVertexRadius
///
/// @throws InvalidArgument if the child index is not less than the child count.
///
NonNegative<Length> GetVertexRadius(const Shape& shape, ChildCounter idx);

/// @brief Transforms all of the given shape's vertices by the given transformation matrix.
/// @see https://en.wikipedia.org/wiki/Transformation_matrix
/// @note This may throw <code>std::bad_alloc</code> or any exception that's thrown
///   by the constructor for the model's underlying data type.
/// @throws std::bad_alloc if there's a failure allocating storage.
void Transform(Shape& shape, const Mat22& m);

/// @brief Visits the given shape with the potentially non-null user data pointer.
/// @see https://en.wikipedia.org/wiki/Visitor_pattern
bool Visit(const Shape& shape, void* userData);

/// @brief Gets a pointer to the underlying data.
/// @note Provided for introspective purposes like visitation.
/// @note Generally speaking, try to avoid using this method unless there's
///   no other way to access the underlying data.
const void* GetData(const Shape& shape) noexcept;

/// @brief Gets the type info of the use of the given shape.
/// @note This is not the same as calling <code>typeid(Shape)</code>.
/// @return Type info of the underlying value's type.
const std::type_info& GetUseTypeInfo(const Shape& shape);

/// @brief Visitor type alias for underlying shape configuration.
using TypeInfoVisitor = std::function<void(const std::type_info& ti, const void* data)>;

/// @brief Accepts a visitor.
/// @details This is the "accept" method definition of a "visitor design pattern"
///   for doing shape configuration specific types of processing for a constant shape.
/// @see https://en.wikipedia.org/wiki/Visitor_pattern
void Accept(const Shape& shape, const TypeInfoVisitor& visitor);

/// @brief Equality operator for shape to shape comparisons.
bool operator== (const Shape& lhs, const Shape& rhs) noexcept;

/// @brief Inequality operator for shape to shape comparisons.
bool operator!= (const Shape& lhs, const Shape& rhs) noexcept;

// Now define the shape class...
    
/// @defgroup PartsGroup Shape Classes
/// @brief Classes for configuring shapes with material properties.
/// @details These are classes that specify physical characteristics of: shape,
///   friction, density and restitution. They've historically been called shape classes
///   but are now &mdash; with the other properties like friction and density having been
///   moved into them &mdash; maybe better thought of as "parts".

/// @brief Shape.
///
/// @details A shape is used for collision detection. You can create a shape from any
///   supporting type. Shapes are conceptually made up of zero or more convex child shapes
///   where each child shape is made up of zero or more vertices and an associated radius
///   called its "vertex radius".
///
/// @note This class implements polymorphism without inheritance. This is based on a technique
///   that's described by Sean Parent in his January 2017 Norwegian Developers Conference
///   London talk "Better Code: Runtime Polymorphism". With this implementation, different
///   shapes types can be had by constructing instances of this class with the different types
///   that provide the required support. Different shapes of a given type meanwhile are had by
///   providing different values for the type.
///
/// @note This data structure is 32-bytes large (on at least one 64-bit platform).
///
/// @ingroup PartsGroup
///
/// @see Fixture
/// @see https://youtu.be/QGcVXgEVMJg
///
class Shape
{
public:
    /// @brief Default constructor.
    /// @throws std::bad_alloc if there's a failure allocating storage.
    Shape();

    /// @brief Initializing constructor.
    /// @param arg Configuration value to construct a shape instance for.
    /// @note Only usable with types of values that have all of the support functions required
    ///   by this class. The compiler emits errors if the given type doesn't.
    /// @see GetChildCount
    /// @see GetChild
    /// @see GetMassData
    /// @see GetVertexRadius
    /// @see GetDensity
    /// @see GetFriction
    /// @see GetRestitution
    /// @throws std::bad_alloc if there's a failure allocating storage.
    template <typename T>
    explicit Shape(T arg): m_self{std::make_shared<Model<T>>(std::move(arg))}
    {
        // Intentionally empty.
    }
    
    /// @brief Copy constructor.
    Shape(const Shape& other) = default;
    
    /// @brief Move constructor.
    Shape(Shape&& other) = default;
    
    /// @brief Copy assignment operator.
    Shape& operator= (const Shape& other) = default;
    
    /// @brief Move assignment operator.
    Shape& operator= (Shape&& other) = default;

    friend ChildCounter GetChildCount(const Shape& shape) noexcept
    {
        return shape.m_self->GetChildCount_();
    }

    friend DistanceProxy GetChild(const Shape& shape, ChildCounter index)
    {
        return shape.m_self->GetChild_(index);
    }
    
    friend MassData GetMassData(const Shape& shape) noexcept
    {
        return shape.m_self->GetMassData_();
    }
    
    friend NonNegative<Length> GetVertexRadius(const Shape& shape, ChildCounter idx)
    {
        return shape.m_self->GetVertexRadius_(idx);
    }
    
    friend Real GetFriction(const Shape& shape) noexcept
    {
        return shape.m_self->GetFriction_();
    }
    
    friend Real GetRestitution(const Shape& shape) noexcept
    {
        return shape.m_self->GetRestitution_();
    }

    friend NonNegative<AreaDensity> GetDensity(const Shape& shape) noexcept
    {
        return shape.m_self->GetDensity_();
    }
    
    friend void Transform(Shape& shape, const Mat22& m)
    {
        auto copy = shape.m_self->Clone();
        copy->Transform_(m);
        shape.m_self = std::unique_ptr<const Shape::Concept>{std::move(copy)};
    }
    
    friend bool Visit(const Shape& shape, void* userData)
    {
        return shape.m_self->Visit_(userData);
    }
    
    friend const void* GetData(const Shape& shape) noexcept
    {
        return shape.m_self->GetData_();
    }
    
    friend const std::type_info& GetUseTypeInfo(const Shape& shape)
    {
        return shape.m_self->GetUseTypeInfo_();
    }

    friend void Accept(const Shape& shape, const TypeInfoVisitor& visitor)
    {
        const auto self = shape.m_self;
        visitor(self->GetUseTypeInfo_(), self->GetData_());
    }
    
    friend bool operator== (const Shape& lhs, const Shape& rhs) noexcept
    {
        return lhs.m_self == rhs.m_self || *lhs.m_self == *rhs.m_self;
    }

    friend bool operator!= (const Shape& lhs, const Shape& rhs) noexcept
    {
        return !(lhs == rhs);
    }

private:

    /// @brief Internal shape configuration concept.
    /// @note Provides an interface for runtime polymorphism for shape configuration.
    struct Concept
    {
        virtual ~Concept() = default;

        /// @brief Clones this concept and returns a pointer to a mutable copy.
        /// @note This may throw <code>std::bad_alloc</code> or any exception that's thrown
        ///   by the constructor for the model's underlying data type.
        /// @throws std::bad_alloc if there's a failure allocating storage.
        virtual std::unique_ptr<Concept> Clone() const = 0;
        
        /// @brief Gets the "child" count.
        virtual ChildCounter GetChildCount_() const noexcept = 0;
        
        /// @brief Gets the "child" specified by the given index.
        virtual DistanceProxy GetChild_(ChildCounter index) const = 0;
        
        /// @brief Gets the mass data.
        virtual MassData GetMassData_() const noexcept = 0;
        
        /// @brief Gets the vertex radius.
        /// @param idx Child index to get vertex radius for.
        virtual NonNegative<Length> GetVertexRadius_(ChildCounter idx) const = 0;

        /// @brief Gets the density.
        virtual NonNegative<AreaDensity> GetDensity_() const noexcept = 0;
        
        /// @brief Gets the friction.
        virtual Real GetFriction_() const noexcept = 0;
        
        /// @brief Gets the restitution.
        virtual Real GetRestitution_() const noexcept = 0;
        
        /// @brief Transforms all of the shape's vertices by the given transformation matrix.
        /// @see https://en.wikipedia.org/wiki/Transformation_matrix
        virtual void Transform_(const Mat22& m) = 0;
        
        /// @brief Draws the shape.
        virtual bool Visit_(void* userData) const = 0;
        
        /// @brief Equality checking method.
        virtual bool IsEqual_(const Concept& other) const noexcept = 0;
        
        /// @brief Gets the use type information.
        /// @return Type info of the underlying value's type.
        virtual const std::type_info& GetUseTypeInfo_() const = 0;
        
        /// @brief Gets the data for the underlying configuration.
        virtual const void* GetData_() const noexcept = 0;
        
        /// @brief Equality operator.
        friend bool operator== (const Concept& lhs, const Concept &rhs) noexcept
        {
            return &lhs == &rhs || lhs.IsEqual_(rhs);
        }
        
        /// @brief Inequality operator.
        friend bool operator!= (const Concept& lhs, const Concept &rhs) noexcept
        {
            return !(lhs == rhs);
        }
    };

    /// @brief Internal model configuration concept.
    /// @note Provides an implementation for runtime polymorphism for shape configuration.
    template <typename T>
    struct Model final: Concept
    {
        /// @brief Type alias for the type of the data held.
        using data_type = T;

        /// @brief Initializing constructor.
        Model(T arg): data{std::move(arg)} {}
        
        std::unique_ptr<Concept> Clone() const override
        {
            return std::make_unique<Model>(data);
        }

        ChildCounter GetChildCount_() const noexcept override
        {
            return GetChildCount(data);
        }

        DistanceProxy GetChild_(ChildCounter index) const override
        {
            return GetChild(data, index);
        }

        MassData GetMassData_() const noexcept override
        {
            return GetMassData(data);
        }
        
        NonNegative<Length> GetVertexRadius_(ChildCounter idx) const override
        {
            return GetVertexRadius(data, idx);
        }
        
        NonNegative<AreaDensity> GetDensity_() const noexcept override
        {
            return GetDensity(data);
        }
        
        Real GetFriction_() const noexcept override
        {
            return GetFriction(data);
        }
        
        Real GetRestitution_() const noexcept override
        {
            return GetRestitution(data);
        }
        
        void Transform_(const Mat22& m) override
        {
            Transform(data, m);
        }

        bool Visit_(void* userData) const override
        {
            return ::playrho::Visit(data, userData);
        }
        
        bool IsEqual_(const Concept& other) const noexcept override
        {
            // Would be preferable to do this without using any kind of RTTI system.
            // But how would that be done?
            return (GetUseTypeInfo_() == other.GetUseTypeInfo_()) &&
                (data == *static_cast<const T*>(other.GetData_()));
        }
        
        const std::type_info& GetUseTypeInfo_() const override
        {
            return typeid(data_type);
        }
        
        const void* GetData_() const noexcept override
        {
            // Note address of "data" not necessarily same as address of "this" since
            // base class is virtual.
            return &data;
        }

        T data; ///< Data.
    };

    std::shared_ptr<const Concept> m_self; ///< Self shared pointer.
};

// Related free functions...

/// @brief Test a point for containment in the given shape.
/// @param shape Shape to use for test.
/// @param point Point in local coordinates.
/// @return <code>true</code> if the given point is contained by the given shape,
///   <code>false</code> otherwise.
/// @relatedalso Shape
/// @ingroup TestPointGroup
bool TestPoint(const Shape& shape, Length2 point) noexcept;

} // namespace d2

/// @brief Visits the given shape with the potentially non-null user data pointer.
/// @note This is a specialization of the <code>Visit</code> function template for the
///   <code>d2::Shape</code> class.
/// @see https://en.wikipedia.org/wiki/Visitor_pattern
template <>
inline bool Visit<d2::Shape>(const d2::Shape& shape, void* userData)
{
    return d2::Visit(shape, userData);
}

} // namespace playrho

#endif // PLAYRHO_COLLISION_SHAPES_SHAPE_HPP
