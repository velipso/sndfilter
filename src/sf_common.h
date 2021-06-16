// (c) Copyright 2021, Kazuki Tanaka (@tk-aria)
// MIT License
// Project Home: https://github.com/velipso/sndfilter

#ifndef SNDFILTER_COMMON__H
#define SNDFILTER_COMMON__H

#ifdef __cplusplus

#define SF_API_BEGIN extern "C" {
#define SF_API_END }

#else

#define SF_API_BEGIN
#define SF_API_END

#endif

#endif // SNDFILTER_COMMON__H
