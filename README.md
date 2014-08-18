Cooperative-Visitor
===================

Flexible and efficient variation of the Visitor design pattern in C++ that accommodates arbitrary argument and return types.

This implementation is based on Anand Shankar Krishnamoorthi's:  <br/>
[Cooperative Visitor: A Template Technique for Visitor Creation](http://www.artima.com/cppsource/cooperative_visitor.html)

> Cooperative Visitor is a template-based technique that allows:  <br\>
* Flexibility of acyclic visitor with performance close to cyclic visitor
* Easier to use, maintain and modify
* Better semantic names rather than "visit"
* Easy creation of const/non-const visitors
* Flexible return and argument types
* Dynamic configuration of visitation behavior
* Implementing a default handler
* Developing template code upon visitor
* Fallback to nearest ancestor
* Custom signature for 'visit' methods

This implementation uses C++11 and replaced the "Loki" TypeList by C++11 variadic templates.  <br\>
Only one virtual method is needed for the visitation (dispatching is performed via a custom virtual table (more information in the link above)).  <br/>
Some efforts have been made to simplify the Visitor utilisation via some macro helpers.

Example:
```cpp
#include <cassert>

#include <Visitor.hpp>
#include <Visitable.hpp>


class Shape : public Visitable<Shape>
{
    public:
        META_BaseVisitable(Shape)
};

class Circle : public Shape
{
    public:
        META_Visitable(Circle, Shape)
};

class Polygon : public Shape
{
    public:
        META_Visitable(Polygon, Shape)
};

class PolyPolygon : public Polygon
{
    public:
        META_Visitable(PolyPolygon, Polygon)
};

class PolyPolyPolygon : public PolyPolygon
{
    public:
        META_Visitable(PolyPolyPolygon, PolyPolygon)
};


class ShapeVisitor : public Visitor<Shape, bool, float, std::string const &>
{
    public:
        META_Visitor(ShapeVisitor, draw)

        ShapeVisitor()
        {
            META_Visitables(Shape, Circle, Polygon);
        }

    protected:
        bool draw(Shape & shape, float, std::string const &)
        {
            return false;
        }

        bool draw(Circle & circle, float, std::string const &)
        {
            return true;
        }

        bool draw(Polygon & polygon, float, std::string const &)
        {
            return true;
        }
};

int main(int argc, char const ** argv)
{
    Shape shape;
    Circle circle;
    Polygon polygon;
    PolyPolygon pp;
    PolyPolyPolygon ppp;

    ShapeVisitor sv;

    bool b;
    b = sv(shape, 1.0f, "shape");     assert(!b);
    b = sv(circle, 2.0f, "circle");   assert(b);
    b = sv(polygon, 3.0f, "polygon"); assert(b);
    b = sv(pp, 4.0f, "pp");   assert(b); // Fallback to ShapeVisitor::draw(Polygon &, ...)
    b = sv(ppp, 5.0f, "ppp"); assert(b); // Fallback to ShapeVisitor::draw(Polygon &, ...)

    return 0;
}
```
