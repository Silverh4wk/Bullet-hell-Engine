#ifndef EQUATIONS_H
#define EQUATIONS_H

#include "../types.h"
#include <linmath.h>

typedef enum {
  TIMING_LINEAR,
  TIMING_EASE_IN,
  TIMING_EASE_IN_SINE,
  TIMING_EASE_OUT,
  TIMING_EASE_OUT_SINE,
  TIMING_EASE_IN_OUT,
  TIMING_EASE_IN_OUT_BACK,
  TIMING_SPIKE,
} Timing;

#define Pi32 3.14159265359f
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

int32  pseudoRand(int32 n);

real32 lerp(real32 start_angle, real32 end_angle, real32 t);

real32 degreeToRadians(real32 angle);


// timing functions
// https://easings.net/
// https://blog.febucci.com/2018/08/easing-functions/

real32
flip(real32 t) ;

real32
timingEaseLinear(  real32 t  );


//starts slow, then accelerates.
real32
timingEaseIn( real32 t ) ;

real32
timingEaseInSine( real32 t );

// fast start, slow end
real32
timingEaseOut(  real32 t  );


real32
timingEaseOutSine(  real32 t  );

//start accelerating (EaseIn) and stop decelerating (EaseOut),
real32
timingEaseInOut(  real32 t  ) ;

real32
timingEaseInOutBack( real32 t ) ;

//"mirrored" easing
//we use easein until we get to 50% and then flip and do the same
real32
timingSpike( real32 t ) ;

real32
getTimingFactor(uint32 func, real32 t);

#endif
