#ifndef SIMULATION_H
#define SIMULATION_H

#include "hairCommon.h"
#include "integrator.h"
#include "objmesh.h"
#include "md5.h"
#include <QMap>
#include <tuple>
#include <iostream>
#include <map>
#include <string>

#include <pthread.h>




class HairObject;
class Hair;
class GLWidget;

#define HAIRS_PER_THREAD 20


struct fluid
{
    glm::vec3 velocity;
    double    density;

    fluid()
    {
        velocity = glm::vec3(0.0);
        density = 0.0;
    }

    fluid(double d, glm::vec3 v)
    {
        velocity = v;
        density = d;
    }

};

struct grid_loc
{
    glm::vec3 pos;

    grid_loc()
    {
        pos = glm::vec3(0.0);
    }

    grid_loc(glm::vec3 p)
    {
        pos = p;
    }

    bool operator==(const grid_loc &other) const
    {
        return (pos == other.pos);
    }

    bool operator<(const grid_loc &rhs) const {
        return (glm::length(pos) < glm::length(rhs.pos));
    }
};

struct grid_loc_hash
{
    std::size_t operator() (const grid_loc &key) const
    {
        QTime t;
        t.start();
        std::string s = md5(glm::to_string(key.pos));
        const char *x = s.c_str();

        size_t returnValue = htoi(x);
        return returnValue;
    }
};

struct HairSimulationThreadInfo {
    Hair *hairs[HAIRS_PER_THREAD];

//    QMap<std::tuple<double, double, double>, double> *densityGrid;
//    QMap<std::tuple<double, double, double>, glm::vec3> *velocityGrid;
    std::map<grid_loc, fluid> *fluidGrid;
    float friction;

};


class Simulation
{
    friend class HairInterface;
public:
    Simulation(GLWidget *widget, ObjMesh *mesh);
    
    virtual ~Simulation();
    
    void update(float _time);
    void simulate(HairObject *_object);
    void updateHairPosition(HairObject *object);
    void updatePosition(HairObject *object, glm::vec3 xform);
    void updateRotation(HairObject *object, float angle, glm::vec3 axis);
    
    glm::mat4 m_xform;
    
    
    
private:
    void moveObjects(HairObject *_object);
    
    void calculateExternalForces(HairObject *_object);
    
    void calculateFluidGrid(HairObject *_object);
    
    void calculateFrictionAndRepulsion(HairObject *_object);
    static void* calculateFrictionAndRepulsionThread(void *untypedInfoStruct);
    
    static glm::vec3 gradient(std::map<grid_loc, fluid> &map, glm::vec3 pt);
    
    void integrate(HairObject *_object);
    void integrate2(HairObject *_object);
    void integrate3(HairObject *_object);
    void integrate4(HairObject *_object);
    
    void particleSimulation(HairObject *obj);
    
    void insertFluid(std::map<grid_loc, fluid> &map, glm::vec3 pos, double density, glm::vec3 vel);
    static fluid getFluid(std::map<grid_loc, fluid> &map, glm::vec3 pos);
    static glm::vec3 getFluidVelocity(std::map<grid_loc, fluid> &map, glm::vec3 pos);
    static double getFluidDensity(std::map<grid_loc, fluid> &map, glm::vec3 pos);

    
    
    
public:
    QList<glm::vec3> m_externalForces;
    std::map<grid_loc, fluid> m_fluidGrid;
    bool m_headMoving;

    glm::vec3 m_windDir;
    float m_windMagnitude;
    float m_friction;
    float m_stiffness;
    
    
    
private:
    float m_time;
    GLWidget *m_widget;
    ObjMesh *m_mesh;
        
    pthread_t m_threads[200];
    HairSimulationThreadInfo m_threadData[200];
    
};

#endif // SIMULATION_H

