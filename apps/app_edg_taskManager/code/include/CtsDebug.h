#ifndef __CTSDEBUG_H__
#define __CTSDEBUG_H__

#define PIPEPRINT(en_flag,fmt...) ({\
    if(getenv("DEBUG_CTS") != NULL){\
    if((atoi(getenv("DEBUG_CTS")) == en_flag)||(atoi(getenv("DEBUG_CTS")) == 55))\
        {printf(fmt);}}\
})

#endif
