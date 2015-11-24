#ifndef LAMMPSFRAME_H
#define LAMMPSFRAME_H

#include <vector>
#include "vec3.h"

class LammpsFrame
{ 
public:
    LammpsFrame();
    LammpsFrame(vec3 hi, vec3 lo, int timestep);

    void addParticle(vec3 position, vec3 velocity, int type, char element);
    std::vector<vec3> getPositionsType(int);
    std::vector<int> getIdsType(int);
    void allocate();

    int timestep;
    int numParticles;
    double xlo;
    double xhi;
    double ylo;
    double yhi;
    double zlo;
    double zhi;
    std::vector<vec3> positions;
    std::vector<vec3> velocities;
    std::vector<vec3> p_diag;
    std::vector<vec3> p_adiag;
    std::vector<int>  types;
    std::vector<char> names;
    std::vector<int>  ids;
};

#endif // LAMMPSFRAME_H
