#include <iostream>
#include <initializer_list>
#include <vector>
#include <cmath>
#include <queue>

const double PI = 3.1415926;
const double eps = 10e-5;

struct Point {
    double x;
    double y;

    Point() = default;

    Point(double x, double y) : x(x), y(y) {}

    Point rotate(const Point& center, double angle) const {
        Point vec(-center.x + x, -center.y + y);
        double result_x = vec.x * cos(angle) - vec.y * sin(angle) + center.x;
        double result_y = vec.x * sin(angle) + vec.y * cos(angle) + center.y;
        return {result_x, result_y};
    }

    Point reflect(const Point& center) const {
        double result_x = 2 * center.x - x;
        double result_y = 2 * center.y - y;
        return {result_x, result_y};
    }

    Point scale(const Point& center, double coefficient) const {
        double result_x = coefficient * x - (coefficient - 1) * center.x;
        double result_y = coefficient * y - (coefficient - 1) * center.y;
        return {result_x, result_y};
    }

    ~Point() = default;
};

double point_distance(Point a, Point b) {
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

bool operator==(const Point& left, const Point& right) {
    return std::abs(left.x - right.x) < eps && std::abs(left.y - right.y) < eps;
}
bool operator!=(const Point& left, const Point& right) {
    return !(left == right);
}

struct Vector {
    double x_;
    double y_;

    Vector() = default;

    Vector(Point a, Point b) : x_(b.x - a.x), y_(b.y - a.y) {}

    Vector(double x, double y) : x_(x), y_(y) {}
};

bool collinear(Vector u, Vector v) {
    return (std::abs(u.x_ * v.y_ - u.y_ * v.x_) < eps);
}

Vector normal(Vector v) {
    if (v.y_ != 0) {
        return {1, -v.x_ / v.y_};
    }
    return {-v.y_ / v.x_, 1};
}

Point middle(Point a, Point b) {
    return {(a.x + b.x) / 2, (a.y + b.y) / 2};
}

double product(Vector a, Vector b) {
    return a.x_ * b.y_ - a.y_ * b.x_;
}

double vector_len(Vector a) {
    return sqrt(a.x_ * a.x_ + a.y_ * a.y_);
}

double sinus(Vector a, Vector b) {
    return (a.x_ * b.y_ - a.y_ * b.x_) / (vector_len(a) * vector_len(b));
}

class Line {
public:
    Vector vector_;
    Point point_;

    explicit Line() = default;

    Line(Point x, Point y) : vector_(Vector(x, y)), point_(x) {}

    Line(Point x, double k) : vector_(Vector(Point(1, k), Point(0, 0))), point_(x) {}

    Line(double k, double shift) : vector_(Vector(Point(0, 0), Point(1, k))),
                                   point_(Point(0, shift)) {}

    Line(Vector v, Point p) : vector_(v), point_(p) {}

    ~Line() = default;
};

bool operator==(const Line& l1, const Line& l2) {
    return (collinear(l1.vector_, Vector(l1.point_, l2.point_)));
}
bool operator!=(const Line& l1, const Line& l2) {
    return !(l1 == l2);
}

bool collinear(const Line& l1, const Line& l2) {
    return collinear(l1.vector_, l2.vector_);
}

Point intersection(Line l1, Line l2) {
    double x1 = l1.vector_.x_;
    double y1 = l1.vector_.y_;
    double x2 = l2.vector_.x_;
    double y2 = l2.vector_.y_;
    double a1 = l1.point_.x;
    double b1 = l1.point_.y;
    double a2 = l2.point_.x;
    double b2 = l2.point_.y;
    double result_y = (y1 * y2 * (a1 - a2) - b1 * x1 * y2 + b2 * x2 * y1) / (x2 * y1 - x1 * y2);
    double result_x = (x1 * x2 * (b2 - b1) - a2 * x1 * y2 + a1 * x2 * y1) / (x2 * y1 - x1 * y2);
    return {result_x, result_y};
}

Point point_reflect(Point p, const Line &axis) {
    Line l2(normal(axis.vector_), p);
    Point projection = intersection(axis, l2);
    return {2 * projection.x - p.x, 2 * projection.y - p.y};
}

void shift(std::deque<double>& deq) {
    deq.push_back(deq.front());
    deq.pop_front();
}

bool check(std::deque<double> a, std::deque<double> b) {
    bool flag1 = true;
    bool flag2 = true;
    for (size_t i = 0; i < a.size(); ++i) {
        if (std::abs(a[i] - b[i]) > eps) {
            flag1 = false;
        }
    }
    bool flag3 = true;
    for (size_t i = 0; i < a.size(); ++i) {
        if (std::abs(a[i] + b[i]) > eps) {
            flag3 = false;
        }
    }
    for (size_t i = 0; i < a.size(); ++i) {
        if (std::abs(a[i] - b[b.size() - i - 1]) > eps) {
            flag2 = false;
        }
    }
    bool flag4 = true;
    for (size_t i = 0; i < a.size(); ++i) {
        if (std::abs(a[i] + b[b.size() - i - 1]) > eps) {
            flag4 = false;
        }
    }
    if (flag1 || flag2 || flag3 || flag4) {
        return true;
    }
    return false;
}

class Shape {
public:
    virtual double area() const = 0;
    virtual double perimeter() const = 0;
    virtual bool operator==(const Shape&) const = 0;
    virtual bool operator!=(const Shape&) const = 0;
    virtual bool isCongruentTo(const Shape&) const = 0;
    virtual bool isSimilarTo(const Shape&) const = 0;
    virtual void rotate(const Point& center, double angle) = 0;
    virtual void reflect(const Point& center) = 0;
    virtual void reflect(const Line& axis) = 0;
    virtual void scale(const Point& center, double coefficient) = 0;
    virtual bool containsPoint(const Point &point) const = 0;

    virtual ~Shape() = default;
};

class Polygon : public Shape {
public:
    std::vector<Point> vertices;

    Polygon() = default;

    Polygon(const Polygon&) = default;

    Polygon(const std::vector<Point>& points) : vertices(points) {}

    template<typename... T>
    Polygon(const T&... vertices) : vertices{vertices...} {}

    Polygon& operator=(const Polygon&) = default;
    Polygon& operator=(const std::vector<Point>& points) {
        vertices = points;
        return *this;
    }

    double area() const final {
        double square = 0;
        for (size_t i = 0; i < vertices.size() - 1; ++i) {
            square += vertices[i].x * vertices[i + 1].y;
        }
        square += vertices[vertices.size() - 1].x * vertices[0].y;
        for (size_t i = 0; i < vertices.size() - 1; ++i) {
            square -= vertices[i + 1].x * vertices[i].y;
        }
        square -= vertices[vertices.size() - 1].y * vertices[0].x;
        return std::abs(square) / 2;
    }

    double perimeter() const final {
        double perimeter = 0;
        for (size_t i = 1; i < vertices.size(); ++i) {
            perimeter += point_distance(vertices[i], vertices[i - 1]);
        }
        perimeter += point_distance(vertices[0], vertices[vertices.size() - 1]);
        return perimeter;
    }

    size_t verticesCount() const {
        return vertices.size();
    }

    std::vector<Point> getVertices() const {
        return vertices;
    };

    bool isConvex() {
        Vector ab(vertices[vertices.size() - 1], vertices[0]);
        Vector bc(vertices[0], vertices[1]);
        double sign = product(ab, bc);
        bool answer = true;
        for (size_t i = 1; i < vertices.size() - 1; ++i) {
            ab = bc;
            bc = Vector(vertices[i], vertices[i + 1]);
            double new_sign = product(ab, bc);
            if (sign * new_sign < 0) {
                answer = false;
            }
            sign = new_sign;
        }
        ab = bc;
        bc = Vector(vertices[vertices.size() - 1], vertices[0]);
        double new_sign = product(ab, bc);
        if (sign * new_sign < 0) {
            answer = false;
        }
        return answer;
    }

    void rotate(const Point& center, double angle) final {
        angle *= PI / 180;
        for (size_t i = 0; i < vertices.size(); ++i) {
            vertices[i] = vertices[i].rotate(center, angle);
        }
    }

    void reflect(const Line& axis) final {
        for (Point& i : vertices) {
            i = point_reflect(i, axis);
        }
    }

    void reflect(const Point& center) final {
        for (Point& i : vertices) {
            i = i.reflect(center);
        }
    }

    void scale(const Point& center, double coefficient) final {
        for (Point& i : vertices) {
            i = i.scale(center, coefficient);
        }
    }

    bool containsPoint(const Point& point) const final {
        Point point2(point.x + 1, point.y + 1);
        Line ray(point, point2);
        int intersections = 0;
        for (size_t i = 0; i < vertices.size() - 1; ++i) {
            if (point == vertices[i]) {
                return true;
            }
            Line l_now{};
            if (i == vertices.size() - 1) {
                l_now = Line(vertices[i], vertices[0]);
            } else {
                l_now = Line(vertices[i], vertices[i + 1]);
            }
            if (!collinear(ray, l_now)) {
                Point P = intersection(ray, l_now);
                if (P != vertices[i] && (P.x - vertices[i].x) * (vertices[i + 1].x - P.x) >= 0 &&
                    (P.y - vertices[i].y) * (vertices[i + 1].y - P.y) >= 0 &&
                    (P.x >= point.x) && (P.y >= point.y)) {
                    ++intersections;
                }
            }
        }
        if (intersections % 2 == 1) {
            return true;
        }
        return false;
    }

    bool operator==(const Shape& another) const final {
        const auto *temp = dynamic_cast<const Polygon *>(&another);
        if (temp == nullptr) {
            return false;
        }
        if (temp->vertices.size() != vertices.size()) {
            return false;
        }
        size_t i = 0;
        while (i < vertices.size() && temp->vertices[0] != vertices[i]) {
            ++i;
        }
        bool flag1 = true;
        size_t cnt = i % vertices.size();
        for (size_t j = 0; j < vertices.size(); ++j) {
            if (temp->vertices[j] != vertices[cnt]) {
                flag1 = false;
                break;
            }
            ++cnt;
            if (cnt == vertices.size()) {
                cnt = 0;
            }
        }
        cnt = i % vertices.size();
        bool flag2 = true;
        if (temp->vertices[0] != vertices[cnt]) {
            flag2 = false;
        }
        for (size_t j = vertices.size(); j > 1; --j) {
            ++cnt;
            if (cnt >= vertices.size()) {
                cnt = 0;
            }
            if (temp->vertices[j - 1] != vertices[cnt]) {
                flag2 = false;
                break;
            }
        }
        if (flag1 || flag2) {
            return true;
        }
        return false;
    }

    bool operator!=(const Shape& another) const final {
        return !(*this==another);
    }

    bool isCongruentTo(const Shape& another) const final {
        const auto *temp = dynamic_cast<const Polygon *>(&another);
        if (temp == nullptr) {
            return false;
        }
        if (temp->vertices.size() != vertices.size()) {
            return false;
        }
        std::deque<double> first;
        std::deque<double> second;
        for (size_t i = 1; i <= vertices.size(); ++i) {
            first.push_back(product(Vector(vertices[i - 1], vertices[i % vertices.size()]),
                                    Vector(vertices[i % vertices.size()], vertices[(i + 1) % vertices.size()])));
            second.push_back(product(Vector(temp->vertices[i - 1], temp->vertices[i % vertices.size()]),
                                     Vector(temp->vertices[i % vertices.size()],
                                            temp->vertices[(i + 1) % vertices.size()])));
        }
        for (size_t i = 0; i < first.size(); ++i) {
            if (check(first, second)) {
                return true;
            }
            shift(second);
        }
        return false;
    }

    bool isSimilarTo(const Shape& another) const final {
        const auto *temp = dynamic_cast<const Polygon *>(&another);
        if (temp == nullptr) {
            return false;
        }
        if (temp->vertices.size() != vertices.size()) {
            return false;
        }
        std::deque<double> first;
        std::deque<double> second;
        for (size_t i = 1; i <= vertices.size(); ++i) {
            first.push_back(sinus(Vector(vertices[i - 1], vertices[i % vertices.size()]),
                                  Vector(vertices[i % vertices.size()], vertices[(i + 1) % vertices.size()])));
            second.push_back(sinus(Vector(temp->vertices[i - 1], temp->vertices[i % vertices.size()]),
                                   Vector(temp->vertices[i % vertices.size()],
                                          temp->vertices[(i + 1) % vertices.size()])));
        }
        for (size_t i = 0; i < first.size(); ++i) {
            if (check(first, second)) {
                return true;
            }
            shift(second);
        }
        return false;
    }
};

class Ellipse : public Shape {
    std::pair<Point, Point> focuses_;
    double distance_{};
    double a_{};
    double b_{};

public:
    Ellipse() = default;

    explicit Ellipse(Point f1, Point f2, double d) :
            focuses_({f1, f2}),
            distance_(d),
            a_(d / 2), b_(sqrt(a_ * a_ - point_distance(f1, f2) * point_distance(f1, f2) / 4)) {}

    double perimeter() const override {
        double h = ((a_ - b_) * (a_ - b_)) / ((a_ + b_) * (a_ + b_));
        return PI * (a_ + b_) * (1.0 + (3 * h) / (10 + sqrt(4 - 3 * h)));
    }

    double area() const override {
        return PI * a_ * b_;
    }

    Point center() const {
        return middle(focuses_.first, focuses_.second);
    }

    double eccentricity() const {
        return sqrt(1.0 - ((b_ * b_) / (a_ * a_)));
    };

    std::pair<Point, Point> focuses() const {
        return focuses_;
    }

    std::pair<Line, Line> directrices() {
        Point p1 = focuses_.first.scale(center(), a_ * a_ / (a_ * a_ - b_ * b_));
        Point p2 = focuses_.second.scale(center(), a_ * a_ / (a_ * a_ - b_ * b_));
        Vector major_axes(focuses_.first, focuses_.second);
        return {Line(normal(major_axes), p1),
                Line(normal(major_axes), p2)};
    }

    void rotate(const Point& center, double angle) final {
        angle *= PI / 180;
        focuses_.first = focuses_.first.rotate(center, angle);
        focuses_.second = focuses_.second.rotate(center, angle);
    }

    void reflect(const Line& axis) final {
        focuses_.first = point_reflect(focuses_.first, axis);
        focuses_.second = point_reflect(focuses_.second, axis);
    }

    void reflect(const Point& center) final {
        focuses_.first = focuses_.first.reflect(center);
        focuses_.second = focuses_.second.reflect(center);
    }

    void scale(const Point& center, double coefficient) final {
        focuses_.first = focuses_.first.scale(center, coefficient);
        focuses_.second = focuses_.second.scale(center, coefficient);
        distance_ *= std::abs(coefficient);
        a_ *= std::abs(coefficient);
        b_ *= std::abs(coefficient);
    }

    bool containsPoint(const Point& point) const final {
        if (point_distance(point, focuses_.first) + point_distance(point, focuses_.second) > distance_) {
            return false;
        }
        return true;
    }

    bool operator==(const Shape& another) const final {
        const auto *temp = dynamic_cast<const Ellipse*>(&another);
        if (temp == nullptr) {
            return false;
        }
        if (std::abs(a_ - temp->a_) < eps && std::abs(b_ - temp->b_) < eps && center() == temp->center()) {
            return true;
        }
        return false;
    }

    bool operator!=(const Shape& another) const final {
        return !(*this == another);
    }

    bool isCongruentTo(const Shape& another) const final {
        const auto *temp = dynamic_cast<const Ellipse *>(&another);
        if (temp == nullptr) {
            return false;
        }
        if (std::abs(a_ - temp->a_) < eps && std::abs(b_ - temp->b_) < eps &&
            std::abs(distance_ - temp->distance_) < eps) {
            return true;
        }
        return false;
    }

    bool isSimilarTo(const Shape& another) const final {
        const auto *temp = dynamic_cast<const Ellipse *>(&another);
        if (temp == nullptr) {
            return false;
        }
        if ((a_ / temp->a_) == (b_ / temp->b_) == (distance_ / temp->distance_)) {
            return true;
        }
        return false;
    }

};

class Circle : public Ellipse {
public:
    Point center_;
    double radius_;

    Circle() = default;

    Circle(Point point, double r) : Ellipse(point, point, 2 * r), center_(point), radius_(r) {}

    double perimeter() const final {
        return 2 * PI * radius_;
    }

    double area() const final {
        return PI * radius_ * radius_;
    }

    double radius() const {
        return radius_;
    }
};

class Rectangle : public Polygon {
public:

    Rectangle() = default;

    Rectangle(const std::vector<Point>& points) : Polygon(points) {}

    Rectangle(Point v1, Point v2, Point v3, Point v4) : Polygon(v1, v2, v3, v4) {}

    Rectangle(Point v1, Point v3, double k) {
        double angle1;
        double angle2;
        if (k > 1) {
            angle1 = atan(k);
            angle2 = atan(1 / k);
        } else {
            angle1 = atan(1 / k);
            angle2 = atan(k);
        }
        Line ab(v1, v3.rotate(v1, angle1));
        Line bc(v3, v1.rotate(v3, -angle2));
        Point v2 = intersection(ab, bc);
        Point v4(2 * middle(v1, v3).x - v2.x, 2 * middle(v1, v3).y - v2.y);
        vertices = {v1, v2, v3, v4};
    }

    Point center() {
        return middle(vertices[0], vertices[2]);
    }

    std::pair<Line, Line> diagonals() {
        return {Line(vertices[0], vertices[2]), Line(vertices[1], vertices[3])};
    }
};

class Square : public Rectangle {
public:
    Square() = default;

    Square(const std::vector<Point>& points) : Rectangle(points) {}

    Square(Point v1, Point v2, Point v3, Point v4) : Rectangle(v1, v2, v3, v4) {}

    Square(Point v1, Point v3) : Rectangle(v1, v3, 1) {}

    Circle circumscribedCircle() {
        double radius = point_distance(vertices[0], vertices[2]) / 2;
        return {middle(vertices[0], vertices[2]), radius};
    }

    Circle inscribedCircle() {
        double radius = point_distance(vertices[0], vertices[1]) / 2;
        return {middle(vertices[0], vertices[2]), radius};
    }
};

class Triangle : public Polygon {
public:

    Triangle(const std::vector<Point>& points) : Polygon(points) {}

    Triangle(Point v1, Point v2, Point v3) : Polygon(v1, v2, v3) {}

    Circle circumscribedCircle() {
        Line l1(normal(Vector(vertices[0], vertices[1])), middle(vertices[0], vertices[1]));
        Line l2(normal(Vector(vertices[1], vertices[2])), middle(vertices[1], vertices[2]));
        Point center = intersection(l1, l2);
        return {center, point_distance(center, vertices[0])};
    }

    Circle inscribedCircle() {
        double vert_a = point_distance(vertices[0], vertices[1]);
        double vert_b = point_distance(vertices[1], vertices[2]);
        double vert_c = point_distance(vertices[0], vertices[2]);
        double result_x = (vert_a * vertices[2].x + vert_b * vertices[0].x + vert_c * vertices[1].x)
                          / (vert_a + vert_b + vert_c);
        double result_y = (vert_a * vertices[2].y + vert_b * vertices[0].y + vert_c * vertices[1].y)
                          / (vert_a + vert_b + vert_c);
        double per = (vert_a + vert_b + vert_c) / 2;
        double radius = sqrt((per - vert_a) * (per - vert_b) * (per - vert_c) / per);
        return {{result_x, result_y}, radius};
    }

    Point centroid() {
        return {(vertices[0].x + vertices[1].x + vertices[2].x) / 3,
                (vertices[0].y + vertices[1].y + vertices[2].y) / 3};
    }

    Point orthocenter() {
        Vector vec_bc(vertices[1], vertices[2]);
        Vector vec_ah = normal(vec_bc);
        Line line_ah(vec_ah, vertices[0]);
        Vector vec_ab(vertices[0], vertices[1]);
        Vector vec_ch = normal(vec_ab);
        Line line_ch(vec_ch, vertices[2]);
        return intersection(line_ah, line_ch);
    }

    Line EulerLine() {
        return {orthocenter(), centroid()};
    }

    Circle ninePointsCircle() {
        double radius = circumscribedCircle().radius_ / 2;
        Point center = middle(circumscribedCircle().center_, orthocenter());
        return {center, radius};
    }
};
