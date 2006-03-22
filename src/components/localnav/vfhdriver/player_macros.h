#ifndef PLAYER_MACROS_H
#define PLAYER_MACROS_H

// A set of macros, copied from Player, used by vfh_algorithm.cc

/* never worry about timersub type activies again -- from GLIBC and upcased. */
#define TIMESUB(a, b, result)                                                 \
  do {                                                                        \
    (result)->tv_sec = (a)->tv_sec - (b)->tv_sec;                             \
    (result)->tv_usec = (a)->tv_usec - (b)->tv_usec;                          \
    if ((result)->tv_usec < 0) {                                              \
      --(result)->tv_sec;                                                     \
      (result)->tv_usec += 1000000;                                           \
    }                                                                         \
  } while (0)

#endif
