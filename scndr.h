#ifndef _SCANDR_H_
#define _SCANDR_H_
#include "area.h"
#include "mask.h"

class CScandir
{
        public:
                CScandir(string s_Path);
                ~CScandir();
                CArea A_Area;
                int firstMask;
                int lastMask;
};
#endif
