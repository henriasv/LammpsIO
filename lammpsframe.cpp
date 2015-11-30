#include "lammpsframe.h"
#include <algorithm>

LammpsFrame::LammpsFrame() :
                  timestep(-1)
                , numParticles(0)
                , xlo(0), xhi(0)
                , ylo(0), yhi(0)
                , zlo(0), zhi(0)
{
}

LammpsFrame::LammpsFrame(vec3 hi, vec3 lo, int in_timestep) :
    numParticles(0)
{
    xlo = lo[0];
    ylo = lo[1];
    zlo = lo[2];
    xhi = hi[0];
    yhi = hi[1];
    zhi = hi[2];
    timestep = in_timestep;
}

void LammpsFrame::addParticle(vec3 position, vec3 velocity, int type, char name)
{
    int id;
    if (ids.size() > 0)
    {
        id = *std::max_element(ids.begin(), ids.end())+1;
    }
    else
    {
        id = 1;
    }

    positions.push_back(position);
    velocities.push_back(velocity);
    types.push_back(type);
    names.push_back(name);
    ids.push_back(id);
    numParticles ++;
}

std::vector<vec3> LammpsFrame::getPositionsType(int type)
{
    std::vector<vec3> retParticles;
    for (unsigned int i = 0; i < numParticles; i++)
    {
        if (types[i] == type)
        {
            retParticles.push_back(positions[i]);
        }
    }
    return retParticles;
}

std::vector<int> LammpsFrame::getIdsType(int type)
{
    std::vector<int> retIds;
    for (unsigned int i = 0; i < numParticles; i++)
    {
        if (types[i] == type)
        {
            retIds.push_back(ids[i]);
        }
    }
    return retIds;
}

void LammpsFrame::allocate()
{
    types.resize(numParticles);
    names.resize(numParticles);
    ids.resize(numParticles);
    positions.resize(numParticles);
    velocities.resize(numParticles);
    p_diag.resize(numParticles);
    p_adiag.resize(numParticles);

}
