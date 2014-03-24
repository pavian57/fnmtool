#ifndef _OP_H_
#define _OP_H_
#include "mask.h"

class COperation
{
        public:
                COperation(string s_MaskStr);
                ~COperation();
                CMask   M_Mask;
                int firstAction;
                int lastAction;
};
#endif
