#ifndef VISITOR_INL
#define VISITOR_INL

#include <Visitor.hpp>

namespace visitor_details {

template <typename Visitable, typename Base>
struct GetVisitMethodArgumentType
{
    using Type = Visitable; // ReturnType visit(Visitable &)
};

// Specialization for const Base
template <typename Visitable, typename Base>
struct GetVisitMethodArgumentType<Visitable, Base const>
{
    using Type = Visitable const; // ReturnType visit(Visitable const &)
};

} // visitor_details


template <typename Base, typename ReturnType>
inline ReturnType Visitor<Base, ReturnType>::operator()(Base & b)
{
    // Retrieve the invocation info of the Visitable
    auto info = b.visitable_invocation_info(m_vtable->getStatusTable());
    Thunk thunk = (*m_vtable)[info.vtableIndex]; // Fetch thunk
    return (this->*thunk)(*static_cast<Base *>(info.visitable)); // Pointer to member function syntax
}

template <typename Base, typename ReturnType>
template <typename VisitorImpl, typename Visitable, typename Invoker>
inline ReturnType Visitor<Base, ReturnType>::thunk(Base & b)
{
    using VisitableType =
        typename visitor_details::GetVisitMethodArgumentType<Visitable, Base>::Type;

    VisitorImpl & visitor = static_cast<VisitorImpl&>(*this);

    VisitableType & visitable = static_cast<VisitableType &>(b);

    return Invoker::Invoke(visitor, visitable);
}


/// visitor_details ///

////////////////////////////////////////////////////////////////////////////////
/// \brief Class used to initialize a visitor vtable.
////////////////////////////////////////////////////////////////////////////////
template <typename Visitor, typename Invoker, typename ...VisitedList>
struct VisitorVTableSetter
{
    static void SetVTable(Visitor & visitor)
    {
        // Instantiate the static vtable and set the vtable pointer
        visitor.m_vtable =
            visitor_details::GetVisitorVTable<Visitor, Invoker, VisitedList...>();
    }
};

//! Internal macro used to configure a Visitor with visit methods named "visit"
#define META_DefaultVisitor(VisitorImpl) \
    META_VisitorWithCustomInvoker(VisitorImpl, visit)

//! Internal macro used to configure a Visitor with visit methods with
/// the custom name <VisitInvoker>
#define META_VisitorWithCustomInvoker(VisitorImpl, VisitInvoker) \
class visitor_details \
{ \
    using VisitorType = VisitorImpl; \
    using InvokerType = \
    struct  \
    { \
        template <typename VisitorImpl, typename VisitableImpl> \
        static typename VisitorImpl::RType Invoke( \
            VisitorImpl & visitor, VisitableImpl & visitable \
        ) \
        { \
            return visitor.VisitInvoker(visitable); \
        } \
    }; \
}; \

#define SWITCH_2(_1, _2, MACRO, ...) MACRO

#endif //VISITOR_INL
