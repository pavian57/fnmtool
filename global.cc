#include "global.h"

CConfig * cfg;
CLog * log;

using namespace std;

void capitalize (string &s) { bool cap = true;

for(unsigned int i = 0; i <= s.length(); i++)
{
    if (isalpha(s[i]) && cap == true)
    {
        s[i] = toupper(s[i]);
        cap = false;
    }
    else if (isspace(s[i]))
    {  
        cap = true;
    }
}

}