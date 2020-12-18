#define GLFW_INCLUDE_GLU
#define GLFW_DLL
#include "GL/glew.h"
#include <GLFW/glfw3.h>
#include <GL/glut.h>
#include <GL/gl.h>

#include <vector>
#include <random>
#include <iostream>
#include "Voronoi.h"

struct FieldColor {
    float r, g, b;
    FieldColor(float R, float G, float B) : r(R), g(G), b(B) { }
};

struct Field {
    int fieldId;
    std::vector<float> state;
    int fieldBufferOffset;
    FieldColor color = {1.0, 1.0, 1.0};
    int changeColor = true;
    std::pair<double, double> fieldCenter;
    std::vector<Point> fieldPoints;
    std::vector<Edge> fieldEdges;
    std::vector<int> neighbours;
    Field();
    Field(int id, std::pair<double, double> center);
    Field(int id, std::vector<Point> points);
    void sortPoints();
};
