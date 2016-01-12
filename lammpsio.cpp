#include "lammpsio.h"


int elementCharToType(char);

LammpsIO::LammpsIO(std::string inputFile)
{
    m_infile.open(inputFile);
    if (!m_infile)
    {
        std::cout << "could not open file " << inputFile << " for input" << std::endl;
        exit(1);
    }
    LammpsFrame frame;
    scanFileForTimestepPositions();
    m_infile.close();
    m_infile.open(inputFile);
}

LammpsIO::~LammpsIO()
{
    m_infile.close();
    m_outfile.close();
}


void LammpsIO::readNextFrame(LammpsFrame & frame)
{
    int buflen = 8192;
    char buf[buflen];
    char* token;


    // Find timestep number
    m_infile.getline(buf, buflen);
    m_infile.getline(buf, buflen);

    if (m_infile.eof())
    {
        return;
    }

    frame.timestep = atoi(buf);
    // Find number of atoms
    m_infile.getline(buf, buflen);
    m_infile.getline(buf, buflen);
    frame.numParticles = atoi(buf);
    std::cout << "NumParticles " << frame.numParticles << std::endl;

    // Find box bounds
    m_infile.getline(buf, buflen);
    m_infile.getline(buf, buflen);
    token = std::strtok(buf, " ");
    frame.xlo = atof(token);
    token = std::strtok(NULL, " ");
    frame.xhi = atof(token);
    m_infile.getline(buf, buflen);
    token = std::strtok(buf, " ");
    frame.ylo = atof(token);
    token = std::strtok(NULL, " ");
    frame.yhi = atof(token);
    m_infile.getline(buf, buflen);
    token = std::strtok(buf, " ");
    frame.zlo = atof(token);
    token = std::strtok(NULL, " ");
    frame.zhi = atof(token);

    // Read atom data
    frame.allocate();

    m_infile.getline(buf, buflen);
    int counter = 0;
    int id;
    double x, y, z;
    double pxx, pyy, pzz, pxy, pxz, pyz;
    while (counter < frame.numParticles)
    {
        m_infile.getline(buf, buflen);

        token = std::strtok(buf, " ");
        id = atoi(token);
        frame.ids[counter] = id;
        token = std::strtok(NULL, " ");
        char* element = token;
        frame.names[counter] = element[0];
        frame.types[counter] = elementCharToType(element[0]);
        token = std::strtok(NULL, " ");
        x = atof(token);
        token = std::strtok(NULL, " ");
        y = atof(token);
        token = std::strtok(NULL, " ");
        z = atof(token);
        //std::cout<<"z: " << z << " ";

        token = std::strtok(NULL, " ");
        token = std::strtok(NULL, " ");
        token = std::strtok(NULL, " ");

        token = std::strtok(NULL, " ");
        pxx = atof(token);
        token = std::strtok(NULL, " ");
        pyy = atof(token);
        token = std::strtok(NULL, " ");
        pzz = atof(token);
        token = std::strtok(NULL, " ");
        pxy = atof(token);
        token = std::strtok(NULL, " ");
        pxz = atof(token);
        token = std::strtok(NULL, " ");
        pyz = atof(token);


//        frame.positions.push_back(vec3(x, y, z));
        frame.positions[counter].setX(x);
        frame.positions[counter].setY(y);
        frame.positions[counter].setZ(z);
//        frame.ids.push_back(id);

//        frame.p_diag.push_back(vec3(pxx, pyy, pzz));
        frame.p_diag[counter].setX(pxx);
        frame.p_diag[counter].setY(pyy);
        frame.p_diag[counter].setZ(pzz);
//        frame.p_adiag.push_back(vec3(pxy, pxz, pyz));
        frame.p_adiag[counter].setX(pxy);
        frame.p_adiag[counter].setY(pxz);
        frame.p_adiag[counter].setZ(pyz);
        counter ++;
    }
    std::cout << "Read timestep " << frame.timestep << std::endl;
    //printLammpsFrame(frame);
}

void LammpsIO::readFrame(LammpsFrame & frame, int wantFrame)
{
    if (m_framePositions.count(wantFrame)>0)
        m_infile.seekg(m_framePositions[wantFrame]);
    else
    {
        std::cout << "Unable to read timestep " << wantFrame << std::endl;
        return;
    }
    this->readNextFrame(frame);
}

void LammpsIO::scanFileForTimestepPositions()
{
    int buflen = 8196;
    char buf[buflen];
    int timestep = -1;

    while (timestep < INFINITY)
    {
        std::ifstream::pos_type timestep_start = m_infile.tellg();
        m_infile.getline(buf, buflen);

        if (m_infile.eof())
            return;

        m_infile.getline(buf, buflen);
        timestep = atof(buf);
        m_availableFrames[timestep] = true;
        m_framePositions[timestep] = timestep_start;

        std::cout << "Passing timestep: " << timestep << std::endl;


        m_infile.getline(buf, buflen);
        m_infile.getline(buf, buflen);
        int num_particles = atoi(buf);

        for (int j= 0; j<5; j++)
        {
            m_infile.getline(buf, buflen);
        }

        std::ifstream::pos_type timestep_start2;
        int particle_entry_length = 114;
        m_infile.seekg(m_infile.tellg()+(particle_entry_length*num_particles));
        const char* buf1 = "ITEM: TI";
        while (strncmp(buf, buf1, 8)) // evaluates to false when string starts with ITEM: T
        {
            timestep_start2 = m_infile.tellg();
            m_infile.getline(buf, buflen);
            if (m_infile.eof())
            {
                return;
            }
        }
        m_infile.seekg(timestep_start2);
    }
}

bool LammpsIO::isAvailable(int frameNum)
{
    return m_availableFrames[frameNum];
}

std::vector<int> LammpsIO::availableFrames()
{
    std::vector<int> frames;
    for (auto keyval : m_availableFrames)
        frames.push_back(keyval.first);
    return frames;
}

void LammpsIO::dumpFrame(LammpsFrame & frame)
{
    if (m_outfile.is_open())
    {
        m_outfile << "ITEM: TIMESTEP\n";
        m_outfile << frame.timestep << std::endl;
        m_outfile << "ITEM: NUMBER OF ATOMS\n";
        m_outfile << frame.numParticles << std::endl;
        m_outfile << "ITEM: BOX BOUNDS pp pp pp\n";
        m_outfile << frame.xlo << " " << frame.xhi << std::endl;
        m_outfile << frame.ylo << " " << frame.yhi << std::endl;
        m_outfile << frame.zlo << " " << frame.zhi << std::endl;
        m_outfile << "ITEM: ATOMS id element x y z\n";
        for (int i = 0; i<frame.numParticles; i++)
        {
            m_outfile << frame.ids[i] << " " << frame.names[i]
                       << " " << frame.positions[i].x() << " " << frame.positions[i].y()
                       << " " << frame.positions[i].z() << "\n";
        }
    }
    else
    {
        std::cout << "Trying to dump frame to closed output file" << std::endl;
        exit(1);
    }
}

void LammpsIO::dumpVoro(LammpsFrame & frame, std::string filePath)
{
    std::ofstream outfile(filePath);
    for (int i = 0; i<frame.numParticles; i++)
    {
        outfile << frame.ids[i] << " " << frame.positions[i].x() << " " << frame.positions[i].y() << " " << frame.positions[i].z() << std::endl;
    }
    outfile.close();
    std::cout << frame.xlo << " " << frame.xhi << " " << frame.ylo << " " << frame.yhi << " " << frame.zlo << " " << frame.zhi << std::endl;
}

void LammpsIO::openDumpFile(std::string dumpFile)
{
    m_outfile.open(dumpFile);
    if (!m_outfile.is_open())
    {
        std::cout << "Failed to open output file " << dumpFile << std::endl;
    }
}

int LammpsIO::elementCharToType(char element)
{
   if (element == 'H')
   {
       return 1;
   }
   else if (element == 'O')
   {
       return 2;
   }
   else if (element == 'C')
   {
       return 3;
   }
   if (element == 'S')
   {
       return 4;
   }
}
