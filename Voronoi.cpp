#include <GL/glut.h>
#include <GL/gl.h>

#include "Voronoi.h"
#include <cmath>
#include <iostream>

Point::Point(double xPar, double yPar) : x(xPar), y(yPar) { }

Event::Event() : type(emptyEvent) { }
Event::Event(double x, double y, int fieldId) : type(siteEvent), xCord(x), yCord(y), fieldId(fieldId) { }
Event::Event(double y, Point point, std::list<BeachlineField>::iterator par) : type(circleEvent), yCord(y), crossingPoint(point), parabola(par)
{
    fieldId = (*par).parabola.fieldId;
}

HalfEdge::HalfEdge() : startPoint(Point(0, 0)), directionVector(Point(0, 0)) { }
HalfEdge::HalfEdge(Point start, Point direction) : startPoint(start), directionVector(direction) { }
Edge::Edge(Point start, Point end) : start(start), end(end) { }

Parabola::Parabola(Point p, int fieldId) : focusPoint(p), fieldId(fieldId) { }
Parabola::Parabola() : focusPoint(Point(0.0, 0.0)) { }

BeachlineField::BeachlineField(Parabola p) : parabola(p)
{
    type = parabolaType;
}

BeachlineField::BeachlineField(HalfEdge e) : halfEdge(e)
{
    type = halfEdgeType;
}

Beachline::Beachline(int n)
{
    edges.resize(n + 7);
    graph.resize(n + 7);
}

double HalfEdge::findY(double x)
{
    double A = directionVector.y / directionVector.x;
    double B = startPoint.y - (startPoint.x * A);
    return A * x + B;
}

void HalfEdge::print(double l, double r)
{
    glPointSize(3.0);
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_LINES);

    double nextPointX, nextPointY;
    if(directionVector.x < 0)
    {
        glVertex2f(startPoint.x, startPoint.y);
        double distX = (-1.0) * fabs(l - startPoint.x);
        nextPointX = l;
        nextPointY = startPoint.y + ((distX / directionVector.x) * directionVector.y);
        glVertex2f(nextPointX, nextPointY);
    }
    else
    {
        glVertex2f(startPoint.x, startPoint.y);
        double distX = fabs(r - startPoint.x);
        nextPointX = r;
        nextPointY = startPoint.y + ((distX / directionVector.x) * directionVector.y);
        glVertex2f(nextPointX, nextPointY);
    }
    glEnd();
    glColor3f(1.0, 0.0, 0.0);
}

void Edge::print()
{
    glPointSize(3.0);
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_LINES);
    glVertex2f(start.x, start.y);
    glVertex2f(end.x, end.y);
    glEnd();
    glColor3f(1.0, 0.0, 0.0);
}

double Parabola::findY(double x, double sweepY)
{
    double part1 = ((x - focusPoint.x) * (x - focusPoint.x)) / (2.0 * (focusPoint.y - sweepY));
    double part2 = (focusPoint.y + sweepY) / 2.0;
    return part1 + part2;
}

void Parabola::print(double l, double r, double sweepY)
{
    glPointSize(3.0);
    if(l > r) std::swap(l, r);
    if(fabs(sweepY - focusPoint.y) < 0.001)
    {
        glBegin(GL_LINES);
        glVertex2f(focusPoint.x, sweepY);
        glVertex2f(focusPoint.x, -1.0);
        glEnd();
        return;
    }

    glBegin(GL_LINES);
    double seg = 0.005;
    double i;
    for(i = l + seg; i < r; i += seg)
    {
        glVertex2f((i - seg), findY((i - seg), sweepY));
        glVertex2f(i, findY(i, sweepY));
    }
    for(i = i - seg; i <= r; i += seg / 10.0)
    {
        glVertex2f((i - seg), findY((i - seg), sweepY));
        glVertex2f(i, findY(i, sweepY));
    }
    glEnd();
}

double Parabola::tangent(Point parabolaPoint, double sweepY, double lineX)
{
    double tmp1 = 2.0 * (focusPoint.y - sweepY);
    double tmp2 = (focusPoint.y + sweepY) / 2.0;
    double A1 = 1.0 / tmp1, B1 = (-2.0 * focusPoint.x) / tmp1, C1 = (focusPoint.x * focusPoint.x) / tmp1 + tmp2;

    double tangentA, tangentB;
    tangentA = (2.0 * A1 * parabolaPoint.x) + B1;
    tangentB = ((A1 * (parabolaPoint.x * parabolaPoint.x)) + (B1 * parabolaPoint.x) + C1) - (tangentA * parabolaPoint.x);
    return (tangentA * lineX) + tangentB;
}

double Beachline::parabolaHalfEdgeIntersection(Parabola p, HalfEdge e, double sweepY)
{
    double tmp1 = 2.0 * (p.focusPoint.y - sweepY);
    if(p.focusPoint.y == sweepY)
        return p.focusPoint.x;
    double tmp2 = (p.focusPoint.y + sweepY) / 2.0;
    double PA = 1.0 / tmp1, PB = (-2.0 * p.focusPoint.x) / tmp1, PC = (p.focusPoint.x * p.focusPoint.x) / tmp1 + tmp2;

    if(e.directionVector.x == 0)
        return e.startPoint.x;
    double EA = e.directionVector.y / e.directionVector.x;
    double EB = e.startPoint.y - (e.startPoint.x * EA);

    PB -= EA;
    PC -= EB;

    double delta = (PB * PB) - (4.0 * PA * PC);
    if(fabs(delta) <= 0.0001)
    {
        return (-PB) / (2.0 * PA);
    }
    double sol1 = (-PB - sqrt(delta)) / (2.0 * PA);
    double sol2 = (-PB + sqrt(delta)) / (2.0 * PA);

    if(PA == 0)
    {
        sol1 = p.focusPoint.x;
        return sol1;
    }

    if((sol1 >= e.startPoint.x && e.directionVector.x >= 0) || (sol1 <= e.startPoint.x && e.directionVector.x <= 0))
    {
        return sol1;
    }
    else return sol2;
}

double Beachline::parabolaIntersection(Parabola p1, Parabola p2, double sweepY, bool left)
{
    double tmp1 = 2.0 * (p1.focusPoint.y - sweepY);
    if(p1.focusPoint.y == sweepY)
        return p1.focusPoint.x;
    double tmp2 = (p1.focusPoint.y + sweepY) / 2.0;
    double A1 = 1.0 / tmp1, B1 = (-2.0 * p1.focusPoint.x) / tmp1, C1 = (p1.focusPoint.x * p1.focusPoint.x) / tmp1 + tmp2;
    tmp1 = 2.0 * (p2.focusPoint.y - sweepY);
    if(p2.focusPoint.y == sweepY)
        return p2.focusPoint.x;
    tmp2 = (p2.focusPoint.y + sweepY) / 2.0;
    double A2 = 1.0 / tmp1, B2 = (-2.0 * p2.focusPoint.x) / tmp1, C2 = (p2.focusPoint.x * p2.focusPoint.x) / tmp1 + tmp2;
    A1 -= A2;
    B1 -= B2;
    C1 -= C2;
    double delta = (B1 * B1) - (4.0 * A1 * C1);
    double sol1 = (-B1 - sqrt(delta)) / (2.0 * A1);
    double sol2 = (-B1 + sqrt(delta)) / (2.0 * A1);
    double leftPoint = -1.0, rightPoint = 1.0;
    leftPoint = fmin(sol1, sol2);
    rightPoint = fmax(sol1, sol2);
    return left ? fmin(rightPoint, 1.0) : fmax(leftPoint, -1.0);
}

std::list<BeachlineField>::iterator Beachline::findParabolaForNewSite(double sweepY, Point site)
{
    auto resultIndex = container.end();

    int containerSize = static_cast<int>(container.size());
    int i = 0;
    for(auto it = container.begin(); it != container.end(); it ++)
    {
        auto field = *it;
        if(field.type == halfEdgeType)
        {
            i++;
            continue;
        }

        double rightPoint = 1.0, leftPoint = -1.0;
        auto rightIndex = container.end(), leftIndex = container.end();;
        std::list<BeachlineField>::iterator tmp;
        if(i + 1 < containerSize)
        {
            tmp = it;
            tmp ++;
            if((*tmp).type == halfEdgeType)
                rightIndex = tmp;
        }

        if(rightIndex != container.end())
        {
            rightPoint = parabolaHalfEdgeIntersection(field.parabola, (*rightIndex).halfEdge, sweepY);
        }

        if(i - 1 >= 0 )
        {
            tmp = it;
            tmp --;
            if((*tmp).type == halfEdgeType)
                leftIndex = tmp;
        }

        if(leftIndex != container.end())
        {
            leftPoint = parabolaHalfEdgeIntersection(field.parabola, (*leftIndex).halfEdge, sweepY);
        }

        leftPoint = fmax(leftPoint, -1.0);
        rightPoint = fmin(rightPoint, 1.0);

        if(rightPoint >= site.x && leftPoint <= site.x)
        {
            resultIndex = it;
            break;
        }
        i ++;
    }
    return resultIndex;
}

BeachlineField Beachline::getField(std::list<BeachlineField>::iterator index)
{
    return *index;
}

std::list<BeachlineField>::iterator Beachline::addNewField(BeachlineField field, std::list<BeachlineField>::iterator index)
{
    if(field.type == parabolaType)
        field.parabola.number = parabolasNumber ++;
    circleEventForParabola[field.parabola.number] = Event();
    return container.insert(index, field);;
}

void Beachline::removeField(std::list<BeachlineField>::iterator index)
{
    container.erase(index);
}

void Beachline::handleSiteEvent(Event event, double sweepY, std::priority_queue<Event, std::vector<Event>, Compare>& eventQueue)
{
    auto index = findParabolaForNewSite(event.yCord, Point(event.xCord, event.yCord));
    if(index != container.end())
    {
        BeachlineField oldParabola = getField(index);
        BeachlineField newParabola = BeachlineField(Parabola(Point(event.xCord, event.yCord), event.fieldId));
        HalfEdge edgeLeft;
        edgeLeft.startPoint = Point(event.xCord, oldParabola.parabola.findY(event.xCord, sweepY));

        double leftPointX, leftPointY;
        leftPointX = edgeLeft.startPoint.x - 0.1;
        leftPointY = oldParabola.parabola.tangent(edgeLeft.startPoint, sweepY, leftPointX);

        edgeLeft.directionVector = Point(edgeLeft.startPoint.x - leftPointX, edgeLeft.startPoint.y - leftPointY);

        double dist = sqrt( (edgeLeft.startPoint.x * edgeLeft.startPoint.x) + (edgeLeft.startPoint.y * edgeLeft.startPoint.y));
        edgeLeft.directionVector.x /= dist;
        edgeLeft.directionVector.y /= dist;
        HalfEdge edgeRight = HalfEdge(edgeLeft.startPoint, {-edgeLeft.directionVector.x, -edgeLeft.directionVector.y});

        std::swap(edgeLeft, edgeRight);
        auto rightArc = index;
        index = addNewField(BeachlineField(edgeRight), index);
        index = addNewField(newParabola, index);
        index = addNewField(BeachlineField(edgeLeft), index);
        index = addNewField(oldParabola, index);
        addCircleEvents(index, eventQueue);
        addCircleEvents(rightArc, eventQueue);
    }
    else
    {
        addNewField(BeachlineField(Parabola(Point(event.xCord, event.yCord), event.fieldId)), container.begin());
    }
}

void Beachline::addCircleEvents(std::list<BeachlineField>::iterator newParabola, std::priority_queue<Event, std::vector<Event>, Compare>& eventQueue)
{
    std::list<BeachlineField>::iterator right = std::next(newParabola);
    if(newParabola == container.begin() || right == container.end())
        return;
    std::list<BeachlineField>::iterator left = newParabola;
    advance(left, -1);
    if(!checkEdgesIntersection((*left).halfEdge, (*right).halfEdge))
        return;
    Point intersection = edgesIntersection((*left).halfEdge, (*right).halfEdge);

    double circleEventY = sqrt((((*newParabola).parabola.focusPoint.x - intersection.x) * ((*newParabola).parabola.focusPoint.x - intersection.x))
                            + (((*newParabola).parabola.focusPoint.y - intersection.y) * ((*newParabola).parabola.focusPoint.y - intersection.y)));
    circleEventY += intersection.y;

    if(circleEventForParabola[(*newParabola).parabola.number].type == emptyEvent || circleEventForParabola[(*newParabola).parabola.number].yCord > circleEventY)
    {
        Event event(circleEventY, intersection, newParabola);
        circleEventForParabola[(*newParabola).parabola.number] = event;
        eventQueue.push(event);
    }
}

bool Beachline::checkEdgesIntersection(HalfEdge leftEdge, HalfEdge rightEdge)
{
    if(leftEdge.directionVector.y / leftEdge.directionVector.y == rightEdge.directionVector.y / rightEdge.directionVector.x)
        return false;

    double A1 = leftEdge.directionVector.y / leftEdge.directionVector.x;
    double B1 = leftEdge.startPoint.y - (leftEdge.startPoint.x * A1);
    double A2 = rightEdge.directionVector.y / rightEdge.directionVector.x;
    double B2 = rightEdge.startPoint.y - (rightEdge.startPoint.x * A2);
    double X = (B2 - B1) / (A1 - A2);
    double Y = A1 * X + B1;

    if((Y < leftEdge.startPoint.y && leftEdge.directionVector.y > 0.0) || (Y > leftEdge.startPoint.y && leftEdge.directionVector.y < 0.0))
        return false;
    if((Y < rightEdge.startPoint.y && rightEdge.directionVector.y > 0.0) || (Y > rightEdge.startPoint.y && rightEdge.directionVector.y < 0.0))
        return false;
    if((X < leftEdge.startPoint.x && leftEdge.directionVector.x > 0.0) || (X > leftEdge.startPoint.x && leftEdge.directionVector.x < 0.0))
        return false;
    if((X < rightEdge.startPoint.x && rightEdge.directionVector.x > 0.0) || (X > rightEdge.startPoint.x && rightEdge.directionVector.x < 0.0))
        return false;
    return true;
}

Point Beachline::edgesIntersection(HalfEdge leftEdge, HalfEdge rightEdge)
{
    double A1 = leftEdge.directionVector.y / leftEdge.directionVector.x;
    double B1 = leftEdge.startPoint.y - (leftEdge.startPoint.x * A1);
    double A2 = rightEdge.directionVector.y / rightEdge.directionVector.x;
    double B2 = rightEdge.startPoint.y - (rightEdge.startPoint.x * A2);
    double X = (B2 - B1) / (A1 - A2);
    double Y = A1 * X + B1;
    return Point(X, Y);
}

void Beachline::handleCircleEvent(Event event, double sweepY, std::priority_queue<Event, std::vector<Event>, Compare>& eventQueue)
{
    Point crossingPointToCheck = circleEventForParabola[(*(event.parabola)).parabola.number].crossingPoint;
    if(crossingPointToCheck.x != event.crossingPoint.x || crossingPointToCheck.y != event.crossingPoint.y)
        return;
    auto leftEdgeIt = event.parabola;
    auto rightEdgeIt = event.parabola;
    leftEdgeIt --;
    rightEdgeIt ++;
    HalfEdge leftEdge = (*leftEdgeIt).halfEdge, rightEdge = (*rightEdgeIt).halfEdge;
    Edge completeLeftEdge(leftEdge.startPoint, event.crossingPoint);
    Edge completeRightEdge(rightEdge.startPoint, event.crossingPoint);


    int middleId = event.fieldId;
    int rightId = -1, leftId = -1;
    edges[middleId].push_back(completeLeftEdge);
    edges[middleId].push_back(completeRightEdge);
    std::list<BeachlineField>::iterator leftIt;
    if(event.parabola != container.begin())
    {
        leftIt = event.parabola;
        advance(leftIt, -2);
        leftId = (*leftIt).parabola.fieldId;
        edges[leftId].push_back(completeLeftEdge);
        if((leftEdge.startPoint.x <= 1.0 && leftEdge.startPoint.x >= -1.0 && leftEdge.startPoint.y <= 1.0 && leftEdge.startPoint.y >= -1.0) ||
            (event.crossingPoint.x <= 1.0 && event.crossingPoint.x >= -1.0 && event.crossingPoint.y <= 1.0 && event.crossingPoint.y >= -1.0))
        {
            graph[middleId].push_back(leftId);
            graph[leftId].push_back(middleId);
        }
    }
    auto rightIt = event.parabola;
    rightIt ++;
    if(rightIt != container.end())
    {
        rightIt ++;
        rightId = (*rightIt).parabola.fieldId;
        edges[rightId].push_back(completeRightEdge);
        if((rightEdge.startPoint.x <= 1.0 && rightEdge.startPoint.x >= -1.0 && rightEdge.startPoint.y <= 1.0 && rightEdge.startPoint.y >= -1.0) ||
            (event.crossingPoint.x <= 1.0 && event.crossingPoint.x >= -1.0 && event.crossingPoint.y <= 1.0 && event.crossingPoint.y >= -1.0))
        {
            graph[middleId].push_back(rightId);
            graph[rightId].push_back(middleId);
        }
    }
    double x = (*rightIt).parabola.focusPoint.y - (*leftIt).parabola.focusPoint.y;
    double y = (*leftIt).parabola.focusPoint.x - (*rightIt).parabola.focusPoint.x;
    double a = y / x;
    Point newDirection(x, y);

    double dist = sqrt( (newDirection.x * newDirection.x) + (newDirection.y * newDirection.y));
    newDirection.x /= dist;
    newDirection.y /= dist;
    newDirection.x *= (-1.0);
    newDirection.y *= (-1.0);

    HalfEdge newEdge(event.crossingPoint, newDirection);
    BeachlineField newField(newEdge);

    removeField(leftEdgeIt);
    removeField(event.parabola);
    removeField(rightEdgeIt);
    addNewField(newField, rightIt);
    addCircleEvents(leftIt, eventQueue);
    addCircleEvents(rightIt, eventQueue);
}

void Beachline::completePolygons()
{
    for(auto it = container.begin(); it != container.end(); it ++)
    {
        auto field = (*it);
        if(field.type != halfEdgeType)
            continue;
        auto leftIt = it, rightIt = it;
        leftIt --;
        rightIt ++;
        int leftId = (*leftIt).parabola.fieldId, rightId = (*rightIt).parabola.fieldId;

        HalfEdge halfEdge = field.halfEdge;

        Point endPoint(halfEdge.startPoint.x + 20.0 * halfEdge.directionVector.x, halfEdge.startPoint.y + 20.0 * halfEdge.directionVector.y);
        Edge completeEdge(halfEdge.startPoint, endPoint);
        edges[leftId].push_back(completeEdge);
        edges[rightId].push_back(completeEdge);
        if((halfEdge.startPoint.x <= 1.0 && halfEdge.startPoint.x >= -1.0 && halfEdge.startPoint.y <= 1.0 && halfEdge.startPoint.y >= -1.0) ||
            (endPoint.x <= 1.0 && endPoint.x >= -1.0 && endPoint.y <= 1.0 && endPoint.y >= -1.0))
        {
            graph[leftId].push_back(rightId);
            graph[rightId].push_back(leftId);
        }

    }
    std::cout << "Generator finished\n";
}

void Beachline::print(double sweepY)
{
    int containerSize = static_cast<int>(container.size());
    int i = 0;
    for(auto it = container.begin(); it != container.end(); it ++)
    {
        auto field = *it;
        double rightPoint = 1.0, leftPoint = -1.0;
        auto rightIndex = container.end(), leftIndex = container.end();;
        std::list<BeachlineField>::iterator tmp;

        if(field.type == halfEdgeType)
        {
            i++;
            tmp = it;
            tmp ++;
            if((*tmp).type == parabolaType)
                rightIndex = tmp;
            rightPoint = parabolaHalfEdgeIntersection((*rightIndex).parabola, field.halfEdge, sweepY);
            if(rightPoint > field.halfEdge.startPoint.x)
                field.halfEdge.print(field.halfEdge.startPoint.x, rightPoint);
            else field.halfEdge.print(rightPoint, field.halfEdge.startPoint.x);
            continue;
        }
        if(i + 1 < containerSize)
        {
            tmp = it;
            tmp ++;
            if((*tmp).type == halfEdgeType)
                rightIndex = tmp;
        }

        if(rightIndex != container.end())
        {
            rightPoint = parabolaHalfEdgeIntersection(field.parabola, (*rightIndex).halfEdge, sweepY);
        }

        if(i - 1 >= 0 )
        {
            tmp = it;
            tmp --;
            if((*tmp).type == halfEdgeType)
                leftIndex = tmp;
        }

        if(leftIndex != container.end())
        {
            leftPoint = parabolaHalfEdgeIntersection(field.parabola, (*leftIndex).halfEdge, sweepY);
        }

        if(leftIndex == container.end()) leftPoint = -1.0;
        if(rightIndex == container.end()) rightPoint = 1.0;

        if(leftPoint > rightPoint)
            std::swap(leftPoint, rightPoint);
        leftPoint = fmax(leftPoint, -1.0);
        rightPoint = fmin(rightPoint, 1.0);

        if((leftPoint >= -1.0 && rightPoint <= 1.0) || (container.size() == 0))
        {
            field.parabola.print(leftPoint, rightPoint, sweepY);
        }
        i ++;
    }
}

void Beachline::printEdges()
{
    for(unsigned int i = 0; i < edges.size(); i ++)
    {
        for(auto edge: edges[i])
        {
            edge.print();
        }
    }
}
