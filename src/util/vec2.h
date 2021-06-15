#ifndef UTIL_VEC2_H
#define UTIL_VEC2_H

namespace util {

  /// @brief Class of vectors using 2 int types
  class Vector {
  public:
    Vector();
    Vector(const Vector &other);
    Vector(Vector &&other) noexcept;
    Vector &operator=(const Vector &other);
    Vector &operator=(Vector &&other) noexcept;

    Vector(int x, int y);

    Vector &operator+=(const Vector &other);
    Vector &operator-=(const Vector &other);

    Vector &operator*=(const int scalar);
    Vector operator*(const int scalar) const;

    int x;
    int y;
  };

  /// @brief Class of vectors using 2 float types
  class Vectorf {
  public:
    Vectorf();
    Vectorf(const Vectorf &other);
    Vectorf(Vectorf &&other) noexcept;
    Vectorf &operator=(const Vectorf &other);
    Vectorf &operator=(Vectorf &&other) noexcept;

    Vectorf(float x, float y);

    Vectorf &operator+=(const Vectorf &other);
    Vectorf &operator-=(const Vectorf &other);

    Vectorf &operator*=(const float scalar);
    Vectorf operator*(const float scalar) const;

    float x;
    float y;
  };
} // namespace util

#endif // UTIL_VEC2_H
