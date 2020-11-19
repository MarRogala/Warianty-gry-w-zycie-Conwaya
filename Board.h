#include "Field.h"
#include <queue>

struct Board {
    std::vector<Field> fields;

    Board();
    void fillFieldsCenters(int fieldsNumber);
    void print();
    void neighbours(int v);
};
