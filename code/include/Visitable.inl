#ifndef VISITABLE_INL
#define VISITABLE_INL

template <typename Base>
template <typename VisitableImpl>
inline std::size_t Visitable<Base>::getTagHelper(VisitableImpl const *) const
{
    return details::GetVisitableTag<VisitableImpl, Base>();
}

#endif //VISITABLE_INL
