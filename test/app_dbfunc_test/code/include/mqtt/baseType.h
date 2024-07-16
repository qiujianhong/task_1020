#ifndef __BASE_TYPE_H__
#define __BASE_TYPE_H__

#ifdef __cplusplus
extern "C"
{
#endif

    typedef unsigned char uint8;
    typedef unsigned short int uint16;
    typedef unsigned int uint32;
    typedef unsigned long int uint64;

    typedef char sint8;
    typedef short sint16;
    typedef int sint32;
    typedef long int sint64;

#define   UNUSED(v)   ((void)(v))

#ifdef __cplusplus
}
#endif

#endif