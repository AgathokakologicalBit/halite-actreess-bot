#pragma once


#include "point.h"


struct Connection final
{
    Point * begin, * end;
    unsigned int score;
};
