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

/// \brief Macro helper defining a virtual function in every Visitable class
/// to return its invocation info.
/// This function allows to fallback falling through the type hierarchy
/// (recursively) to find the nearest base class conversion.
/// \param VisitableImpl     Type of the visitable class.
/// \param VisitableFallback Ancestor visitable class to use as fallback.
#define META_Visitable(VisitableImpl, VisitableFallback) \
    virtual visitor_details::InvocationInfo visitable_invocation_info( \
        std::vector<bool> const & statusTable \
    ) \
    { \
        std::size_t tag = this->getTagHelper(this); \
        if(tag < statusTable.size() && statusTable[tag]) \
        { \
            return visitor_details::InvocationInfo(tag, this); \
        } \
        return VisitableFallback::visitable_invocation_info(statusTable); \
    }

/// \brief Macro helper for the base class of a visitable hierarchy
#define META_BaseVisitable(VisitableImpl) \
    META_Visitable(VisitableImpl, VisitableImpl)

#include "Visitable.inl"

#endif //VISITABLE_HPP
