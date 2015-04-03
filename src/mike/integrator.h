#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include <functional>

class Integrator
{
public:
    Integrator();
    ~Integrator();

    static double rk4(std::function<double(double, double)> rate, double theta, double omega, double timeStep);
};

#endif // INTEGRATOR_H
