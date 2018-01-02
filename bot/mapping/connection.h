#pragma once


#include "point.h"


struct Connection final
{
    Point const * begin, * end;
    double score;
};
