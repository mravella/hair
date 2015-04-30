/**
 * @file hairCommon.h
 *
 * Contains data structures and macros commonly used in hair.
 */
#pragma once
#ifndef __HAIRCOMMON_H__
#define __HAIRCOMMON_H__

#include "GL/glew.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <vector>

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"             // glm::vec*, mat*, and basic glm functions
#include "glm/gtx/string_cast.hpp" // glm::to_string
#include "glm/gtx/transform.hpp"   // glm::translate, scale, rotate
#include "glm/gtc/type_ptr.hpp"    // glm::value_ptr
#include "glm/gtc/constants.hpp"

// glu.h in different location on macs
#ifdef __APPLE__
#include <glu.h>
#else
#include <GL/glu.h>
#endif

uint qHash ( std::tuple<double, double, double> key);

#include <QMessageBox>

// from http://en.wikipedia.org/wiki/Assertion_(computing)
#define COMPILE_TIME_ASSERT(pred) switch(0){case 0:case pred:;}

using std::cerr;
using std::cout;
using std::endl;
using std::max;
using std::min;



struct BGRA
{
    BGRA() : b(0), g(0), r(0), a(0) {}
    BGRA(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha = 255) : b(blue), g(green), r(red), a(alpha) {}

    union {
        struct { unsigned char b, g, r, a; };
        unsigned char channels[4];
    };
};



struct Triangle {
    Triangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3,
             glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3,
             glm::vec3 n1, glm::vec3 n2, glm::vec3 n3) :
        v1(v1), v2(v2), v3(v3), uv1(uv1), uv2(uv2), uv3(uv3), n1(n1), n2(n2), n3(n3) {}
    glm::vec3 v1, v2, v3;
    glm::vec2 uv1, uv2, uv3;
    glm::vec3 n1, n2, n3;

    float area() { return glm::length(glm::cross(v3 - v1, v2 - v1)) / 2.f; }

    void randPoint(glm::vec3 &pos, glm::vec2 &uv, glm::vec3 &normal)
    {
        float t = sqrt(rand() / (float) RAND_MAX);
        glm::vec3 randPtBary;
        randPtBary.x = 1 - t;
        randPtBary.y = rand() / (float) RAND_MAX;
        randPtBary.z = 1 - randPtBary.x - randPtBary.y;
        uv = glm::mat3x2(uv1, uv2, uv3) * randPtBary;
        pos = glm::mat3(v1, v2, v3) * randPtBary;
        normal = glm::mat3(n1, n2, n3) * randPtBary;
    }

    // Using Moller-Trumbore method for triangle-ray intersection
    bool intersect(glm::vec3 &intersection, glm::vec3 ro, glm::vec3 rd)
    {
        glm::vec3 edge1 = v2 - v1;
        glm::vec3 edge2 = v3 - v1;
        glm::vec3 pVec = glm::cross(rd, edge2);

        float det = glm::dot(edge1, pVec);
        if (det == 0) return false;
        float invDet = 1.0f / det;

        glm::vec3 tVec = ro - v1;
        float u = glm::dot(tVec, pVec) * invDet;
        if (u < 0.0 || u > 1.0) return false;

        glm::vec3 qVec = glm::cross(tVec, edge1);
        float v = glm::dot(rd, qVec) * invDet;
        if (v < 0.0 || u + v > 1.0) return false;

        glm::vec3 intersectionPoint = v1 + u * edge1 + v * edge2;
        float t = (intersectionPoint.x - ro.x) / rd.x;
        if (t < 0) return false;

        intersection = intersectionPoint;
        return true;
    }

};

inline unsigned int htoi(const char s[])
{
    unsigned int val = 0;
    int x = 0;

    if(s[x] == '0' && (s[x+1]=='x' || s[x+1]=='X')) x+=2;

    while(s[x]!='\0')
    {
       if(val > UINT_MAX) return 0;
       else if(s[x] >= '0' && s[x] <='9')
       {
          val = val * 16 + s[x] - '0';
       }
       else if(s[x]>='A' && s[x] <='F')
       {
          val = val * 16 + s[x] - 'A' + 10;
       }
       else if(s[x]>='a' && s[x] <='f')
       {
          val = val * 16 + s[x] - 'a' + 10;
       }
       else return 0;

       x++;
    }
    return val;
}


inline bool operator==(const Triangle& lhs, const Triangle& rhs){
    if (lhs.v1 != rhs.v1) return false;
    if (lhs.v2 != rhs.v2) return false;
    if (lhs.v3 != rhs.v3) return false;
    return true;
}

struct HairVertex
{
    glm::vec3 position;
    glm::vec3 tempPos;
    glm::vec3 velocity;
    glm::vec3 forces;
    glm::vec3 correctionVector;
    glm::vec3 startPosition;
    glm::vec3 prevPos;
    glm::vec3 pointVector;
    double    theta;
    double    omega;
    float    segLen;
    float     mass;
    float     invMass;

    HairVertex(void)
    {
        position = glm::vec3(0.0);
        velocity = glm::vec3(0.0);
        prevPos = glm::vec3(0.0);
        omega = 0.0;
        theta = 0.0;
        segLen = 0.0;
        tempPos = glm::vec3(0.0);
        startPosition = glm::vec3(0.0);
        forces = glm::vec3(0.0);
        correctionVector = glm::vec3(0.0);
        mass = 1.0;
        invMass = 1.0;
        pointVector = glm::vec3(0.0);
    }

    HairVertex(glm::vec3 x)
    {
        position = x;
        tempPos = x;
        startPosition = x;
        prevPos = x;
        velocity = glm::vec3(0.0);
        omega = 0.0;
        theta = 0.0;
        segLen = 0.0;
        forces = glm::vec3(0.0);
        correctionVector = glm::vec3(0.0);
        mass = 1.0;
        invMass = 1.0;
        pointVector = glm::vec3(0.0);

    }
};


struct Joint
{
    glm::vec3 position;
    glm::vec3 linearForce;
    glm::vec3 angularForce;
    glm::vec3 constraintForce;

    Joint()
    {
        position = glm::vec3(0.);
        linearForce = glm::vec3(0.);
        angularForce = glm::vec3(0.);
        constraintForce = glm::vec3(0.);
    }
    Joint(glm::vec3 _position)
    {
        position = _position;
        linearForce = glm::vec3(0.);
        angularForce = glm::vec3(0.);
        constraintForce = glm::vec3(0.);
    }
};

// ----------------------
// Deallocation Utilities
// ----------------------

#define safeFree(p)  \
   do {              \
      if ((p)) {     \
         free((p));  \
         (p) = NULL; \
      }              \
   } while(0)

#define safeDelete(p)   \
   do {                 \
      if ((p)) {        \
         delete ((p));  \
         (p) = NULL;    \
      }                 \
   } while(0)

#define safeDeleteArray(p) \
   do {                    \
      if ((p)) {           \
         delete[] ((p));   \
         (p) = NULL;       \
      }                    \
   } while(0)


// ---------------------
// Common math utilities
// ---------------------

#ifdef MIN
    #undef MIN
#endif

#ifdef MAX
    #undef MAX
#endif

#ifdef MIN3
    #undef MIN3
#endif

#ifdef MAX3
    #undef MAX3
#endif

#ifdef ABS
    #undef ABS
#endif

#define MIN(x, y) ({          \
   float x_ = (x);       \
   float y_ = (y);       \
   ((x_ < y_) ? x_ : y_);     \
})

#define MAX(x, y) ({          \
   float _x_ = (x);      \
   float _y_ = (y);      \
   ((_x_ > _y_) ? _x_ : _y_); \
})

#define MIN3(x, y, z) ({      \
   float x__ = (x);      \
   float y__ = (y);      \
   float z__ = (z);      \
   ((x__ < y__) ? (x__ < z__ ? x__ : z__) : (y__ < z__ ? y__ : z__)); \
})

#define MAX3(x, y, z) ({      \
   float __x = (x);      \
   float __y = (y);      \
   float __z = (z);      \
   ((__x > __y) ? (__x > __z ? __x: __z) : (__y > __z ? __y : __z)); \
})

#define ABS(x) ({                   \
   register typeof(x) ___tx = (x);  \
   (___tx >= 0 ? ___tx : -___tx);   \
})

#define CLAMP(x, y, z) ({           \
    double _x_ = (x);    \
    double _y_ = (y);    \
    double _z_ = (z);    \
    (std::max(_y_, std::min(_x_, _z_))); \
})

template <typename T> int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}


#ifndef M_PI
    #define M_PI 3.14159265f
#endif

#define _EPSILON_ 1e-6
#define EPSILON _EPSILON_

#define EQ(a, b) (fabs((a) - (b)) < _EPSILON_)
#define NEQ(a, b) (fabs((a) - (b)) > _EPSILON_)


#define NYI(f) { \
       char ss[999]; \
       (sprintf(ss, "Not yet implemented: %s, file %s, line %d\n", \
                              f, __FILE__, __LINE__)); \
       QMessageBox mb; \
       mb.setText(QString(ss)); \
       mb.setIcon(QMessageBox::Critical); \
       mb.exec(); \
       exit(0xf); \
   }

#define PRINT_VEC(__name, __vec) cout << __name << glm::to_string(__vec) << endl;

#endif


