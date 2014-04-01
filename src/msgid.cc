/* msgid.c - a function to generate MSGID

 */

#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>

#include "msgid.h"

unsigned long createMsgId()
{
    const unsigned long mask = 0xfffffffUL; // 28 lower bits (?)

    static unsigned long counter = 0;
    time_t curr_time = time (NULL);
    static time_t last_time = 0;
    unsigned long ret;
    struct timeval time_val;
    struct timezone tz;

    if( last_time == 0 ) last_time = time (NULL);

    if (curr_time != last_time) {
        last_time = curr_time;
        counter   = 0;
        ret = curr_time & mask;
    } else {
        if (++counter <= 16) {
            gettimeofday (&time_val, &tz);
            usleep (1000000 - time_val.tv_usec);
            counter = 0;
            curr_time = time (NULL);
        }
        ret =  (counter++ << 28) | (curr_time & mask);
    }

    return ret;
}
