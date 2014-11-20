#ifndef VISITOR_HPP
#define VISITOR_HPP

#include "VisitorDetails.hpp"

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
///     class NodeVisitor : public Visitor<Node, int, float>
///     {
///         public:
///             META_Visitor(NodeVisitor)
///
///             NodeVisitor()
///             {
///                 // Base class (Node) could be omitted (added by default)
///                 META_Visitables(Node, Group);
///             }
//
//          protected:
//              int visit(Node & node, float f);
//              int visit(Group & node, float f);
///     };
///
///     Node node;      // Inherits from Visitable<Node>
///     Group group;    // Inherits from Node
///     Meta  meta;     // Inherits from Group
///     NodeVisitor nv; // Inherits from Visitor<Node, int, float>
///
///     int result = nv(node, 42.0f); // Call NodeVisitor::visit(Node &, float)
///     result = nv(group, 13.37f);   // Call NodeVisitor::visit(Group &, float)
///     result = nv(meta, 9000.f);    // Call NodeVisitor::visit(Group &, float)
///
/// \endcode
////////////////////////////////////////////////////////////////////////////////
template <typename Base, typename ReturnType = void, typename ...Args>
class Visitor
{
    public:
        ////////////////////////////////////////////////////////////////////////
        /// \brief Perform the visitation of the given visitable.
        /// Double dispatch is performed via a custom vtable.
        /// \param b Visitable to visit.
        /// \return The result of the visitation.
        ////////////////////////////////////////////////////////////////////////
        ReturnType operator()(Base & b, Args && ...args);

        ////////////////////////////////////////////////////////////////////////
        /// \brief Call the right function from the vtable by using a thunk.
        ////////////////////////////////////////////////////////////////////////
        template <typename VisitorImpl, typename Visitable, typename Invoker>
        ReturnType thunk(Base & b, Args && ...args);

    public:
        using BaseType   = Base;
        using Thunk      = ReturnType (Visitor::*)(Base &, Args && ...);
        using VTableType = visitor_details::VisitorVTable<Base const, Thunk>;
        using RType      = ReturnType;

    private:
        template <typename VisitorImpl, typename Invoker, typename ...VisitedList>
        friend struct VisitorVTableSetter;

        VTableType const * m_vtable; ///< Vtable pointer
};


//! Must be present in each visitor class with its name as first parameter and optionally
/// the name of the visit method as second one ("visit" is set if nothing specified)
/// Example:
///  META_Visitor(NodeVisitor) // Default invoker 'visit'
///  META_Visitor(UpdateVisitor, update) // Custom invoker 'update'
#define META_Visitor(/*Visitor [, VisitInvoker]*/...) \
    _SELECT_META_VISITOR_(__VA_ARGS__)

//! Must be called in each visitor constructor to build the virtual table of the visitor.
/// The parameters are the class with are visitable by this visitor.
#define META_Visitables(...) \
    VisitorVTableSetter< \
        visitor_invoker_details::VisitorType, \
        visitor_invoker_details::InvokerType, \
        __VA_ARGS__ \
    >::SetVTable(*this)


#include "Visitor.inl"

#endif //VISITOR_HPP
