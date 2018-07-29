#ifndef BUILDID_H_
#define BUILDID_H_

extern char __BUILD_DATE;
extern char __BUILD_NUMBER;

#define BUILD_DATE      ((uint32_t)&__BUILD_DATE)
#define BUILD_NUMBER    ((uint32_t)&__BUILD_NUMBER)

#endif
