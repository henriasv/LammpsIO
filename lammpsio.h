#ifndef LAMMPSREADER_H
#define LAMMPSREADER_H

#include <string>
#include <vector>
#include <fstream>
#include "vec3/vec3.h"
#include "lammpsframe.h"
#include <map>

class LammpsIO
{
public:
    LammpsIO(std::string);
    ~LammpsIO();


    /**
     * @brief readNextFrame
     * Reads a timestep entry from m_infile given that the current position in the file is correct.
     * The implementation assumes a specific output format (including per-atom pressures) when reading.
     */
    void readNextFrame(LammpsFrame &);

    /**
     * @brief readFrame
     * Finds the position of the wanted timestep in the file (if it exists) and then calls readNextFrame to actually read the data.
     * A special feature in the implementation of the position search is the particle_entry_length*num_particles skip in the file. This is used to avoid actually reading all lines. The value of particle_entry_length can be tuned to speed up searcing, but if the value is too high, the result is skipping over whole timesteps without cheching what timesteps they are.
     *
     */
    void readFrame(LammpsFrame &, int);
    bool isAvailable(int frameNum);
    std::vector<int> availableFrames();
    void dumpFrame(LammpsFrame&);
    void dumpVoro(LammpsFrame&, std::string filePath);
    void openDumpFile(std::string);
    void resetFile();
    int elementCharToType(char);

private:
    std::ifstream m_infile;
    std::ofstream m_outfile;
    std::map<int, bool> m_availableFrames;
};

#endif // LAMMPSREADER_H
