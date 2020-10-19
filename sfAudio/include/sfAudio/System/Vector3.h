#ifndef VECTOR3_H
#define VECTOR3_H

struct Vector3f {

    Vector3f()
        : x(0)
        , y(0)
        , z(0)
    {

    }

    Vector3f(float _x, float _y, float _z)
        : x(_x)
        , y(_y)
        , z(_z)
    {

    }

    float x, y, z;

};

#endif // VECTOR3_H
