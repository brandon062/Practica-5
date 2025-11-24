#ifndef VECTOR2D_H
#define VECTOR2D_H

#include <cmath>

struct Vector2D {
    double x{0.0}, y{0.0};
    Vector2D() = default;
    Vector2D(double x_, double y_) : x(x_), y(y_) {}

    Vector2D operator+(const Vector2D& o) const { return {x+o.x, y+o.y}; }
    Vector2D operator-(const Vector2D& o) const { return {x-o.x, y-o.y}; }
    Vector2D operator*(double k)        const { return {x*k, y*k}; }
    Vector2D operator/(double k)        const { return {x/k, y/k}; }

    Vector2D& operator+=(const Vector2D& o){ x+=o.x; y+=o.y; return *this; }
    Vector2D& operator-=(const Vector2D& o){ x-=o.x; y-=o.y; return *this; }
    Vector2D& operator*=(double k){ x*=k; y*=k; return *this; }
};

inline double productoPunto(const Vector2D& a, const Vector2D& b){
    return a.x*b.x + a.y*b.y;
}
inline double magnitud2(const Vector2D& a){
    return productoPunto(a,a);
}
inline double magnitud(const Vector2D& a){
    return std::sqrt(magnitud2(a));
}
inline Vector2D normalizar(const Vector2D& a){
    double m = magnitud(a);
    return (m>0.0) ? a/m : Vector2D(0,0);
}
inline double limitar(double v, double minimo, double maximo){
    if (v < minimo) return minimo;
    if (v > maximo) return maximo;
    return v;
}

#endif // VECTOR2D_H
