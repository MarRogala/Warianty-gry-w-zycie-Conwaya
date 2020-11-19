#include "Board.h"

void Board::fillFieldsCenters(int fieldsNumber)
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(-1.0, 1.0);

    for(unsigned int i = 0; i < fieldsNumber; i ++)
    {
        std::pair<double, double> center = {dist(mt), dist(mt)};
        fields.push_back(Field(i, center));
    }

    /*fields.push_back(Field(0, {0.116759, -0.950588}));
    fields.push_back(Field(1, {-0.458969, -0.830154}));
    fields.push_back(Field(2, {-0.251897, 0.928944}));
    fields.push_back(Field(3, {0.551897, 0.968944}));
    //fields.push_back(Field(4, {-0.351897, 0.128944}));
    fields.push_back(Field(4, {0.991897, 0.828944}));
    //fields.push_back(Field(6, {0.011897, -0.928944}));
    fields.push_back(Field(5, {0.581897, -0.938944}));*/

    std::priority_queue<Event, std::vector<Event>, Compare> eventQueue;
    Beachline beachline = Beachline(fields.size());

    double sweepYValue;

    for(auto field: fields)
    {
        Event e(field.fieldCenter.first, field.fieldCenter.second, field.fieldId);
        eventQueue.push(e);
        std::cout << "E: " << field.fieldId << " " << field.fieldCenter.first << " " << field.fieldCenter.second << "\n";
    }

    while(!eventQueue.empty())
    {
        Event currentEvent = eventQueue.top();
        eventQueue.pop();
        sweepYValue = currentEvent.yCord;

        if(currentEvent.type == siteEvent)
        {
            beachline.handleSiteEvent(currentEvent, sweepYValue, eventQueue);
        }
        else
        {
            beachline.handleCircleEvent(currentEvent, sweepYValue, eventQueue);
        }
    }
    if(eventQueue.empty())
    {
        beachline.completePolygons();
        for(unsigned int i = 0; i < fields.size(); i ++)
        {
            for(auto edge: beachline.edges[i])
            {
                fields[i].fieldEdges.push_back(edge);
            }
            beachline.edges[i].clear();
            fields[i].sortPoints();
            for(auto g: beachline.graph[i])
            {
                fields[i].neighbours.push_back(g);
            }
        }
        return;
    }
}

Board::Board() = default;

void Board::print()
{
    for(auto field : fields)
    {
        //float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        //float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        //float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        //field.color = {r, g, b};
        field.print();
    }
}

void Board::neighbours(int v)
{
    fields[v].color = {1, 0, 0};
    fields[v].state = 1;

    /*for(auto u: fields[v].neighbours)
    {
        fields[u].color = {1, 0, 0};
        fields[u].state = 1;
    }*/
}
