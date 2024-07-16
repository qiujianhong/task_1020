#ifndef _FILELOCK_H_
#define _FILELOCK_H_

#ifdef __cplusplus
extern "C" {
#endif

int set_lock(int fd, int type);
int release_lock(int fd);

#ifdef __cplusplus
}
#endif

#endif

