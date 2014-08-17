#ifndef VISITOR_HPP
#define VISITOR_HPP

#include <VisitorDetails.hpp>

////////////////////////////////////////////////////////////////////////////////
/// \brief Visitor base class used by the Visitor Pattern.
///
/// This visitor is a cooperative visitor based on this implementation:
/// http://www.artima.com/cppsource/cooperative_visitor.html
///
/// It allows to specify the base class of the visitable hierarchy and the
/// return type of the "visit" method. The "visit" method name can be
/// customized with the macro META_Visitor.
///
/// Double dispatch is performed via a custom vtable and no virtual method are
/// needed.
///
/// To perform the visitation, you must used the operator().
/// For example:
/// \code
///     class NodeVisitor : public Visitor<Node, int>
///     {
///         public:
///             META_Visitor(NodeVisitor)
///
///             NodeVisitor()
///             {
///                 // Base class (Node) could be omitted (added by default)
///                 META_Visitables(Node, Group);
///             }
///     };
///
///     Node node;      // Inherits from Visitable<Node>
///     Group group;    // Inherits from Node
///     NodeVisitor nv; // Inherits from Visitor<Node, int>
///
///     int result = nv(node); // Visit the node.
///     result = nv(group); // Visit the group node.
/// \endcode
////////////////////////////////////////////////////////////////////////////////
template <typename Base, typename ReturnType = void>
class Visitor
{
    public:
        ////////////////////////////////////////////////////////////////////////
        /// \brief Perform the visitation of the given visitable.
        /// Double dispatch is performed via a custom vtable.
        /// \param b Visitable to visit.
        /// \return The result of the visitation.
        ////////////////////////////////////////////////////////////////////////
        ReturnType operator()(Base & b);

        ////////////////////////////////////////////////////////////////////////
        /// \brief Call the right function from the vtable by using a thunk.
        ////////////////////////////////////////////////////////////////////////
        template <typename VisitorImpl, typename Visitable, typename Invoker>
        ReturnType thunk(Base & b);

    public:
        using BaseType   = Base;
        using Thunk      = ReturnType (Visitor::*)(Base &);
        using VTableType = visitor_details::VisitorVTable<Base const, Thunk>;
        using RType      = ReturnType;

    private:
        template <typename VisitorImpl, typename Invoker, typename ...VisitedList>
        friend class VisitorVTableSetter;

        VTableType const * m_vtable; ///< Vtable pointer
};

//! Must be present in each visitor class with its name as first parameter and optionally
/// the name of the visit method as second one ("visit" is set if nothing specified)
#define META_Visitor(/*Visitor [, VisitInvoker]*/...) \
    SWITCH_2(__VA_ARGS__, META_VisitorWithCustomInvoker, META_DefaultVisitor)(__VA_ARGS__)

//! Must be called in each visitor constructor to build the virtual table of the visitor.
/// The parameters are the class with are visitable by this visitor.
#define META_Visitables(...) \
    VisitorVTableSetter< \
        visitor_details::VisitorType, \
        visitor_details::InvokerType, \
        __VA_ARGS__ \
    >::SetVTable(*this);

#include "Visitor.inl"

#endif //VISITOR_HPP
