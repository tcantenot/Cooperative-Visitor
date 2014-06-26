#ifndef VISITOR_DETAILS_HPP
#define VISITOR_DETAILS_HPP

#include <cstddef>
#include <vector>

namespace details {

/////////////////////////////// TAG GOUNTER ////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// \brief Generate a tag counter for each visitable hierarchy to keep track of
/// the next available tag.
////////////////////////////////////////////////////////////////////////////////
template <typename Base>
struct HierarchyTagCounter
{
    static std::size_t s_counter; ///< Tag counter
};

template <typename Base>
std::size_t HierarchyTagCounter<Base>::s_counter = 0u;


////////////////////////////////////////////////////////////////////////////////
/// \brief Store a tag for every visitable class in a hierarchy.
////////////////////////////////////////////////////////////////////////////////
template <typename Visitable, typename Base>
struct VisitableTagHolder
{
    static std::size_t s_tag; ///< Visitable tag
};

////////////////////////////////////////////////////////////////////////////////
/// \brief Get the tag of a visitable class in a hierarchy.
/// \return The tag of a visitable class in a hierarchy.
////////////////////////////////////////////////////////////////////////////////
template <typename Visitable, typename Base>
std::size_t GetVisitableTag()
{
    std::size_t & tag = VisitableTagHolder<Visitable const, Base const>::s_tag;

    // If the tag has not been generated yet, generate it and return it
    return tag == 0 ? (tag = ++HierarchyTagCounter<Base const>::s_counter) : tag;
}

template <typename Visitable, typename Base>
std::size_t
VisitableTagHolder<Visitable, Base>::s_tag = GetVisitableTag<Visitable, Base>();


////////////////////////////// VIRTUAL TABLE ////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// \brief Class representing a virtual table used for visit dispatch.
////////////////////////////////////////////////////////////////////////////////
template <typename Base, typename Func>
class VisitorVTable
{
    public:
        template <typename Visitable>
        void add(Func f)
        {
            std::size_t index = GetVisitableTag<Visitable, Base>();

            if(index >= m_table.size())
            {
                // Find function for Base if it exists
                std::size_t const baseTag = GetVisitableTag<Base, Base>();

                Func defaultFunc =
                    (baseTag >= m_table.size()) ? nullptr : m_table[baseTag];

                // Expand the table
                m_table.resize(index + 1, defaultFunc);
            }

            m_table[index] = f;
        }

        Func operator[](std::size_t index ) const
        {
            if(index >= m_table.size()) index = GetVisitableTag<Base, Base>();

            return m_table[index];
        }

    private:
        std::vector<Func> m_table; ///< Functions table
};


// Thunk tag used to select non-empty variadic overload
template <typename ...>
struct ThunkTag { };

// Thunk tag used to select empty variadic overload
template <>
struct ThunkTag<> { };


template <typename Visitor, typename Invoker, typename ...VisitedList>
struct GetVisitorVTable;

////////////////////////////////////////////////////////////////////////////////
/// \brief Class used to generate a vtable for the triplet
/// (Visitor, Invoker, VisitedList...).
////////////////////////////////////////////////////////////////////////////////
template <typename Visitor, typename Invoker, typename ...VisitedList>
class VisitorVTableCreator
{
    private:
        ////////////////////////////////////////////////////////////////////////
        /// \brief Constructor.
        ////////////////////////////////////////////////////////////////////////
        VisitorVTableCreator()
        {
            // Add Base's visit function to the vtable first
            this->addThunk<typename Visitor::BaseType>();

            // Add visit function for each type in VisitedList in the vtable
            this->addThunks(ThunkTag<VisitedList...>());
        }

        ////////////////////////////////////////////////////////////////////////
        /// \brief Add a thunk to the vtable.
        ////////////////////////////////////////////////////////////////////////
        template <typename Visitable>
        void addThunk()
        {
            m_vtable.template add<Visitable>(
                &Visitor::template thunk<Visitor, Visitable, Invoker>
            );
        }

        ////////////////////////////////////////////////////////////////////////
        /// \brief Add thunks to the vtable.
        ////////////////////////////////////////////////////////////////////////
        template <typename Head, typename ...Tail>
        void addThunks(ThunkTag<Head, Tail...>)
        {
            this->addThunk<Head>();
            this->addThunks(ThunkTag<Tail...>());
        }

        ////////////////////////////////////////////////////////////////////////
        /// \brief Add thunks to the vtable (break the variadic recursion).
        ////////////////////////////////////////////////////////////////////////
        void addThunks(ThunkTag<>) { }

    private:
        friend struct GetVisitorVTable<Visitor, Invoker, VisitedList...>;

        typename Visitor::VTableType m_vtable; ///< Virtual table
};

////////////////////////////////////////////////////////////////////////////////
/// \brief Class used to retrieved the vtable created by a VisitorVTableCreator.
////////////////////////////////////////////////////////////////////////////////
template <typename Visitor, typename Invoker, typename ...VisitedList>
struct GetVisitorVTable
{
    //! Static instance of vtable
    static VisitorVTableCreator<Visitor, Invoker, VisitedList...> const s_table;

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Return the vtable.
    ////////////////////////////////////////////////////////////////////////////
    operator typename Visitor::VTableType const *() const
    {
        return &s_table.m_vtable;
    }
};

template <typename Visitor, typename Invoker, typename ...VisitedList>
VisitorVTableCreator<Visitor, Invoker, VisitedList...> const
GetVisitorVTable<Visitor, Invoker, VisitedList...>::s_table;

} // details


#endif //VISITOR_DETAILS_HPP
