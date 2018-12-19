#ifndef Calculator_Head_H
#define Calculator_Head_H

#include "Utilities.h"
#include "SubpixelInfoProvider.h"
#include <vector>

using namespace std;

 struct Info { 
    int xi, eta; 
    float f; 
    const SubpixelInfo* p; 
};

class Booker
{ 
	SubpixelInfo** mLastBuffer;
	int mIndexOfLastBuffer;
	int mNextPositionInsideLastBuffer;

public:
 
	void MarkOccupied(SubpixelInfo*);
	void Flush(char savingMask);
	vector<SubpixelInfo*> SubInfoVec;
};

void inline Booker::MarkOccupied(SubpixelInfo* inInfo)
{
	if (0 != (inInfo->occupiedState & 
		(kOccupiedBySelfMask | kOutsideBoundsMask)))
	{
		return;
	}
	inInfo->occupiedState |= kOccupiedBySelfMask;
	SubInfoVec.push_back(inInfo);
}

void inline Booker::Flush(char inSavingMask)
{  
	int i;
	for(i= SubInfoVec.size()-1; i>=0; i--)
	{ 
		SubpixelInfo*p = SubInfoVec[i];
		char &ch = p->occupiedState;
		ch = (ch & (~kOccupiedBySelfMask)) | inSavingMask;
	}
    SubInfoVec.clear();
}

class Calculator
{
public:

	typedef Info* InfoPtr;
	typedef Info* ConstInfoPtr;
	typedef bool (*CompareIntsFunction)(int, int);

	mutable SubpixelInfoProvider mSubpixelInfoProvider;

	Booker mBooker;
	//BufferKeeper mBufferKeeper1, mBufferKeeper2;
 
	ConstInfoPtr mInfoWithMaxValue;
	char mOverlapFlags;
	bool mIsEndToEnd;

	int mMinXi, mMaxXi, mMinEta, mMaxEta, mMaxLimit;
	int mInitialDotHalfsizeBig;
	float mDeltaXi, mLineHalfwidth, mCachedVx, mCachedVy;
	float mMaxDotHalfsize, mTooBigSquaredLineWidth;
	float mThresholdFactor;
	
	// coefficients of an approximating parabola (f = a*x*x + b*x + c)
	float mParabolaA, mParabolaB, mParabolaC;
	float mApproximationEpsilon, mSquaredLineWidth;

public:

	vector<Info*> InfoPtrVec;
	vector<Info> CacheInfoVec;
	Calculator( IBitmapPicture*, float blurRadius);
	 

	const SubpixelInfo* GetInfoBuffer() const;
	const SubpixelInfo* GetInfo(int x, int y) const;

	void SetThresholdFactor(float);
	void SetInitialBounds(float halfsize);
	void SetInitialDefaultBounds();
	void ExtendXiBounds();

	bool UpdateCache(float x, float y, float vx, float vy, bool checkOccupied = false);
	bool CalculateF(float a, float b);
	void SummarizeAB(float& outA, float& outB) const;

	void AdjustXiBounds();
	void AdjustRectExceptFront();
	void AdjustRectExceptBack();

	bool FindAlternativeXiBounds();

	// check alignment of gradient direction in the pixel with maximum,
	// and the coordinates of the reference point 
	// (xi = 0, eta = referencePointEta * kTwiceMagnification).
	bool CheckAlignmentOfMaxPixel(float referencePointEta) const;

	// returns precision of approximation (epsilon)
	bool ApproximateByParabola(bool calculateAll = true);
	float GetLastApproximationEpsilon() const;

	void SetMaximumDotSize(float size);
	float GetDeltaXi() const;
	float GetLineWidth() const;
	float GetDotHalfsize() const;
	bool IsForwardOverlapping() const;

	void MarkOccupied();
	void UpdateBooking(char savingMask);
	
	bool IsMiddlePoint() const;
	void GetXiBounds(float& outMinXi, float& outMaxXi) const;
	float GetMinEta() const;
	float GetMaxEta() const;
    void  SummarizeSideColors(float inXiShift, short int* outColorsComponents);
private:

	static inline 
	ConstInfoPtr* FilterAndSort(ConstInfoPtr* begin, ConstInfoPtr* end, int xiLimit,
		CompareIntsFunction compareXi, CompareIntsFunction compareEta);
};

#include <algorithm>

//////////////////////////////////////////////////////////////////////////
// Constants
static const int kMagnificationBit = 12;
static const int kMagnification = 1 << kMagnificationBit;
static const int kTwiceMagnification = 2 * kMagnification;
static const int kMagnificationMinusOne = kMagnification - 1;

static const int kInitialBufferLength = 1024;

static const float kPoorApproximationPrecision = 2.0f;
static const float kParabolaThreshold = 0.125f;
static const float kBestApproximationToParabolaFactor = 0.38f;

///////////////////////////////////////////////////////////////////////////
inline Calculator::Calculator( IBitmapPicture* inBitmap, float inBlurRadius)
	: mSubpixelInfoProvider(inBitmap, inBlurRadius),
	mBooker(),
	mInitialDotHalfsizeBig(int(inBlurRadius * kTwiceMagnification)),
	mThresholdFactor(kParabolaThreshold)
{
	SetMaximumDotSize(4 * inBlurRadius);
}


void inline Calculator::SummarizeSideColors(float inXiShift, short int* outColorsComponents)
{
	float sumC[3], sumCF[3], sumF, sumFF;
	sumF = sumFF = 0.0f;
	int i;

	for ( i = 0; i < 3; ++i)
	{
		sumC[i] = sumCF[i] = 0.0f;
	}

	int pixelsCountInt = 0;
	const float xiMax = ::sqrt(mSquaredLineWidth);
	const int safetyMargin = (MAX_(mMaxXi, -mMinXi) + 
		MAX_(mMaxEta, -mMaxEta)) / kMagnification;

	int t;
	for (t = 0; t <CacheInfoVec.size();t++)
	{
		Info* p = &(CacheInfoVec[t]);
 
		//for (const Info* p = mInfos; p < mInfosEnd; ++p)
		//{
			if (mMinXi <= p->xi && p->xi <= mMaxXi &&
				mMinEta <= p->eta && p->eta <= mMaxEta)
			{
				float x = float(p->xi) / xiMax;
				if (::fabs(x) <= 1.0f)
				{
					++pixelsCountInt;
					float x2 = x * x;
					// "f" is an approximation of integral from function (1-x*x)^(3/2)
					float f = x * (1 - x2 * (0.491826706f -
						x2 * (0.045323591f + x2 * 0.035551738f)));
					sumF += f;
					sumFF += f * f;
					const float* colors = 
						mSubpixelInfoProvider.GetColorsAtGradientPoint(p->p);
					for (int i = 3; i--;)
					{
						// make the red color be stored the first
						// (in the SubpixelInfo::colors the order is: blue, green, red)
						const float& c = colors[2 - i];
						sumC[i] += c;
						sumCF[i] += c * f;
					}
				}
			}
		}
 

	// calculate output, using statistics
	const float pixelsCount = float(pixelsCountInt);
	const float delta = sumFF * pixelsCount - sumF * sumF;
	for ( i = 0; i < 3; ++i)
	{
		if (0 != delta)
		{
			float c0 = sumC[i] * sumFF - sumCF[i] * sumF;
			float c1 = 0.58904862f * 
				(sumCF[i] * pixelsCount - sumC[i] * sumF);
			outColorsComponents[i] = (short int)((c0 - c1) / delta);
			outColorsComponents[i + 3] = (short int)((c0 + c1) / delta);
		}
		else
		{
			// unlikely to go here because there is great color gradient
			// at the place of contour dot.  As a precaution, show the average color
			outColorsComponents[i] = outColorsComponents[i + 3] = 
				(short int)(sumC[i] / pixelsCount);
		}
	}
}
 

const inline SubpixelInfo* Calculator::GetInfoBuffer() const
{
	return mSubpixelInfoProvider.GetInfoBuffer();
}

const inline SubpixelInfo* Calculator::GetInfo(int inX, int inY) const
{
	return mSubpixelInfoProvider.GetInfo(2 * inX, 2 * inY);
}


void inline Calculator::SetThresholdFactor(float inThresholdFactor)
{
	mThresholdFactor = inThresholdFactor;
}

void inline Calculator::SetInitialBounds(float inHalfsize)
{
	mMaxXi = mMaxEta = mMaxLimit = int(inHalfsize * kTwiceMagnification);
	mMinXi = mMinEta = - mMaxXi;
}

void inline Calculator::SetInitialDefaultBounds()
{
	mMaxXi = mMaxEta = mMaxLimit = mInitialDotHalfsizeBig;
	mMinXi = mMinEta = - mMaxXi;
}

void inline Calculator::ExtendXiBounds()
{
	int deltaXi = int(mDeltaXi);
	if (deltaXi > 0)
	{
		mMaxXi += deltaXi;
		if (mMaxXi > mMaxLimit)
		{
			mMaxXi = mMaxLimit;
		}
	}
	else
	{
		mMinXi += deltaXi;
		if (mMinXi < -mMaxLimit)
		{
			mMinXi = -mMaxLimit;
		}
	}
}

bool inline Calculator::UpdateCache(float inX, float inY, float inVx, float inVy, 
	bool inCheckOccupied)
{
	mOverlapFlags = 0;
	mCachedVx = inVx;
	mCachedVy = inVy;

	int halfSizeXi  = MAX_(mMaxXi, -mMinXi);
	int halfSizeEta = MAX_(mMaxEta, -mMinEta);
	int deltaBigX = ::abs(int(halfSizeXi * inVx)) + ::abs(int(halfSizeEta * inVy));
	int deltaBigY = ::abs(int(halfSizeXi * inVy)) + ::abs(int(halfSizeEta * inVx));

	int bigCenterX = int(kTwiceMagnification * inX);
	int bigCenterY = int(kTwiceMagnification * inY);
	int beginTwiceX = (bigCenterX - deltaBigX) >> kMagnificationBit;
	int endTwiceX = ((bigCenterX + deltaBigX) >> kMagnificationBit) + 1;
	int beginTwiceY = (bigCenterY - deltaBigY) >> kMagnificationBit;
	int endTwiceY = ((bigCenterY + deltaBigY) >> kMagnificationBit) + 1;

	if (beginTwiceX >= endTwiceX || beginTwiceY >= endTwiceY)
	{ 
		CacheInfoVec.clear();
		return false;
	}

	int rangeY = endTwiceY - beginTwiceY;

	int bigVx = int(::floor(kMagnification * inVx + 0.5));
	int bigVy = int(::floor(kMagnification * inVy + 0.5));

	int bigDx = (beginTwiceX << kMagnificationBit) - bigCenterX;
	int bigDy = (beginTwiceY << kMagnificationBit) - bigCenterY;
	int xi = int(- inVy * bigDx + inVx * bigDy);
	int eta = int(inVx * bigDx + inVy * bigDy);

	int deltaXi_y = bigVx;
	int deltaEta_y = bigVy;
	int deltaXi_x = -bigVy - bigVx * rangeY;
	int deltaEta_x = bigVx - bigVy * rangeY;

	float othersA = 0, othersB = 0;
	int othersCount = 0;

 
	CacheInfoVec.clear();
	Info TempInfo;

	for (int x = beginTwiceX; x < endTwiceX; ++x)
	{
		for (int y = beginTwiceY; y < endTwiceY; ++y)
		{
			if (mMinXi <= xi && xi <= mMaxXi &&
				mMinEta <= eta && eta <= mMaxEta)
			{
				const SubpixelInfo* subPixelInfo = mSubpixelInfoProvider.GetInfo(x, y);
				if (0 == (subPixelInfo->occupiedState & (~kOccupiedBySelfMask)))
				{
					/*p->xi = xi;
					p->eta = eta;
					p->p = subPixelInfo;
					++p;*/
					TempInfo.xi  = xi;
					TempInfo.eta = eta;
					TempInfo.p = subPixelInfo;
					//TempInfo.p->dXX = x;
					//TempInfo.p->dYY = y;
					CacheInfoVec.push_back(TempInfo);
				}
				else if (inCheckOccupied)
				{
					char ch = subPixelInfo->occupiedState;
					mOverlapFlags |= ch;
					if (kOccupiedByOthersMask & ch)
					{
						++othersCount;
						othersA += subPixelInfo->a;
						othersB += subPixelInfo->b;
					}
				}
			}
			xi += deltaXi_y;
			eta += deltaEta_y;
		}
		xi += deltaXi_x;
		eta += deltaEta_x;
	}
	 

	return 0 == othersCount  ||
		(othersA * (inVx * inVx - inVy * inVy) + 2.0f * othersB * inVx * inVy <= 0);
}

bool inline Calculator::CalculateF(float inA, float inB)
{
	mInfoWithMaxValue = NULL;
	float fmax = 0;
	//mInfos;.clear();
	//Info tp = CacheInfoVec[0];
	int i;
	for (i = 0; i <CacheInfoVec.size();i++)
	{
		Info* p = &(CacheInfoVec[i]);
		if (mMinXi <= p->xi && p->xi <= mMaxXi)
		{
			float f = inA * (p->p->a) + inB * (p->p->b);
			p->f = f;
			if (f > fmax)
			{
				fmax = f;
				mInfoWithMaxValue = p;
			}
		}
	}
	return (NULL != mInfoWithMaxValue);
}

void inline Calculator::SummarizeAB(float& outA, float& outB) const
{
	float a = 0, b = 0;

	int i;
	for (i = 0; i < CacheInfoVec.size(); i++)
	{
		Info* p = (Info*)&(CacheInfoVec[i]);
		if (mMinXi <= p->xi && p->xi <= mMaxXi &&
			mMinEta <= p->eta && p->eta <= mMaxEta)
		{
			a += p->p->a;
			b += p->p->b;
		}	
	}
	outA = a;
	outB = b;
}

void inline Calculator::AdjustXiBounds()
{
	if (NULL == mInfoWithMaxValue)
	{
		return;
	}
	int xiOfMax = mInfoWithMaxValue->xi;
	float threshold = mInfoWithMaxValue->f * mThresholdFactor;
	int lowXi = mMinXi - 1;
	int highXi = mMaxXi + 1;

	int i;
	for (i = 0; i < CacheInfoVec.size(); i++)
	{
		Info* p = (Info*)&CacheInfoVec[i];

		int xi = p->xi;
		if (lowXi < xi && xi < highXi &&
			mMinEta <= p->eta && p->eta <= mMaxEta)
		{
			if (p->f < threshold)
			{
				if (xi < xiOfMax)
				{
					lowXi = xi;
				}
				else
				{
					highXi = xi;
				}
			}
		}
	}
	mMinXi = lowXi + 1;
	mMaxXi = highXi - 1;
}

//////////////////////////////////////////////////////////////////////////////

static inline bool LessEqual(int inLeft, int inRight)
{
	return inLeft <= inRight;
}
static inline bool GreaterEqual(int inLeft, int inRight)
{
	return inLeft >= inRight;
}

inline Calculator::ConstInfoPtr* 
	Calculator::FilterAndSort(ConstInfoPtr* inBegin, ConstInfoPtr* inEnd, 
		int inXiLimit,
		CompareIntsFunction inCompareXi, CompareIntsFunction inCompareEta)
{
	ConstInfoPtr* qEnd = inBegin;
	for (const ConstInfoPtr* p = inBegin; p < inEnd; ++p)
	{
		int xi = (*p)->xi;
		if (inCompareXi(inXiLimit, xi))
		{
			continue;
		}

		int eta = (*p)->eta;
		ConstInfoPtr* q = inBegin;

		while (q < qEnd && inCompareEta(eta, (*q)->eta))
		{
			++q;
		}
		if (q > inBegin && inCompareXi(q[-1]->xi, xi))
		{
			continue;
		}
		if (q == qEnd)
		{
			*qEnd++ = *p;
			continue;
		}
		ConstInfoPtr* qSkipper = q;
		while (qSkipper < qEnd && inCompareXi(xi, (*qSkipper)->xi))
		{
			++qSkipper;
		}
		if (q > inBegin && eta == (*q)->eta)
		{
			--q;
		}
		// insert at q, then put values from range [qSkipper...qEnd[
		if (q == qSkipper)
		{
			for (ConstInfoPtr* r = qEnd++; r > q; --r)
			{
				r[0] = r[-1];
			}
			*q = *p;
		}
		else if (q + 1 == qSkipper)
		{
			*q = *p;
		}
		else
		{
			*q++ = *p;
			while (qSkipper < qEnd)
			{
				*q++ = *qSkipper++;
			}
			qEnd = q;
		}
	}
	return qEnd;
}


void inline Calculator::AdjustRectExceptFront()
{
	InfoPtr *lowLimiters, *highLimiters;
	int onePartSize = CacheInfoVec.size(); 
	
	//mBufferKeeper2.NewBufferWithCapacity(2 * onePartSize, &lowLimiters);
	 
	InfoPtrVec.resize(2*onePartSize);
	lowLimiters =  &InfoPtrVec[0];
	highLimiters = lowLimiters + onePartSize;

	ConstInfoPtr *lowPtr = lowLimiters, *highPtr = highLimiters;

	const float threshold = mInfoWithMaxValue->f * mThresholdFactor;
	const int xiOfMax = mInfoWithMaxValue->xi;
	const int etaOfMax = mInfoWithMaxValue->eta;

	int xi_low = mMinXi - 1;
	int xi_high = mMaxXi + 1;
	
 
	int i;
	for (i = 0; i < CacheInfoVec.size(); i++)
	{
		Info* p = &CacheInfoVec[i];

		if (p->f >= threshold)
		{
			continue;
		}

		int xi = p->xi; 
		if (p->eta < etaOfMax)
		{
			if (xi < xiOfMax) *lowPtr++ = p;
			else *highPtr++ = p;
		}
		else
		{
			if (xi < xiOfMax)
			{
				if (xi > xi_low) 
				{
					xi_low = xi;
				}
			}
			else if (xi < xi_high)
			{
				xi_high = xi;
			}
		}
	}
	// filter and sort lowLimiters
	ConstInfoPtr *lowEnd = FilterAndSort(lowLimiters, lowPtr, xi_low,
		GreaterEqual, LessEqual);
	ConstInfoPtr *highEnd = FilterAndSort(highLimiters, highPtr, xi_high,
		LessEqual, LessEqual);
	
	int savedXiLow = xi_low;
	int savedXiHigh = xi_high;
	int savedEtaLow = mMinEta - 1;
	unsigned int maxArea = 0;

	bool savedIsEndToEnd = true;

	lowPtr = lowLimiters;
	highPtr = highLimiters;
	mIsEndToEnd = true;
	
	for (bool isEnd = false; !isEnd;)
	{
		int eta;
		if (lowPtr < lowEnd)
		{
			if (highPtr < highEnd && (*highPtr)->eta > (*lowPtr)->eta)
			{
				eta = (*highPtr)->eta;
				xi_high = (*highPtr)->xi;
				++highPtr;
			}
			else
			{
				eta = (*lowPtr)->eta;
				xi_low = (*lowPtr)->xi;
				++lowPtr;
			}
		}
		else
		{
			if (highPtr < highEnd)
			{
				eta = (*highPtr)->eta;
				xi_high = (*highPtr)->xi;
				++highPtr;
			}
			else
			{
				eta = mMinEta - 1;
				isEnd = true;
			}
		}

		unsigned int area = (unsigned int)(xi_high - xi_low) * (mMaxEta - eta);
		if (area > maxArea)
		{
			maxArea = area;
			savedXiLow = xi_low;
			savedXiHigh = xi_high;
			savedEtaLow = eta;
			savedIsEndToEnd = isEnd;
		}
	}
	mMinXi = savedXiLow + 1;
	mMaxXi = savedXiHigh - 1;
	mMinEta = savedEtaLow + 1;
	mIsEndToEnd = savedIsEndToEnd;
}

void inline Calculator::AdjustRectExceptBack()
{
	ConstInfoPtr *lowLimiters, *highLimiters;
	int onePartSize = CacheInfoVec.size(); 
	 
	InfoPtrVec.resize(2 * onePartSize);
	lowLimiters = &InfoPtrVec[0];
	highLimiters = lowLimiters + onePartSize;

	ConstInfoPtr *lowPtr = lowLimiters, *highPtr = highLimiters;

	const float threshold = mInfoWithMaxValue->f * mThresholdFactor;
	const int xiOfMax = mInfoWithMaxValue->xi;
	const int etaOfMax = mInfoWithMaxValue->eta;

	int xi_low = mMinXi - 1;
	int xi_high = mMaxXi + 1;
	
	// gather limiting points with eta > etaOfMax
	int i;
	for (i = 0; i < CacheInfoVec.size(); i++)
	{
		Info* p = &CacheInfoVec[i];

		if (p->f >= threshold)
		{
			continue;
		}

		int xi = p->xi; 
		if (p->eta > etaOfMax)
		{
			if (xi < xiOfMax) *lowPtr++ = p;
			else *highPtr++ = p;
		}
		else
		{
			if (xi < xiOfMax)
			{
				if (xi > xi_low) 
				{
					xi_low = xi;
				}
			}
			else if (xi < xi_high)
			{
				xi_high = xi;
			}
		}
	}
	// filter and sort lowLimiters
	ConstInfoPtr *lowEnd = FilterAndSort(lowLimiters, lowPtr, xi_low,
		GreaterEqual, GreaterEqual);
	ConstInfoPtr *highEnd = FilterAndSort(highLimiters, highPtr, xi_high,
		LessEqual, GreaterEqual);
	
	
	int savedXiLow = xi_low;
	int savedXiHigh = xi_high;
	int savedEtaHigh = mMaxEta + 1;
	unsigned int maxArea = 0;

	int xi_low0 = xi_low;
	int xi_high0 = xi_high;

	bool savedIsEndToEnd = true;

	lowPtr = lowLimiters;
	highPtr = highLimiters;
	mIsEndToEnd = true;
	
	for (bool isEnd = false; !isEnd;)
	{
		int eta;
		if (lowPtr < lowEnd)
		{
			if (highPtr < highEnd && (*highPtr)->eta < (*lowPtr)->eta)
			{
				xi_high = (*highPtr)->xi;
				eta = (*highPtr)->eta;
				++highPtr;
			}
			else
			{
				xi_low = (*lowPtr)->xi;
				eta = (*lowPtr)->eta;
				++lowPtr;			
			}
		}
		else
		{
			if (highPtr < highEnd)
			{
				xi_high = (*highPtr)->xi;
				eta = (*highPtr)->eta;
				++highPtr;
			}
			else
			{
				eta = mMaxEta + 1;
				isEnd = true;
			}
		}
		unsigned int area = (unsigned int)(xi_high - xi_low) * (eta - mMinEta);
		if (area > maxArea)
		{
			maxArea = area;
			savedXiLow = xi_low;
			savedXiHigh = xi_high;
			savedEtaHigh = eta;
			savedIsEndToEnd = isEnd;
		}
	}
	mMinXi = savedXiLow + 1;
	mMaxXi = savedXiHigh - 1;
	mMaxEta = savedEtaHigh - 1;
	mIsEndToEnd = savedIsEndToEnd;
}

bool inline Calculator::FindAlternativeXiBounds()
{
	int t;//<--wtz debug

	if (NULL == mInfoWithMaxValue)
	{
		return false;
	}
	int xiOfMax = mInfoWithMaxValue->xi;

	ConstInfoPtr *pointers;
	
	InfoPtrVec.resize(CacheInfoVec.size());
	pointers = &InfoPtrVec[0];
	int count = 0;
	 
	if (0 < xiOfMax)
	{
		 
	   for (t = 0; t < CacheInfoVec.size(); t++)
	   {
		   Info* p = &CacheInfoVec[t];
		 
			int xi = p->xi;
			if (xi >= xiOfMax)
			{
				continue;
			}
			int a = 0, b = count;
			while (a < b)
			{
				int i = (a + b) >> 1;
				if (xi < pointers[i]->xi)
				{
					a = i + 1;
				}
				else 
				{
					b = i;
				}
			}
			if (a < count)
			{
				::memmove(&pointers[a+1], &pointers[a], (count - a) * sizeof(pointers[0]));
			}
			pointers[a] = p;
			++count;
		}
	}
	else // here xiOfMax < 0
	{
	   
	   for (t = 0; t < CacheInfoVec.size(); t++)
	  {
		    Info* p = &CacheInfoVec[t];

			int xi = p->xi;
			if (xi <= xiOfMax)
			{
				continue;
			}
			int a = 0, b = count;
			while (a < b)
			{
				int i = (a + b) >> 1;
				if (xi > pointers[i]->xi)
				{
					a = i + 1;
				}
				else 
				{
					b = i;
				}
			}
			if (a < count)
			{
				::memmove(&pointers[a+1], &pointers[a], (count - a) * sizeof(pointers[0]));
			}
			pointers[a] = p;
			++count;
		}
	}

	float currentF = mInfoWithMaxValue->f + 1;
	int xi1OfLow = 0, xi2OfLow = 0, countLow = 0;
	int currentXi1 = 0, currentCount = 0;

	ConstInfoPtr currentMaxPtr = NULL;
	ConstInfoPtr maxPtr = NULL;

	float currentMaxF = 0;

	for (int  i = 0; i < count; ++i)
	{
		ConstInfoPtr p = pointers[i];
		if (p->f < currentF)
		{
			if (currentCount > countLow)
			{
				countLow = currentCount;
				xi1OfLow = currentXi1;
				xi2OfLow = p->xi;
				maxPtr = currentMaxPtr;
			}
			currentMaxF = currentF = p->f;
			currentXi1 = p->xi;
			currentCount = 0;
		}
		else
		{
			++currentCount;
			if (p->f > currentMaxF)
			{
				currentMaxF = p->f;
				currentMaxPtr = p;
			}
		}
	}

	if (3 < countLow)
	{
		if (0 > xiOfMax)
		{
			if (0 > xi1OfLow && NULL != maxPtr)
			{
				mMinXi = xi1OfLow;
				mMaxXi = xi2OfLow;
				mInfoWithMaxValue = maxPtr;
			}
		}
		else
		{
			if (0 < xi1OfLow && NULL != maxPtr)
			{
				mMaxXi = xi1OfLow;
				mMinXi = xi2OfLow;
				mInfoWithMaxValue = maxPtr;
			}
		}
	}

	return true;
}

bool inline Calculator::CheckAlignmentOfMaxPixel(float inReferencePointEta) const
{
	if (NULL == mInfoWithMaxValue)
	{
		return false;
	}

	float vEta = float(mInfoWithMaxValue->eta - inReferencePointEta * kTwiceMagnification);
	float vXi = float(mInfoWithMaxValue->xi);

	// this is the direction to the vertex of isosceles triangle 
	float vEta2 = vEta * vEta - vXi * vXi;
	float vXi2 = 2 * vXi * vEta;

	// rotate to XY
	float vx = vEta2 * mCachedVx - vXi2 * mCachedVy;
	float vy = vEta2 * mCachedVy + vXi2 * mCachedVx;

	float a = vx * vx - vy * vy;
	float b = 2 * vx * vy;
	float scalarProduct = a * mInfoWithMaxValue->p->a + b * mInfoWithMaxValue->p->b;
	float vectorProduct = b * mInfoWithMaxValue->p->a - a * mInfoWithMaxValue->p->b;

	return (::fabs(vectorProduct) < scalarProduct);
}

//-------------------------------------
bool inline Calculator::ApproximateByParabola(bool inCalculateAll)
{
	float sx, sx2, sx3, sx4, sf, sxf, sx2f, sff;
	sx = sx2 = sx3 = sx4 = sf = sxf = sx2f = sff = 0;
	int s0 = 0;

	float k = kBestApproximationToParabolaFactor / mInfoWithMaxValue->f;

	int i;
	for (i = 0; i < CacheInfoVec.size(); i++)
	{
		Info* p = &CacheInfoVec[i];

		int xi = p->xi;
		if (mMinXi <= xi && xi <= mMaxXi &&
			mMinEta <= p->eta && p->eta <= mMaxEta)
		{
			float f =  p->f;
			f -= k * f * f;

			++s0;
			sf += f;

			if (inCalculateAll)
			{
				sff += f * f;
			}

			sx += xi;
			sxf += (f *= xi);
			sx2f += f * xi;

			float xi2 = float(xi) * xi;
			sx2 += xi2;
			sx3 += xi2 * xi;
			sx4 += xi2 * xi2;
		}
	}
	if (s0 < 4)
	{
		return false;
	}

	float x22 = sx4 * s0 - sx2 * sx2;
	float x11 = sx2 * s0 - sx * sx;
	float x12 = sx3 * s0 - sx * sx2;
	float z1 = sxf * s0 - sx * sf;
	float z2 = sx2f * s0 - sx2 * sf;
	float Da = z2 * x11 - z1 * x12;
	float Db = z1 * x22 - z2 * x12;
	if (0 <= Da)
	{
		return false;
	}
	mDeltaXi = - 0.5f * Db / Da;
	if (::fabs(mDeltaXi) > float((mMaxXi - mMinXi) / 2))
	{
		return false;
	}

	if (!inCalculateAll)
	{
		// skip calculation of the other variables (besides mDeltaXi)
		return true;
	}
	
	float D = x11 * x22 - x12 * x12;
	float f11 = sff * s0 - sf * sf;
	if (0 >= D || 0 >= f11)
	{
		return false;
	}

	mParabolaA = Da / D;
	mParabolaB = Db / D;
	mParabolaC = (sf - mParabolaA * sx2 - mParabolaB * sx) / s0;

	mSquaredLineWidth = mDeltaXi * mDeltaXi - mParabolaC / mParabolaA;
	if (0 >= mSquaredLineWidth || mSquaredLineWidth >= mTooBigSquaredLineWidth)
	{
		return false;
	}

	mApproximationEpsilon = 1 + (mParabolaA * mParabolaA * x22 + 
		mParabolaB * mParabolaB * x11 + 
		2 * mParabolaA * mParabolaB * x12 -
		2 * mParabolaA * z2 -
		2 * mParabolaB * z1) / f11;

	return true;
}

///////////////////////////////////////////////////////////////////////////
float inline Calculator::GetLastApproximationEpsilon() const
{
	return mApproximationEpsilon;
}

///////////////////////////////////////////////////////////////////////////
void inline Calculator::SetMaximumDotSize(float inSize)
{
	mMaxDotHalfsize = 0.5f * inSize;
	mTooBigSquaredLineWidth = 10 * mMaxDotHalfsize * mMaxDotHalfsize 
		* kTwiceMagnification * kTwiceMagnification;
}
float inline Calculator::GetDeltaXi() const
{
	return mDeltaXi / kTwiceMagnification;
}

float inline Calculator::GetLineWidth() const
{
	return float(::sqrt(mSquaredLineWidth) * 0.375 * M_PI / kTwiceMagnification);
}

float inline Calculator::GetDotHalfsize() const
{
	float halfsize = ::sqrt(mSquaredLineWidth) / kTwiceMagnification;
	
	// clip the value to range (1..mMaxDotHalfsize)
	float tempV = halfsize< mMaxDotHalfsize?
                  halfsize: mMaxDotHalfsize;
	tempV = 1.0 > tempV? 1.0 :tempV; 
	//return max(1, min(halfsize, mMaxDotHalfsize));
	return tempV;
}

bool inline Calculator::IsForwardOverlapping() const
{
	int minEta = MAX_(0, mMinEta);

	int i;
	for (i = 0; i < CacheInfoVec.size(); i++)
	{
		Info* p = (Info*)&CacheInfoVec[i];

		if (0 != (kOccupiedBySelfMask & p->p->occupiedState) &&
			minEta < p->eta && p->eta < mMaxEta &&
			mMinXi <= p->xi && p->xi <= mMaxXi)
		{
			return true;
		}
	}
	return false;
}

void inline Calculator::MarkOccupied()
{	 
	int i;
	for (i = 0; i < CacheInfoVec.size(); i++)
	{
		Info* p = &CacheInfoVec[i];

		if (mMinXi <= p->xi && p->xi <= mMaxXi &&
			mMinEta <= p->eta && p->eta <= mMaxEta)
		{
			mBooker.MarkOccupied(const_cast<SubpixelInfo*>(p->p));
		}
	}
}

void inline Calculator::UpdateBooking(char inSavingMask)
{
	mBooker.Flush(inSavingMask);
}

bool inline Calculator::IsMiddlePoint() const
{
	return mIsEndToEnd;
}

void inline Calculator::GetXiBounds(float& outMinXi, float& outMaxXi) const
{
	outMinXi = float(mMinXi) / kTwiceMagnification;
	outMaxXi = float(mMaxXi) / kTwiceMagnification;
}

float inline Calculator::GetMinEta() const
{
	return float(mMinEta) / kTwiceMagnification;
}

float inline Calculator::GetMaxEta() const
{
	return float(mMaxEta) / kTwiceMagnification;
}

#endif