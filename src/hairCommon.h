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
#include <QMessageBox>
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

// from http://en.wikipedia.org/wiki/Assertion_(computing)
#define COMPILE_TIME_ASSERT(pred) switch(0){case 0:case pred:;}

using std::cerr;
using std::cout;
using std::endl;
using std::max;
using std::min;

typedef double REAL;

struct HairVertex
{
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;

    HairVertex(void)
    {
        position = glm::vec3(0.0);
        velocity = glm::vec3(0.0);
        acceleration = glm::vec3(0.0);
    }

    HairVertex(glm::vec3 x)
    {
        position = x;
        velocity = glm::vec3(0.0);
        acceleration = glm::vec3(0.0);
    }

    HairVertex(glm::vec3 x, glm::vec3 v, glm::vec3 a)
    {
        position = x;
        velocity = v;
        acceleration = a;
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

#endif
