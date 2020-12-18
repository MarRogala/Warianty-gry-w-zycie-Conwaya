#include "Field.h"
#include <set>
#include <algorithm>

Field::Field() { }
Field::Field(int id, std::vector<Point> points) : fieldId(id), fieldPoints(points) { }
Field::Field(int id, std::pair<double, double> center) : fieldId(id), fieldCenter(center) { }

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
