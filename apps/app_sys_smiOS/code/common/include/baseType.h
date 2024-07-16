#ifndef __BASE_TYPE_H__
#define __BASE_TYPE_H__

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef sint8
#define sint8	char
#endif
	
#ifndef uint8
#define uint8    unsigned char
#endif

#ifndef uint16
#define uint16    unsigned short
#endif

#ifndef sint16
#define sint16    short
#endif

#ifndef uint32
#define uint32    unsigned int
#endif

#ifndef sint32
#define sint32    int
#endif

#ifndef uint64
#define uint64    unsigned long int
#endif

#ifndef sint64
#define sint64    long int
#endif


#define   UNUSED(v)   ((void)(v))

#ifdef __cplusplus
}
#endif

#endif