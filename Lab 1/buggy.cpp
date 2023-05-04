#include <iostream>
#include <cstring>

using std::memcpy;
using std::cout;
using std::ostream;

struct Point {
    int x, y;

    Point(): x{}, y{} {}
    Point(int _x, int _y): x{_x}, y{_y} {}
    Point(const Point& other) = default;
    Point(Point&& other) = default;
    ~Point() = default;
    Point& operator=(const Point& other) = default;
    Point& operator=(Point&& other) = default;

    friend ostream& operator<<(ostream& os, const Point& pt) {
        os << pt.x << ", " << pt.y;
        return os;
    }
};

class Shape {
private:
    size_t vertices;
    Point** points;

public:
    Shape(size_t _vertices): vertices{_vertices}, points{new Point * [vertices + 1]} {
        for (size_t i = 0; i <= vertices; ++i)
            this->points[i] = new Point{};
    }
    Shape(const Shape& other) = default;
    Shape(Shape&& other) = default;

    ~Shape() {
        if (this->points) {
            for (size_t i = 0; i <= this->vertices; ++i) {
                delete this->points[i];
                this->points[i] = nullptr;
            }
            delete[] this->points;
        }

        this->points = nullptr;
    }

    Shape& operator=(const Shape& other) = default;
    Shape& operator=(Shape&& other) = default;

    void addPoints(Point* const& pts) {
        for (size_t i = 0; i <= vertices; i++)
            memcpy(this->points[i], &pts[i % vertices], sizeof(Point));
    }

    double area() {
        int temp = 0;
        for (size_t i = 0; i < vertices; ++i) {
            // FIXME: there are two methods to access members of pointers
            //        use one to fix lhs and the other to fix rhs
            int lhs = this->points[i]->x * this->points[i + 1]->y;
            int rhs = (*this->points[i + 1]).x * (*this->points[i]).y;
            temp += (lhs - rhs);
        }

        double area = abs(temp) / 2.0;
        return area;
    }
};

int main() {
    // FIXME: create the following points using the three different methods
    //        of defining structs:
    //          tri1 = (0, 0)
    //          tri2 = (1, 2)
    //          tri3 = (2, 0)

    // adding points to tri

    Point tri1(0, 0);
    Point tri2{};
    tri2.x = 1; tri2.y = 2;
    Point tri3 = Point(2, 0);
    Point triPts[3] = {tri1, tri2, tri3};
    Shape* tri = new Shape(3);
    tri->addPoints(triPts);

    // FIXME: create the following points using your preferred struct
    //        definition:
    //          quad1 = (0, 0)
    //          quad2 = (0, 2)
    //          quad3 = (2, 2)
    //          quad4 = (2, 0)
    Point quad1(0, 0), quad2(0, 2), quad3(2, 2), quad4(2, 0);

    // adding points to quad
    Point quadPts[4] = {quad1, quad2, quad3, quad4};
    Shape* quad = new Shape(4);
    quad->addPoints(quadPts);

    // FIXME: print out area of tri and area of quad
    cout << "Tri Area:\t" << tri->area() << "\n";
    cout << "Quad Area:\t" << quad->area() << "\n";

    delete tri;
    delete quad;
}