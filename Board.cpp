#include "Board.h"

unsigned int Board::CompileShader(unsigned int type, std::string& source) // should be ok
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, NULL);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if(!result)
    {
        int len;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
        char* msg = (char*)malloc(len);
        glGetShaderInfoLog(id, len, &len, msg);
        if(type == GL_VERTEX_SHADER) std::cout << "VERTEX SHADER COMPILE ERROR: " << msg << "\n";
        else std::cout << "FRAGMENT SHADER COMPILE ERROR: " << msg << "\n";
        glDeleteShader(id);
        return 0;
    }
    return id;
}
unsigned int Board::CreateShader(std::string& vertexShader, std::string& fragmentShader) // should be ok
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glDetachShader(program, vs);
    glDetachShader(program, fs);
    glValidateProgram(program);
    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}

void Board::createBuffers()
{
    for(auto f: fields)
    {
        linesSize += (int)f.fieldEdges.size();
        trianglesSize += f.fieldPoints.size();
    }
    lines = (double*)malloc(4 * linesSize * sizeof(double)); // start end
    triangles = (double*)malloc(7 * trianglesSize * sizeof(double)); // state (color itp), 3 points

    int linesIndex = 0;
    int trianglesIndex = 0;
    int fieldIndex = 0;
    for(auto f:fields)
    {
        for(auto l: f.fieldEdges)
        {
            lines[linesIndex] = l.start.x;
            lines[linesIndex + 1] = l.start.y;
            lines[linesIndex + 2] = l.end.x;
            lines[linesIndex + 3] = l.end.y;
            linesIndex += 4;
        }

        fields[f.fieldId].fieldBufferIndex = fieldIndex ++;
        triangles[0] = f.state;
        triangles[1] = f.fieldPoints[0].x;
        triangles[2] = f.fieldPoints[0].y;
        triangles[3] = f.fieldPoints.back().x;
        triangles[4] = f.fieldPoints.back().y;
        triangles[5] = f.fieldCenter.first;
        triangles[6] = f.fieldCenter.second;
        trianglesIndex += 7;
        for(int i = 1; i < (int)f.fieldPoints.size(); i ++)
        {
            triangles[trianglesIndex] = f.state;
            triangles[trianglesIndex + 1] = f.fieldPoints[i].x;
            triangles[trianglesIndex + 2] = f.fieldPoints[i].y;
            triangles[trianglesIndex + 3] = f.fieldPoints[i - 1].x;
            triangles[trianglesIndex + 4] = f.fieldPoints[i - 1].y;
            triangles[trianglesIndex + 5] = f.fieldCenter.first;
            triangles[trianglesIndex + 6] = f.fieldCenter.second;
            trianglesIndex += 7;
        }
    } // arrays filled

    GLuint vertexBuffers[2];
    glGenBuffers(2, vertexBuffers);
    linesBuffer = vertexBuffers[0];
    //trianglesBuffer = vertexBuffers[1];

    glBindBuffer(GL_ARRAY_BUFFER, linesBuffer);
    glBufferData(GL_ARRAY_BUFFER, 4 * linesSize * sizeof(double), lines, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_DOUBLE, GL_FALSE, sizeof(double) * 2, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    std::string lineVertexShader =
     R"(#version 300 es
        layout(location = 0) in vec4 position;
        void main()
        {
            gl_Position = position;
        })";

    std::string lineFragmentShader =
     R"(#version 300 es
        precision mediump float;
        out mediump vec4 color;
        void main()
        {
            color = vec4(0.0, 1.0, 0.0, 1.0);
        })";

    //std::string triangleVertexShader;
    //std::string triangleFrangmetShader;

    unsigned int lineShader = CreateShader(lineVertexShader, lineFragmentShader);
    glUseProgram(lineShader);
}

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
        createBuffers();
        return;
    }
}

Board::Board() = default;

void Board::print()
{
    /*for(auto field : fields)
    {
        //float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        //float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        //float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        //field.color = {r, g, b};
        field.print();
    }*/
    glDrawArrays(GL_LINES, 0, linesSize * 2);
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
