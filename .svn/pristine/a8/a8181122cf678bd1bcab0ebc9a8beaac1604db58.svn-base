#ifndef _WAVHEAD_H_
#define _WAVHEAD_H_

#ifdef __cplusplus
extern "C" {
#endif

#define         BUFFERSIZE              1024
#define         PCM_WAVE_FORMAT         1
#define         TRUE                    1
#define         FALSE                   0

typedef unsigned long u_long;
typedef unsigned short u_short;
typedef unsigned char Byte;
typedef short Int16;
typedef long Int32;
typedef unsigned long UInt32;
typedef unsigned short UInt16;

typedef  struct
{       u_long     dwSize ;
        u_short    wFormatTag ;
        u_short    wChannels ;
        u_long     dwSamplesPerSec ;
        u_long     dwAvgBytesPerSec ;
        u_short    wBlockAlign ;
        u_short    wBitsPerSample ;
} WAVEFORMAT ;

typedef  struct
{       char            RiffID [4] ;
        u_long          RiffSize ;
        char            WaveID [4] ;
        char            FmtID  [4] ;
        u_long          FmtSize ;
        u_short         wFormatTag ;
        u_short         nChannels ;
        u_long          nSamplesPerSec ;
	u_long          nAvgBytesPerSec ;
        u_short         nBlockAlign ;
        u_short         wBitsPerSample ;
        char            DataID [4] ;
        u_long          nDataBytes ;
} WAVE_HEADER ;

char*  findchunk (char* s1, char* s2, size_t n);
int  WaveReadHeader  (int fd, int* channels, u_long* samplerate, int* samplebits, u_long* samples, u_long* datastart);

#ifdef __cplusplus
}
#endif

#endif

