#ifndef ThresholdingIterator_Head_H
#define ThresholdingIterator_Head_H


#include "Utilities.h"
#include "IBitmapPicture.h"

#include <map>
#include <math.h>
#include <algorithm>
#include <numeric>

//#ifdef max
#undef max
#undef min
//#endif

#include <valarray>
using namespace std;


///////////////////////////////////////////////
// Forward declaration
struct SubpixelInfo;

class ThresholdingIterator
{
	int mWidth, mHeight, mScale, mShortWidth, mShortHeight;
	int mWidth4;
	Array mBarbs, mThresholdData, mIntensity;
	float mThreshold, mThresholdRelative, mNoiseLevel;
	int mX, mY;

	// weak link to the buffer inside the instance of Calculator class.
	const SubpixelInfo* mSubpixelInfo; 

public:

	ThresholdingIterator( IBitmapPicture*,
		float blurRadius, float thresholdRelative, float noiseLevel,
		float clutterSuppression);
	~ThresholdingIterator();
	
	void SetSubpixelInfo(const SubpixelInfo*);

	bool Next(int &outX, int &outY);

	float Threshold(int x, int y);  
	float Intensity(int x, int y);
private:
	void AdjustToBrightness(const Array& rgb);

	//-----------------
	

	void FillData( IBitmapPicture*, float noiseLevel, float clutterSuppression);
	float SafeSmoothValue(int x, int y, const float* p);
	void SmoothThreshold();
};

// --- constants ---
double kInvalidBarb = -1;

#undef max
#undef min

ThresholdingIterator::ThresholdingIterator( IBitmapPicture* inPicture,
	float inRadius, float inThresholdRelative, float inNoiseLevel, 
	float inClutterSuppression)
{
	mSubpixelInfo = NULL;
	mScale = int(inRadius * 3);
	mThresholdRelative = inThresholdRelative;
	mNoiseLevel = inNoiseLevel;
	mWidth = inPicture->GetRawWidth();
	mHeight = inPicture->GetRawHeight();
	mWidth4 = 4 * mWidth;
	if (mScale)
	{
		mShortWidth = mWidth / mScale;
		mShortHeight = mHeight / mScale;
		if (mShortWidth <= 0 || mShortHeight <= 0) mScale = 0;
	}

	if (mScale) 
	{
		FillData(inPicture, inNoiseLevel, inClutterSuppression);
		mX = -1; mY = 0;
	}
}


ThresholdingIterator::~ThresholdingIterator()
{
}

void ThresholdingIterator::SetSubpixelInfo(const SubpixelInfo* inSubpixelInfo)
{
	mSubpixelInfo = inSubpixelInfo;
}

bool ThresholdingIterator::Next(int &outX, int &outY)
{
	if (0 >= mScale) return false;

	for (;;)
	{
		if (++mX >= mWidth)
		{
			if (++mY >= mHeight - 1)
			{
				// iterator reached the end.
				--mX; --mY; 
				return false;
			} 
			mX = 0;
		}
		
		if (NULL == mSubpixelInfo || 
			0 == mSubpixelInfo[2 * mX + mY * mWidth4].occupiedState)
		{
			if (mBarbs[mX + mWidth * mY] > Threshold(mX, mY))
			{
				outX = mX; 
				outY = mY;
				return true;
			}
		}
	}
}

//------------------------------------------------------------------------------
/*
float ThresholdingIterator::Threshold(int inX, int inY)  
{
if (0 == mScale) return 0;
float x = MAX_(0.0f, MIN_(float(mShortWidth - 1), float(inX)/mScale - 0.5f));
float y = MAX_(0.0f, MIN_(float(mShortHeight - 1), float(inY)/mScale - 0.5f));

  double xi, yi;
  float dx = modf(x, &xi), dy = modf(y, &yi);
  float V = mThresholdData[int(xi) + (mShortWidth + 1) * int(yi)];
  
	const float* p = &mThresholdData[int(xi) + (mShortWidth + 1) * int(yi)];
	
	  return p[0] * (1 - dx) * (1 - dy) + p[1] * dx * (1 - dy) + 
	  p[mShortWidth + 1] * (1 - dx) * dy + p[mShortWidth + 2] * dx * dy;
}
*/

float ThresholdingIterator::Threshold(int inX, int inY) 
{
	if (0 == mScale) return 0;
	double x = MAX_(0.0f, MIN_(float(mShortWidth - 1), float(inX)/mScale - 0.5f));
	double y = MAX_(0.0f, MIN_(float(mShortHeight - 1), float(inY)/mScale - 0.5f));

	double xi, yi;
	double dx = modf(x, &xi), dy = modf(y, &yi);
    
	int Idx = int(xi) + (mShortWidth + 1) * int(yi);
	float* p = &(mThresholdData[Idx]);

	return p[0] * (1 - dx) * (1 - dy) + p[1] * dx * (1 - dy) + 
		p[mShortWidth + 1] * (1 - dx) * dy + p[mShortWidth + 2] * dx * dy;
}

float ThresholdingIterator::Intensity(int inX, int inY)  
{
	if (0 == mScale) return 0;
	double x = MAX_(0.0f, MIN_(float(mShortWidth - 1), float(inX)/mScale - 0.5f));
	double y = MAX_(0.0f, MIN_(float(mShortHeight - 1), float(inY)/mScale - 0.5f));

	double xi, yi;
	double dx = ::modf(x, &xi), dy = ::modf(y, &yi);

	const float* p = &mIntensity[int(xi) + (mShortWidth + 1) * int(yi)];

	return p[0] * (1 - dx) * (1 - dy) + p[1] * dx * (1 - dy) + 
		p[mShortWidth + 1] * (1 - dx) * dy + p[mShortWidth + 2] * dx * dy;
}


//-------------------------------
template <class T>
void  Squeeze(T* inBuffer, int inOffsetX, int inOffsetY, float* outBuffer,
			  int mWidth,int mHeight,int mScale)
{ 
	div_t dvx = ::div(mWidth, mScale);
	div_t dvy = ::div(mHeight, mScale);
	int stepX = inOffsetX * mScale;
	int w = dvx.quot, h = dvy.quot;
	
	const T* p0 = inBuffer;
	float* q0 = outBuffer;
	
	int dy = 0;
	for (int y = 0; y < mHeight; ++y, p0 += inOffsetY)
	{
		float* q = q0 - 1;
		const T* p = p0;
		for (int x = 0; x < w; ++x, p += stepX)
		{
			*(++q) += std::accumulate(p, p + stepX, int(0));		
		}
		if (int remainderX = inOffsetX * dvx.rem)
		{
			float s = float(std::accumulate(p, p + remainderX, int(0)));
			*(++q) += (s * stepX) / remainderX; 
		}
		else
		{
			q[1] = q[0]; ++q;
		}
		if (++dy >= mScale)
		{
			dy = 0; q0 += w + 1;
		}
	}
	if (int remainderY = dvy.rem)
	{
		float k = float(mScale) / remainderY;
		for (int x = 0; x <= w; ++x) *q0++ *= k;
	}
	else
	{
		for (int x = 0; x <= w; ++x, ++q0)
		{
			q0[0] = q0[-w-1];
		}
	}
}

//----------
void ThresholdingIterator::FillData( IBitmapPicture* inPicture, 
	float inNoiseLevel, float inClutterSuppression)
{
	const BYTE* rgbPtr = inPicture->GetData();
	int offsetX = inPicture->GetOffsetX();
	int offsetY = inPicture->GetOffsetY();
	Array intensity((mShortWidth + 1) * (mShortHeight + 1));
	
	//Squeeze(rgbPtr, inPicture->GetOffsetX(),
	//	    inPicture->GetOffsetY(), &intensity[0]);
	
	Squeeze(rgbPtr, 
		inPicture->GetOffsetX(),
		inPicture->GetOffsetY(), &intensity[0],mWidth, mHeight, mScale);

	mIntensity = intensity;

	std::valarray<int> gradient(mWidth * mHeight);
	int* q = &gradient[0];
	for (int y = 0; y < mHeight-1; ++y)
	{
		const BYTE* p = rgbPtr + y * offsetY;
		for (int x = 1; x < mWidth; ++x)
		{
			int s = 0;
			for (int i = 0; i < offsetX; ++i, ++p)
			{
				int d1 = int(p[0]) - p[offsetX + offsetY];
				int d2 = int(p[offsetX]) - p[offsetY];
				s += d1*d1 + d2*d2;
			}
			*q++ = s;
		}
		++q;
	}

	mBarbs.resize(mWidth * mHeight); 
	for (int i = mBarbs.size(); i--;) 
	{
		mBarbs[i] = float(gradient[i]);
	}	

	Array gradSqueezed(intensity.size());
	//Squeeze(&gradient[0], 1, mWidth, &gradSqueezed[0]);
	Squeeze(&gradient[0], 1, mWidth, &gradSqueezed[0],mWidth, mHeight, mScale);

	intensity += inNoiseLevel * intensity.max();
	mThresholdData = intensity;
	mThresholdData *= intensity;
	
	gradSqueezed *= inClutterSuppression * inClutterSuppression * mScale * mScale * mScale;
	float maxgs = gradSqueezed.max();
	float maxtr = mThresholdData.max();

	mThresholdData += gradSqueezed;
	float maxG = float(gradient.max());
	float maxT = mThresholdData.max();
	mThresholdData *= mThresholdRelative * mThresholdRelative * maxG / maxT;

	SmoothThreshold();
}

//-----------
float ThresholdingIterator::SafeSmoothValue(int inX, int inY,
	const float* inPtr)
{
	int offsetY = mShortWidth + 1;
	int dxm = inX < 1 ? 0: -1;
	int dxp = inX < mShortWidth ? 1 : 0;
	int dym = inY < 1 ? 0 : -offsetY; 
	int dyp = inY < mShortHeight ? offsetY : 0;

	return 4 * inPtr[0] + 3 * (inPtr[dxm] + inPtr[dxp] + inPtr[dym] + inPtr[dyp]) +
		2 * (inPtr[dxm + dym] + inPtr[dxm + dyp] + inPtr[dxp + dym] + inPtr[dxp + dyp]);
}

void ThresholdingIterator::SmoothThreshold()
{
	int x,y;
	Array t(mThresholdData.size());
	int w = mShortWidth + 1;
	int offsetY = w;
	float* q = &t[0];
	const float* p = &mThresholdData[0];
	for ( x = 0; x < w; ++x, ++p) 
		*q++ = SafeSmoothValue(x, 0, p);

	for ( y = 1; y < mShortHeight; ++y)
	{
		*q++ = SafeSmoothValue(0, y, p++);
		for (int x = 1; x < mShortWidth; ++x, ++p)
		{
			*q++ = 4 * p[0] + 
				3 * (p[-offsetY] + p[offsetY] + p[-1] + p[1]) +
				2 * (p[-offsetY - 1] + p[offsetY - 1] + p[-offsetY + 1] + p[offsetY + 1]);
		}
		*q++ = SafeSmoothValue(mShortWidth, y, p++);
	}
	for ( x = 0; x < w; ++x, ++p) 
		*q++ = SafeSmoothValue(x, mShortHeight, p);

	mThresholdData = t;
}

#endif
