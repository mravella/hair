#include "integrator.h"

Integrator::Integrator()
{

}

Integrator::~Integrator()
{

}

double Integrator::rk4(std::function<double(double, double)> rate, double x, double y, double timeStep)
{
    double k1 = rate(x, y);
    double k2 = rate(x + .5 * timeStep * k1, y + 0.5 * timeStep * k1);
    double k3 = rate(x + .5 * timeStep * k2, y + 0.5 * timeStep * k2);
    double k4 = rate(x      + timeStep * k3, y       + timeStep * k3);

    return timeStep / 6.0 * (k1 + 2.0 * k2 + 2.0 * k3 + k4);
}
