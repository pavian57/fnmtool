#ifdef __alpha
     /* this part of the ifdef works for 64 or 32 bit machines */
    typedef unsigned int dword;
#else    
     /* this part of the ifdef works for 16 or 32 bit machines */
    typedef unsigned long dword;
#endif