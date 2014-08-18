#ifndef VISITABLE_INL
#define VISITABLE_INL

#include "Visitable.hpp"

template <typename Base>
template <typename VisitableImpl>
inline std::size_t Visitable<Base>::getTagHelper(VisitableImpl const *) const
{
    return visitor_details::GetVisitableTag<VisitableImpl, Base>();
}

#endif //VISITABLE_INL
