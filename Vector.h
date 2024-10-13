#pragma once
#ifndef Vector_h
#define Vector_h

#include <math.h>

class Vector2 {
  public:
    float x = 0.f;
    float y = 0.f;

  public:
    Vector2() {
        x = 0.f;
        y = 0.f;
    }

    Vector2(float _x, float _y) {
        x = _x;
        y = _y;
    }

    inline bool operator==(const Vector2 &A) const {
        return A.x == x && A.y == y;
    }

    inline Vector2 operator+(const float A) const {
        return Vector2(x + A, y + A);
    }

    inline Vector2 operator+(const Vector2 &A) const {
        return Vector2(x + A.x, y + A.y);
    }

    inline Vector2 operator*(const float A) const {
        return Vector2(A * x, A * y);
    }

    inline Vector2 operator*(const Vector2 &A) const {
        return Vector2(A.x * x, A.y * y);
    }

    inline Vector2 operator-(const float A) const {
        return Vector2(A - x, A - y);
    }

    inline Vector2 operator-(const Vector2 &A) const {
        return Vector2(x - A.x, y - A.y);
    }

    inline Vector2 operator/(const float A) const {
        return Vector2(A / x, A / y);
    }

    inline Vector2 operator/(const Vector2 &A) const {
        return Vector2(A.x / x, A.y / y);
    }

    float length() {
        return sqrt(x * x + y * y);
    }

    float distance(const Vector2 &o) {
        return sqrt(pow(x - o.x, 2) + pow(y - o.y, 2));
    }

    float distance_squared(const Vector2 &o) {
        return pow(x - o.x, 2) + pow(y - o.y, 2);
    }

    Vector2 vscale(const Vector2 &s) {
        return Vector2(x * s.x, y * s.y);
    }

    Vector2 scale(float s) {
        return Vector2(x * s, y * s);
    }

    Vector2 normalize() {
        float l = length();
        return Vector2(x / l, y / l);
    }

    Vector2 add(const Vector2 &o) {
        return Vector2(x + o.x, y + o.y);
    }

    Vector2 sub(const Vector2 &o) {
        return Vector2(x - o.x, y - o.y);
    }

    Vector2 clone() {
        return Vector2(x, y);
    }
};

class Vector3 {
  public:
    float x = 0.f;
    float y = 0.f;
    float z = 0.f;

  public:
    Vector3() {
        x = 0.f;
        y = 0.f;
        z = 0.f;
    }

    Vector3(float _x, float _y, float _z) {
        x = _x;
        y = _y;
        z = _z;
    }

    float length() {
        return sqrt(x * x + y * y + z * z);
    }

    float distance(const Vector3 &o) {
        return sqrt(pow(x - o.x, 2) + pow(y - o.y, 2) + pow(z - o.z, 2));
    }

    float distance_squared(const Vector3 &o) {
        return pow(x - o.x, 2) + pow(y - o.y, 2) + pow(z - o.z, 2);
    }

    inline Vector3 &operator=(const Vector3 &A) {
        this->x = A.x;
        this->y = A.y;
        this->z = A.z;

        return *this;
    }

    inline bool operator==(const Vector3 &A) const {
        return A.x == x && A.y == y && A.z == z;
    }

    inline Vector3 operator+(const Vector3 &A) const {
        return Vector3(x + A.x, y + A.y, z + A.z);
    }

    inline Vector3 operator+(const float A) const {
        return Vector3(x + A, y + A, z + A);
    }

    inline Vector3 operator*(const float A) const {
        return Vector3(A * x, A * y, A * z);
    }

    inline Vector3 operator*(const Vector3 &A) const {
        return Vector3(A.x * x, A.y * y, A.z * z);
    }

    inline Vector3 operator-(const float A) const {
        return Vector3(A - x, A - y, A - z);
    }

    inline Vector3 operator-(const Vector3 &A) const {
        return Vector3(x - A.x, y - A.y, z - A.z);
    }

    inline Vector3 operator/(const float A) const {
        return Vector3(A / x, A / y, A / z);
    }

    inline Vector3 operator/(const Vector3 &A) const {
        return Vector3(A.x / x, A.y / y, A.z / z);
    }

    Vector3 rotate_x(float angle) {
        return Vector3(
            x,
            y * cos(angle) - z * sin(angle),
            y * sin(angle) + z * cos(angle));
    }

    Vector3 rotate_y(float angle) {
        return Vector3(
            x * cos(angle) + z * sin(angle),
            y,
            -x * sin(angle) + z * cos(angle));
    }

    Vector3 rotate_z(float angle) {
        return Vector3(
            x * cos(angle) - y * sin(angle),
            x * sin(angle) + y * cos(angle),
            z);
    }

    Vector3 vscale(const Vector3 &s) {
        return Vector3(x * s.x, y * s.y, z * s.z);
    }

    Vector3 scale(float s) {
        return Vector3(x * s, y * s, z * s);
    }

    Vector3 normalize() {
        float l = length();
        return Vector3(x / l, y / l, z / l);
    }

    Vector3 add(const Vector3 &o) {
        return Vector3(x + o.x, y + o.y, z + o.z);
    }

    Vector3 sub(const Vector3 &o) {
        return Vector3(x - o.x, y - o.y, z - o.z);
    }

    Vector3 clone() {
        return Vector3(x, y, z);
    }

    Vector3 extend(const Vector3 &dir, double distance) {
        return add(dir).scale(distance);
    }
};

class Vector4 {
  public:
    float x = 0.f;
    float y = 0.f;
    float z = 0.f;
    float w = 0.f;

  public:
    Vector4() {
        x = 0.f;
        y = 0.f;
        z = 0.f;
        w = 0.f;
    }

    Vector4(float _x, float _y, float _z, float _w) {
        x = _x;
        y = _y;
        z = _z;
        w = _w;
    }

    float length() {
        return sqrt(x * x + y * y + z * z + w * w);
    }

    float distance(const Vector4 &o) {
        return sqrt(pow(x - o.x, 2) + pow(y - o.y, 2) + pow(z - o.z, 2) + pow(w - o.w, 2));
    }

    float distance_squared(const Vector4 &o) {
        return pow(x - o.x, 2) + pow(y - o.y, 2) + pow(z - o.z, 2) + pow(w - o.w, 2);
    }

    Vector4 vscale(const Vector4 &s) {
        return Vector4(x * s.x, y * s.y, z * s.z, w * s.w);
    }

    Vector4 scale(float s) {
        return Vector4(x * s, y * s, z * s, w * s);
    }

    Vector4 normalize() {
        float l = length();
        return Vector4(x / l, y / l, z / l, w / l);
    }

    Vector4 add(const Vector4 &o) {
        return Vector4(x + o.x, y + o.y, z + o.z, w + o.w);
    }

    Vector4 sub(const Vector4 &o) {
        return Vector4(x - o.x, y - o.y, z - o.z, w - o.w);
    }

    Vector4 clone() {
        return Vector4(x, y, z, w);
    }
};

#endif /* Vector_h */