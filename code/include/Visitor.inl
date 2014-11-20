#ifndef VISITOR_INL
#define VISITOR_INL

#include "Visitor.hpp"

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


template <typename Base, typename ReturnType, typename ...Args>
inline ReturnType Visitor<Base, ReturnType, Args...>::operator()(Base & b, Args && ...args)
{
    // Retrieve the invocation info of the Visitable
    auto info = b.visitable_invocation_info(m_vtable->getStatusTable());

    // Fetch thunk
    Thunk thunk = (*m_vtable)[info.vtableIndex];

    // Pointer to member function syntax
    return (this->*thunk)(*static_cast<Base *>(info.visitable), std::forward<Args>(args)...);
}

template <typename Base, typename ReturnType, typename ...Args>
template <typename VisitorImpl, typename Visitable, typename Invoker>
inline ReturnType Visitor<Base, ReturnType, Args...>::thunk(Base & b, Args && ...args)
{
    using VisitableType =
        typename visitor_details::GetVisitMethodArgumentType<Visitable, Base>::Type;

    VisitorImpl & visitor = static_cast<VisitorImpl&>(*this);

    VisitableType & visitable = static_cast<VisitableType &>(b);

    return Invoker::Invoke(visitor, visitable, std::forward<Args>(args)...);
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
#define _META_VISITOR_DEFAULT_(VisitorImpl) \
    _META_VISITOR_WITH_CUSTOM_INVOKER_(VisitorImpl, visit)

//! Internal macro used to configure a Visitor with visit methods with
/// the custom name <VisitInvoker>
#define _META_VISITOR_WITH_CUSTOM_INVOKER_(VisitorImpl, VisitInvoker) \
struct visitor_invoker_details \
{ \
    using VisitorType = VisitorImpl; \
    using InvokerType = \
    struct  \
    { \
        template <typename VisitorImpl, typename VisitableImpl, typename ...Args> \
        static typename VisitorImpl::RType Invoke( \
            VisitorImpl & visitor, VisitableImpl & visitable, Args && ...args \
        ) \
        { \
            return visitor.VisitInvoker(visitable, std::forward<Args>(args)...); \
        } \
    }; \
}; \


#define _META_VISITOR_1_(VisitorImpl) \
	_META_VISITOR_DEFAULT_(VisitorImpl)

#define _META_VISITOR_2_(VisitorImpl, VisitInvoker) \
	_META_VISITOR_WITH_CUSTOM_INVOKER_(VisitorImpl, VisitInvoker)

// Count the number of arguments in a variadic pack (max = 2)
#define _META_VISITOR_N_ARGS_(...) _META_VISITOR_N_ARGS__IMPL_((__VA_ARGS__, 2, 1, 0))
#define _META_VISITOR_N_ARGS__IMPL_(args) _META_VISITOR_N_ARGS__IMPL_2_ args
#define _META_VISITOR_N_ARGS__IMPL_2_(_1, _2, count, ...) count

// Pick the right _META_VISITOR_X_
#define _META_VISITOR_CHOOSER_(count)  _META_VISITOR_CHOOSER_1_(count)
#define _META_VISITOR_CHOOSER_1_(count) _META_VISITOR_CHOOSER_2_(count)
#define _META_VISITOR_CHOOSER_2_(count) _META_VISITOR_##count##_

#define _SELECT_META_VISITOR_HELPER_(func, args) func args
#define _SELECT_META_VISITOR_(...) \
	_SELECT_META_VISITOR_HELPER_(\
		_META_VISITOR_CHOOSER_(_META_VISITOR_N_ARGS_(__VA_ARGS__)), (__VA_ARGS__)\
	)

#endif //VISITOR_INL
