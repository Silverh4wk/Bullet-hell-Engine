#include "equations.h"

real32
lerp(real32 start_angle, real32 end_angle, real32 t)
{
    return start_angle + t* ( end_angle - start_angle );
}

real32
degreeToRadians(real32 angle)
{
    return angle * ( Pi32 / 180.0f );
}


real32
flip(real32 t) {
    return 1 - t;
}

real32
timingEaseLinear(  real32 t  ) {
    return t;   
}


//starts slow, then accelerates.
real32
timingEaseIn( real32 t ) {
    return t * t;
}


real32
timingEaseInSine( real32 t ) {
    return flip(cosf( (t* Pi32 )  / 2 ) );
}

// fast start, slow end
real32
timingEaseOut(  real32 t  ) {
    return flip( ( flip( t ) * flip( t ) ) );   //flip(Square(flip(t)));
}


real32
timingEaseOutSine(  real32 t  ) {
    return sinf((t * Pi32 ) / 2);
}

//start accelerating (EaseIn) and stop decelerating (EaseOut),
real32
timingEaseInOut(  real32 t  ) {
    return lerp(timingEaseIn( t ), timingEaseOut( t ), t); 
}

real32
timingEaseInOutBack( real32 t ) {
    real32 c1 = 1.170158;
    real32 c2 = c1 * 1.525;

    return t < 0.5f ? ( powf( 2 * t, 2 )  * ( ( c2 + 1 ) * 2 * t - c2)) / 2: 
	              ( powf( 2 * t - 2, 2 ) * ( ( c2 + 1 ) * (t * 2 - 2) + c2) + 2) / 2;
}

//"mirrored" easing
//we use easein until we get to 50% and then flip and do the same
real32
timingSpike( real32 t ) {
    if ( t <= 0.5f)
	return timingEaseIn( t / 0.5f );

    return timingEaseIn(flip( t ) / 0.5f );
}


real32
getTimingFactor(  uint32 func, real32 t  ) {
    switch ( func ) {

    case TIMING_LINEAR:             return timingEaseLinear( t );
    case TIMING_EASE_IN:            return timingEaseIn( t );
    case TIMING_EASE_IN_SINE:       return timingEaseInSine( t );
    case TIMING_EASE_OUT:           return timingEaseOut( t );
    case TIMING_EASE_OUT_SINE:      return timingEaseOutSine( t );
    case TIMING_EASE_IN_OUT:        return timingEaseInOut( t );
    case TIMING_EASE_IN_OUT_BACK:   return timingEaseInOutBack( t );
    case TIMING_SPIKE:              return timingSpike( t );

	// just return back the progress by default
    default: return t;
    }
}

