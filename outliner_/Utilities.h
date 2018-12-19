#ifndef Utilities_Head_H
#define Utilities_Head_H

#include "IBitmapPicture.h"
#include <vector> 
#include <deque>
#include <valarray>

using namespace std;

typedef std::vector<BYTE> ByteVector;
typedef std::valarray<float> Array;

const int kInvalidIndex = -1;

enum OccupyMask { 
	kOccupiedByOthersMask = 1, 
	kOccupiedBySelfMask = 2,
	kOutsideBoundsMask = 0x3c,
	kOccupiedBySpeck = 0x40
};

struct SubpixelInfo 
{
	int dXX,dYY;
	float a, b, colors[3];
	short int gradientFormulaID;
	char occupiedState, isColorsReady;
};


#define MAX_(a,b) ((a) > (b) ? (a) : (b))
#define MIN_(a,b) ((a) < (b) ? (a) : (b))

#define M_E        2.71828182845904523536
#define M_LOG2E    1.44269504088896340736
#define M_LOG10E   0.434294481903251827651
#define M_LN2      0.693147180559945309417
#define M_LN10     2.30258509299404568402
#define M_PI       3.14159265358979323846
#define M_PI_2     1.57079632679489661923
#define M_PI_4     0.785398163397448309616
#define M_1_PI     0.318309886183790671538
#define M_2_PI     0.636619772367581343076
#define M_2_SQRTPI 1.12837916709551257390
#define M_SQRT2    1.41421356237309504880
#define M_SQRT1_2  0.707106781186547524401
///////////////////////////////////////////////

#endif


