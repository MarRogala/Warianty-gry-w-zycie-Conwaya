#include <vector>
#include <list>
#include <unordered_map>
#include <queue>

struct Point {
    double x = 0, y = 0;
    Point(double xPar, double yPar);
    Point() = default;
};

enum EventType {siteEvent, circleEvent, emptyEvent}; 
enum BeachlineFieldType {parabolaType, halfEdgeType};

struct HalfEdge {
    Point startPoint;
    Point directionVector;

    HalfEdge();
    HalfEdge(Point start, Point directon);
    double findY(double x);
    void print(double l, double r);
};

struct Edge {
    Point start, end;
    Edge(Point start, Point end);
    void print();
};

struct Parabola {
    Point focusPoint;
    int fieldId = -1;
    int number = 0;

    Parabola();
    Parabola(Point p, int fieldId);
    double findY(double x, double sweepY);
    double tangent(Point parabolaPoint, double sweepY, double lineX);
    void print(double l, double r, double sweepY);
};

struct BeachlineField {
    BeachlineFieldType type;
    Parabola parabola;
    HalfEdge halfEdge;

    BeachlineField(Parabola p);
    BeachlineField(HalfEdge e);
};

struct Event
{
    int fieldId = -1;
    double yCord;
    double xCord;
    EventType type;
        
    std::list<BeachlineField>::iterator parabola;
    Point crossingPoint;

    Event();
    Event(double x, double y, int fieldId);
    Event(double y, Point point, std::list<BeachlineField>::iterator par);
};

struct Compare {
    bool operator()(const Event& first, const Event& second)
    {
        return first.yCord > second.yCord;
    }
};

struct Beachline {
    int parabolasNumber = 0;
    std::list<BeachlineField> container;
    std::unordered_map<int, Event> circleEventForParabola;
    std::vector<std::vector<Edge>> edges;
    std::vector<std::vector<int>> graph;

    Beachline(int n);
    Beachline() = default;

    std::list<BeachlineField>::iterator findParabolaForNewSite(double sweepY, Point site);
    std::list<BeachlineField>::iterator addNewField(BeachlineField field, std::list<BeachlineField>::iterator index);
    void removeField(std::list<BeachlineField>::iterator index);
    BeachlineField getField(std::list<BeachlineField>::iterator index);
    double parabolaIntersection(Parabola p1, Parabola p2, double sweepY, bool left);
    double parabolaHalfEdgeIntersection(Parabola p, HalfEdge e, double sweepY);
    void handleSiteEvent(Event event, double sweepY, std::priority_queue<Event, std::vector<Event>, Compare>& eventQueue);
    void handleCircleEvent(Event event, double sweepY, std::priority_queue<Event, std::vector<Event>, Compare>& eventQueue);
    void addCircleEvents(std::list<BeachlineField>::iterator newParabola, std::priority_queue<Event, std::vector<Event>, Compare>& eventQueue);
    bool checkEdgesIntersection(HalfEdge leftEdge, HalfEdge rightEdhe);
    Point edgesIntersection(HalfEdge leftEdge, HalfEdge rightEdhe);
    void completePolygons();
    void print(double sweepY);
    void printEdges();
};
