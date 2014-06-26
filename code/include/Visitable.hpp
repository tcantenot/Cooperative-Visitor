#ifndef VISITABLE_HPP
#define VISITABLE_HPP

#include <VisitorDetails.hpp>

////////////////////////////////////////////////////////////////////////////////
/// \brief Visitable based class used by the Visitor Pattern.
///
/// This class used CRTP pattern to make the base class of hierarchy visitable.
/// For example:
/// \code
///     class Shape : public Visitable<Shape>
///     {
///         public:
///             META_Visitable(Shape)
///     };
///
///     class Circle : public Shape
///     {
///         public:
///             META_Visitable(Circle)
///     };
/// \endcode
////////////////////////////////////////////////////////////////////////////////
template <typename Base>
struct Visitable
{
    protected:
        template <typename VisitableImpl>
        std::size_t getTagHelper(VisitableImpl const *) const;
};

///! Macro helper defining a virtual function in every Visitable class
/// to return its tag
#define META_Visitable(VisitorImpl) \
    virtual std::size_t tag() const \
    { \
        return this->getTagHelper<VisitorImpl>(this); \
    }

#include "Visitable.inl"

#endif //VISITABLE_HPP
