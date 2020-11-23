#include "Field.h"
#include <queue>

struct Board {
    std::vector<Field> fields;

    unsigned int linesBuffer;
    unsigned int trianglesBuffer;
    double* lines;
    double* triangles;
    int linesSize = 0;
    int trianglesSize = 0;

    Board();
    unsigned int CompileShader(unsigned int type, std::string& source);
    unsigned int CreateShader(std::string& vertexShader, std::string& fragmentShader);
    void createBuffers();
    void fillFieldsCenters(int fieldsNumber);
    void print();
    void neighbours(int v);
};
