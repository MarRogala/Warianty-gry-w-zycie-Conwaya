#include "Field.h"
#include <set>
#include <algorithm>

Field::Field() { }
Field::Field(int id, std::vector<Point> points) : fieldId(id), fieldPoints(points) { }
Field::Field(int id, std::pair<double, double> center) : fieldId(id), fieldCenter(center) {
    /*float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    color = {r, g, b};*/
}

inline bool operator<(const Point& lhs, const Point& rhs)
{
  return lhs.x < rhs.y;
}

void Field::sortPoints()
{
    std::set<std::pair<double, Point>> st;
    for(auto edge: fieldEdges)
    {
        st.insert({atan2(edge.start.x - fieldCenter.first, edge.start.y - fieldCenter.second) , edge.start});
        st.insert({atan2(edge.end.x - fieldCenter.first, edge.end.y - fieldCenter.second) , edge.end});
    }
    for(auto x: st)
    {
        fieldPoints.push_back(x.second);
    }
    st.clear();
}

void Field::print()
{
    glPointSize(2.0);
    glBegin(GL_POINTS);
    glVertex2f(fieldCenter.first, fieldCenter.second);
    glEnd();

    for(auto edge: fieldEdges)
    {
        edge.print();
    }

    // printing green field
    glColor3f(0, 1, 0);

    glBegin(GL_TRIANGLES);
    glVertex2f(fieldPoints[0].x, fieldPoints[0].y);
    glVertex2f(fieldPoints.back().x, fieldPoints.back().y);
    glVertex2f(fieldCenter.first, fieldCenter.second);
    for(unsigned int i = 1; i < fieldPoints.size(); i ++)
    {
        glVertex2f(fieldPoints[i].x, fieldPoints[i].y);
        glVertex2f(fieldPoints[i - 1].x, fieldPoints[i - 1].y);
        glVertex2f(fieldCenter.first, fieldCenter.second);
    }
    glEnd();
}
