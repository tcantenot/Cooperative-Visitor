#include <cassert>
#include <iostream>

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


class ShapeVisitor : public Visitor<Shape, bool>
{
    public:
        META_Visitor(ShapeVisitor, draw);

        ShapeVisitor()
        {
            // Set visitables classes (no need to specify base class)
            META_Visitables(Circle, Polygon);
        }

    protected:
        bool draw(Shape & shape)
        {
            std::cout << "ShapeVisitor::draw(Shape = " << &shape << ")" << std::endl;
            return false;
        }

        bool draw(Circle & circle)
        {
            std::cout << "ShapeVisitor::draw(Circle = " << &circle << ")" << std::endl;
            return true;
        }

        bool draw(Polygon & polygon)
        {
            std::cout << "ShapeVisitor::draw(Polygon = " << &polygon << ")" << std::endl;
            return true;
        }
};

class Node : public Visitable<Node>
{
    public:
        META_BaseVisitable(Node)
};

class Group : public Node
{
    public:
        META_Visitable(Group, Node)
};

class List : public Group
{
    public:
        META_Visitable(List, Group)
};


#define NODEVISITOR_NODE 0
#define NODEVISITOR_LIST 1
#define OTHERNODEVISITOR_NODE 2
#define OTHERNODEVISITOR_GROUP 3

class NodeVisitor : public Visitor<Node, int>
{
    public:
        META_Visitor(NodeVisitor)

        NodeVisitor()
        {
            META_Visitables(List);
        }

        int visit(Node & node)
        {
            std::cout << "NodeVisitor::visit(Node = " << &node << ")" << std::endl;
            return NODEVISITOR_NODE;
        }

        int visit(List & list)
        {
            std::cout << "NodeVisitor::visit(List = " << &list << ")" << std::endl;
            return NODEVISITOR_LIST;
        }
};

class OtherNodeVisitor : public NodeVisitor
{
    public:
        META_Visitor(OtherNodeVisitor);

        OtherNodeVisitor()
        {
            META_Visitables(List, Group);
        }

    protected:
        int visit(Node & node)
        {
            std::cout << "OtherNodeVisitor::visit(Node = " << &node << ")" << std::endl;
            return OTHERNODEVISITOR_NODE;
        }

        int visit(Group & group)
        {
            std::cout << "OtherNodeVisitor::visit(Group = " << &group << ")" << std::endl;
            return OTHERNODEVISITOR_GROUP;
        }

        // Use parent visit methods also
        using NodeVisitor::visit;
};

class VoidVisitor : public Visitor<Node>
{
    public:
        META_Visitor(VoidVisitor, Void);

        VoidVisitor()
        {
            META_Visitables(Node);
        }

    protected:
        void Void(Node & node)
        {
            std::cout << "VoidVisitor::Void(Node = " << &node << ")" << std::endl;
        }
};


class VariadicVisitor : public Visitor<Node, int, int, std::string const &>
{
    public:
        META_Visitor(VariadicVisitor, call);

        VariadicVisitor()
        {
            META_Visitables(Node, Group);
        }

    private:
        int call(Node & node, int i, std::string const & s)
        {
            std::cout << "VariadicVisitor::call(Node = " << &node << ", " << i
                      << ", " << s << ")" << std::endl;
            return i + 1;
        }

        int call(Group & group, int i, std::string const & s)
        {
            std::cout << "VariadicVisitor::call(Group = " << &group << ", " << i
                      << ", " << s << ")" << std::endl;
            return i + 2;
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
    b = sv(shape);  assert(!b);
    b = sv(circle);  assert(b);
    b = sv(polygon); assert(b);
    b = sv(pp);      assert(b);
    b = sv(ppp);     assert(b);

    std::cout << std::endl;

    Node node;
    Group group;
    List list;
    NodeVisitor nv;
    OtherNodeVisitor ov;

    int i;
    i = nv(node);  assert(i == NODEVISITOR_NODE);
    i = nv(group); assert(i == NODEVISITOR_NODE);
    i = nv(list);  assert(i == NODEVISITOR_LIST);
    i = ov(node);  assert(i == OTHERNODEVISITOR_NODE);
    i = ov(group); assert(i == OTHERNODEVISITOR_GROUP);
    i = ov(list);  assert(i == NODEVISITOR_LIST); // would be OTHERNODEVISITOR_GROUP if we remove "using NodeVisitor::visit"

    std::cout << std::endl;


    static auto const proxy = [](NodeVisitor & visitor, Node & node)
    {
        return visitor(node);
    };

    i = proxy(nv, node);  assert(i == NODEVISITOR_NODE);
    i = proxy(nv, group); assert(i == NODEVISITOR_NODE);
    i = proxy(nv, list);  assert(i == NODEVISITOR_LIST);
    i = proxy(ov, node);  assert(i == OTHERNODEVISITOR_NODE);
    i = proxy(ov, group); assert(i == OTHERNODEVISITOR_GROUP);
    i = proxy(ov, list);  assert(i == NODEVISITOR_LIST); // would be OTHERNODEVISITOR_GROUP if we remove "using NodeVisitor::visit"

    std::cout << std::endl;

    VoidVisitor vv;
    vv(node);
    vv(group);
    vv(list);

    std::cout << std::endl;

    VariadicVisitor varv;
    i = varv(node, 1, "node");   assert(i == 2);
    i = varv(group, 2, "group"); assert(i == 4);
    i = varv(list, 3, "list");  assert(i == 5);

    return 0;
}
