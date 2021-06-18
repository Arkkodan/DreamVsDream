#include "vec2.h"

util::Vector::Vector() : x(0), y(0) {}

util::Vector::Vector(const Vector &other) : x(other.x), y(other.y) {}

util::Vector::Vector(Vector &&other) noexcept : x(other.x), y(other.y) {}

util::Vector &util::Vector::operator=(const Vector &other) {
  x = other.x;
  y = other.y;
  return *this;
}

util::Vector &util::Vector::operator=(Vector &&other) noexcept {
  x = other.x;
  y = other.y;
  return *this;
}

util::Vector::Vector(int x, int y) : x(x), y(y) {}

util::Vector &util::Vector::operator+=(const Vector &other) {
  x += other.x;
  y += other.y;
  return *this;
}

util::Vector &util::Vector::operator-=(const Vector &other) {
  x -= other.x;
  y -= other.y;
  return *this;
}

util::Vector &util::Vector::operator*=(const int scalar) {
  x *= scalar;
  y *= scalar;
  return *this;
}

util::Vector util::Vector::operator*(const int scalar) const {
  Vector result;
  return result *= scalar;
}

util::Vectorf::Vectorf() : x(0.0), y(0.0) {}

util::Vectorf::Vectorf(const Vectorf &other) : x(other.x), y(other.y) {}

util::Vectorf::Vectorf(Vectorf &&other) noexcept : x(other.x), y(other.y) {}

util::Vectorf &util::Vectorf::operator=(const Vectorf &other) {
  x = other.x;
  y = other.y;
  return *this;
}

util::Vectorf &util::Vectorf::operator=(Vectorf &&other) noexcept {
  x = other.x;
  y = other.y;
  return *this;
}

util::Vectorf::Vectorf(float x, float y) : x(x), y(y) {}

util::Vectorf &util::Vectorf::operator+=(const Vectorf &other) {
  x += other.x;
  y += other.y;
  return *this;
}

util::Vectorf &util::Vectorf::operator-=(const Vectorf &other) {
  x -= other.x;
  y -= other.y;
  return *this;
}

util::Vectorf &util::Vectorf::operator*=(const float scalar) {
  x *= scalar;
  y *= scalar;
  return *this;
}

util::Vectorf util::Vectorf::operator*(const float scalar) const {
  Vectorf result;
  return result *= scalar;
}
