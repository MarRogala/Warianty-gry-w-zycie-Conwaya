#include "Board.h"

void Board::changeFieldColor(int fieldId, FieldColor color)
{
    if(!fields[fieldId].changeColor)
        return;
    fields[fieldId].changeColor = false;
    int index = fields[fieldId].fieldBufferOffset;
    glBindBuffer(GL_ARRAY_BUFFER, trianglesBuffer);
    float data[3] = {color.r, color.g, color.b};
    for(unsigned int i = 0; i < fields[fieldId].fieldPoints.size(); i ++)
    {
        glBufferSubData(GL_ARRAY_BUFFER, (index + 2) * sizeof(float), 3 * sizeof(float), data);
        index += 5;
        glBufferSubData(GL_ARRAY_BUFFER, (index + 2) * sizeof(float), 3 * sizeof(float), data);
        index += 5;
        glBufferSubData(GL_ARRAY_BUFFER, (index + 2) * sizeof(float), 3 * sizeof(float), data);
        index += 5;
    }
}

unsigned int Board::CompileShader(unsigned int type, std::string& source)
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
unsigned int Board::CreateShader(std::string& vertexShader, std::string& fragmentShader)
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
        linesSize += f.fieldEdges.size();
        trianglesSize += f.fieldPoints.size();
    }
    lines = (float*)malloc(4 * linesSize * sizeof(float));
    triangles = (float*)malloc(15 * trianglesSize * sizeof(float));

    int linesIndex = 0;
    int trianglesIndex = 0;

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

        fields[f.fieldId].fieldBufferOffset = trianglesIndex;

        for(unsigned int i = 0; i < f.fieldPoints.size(); i ++)
        {
            triangles[trianglesIndex + 0] = f.fieldPoints[i].x;
            triangles[trianglesIndex + 1] = f.fieldPoints[i].y;
            triangles[trianglesIndex + 2] = f.color.r;
            triangles[trianglesIndex + 3] = f.color.g;
            triangles[trianglesIndex + 4] = f.color.b;
            if(i == 0)
            {
                triangles[trianglesIndex + 5] = f.fieldPoints.back().x;
                triangles[trianglesIndex + 6] = f.fieldPoints.back().y;
            }
            else
            {
                triangles[trianglesIndex + 5] = f.fieldPoints[i - 1].x;
                triangles[trianglesIndex + 6] = f.fieldPoints[i - 1].y;
            }
            triangles[trianglesIndex + 7] = f.color.r;
            triangles[trianglesIndex + 8] = f.color.g;
            triangles[trianglesIndex + 9] = f.color.b;
            triangles[trianglesIndex + 10] = f.fieldCenter.first;
            triangles[trianglesIndex + 11] = f.fieldCenter.second;
            triangles[trianglesIndex + 12] = f.color.r;
            triangles[trianglesIndex + 13] = f.color.g;
            triangles[trianglesIndex + 14] = f.color.b;
            trianglesIndex += 15;
        }
    }

    GLuint vertexBuffers[2];
    glGenBuffers(2, vertexBuffers);
    linesBuffer = vertexBuffers[0];
    trianglesBuffer = vertexBuffers[1];

    glBindBuffer(GL_ARRAY_BUFFER, linesBuffer);
    glBufferData(GL_ARRAY_BUFFER, 4 * linesSize * sizeof(float), lines, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, trianglesBuffer);
    glBufferData(GL_ARRAY_BUFFER, 15 * trianglesSize * sizeof(float), triangles, GL_DYNAMIC_DRAW);

    std::string lineVertexShader =
     R"(#version 300 es
        layout(location = 0) in vec4 position;
        void main()
        {
            gl_Position = position;
        })";

    std::string linesFragmentShader =
     R"(#version 300 es
        precision mediump float;
        out mediump vec4 color1;
        void main()
        {
            color1 = vec4(0.0, 0.0, 0.0, 1.0);
        })";

    std::string triangleVertexShader =
     R"(#version 300 es
        layout(location = 0) in vec4 position;
        layout(location = 1) in vec3 colorIn1;

        precision mediump float;
        out mediump vec4 color2;

        void main()
        {
            gl_Position = position;
            color2 = vec4(colorIn1, 1.0);
        })";

    std::string trianglesFragmentShader =
     R"(#version 300 es
         precision mediump float;
         in mediump vec4 color2;
         out mediump vec4 color3;
         void main()
         {
            color3 = color2;
         })";


    lineShader = CreateShader(lineVertexShader, linesFragmentShader);
    triangleShader = CreateShader(triangleVertexShader, trianglesFragmentShader);
    free(triangles);
    free(lines);
}

void Board::generateVoronoi()
{
    std::priority_queue<Event, std::vector<Event>, Compare> eventQueue;
    Beachline beachline = Beachline(fields.size());

    double sweepYValue;

    for(auto field: fields)
    {
        Event e(field.fieldCenter.first, field.fieldCenter.second, field.fieldId);
        eventQueue.push(e);
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
        std::cout << "Generator finished\n";
        return;
    }
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
        //std::cout << "field: " << fields.back().fieldId << " " << fields.back().fieldCenter.first << " " << fields.back().fieldCenter.second << "\n";
    }
}

Board::Board() = default;

void Board::print()
{
    //triangles
    glBindBuffer(GL_ARRAY_BUFFER, trianglesBuffer);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (const void*)(2 * sizeof(float)));
    glUseProgram(triangleShader);
    glDrawArrays(GL_TRIANGLES, 0, trianglesSize * 3);

    //lines
    glBindBuffer(GL_ARRAY_BUFFER, linesBuffer);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
    glUseProgram(lineShader);
    glDrawArrays(GL_LINES, 0, linesSize * 2);
}
