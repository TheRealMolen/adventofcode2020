#pragma once

template<typename T>
struct Pt2
{
    using el_type = T;
    using type = Pt2<el_type>;

    el_type x, y;

    type& operator+=(const type& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    type operator+(const type& other) const
    {
        return type{ x + other.x, y + other.y };
    }
    type operator*(el_type scalar) const
    {
        return type{ x * scalar, y * scalar };
    }

    void rotRight(int steps)
    {
        int sgn = steps > 0 ? 1 : -1;
        steps = abs(steps);
        for (int s = 0; s < steps; ++s)
        {
            swap(x, y);
            x *= sgn;
            y *= sgn * -1;
        }
    }
};

using Pt2i = Pt2<int>;
using Pt2d = Pt2<double>;
