#ifndef SubpixelInfoProvider_Head_H
#define SubpixelInfoProvider_Head_H

#define NOMINMAX
#include "Utilities.h"
#include <math.h>
//#include "IBitmapPicture.h"



///////////////////////
// forward declarations
struct AveragingMatrix;
struct LevelPoints;

class SubpixelInfoProvider
{
	const BYTE* mOriginalData; // weak reference to original picture
	int mWidth, mHeight, mOffsetX, mOffsetY;

	float mRadius, mRadiusSquared;
	int mRadius2Int;

	SubpixelInfo *mInfoBuffer;
	int mTwiceWidth, mTwiceHeight, mWidth4;
	int mMaxTwiceX, mMaxTwiceY;
	int mMaxUncheckedX, mMaxUncheckedY;

	float mDebugFriendlyScale;

	short int mGradientFormulaID, mLastCustomFormulaID;
	float mColorGradientFactors[3];

	AveragingMatrix* mMatrixes[4];
	const AveragingMatrix* mCurrentAveragingMatrix;
	void *mAllMatrixesElements;

	int mMinDx, mMaxDx, mMinDy, mMaxDy;

public:
	SubpixelInfoProvider( IBitmapPicture*, float blurRadius);
	~SubpixelInfoProvider();

	const SubpixelInfo* GetInfo(int twiceX, int twiceY);
	void SetColorGradientFactors(const float* threeFactors);

	const SubpixelInfo* GetInfoBuffer() const;
	const float* GetColorsAtGradientPoint(const SubpixelInfo* inReferencePtr);

private:
	const SubpixelInfo* GetMarginInfo(bool atLeft, bool atRight,
		bool atTop, bool atBottom);

	void UpdateGradientABGeneral(const float* p, const float* px,
		const float* py, const float* pxy, SubpixelInfo* outInfo);
	void UpdateGradientABCustomColor(const float* p, const float* px,
		const float* py, const float* pxy, SubpixelInfo* outInfo) const;

	void InitializeAveragingMatrixes();
	void AdoptMatrixes(const AveragingMatrix* fourMatrixes);
	LevelPoints* CalculateMatrixCentral(LevelPoints* startingPoints);
	LevelPoints* CalculateMatrixesXandY(LevelPoints* startingPoints);
	LevelPoints* CalculateMatrixXY(LevelPoints* startingPoints);
	void NormalizeMatrixes();
	void AddOffset(LevelPoints*, int dx, int dy);
	float CalculateCellIntegral(double centerX, double centerY) const;

	const float* SmoothUnchecked(SubpixelInfo*, const BYTE*);
	const float* SmoothWithBoundsCheck(SubpixelInfo*, const BYTE*);
	
};

#include <map>

////////////////////////////////////////////////////////////////////////
// Constants
const int kGeneralPurposeFormulaID = 1;
enum OccupyBits { 
	kOccupiedByOthers = 0, kOccupiedBySelf, 
	kAtLeft, kAtRight, kAtTop, kAtBottom
};

struct Offsets { short int dx, dy; int byteOffset; };
struct LevelPoints { float k; int pointCount; Offsets offsets[8]; };
struct AveragingMatrix { const LevelPoints *levelPoints; int n; };

//-----------------------------------------------------------------
const double sB1 = (M_PI - 5) / 12 + 3 * ::sqrt(3.0) / 16;
const double sC1 = (M_PI + 5) / 24 - 3 * ::sqrt(3.0) / 16;

const LevelPoints kPointsR1_0[3] = {
	{ float(5.0 / (3 * M_PI)), 1, {{0, 0, 0}}},
	{ float(2 * sB1 / M_PI), 4, {{0, 1, 0}, {0, -1, 0}, {1, 0, 0}, {-1, 0, 0}} },
	{ float(2 * sC1 / M_PI), 4, {{1, 1, 0}, {1, -1, 0}, {-1, 1, 0}, {-1, -1, 0}}}
};
const LevelPoints kPointsR1_0x[2] = {
	{ float((4 + 9 * ::sqrt(3.0) / M_PI) / 24), 2, {{0, 0, 0}, {1, 0, 0}} },
	{ float((8 - 9 * ::sqrt(3.0) / M_PI) / 48), 4, 
		{{0, -1, 0}, {0, 1, 0}, {1, -1, 0}, {1, 1, 0}} }
};
const LevelPoints kPointsR1_0y[2] = {
	{ float((4 + 9 * ::sqrt(3.0) / M_PI) / 24), 2, {{0, 0, 0}, {0, 1, 0}} },
	{ float((8 - 9 * ::sqrt(3.0) / M_PI) / 48), 4, 
		{{-1, 0, 0}, {1, 0, 0}, {-1, 1, 0}, {1, 1, 0}} }
};
const LevelPoints kPointsR1_0xy[1] = {
	{ 0.25f, 4, {{0, 0, 0}, {0, 1, 0}, {1, 0, 0}, {1, 1, 0}}}
};

//-----------------------------------------------------------
const double sCapR1_5Total = 81 * M_PI / 32;
const double sD0 = 81 * ::asin(1/3.0) / 32 + 43 * M_SQRT2 / 48;

const LevelPoints kPointsR1_5[3] = {
	{ float(25 / (12 * sCapR1_5Total)), 1, {{0, 0, 0}}},
	{ float((sD0 - 25.0 / 24) / sCapR1_5Total), 4, 
		{{0, 1, 0}, {1,0,0}, {-1, 0, 0}, {0, -1, 0}}},
	{ float((25.0 / 48 + 81 * M_PI / 128 - sD0) / sCapR1_5Total), 4,
		{{1, 1, 0}, {1,-1,0}, {-1, 1, 0}, {-1, -1, 0}}}
};

const double sD1 = 81 * ::asin(2/3.0) / 64 + 37 * ::sqrt(5.0) / 96;

const LevelPoints kPointsR1_5x[4] = {
	{ float(11.0 / (6 * sCapR1_5Total)), 2, {{0, 0, 0}, {1, 0, 0}} },
	{ float((sD1 - 11.0/12) / sCapR1_5Total), 4,
		{{0, 1, 0}, {0, -1, 0}, {1, 1, 0}, {1, -1, 0}} },
	{ float((sD0 - 11.0/6) / sCapR1_5Total), 2, {{-1, 0, 0}, {2, 0, 0}}},
	{ float((11.0/12 + 81 * M_PI / 128 - sD1 - 0.5 * sD0) / sCapR1_5Total), 4,
		{{-1, -1, 0}, {-1, 1, 0}, {2, -1, 0}, {2, 1, 0}} }
};
const LevelPoints kPointsR1_5y[4] = {
	{ float(11.0 / (6 * sCapR1_5Total)), 2, {{0, 0, 0}, {0, 1, 0}} },
	{ float((sD1 - 11.0/12) / sCapR1_5Total), 4,
		{{-1, 0, 0}, {1, 0, 0}, {-1, 1, 0}, {1, 1, 0}} },
	{ float((sD0 - 11.0/6) / sCapR1_5Total), 2, {{0, -1, 0}, {0, 2, 0}}},
	{ float((11.0/12 + 81 * M_PI / 128 - sD1 - 0.5 * sD0) / sCapR1_5Total), 4,
		{{-1, -1, 0}, {1, -1, 0}, {-1, 2, 0}, {1, 2, 0}} }
};
const LevelPoints kPointsR1_5xy[3] = {
	{ float(19 / (12 * sCapR1_5Total)), 4,
		{{0, 0, 0}, {0, 1, 0}, {1, 0, 0}, {1, 1, 0}} },
	{ float((sD1 - 19.0 / 12) / sCapR1_5Total), 8, 
		{{-1, 0, 0}, {2, 0, 0}, {0, -1, 0}, {1, -1, 0},
		 {-1, 1, 0}, {2, 1, 0}, {0, 2, 0}, {1, 2, 0}} },
	{ float((81 * M_PI / 128 + 19.0/12 - 2 * sD1)), 4,
		{{-1, -1, 0}, {-1, 2, 0}, {2, -1, 0}, {2, 2, 0}} }
};

//-----------------------------------------------------------
struct PrecalculatedMatrixInfo 
{ 
	float blurRadius; 
	AveragingMatrix info[4];
};
 
PrecalculatedMatrixInfo kPrecalculatedMatrixInfo[2] = {
	{ 1.0f, {{kPointsR1_0, 3}, {kPointsR1_0x, 2}, 
		{kPointsR1_0y, 2}, {kPointsR1_0xy, 1}}},
	{ 1.5f, {{kPointsR1_5, 3}, {kPointsR1_5x, 4}, 
		{kPointsR1_5y, 4}, {kPointsR1_5xy, 3}}}
};

///////////////////////////////////////////////////////////////////
const float* SubpixelInfoProvider::GetColorsAtGradientPoint(
	const SubpixelInfo* inReferencePtr)
{
	SubpixelInfo* p = const_cast<SubpixelInfo*>(inReferencePtr) + 1 + mTwiceWidth;
	if (!p->isColorsReady)
	{
		size_t index = inReferencePtr - mInfoBuffer;
		int twiceY = index / mTwiceWidth;
		int twiceX = index - twiceY * mTwiceWidth;

		int oddities = (twiceX & 1) + 2 * (twiceY & 1);
		mCurrentAveragingMatrix = mMatrixes[oddities];

		const BYTE* originalPtr = mOriginalData + (twiceX >> 1) * mOffsetX +
			(twiceY >> 1) * mOffsetY;

		if (twiceX >= mRadius2Int && twiceY >= mRadius2Int &&
			twiceX <= mMaxUncheckedX && twiceY <= mMaxUncheckedY)
		{
			SmoothUnchecked(p, originalPtr);
		}
		else
		{
			mMinDx = -(twiceX / 2);
			mMinDy = -(twiceY / 2);
			mMaxDx = (mWidth - (twiceX / 2) - 1);
			mMaxDy = (mHeight - (twiceY / 2) - 1);
			SmoothWithBoundsCheck(p, originalPtr);
		}
	}
	return p->colors;
}

inline SubpixelInfoProvider:: SubpixelInfoProvider( IBitmapPicture* inPicture,
	float inBlurRadius)
	: mOriginalData(inPicture->GetData()),
	mWidth(inPicture->GetRawWidth()), mHeight(inPicture->GetRawHeight()),
	mOffsetX(inPicture->GetOffsetX()), mOffsetY(inPicture->GetOffsetY()),
	mRadius(inBlurRadius), mRadiusSquared(mRadius * mRadius),
	mRadius2Int(int(::ceil(2 * mRadius))),
	mAllMatrixesElements(NULL)
{
	mTwiceWidth = 2 * mWidth;
	mTwiceHeight = 2 * mHeight;
	mWidth4 = 4 * mWidth;
	mMaxTwiceX = mTwiceWidth - 3;
	mMaxTwiceY = mTwiceHeight - 3; 
	mMaxUncheckedX = mMaxTwiceX - mRadius2Int;
	mMaxUncheckedY = mMaxTwiceY - mRadius2Int;

	mInfoBuffer = (SubpixelInfo*)::calloc(mTwiceWidth * mTwiceHeight, sizeof(SubpixelInfo));

    int i,j;
	for(i=0;i<mTwiceWidth;i++)
		for(j=0;j<mTwiceHeight;j++)
		{ mInfoBuffer[i + mTwiceWidth * j].dXX = i;
          mInfoBuffer[i + mTwiceWidth * j].dYY = j;}

	mGradientFormulaID = mLastCustomFormulaID = kGeneralPurposeFormulaID;
	
	mDebugFriendlyScale = mRadiusSquared / 10000.0f;

	InitializeAveragingMatrixes();
}


inline  SubpixelInfoProvider::~SubpixelInfoProvider()
{
	if (NULL != mInfoBuffer)
	{
		::free(mInfoBuffer);
	}
	for (int i = 0; i < 4; ++i)
	{
		delete mMatrixes[i];
		mMatrixes[i] = NULL;
	}
	if (NULL != mAllMatrixesElements)
	{
		::free(mAllMatrixesElements);
		mAllMatrixesElements = NULL;
	}
}

/////////////////////////////////////////////////////
const inline SubpixelInfo* SubpixelInfoProvider::GetInfo(int inTwiceX, int inTwiceY)
{
	if (inTwiceX < 0 || inTwiceY < 0 || 
		inTwiceX >= mMaxTwiceX || inTwiceY >= mMaxTwiceY)
	{
		return GetMarginInfo(inTwiceX < 0, inTwiceX >= mMaxTwiceX,
			inTwiceY < 0, inTwiceY >= mMaxTwiceY);
	}
	SubpixelInfo* infoPtr = &mInfoBuffer[inTwiceX + mTwiceWidth * inTwiceY];
	if (infoPtr->gradientFormulaID == mGradientFormulaID)
	{
		return infoPtr;
	}

	int oddities = (inTwiceX & 1) + 2 * (inTwiceY & 1);
	mCurrentAveragingMatrix = mMatrixes[oddities];

	const float *p, *px, *py, *pxy;

	const BYTE* originalPtr = mOriginalData + (inTwiceX >> 1) * mOffsetX +
		(inTwiceY >> 1) * mOffsetY;

	if (inTwiceX >= mRadius2Int && inTwiceY >= mRadius2Int &&
		inTwiceX <= mMaxUncheckedX && inTwiceY <= mMaxUncheckedY)
	{
		p = SmoothUnchecked(infoPtr, originalPtr);
		px = SmoothUnchecked(infoPtr + 2, originalPtr + mOffsetX);
		py = SmoothUnchecked(infoPtr + mWidth4, originalPtr + mOffsetY);
		pxy = SmoothUnchecked(infoPtr + 2 + mWidth4, 
			originalPtr + mOffsetX + mOffsetY);
	}
	else
	{
		mMinDx = -(inTwiceX / 2);
		mMinDy = -(inTwiceY / 2);
		mMaxDx = (mWidth - (inTwiceX / 2) - 1);
		mMaxDy = (mHeight - (inTwiceY / 2) - 1);
		p = SmoothWithBoundsCheck(infoPtr, originalPtr);
		--mMinDx; --mMaxDx;
		px = SmoothWithBoundsCheck(infoPtr + 2, originalPtr + mOffsetX);
		--mMinDy; --mMaxDy;
		pxy = SmoothWithBoundsCheck(infoPtr + 2 + mWidth4,
			originalPtr + mOffsetX + mOffsetY);
		++mMinDx; ++mMaxDx;
		py = SmoothWithBoundsCheck(infoPtr + mWidth4, originalPtr + mOffsetY);

	}

	if (kGeneralPurposeFormulaID == mGradientFormulaID)
	{
		UpdateGradientABGeneral(p, px, py, pxy, infoPtr);
	}
	else
	{
		UpdateGradientABCustomColor(p, px, py, pxy, infoPtr);
	}
	
	infoPtr->gradientFormulaID = mGradientFormulaID;
	return infoPtr;
}

void inline SubpixelInfoProvider::SetColorGradientFactors(const float* inThreeFactors)
{
	if (NULL == inThreeFactors)
	{
		mGradientFormulaID = kGeneralPurposeFormulaID;
	}
	else
	{
		int i = sizeof(mColorGradientFactors)/sizeof(mColorGradientFactors[0]);
		while (i--)
		{
			mColorGradientFactors[i] = inThreeFactors[i];
		}
		mGradientFormulaID = (++mLastCustomFormulaID);
	}
}

const inline SubpixelInfo* SubpixelInfoProvider::GetInfoBuffer() const
{
	return mInfoBuffer;
}

const inline SubpixelInfo* SubpixelInfoProvider::GetMarginInfo(bool inAtLeft, bool inAtRight,
		bool inAtTop, bool inAtBottom)
{
	static std::map<int, SubpixelInfo> sMap;

	int key = (inAtLeft << kAtLeft) + (inAtRight << kAtRight) +
		(inAtTop << kAtTop) + (inAtBottom << kAtBottom);
	
	SubpixelInfo& info = sMap[key];
	info.occupiedState = key;
	return &info;
}

///////////////////////////////////////////////////////////////////////////////////

void inline SubpixelInfoProvider::UpdateGradientABGeneral(const float* inPtr, const float* inPtrX,
		const float* inPtrY, const float* inPtrXY, SubpixelInfo* outInfo)
{
	float a = 0, b = 0;
	for (int i = 3; i--;)
	{
		float d1 = inPtr[i] - inPtrXY[i];
		float d2 = inPtrX[i] - inPtrY[i];
		a += 2 * d1 * d2;
		b += d2 * d2 - d1 * d1;
	}
	outInfo->a = a * mDebugFriendlyScale;
	outInfo->b = b * mDebugFriendlyScale;
}

void inline SubpixelInfoProvider::UpdateGradientABCustomColor(const float* inPtr,
	const float* inPtrX, const float* inPtrY, const float* inPtrXY,
	SubpixelInfo* outInfo) const
{
	float d1 = 0;
	float d2 = 0;
	for (int i = 3; i--;)
	{
		d1 += mColorGradientFactors[i] * (inPtr[i] - inPtrXY[i]);
		d2 += mColorGradientFactors[i] * (inPtrX[i] - inPtrY[i]);
	}
	outInfo->a = 2 * d1 * d2 * mDebugFriendlyScale;
	outInfo->b = (d2 * d2 - d1 * d1) * mDebugFriendlyScale;
}

///////////////////////////////////////////////////////////////////////////////
void inline SubpixelInfoProvider::InitializeAveragingMatrixes()
{
	int i;
	for ( i = 0; i < 4; ++i)
	{
		mMatrixes[i] = new AveragingMatrix;
		mMatrixes[i]->n = 0;
	}
	// load precalculated values if that will be found
	i = sizeof(kPrecalculatedMatrixInfo) / sizeof(kPrecalculatedMatrixInfo[0]);
	while (i--)
	{
		float triedBlueRadius = kPrecalculatedMatrixInfo[i].blurRadius;
		if (::fabs(triedBlueRadius - mRadius) < 0.01)
		{
			AdoptMatrixes(kPrecalculatedMatrixInfo[i].info);
			return;
		}
	}
	// calculate averaging matrixes using numerical integration
	int maxCountEstimate = int(4 * M_PI * (mRadius + 1) * (mRadius + 1));
	void* buffer = ::calloc(maxCountEstimate, sizeof(LevelPoints));
	mAllMatrixesElements = buffer;
	LevelPoints* currentLevelPoints = reinterpret_cast<LevelPoints*>(buffer);
	currentLevelPoints = CalculateMatrixCentral(currentLevelPoints);
	currentLevelPoints = CalculateMatrixesXandY(currentLevelPoints);
	currentLevelPoints = CalculateMatrixXY(currentLevelPoints);
	NormalizeMatrixes();
}

void inline SubpixelInfoProvider::AdoptMatrixes(const AveragingMatrix* inFourMatrixes)
{
	int i;
	int countOfLevelPointsStructures = 0;
	for ( i = 0; i < 4; ++i)
	{
		countOfLevelPointsStructures += inFourMatrixes[i].n;
	}
	void* buffer = ::calloc(countOfLevelPointsStructures, sizeof(LevelPoints));
	mAllMatrixesElements = buffer;

	// copy the precalculated information to buffer, then calculate byte offsets
	LevelPoints* currentLevelPoints = reinterpret_cast<LevelPoints*>(buffer);
	for ( i = 0; i < 4; ++i)
	{
		const LevelPoints* sourceLevelPoints = inFourMatrixes[i].levelPoints;
		int n = inFourMatrixes[i].n;
		mMatrixes[i]->n = n;
		mMatrixes[i]->levelPoints = currentLevelPoints;

		for (int j = 0; j < n; ++j, ++currentLevelPoints)
		{
			*currentLevelPoints = *sourceLevelPoints++;
			for (int pointIndex = currentLevelPoints->pointCount; pointIndex--;)
			{
				Offsets& offs = currentLevelPoints->offsets[pointIndex];
				offs.byteOffset = offs.dx * mOffsetX + offs.dy * mOffsetY;
			}
		}
	}
}

inline LevelPoints* SubpixelInfoProvider::CalculateMatrixCentral(LevelPoints* inStartingPoints)
{
	AveragingMatrix& matrix = *mMatrixes[0];
	LevelPoints* pt = inStartingPoints;
	pt->k = CalculateCellIntegral(0, 0);
	AddOffset(pt, 0, 0);
	matrix.levelPoints = pt++;
	matrix.n = 1;
	for (int dx = 1; dx < mRadius + 0.5; ++dx)
	{
		for(int dy = 0; dy <= dx; ++dy)
		{
			float k = CalculateCellIntegral(dx, dy);
			if (0 >= k)
			{
				continue;
			}
			pt->k = k;
			AddOffset(pt, dx, dy);
			AddOffset(pt, -dy, dx);
			AddOffset(pt, -dx, -dy);
			AddOffset(pt, dy, -dx);
			if (0 < dy && dy < dx)
			{
				AddOffset(pt, dy, dx);
				AddOffset(pt, -dx, dy);
				AddOffset(pt, -dy, -dx);
				AddOffset(pt, dx, -dy);
			}
			++pt;
			++matrix.n;
		}
	}
	return pt;
}

inline LevelPoints* SubpixelInfoProvider::CalculateMatrixesXandY(
	LevelPoints* inStartingPoints)
{
	LevelPoints* pt = inStartingPoints;
	AveragingMatrix& m1 = *mMatrixes[1];
	m1.levelPoints = pt;
	for (int dx = 0; dx < mRadius; ++dx)
	{
		for (int dy = 0; dy < mRadius + 0.5; ++dy)
		{
			float k = CalculateCellIntegral(dx + 0.5, dy);
			if (0 >= k)
			{
				continue;
			}
			pt->k = k;
			AddOffset(pt, dx + 1, dy);
			AddOffset(pt, -dx, dy);
			if (0 < dy)
			{
				AddOffset(pt, dx + 1, -dy);
				AddOffset(pt, -dx, -dy);
			}
			++pt;
			++m1.n;
		}
	}
	AveragingMatrix& m2 = *mMatrixes[2];
	int n = m1.n;
	m2.n = m1.n;
	m2.levelPoints = pt;
	for (int i = 0; i < n; ++i, ++pt)
	{
		const LevelPoints& prevPoints = pt[-n];
		pt->k = prevPoints.k;
		for (int j = 0; j < prevPoints.pointCount; ++j)
		{
			const Offsets& prevOffs = prevPoints.offsets[j];
			AddOffset(pt, prevOffs.dy, prevOffs.dx);
		}
	}
	return pt;
}

inline LevelPoints* SubpixelInfoProvider::CalculateMatrixXY(LevelPoints* inStartingPoints)
{
	LevelPoints* pt = inStartingPoints;
	AveragingMatrix& matrix = *mMatrixes[3];
	matrix.levelPoints = pt;
	for (int dx = 0; dx < mRadius; ++dx)
	{
		for (int dy = 0; dy <= dx; ++dy)
		{
			float k = CalculateCellIntegral(dx + 0.5, dy + 0.5);
			if (0 >= k)
			{
				continue;
			}
			pt->k = k;
			AddOffset(pt, dx + 1, dy + 1);
			AddOffset(pt, -dy, dx + 1);
			AddOffset(pt, -dx, -dy);
			AddOffset(pt, dy + 1, -dx);
			if (dy < dx)
			{
				AddOffset(pt, dx + 1, -dy);
				AddOffset(pt, dy + 1, dx + 1);
				AddOffset(pt, -dx, dy + 1);
				AddOffset(pt, -dy, -dx);
			}
			++pt;
			++matrix.n;
		}
	}
	return pt;
}
void inline SubpixelInfoProvider::NormalizeMatrixes()
{
	int i,j;
	for ( i = 0; i < 4; ++i)
	{
		AveragingMatrix& matrix = *mMatrixes[i];
		// calculate sum of k
		float sum = 0;
		for ( j = matrix.n; j--; )
		{
			const LevelPoints* pt = &matrix.levelPoints[j];
			sum += pt->k * pt->pointCount;
		}
		// normalize
		for ( j = matrix.n; j--;)
		{
			const LevelPoints* pt = &matrix.levelPoints[j];
			const_cast<float&>(pt->k) /= sum;
		}
	}
}
void inline SubpixelInfoProvider::AddOffset(LevelPoints* ioPoints, 
	int inDx, int inDy)
{
	Offsets& offs = ioPoints->offsets[(ioPoints->pointCount)++];
	offs.dx = inDx;
	offs.dy = inDy;
	offs.byteOffset = inDx * mOffsetX + inDy * mOffsetY;
}

float inline SubpixelInfoProvider::CalculateCellIntegral(double inCenterX, double inCenterY) const
{
	double xmax = inCenterX + 0.5;
	double ymax = inCenterY + 0.5;
	if (xmax * xmax + ymax * ymax <= mRadiusSquared)
	{
		return float(mRadiusSquared - inCenterX * inCenterX -
			inCenterY * inCenterY - 1.0 / 6);
	}
	// numerical integration
	double s = 0;
	const int kNumberOfIntervals = 16;
	double delta = 1.0 / kNumberOfIntervals;
	double xmin = inCenterX - 0.5;
	double ymin = inCenterY - 0.5;
	for (int i = 0; i <= kNumberOfIntervals; ++i)
	{
		int kx = (0 == i || i == kNumberOfIntervals ? 1 : 2);
		double x = xmin + i * delta; 
		for (int j = 0; j <= kNumberOfIntervals; ++j)
		{
			int k = (0 == j || j == kNumberOfIntervals ? kx : 2 * kx);
		
			double y = ymin + j * delta;
			double f = mRadiusSquared - x * x - y * y;
			if (0 < f)
			{
				s += k * f;
			}
		}
	}
	return float(s * 0.25 / (kNumberOfIntervals * kNumberOfIntervals));
}
///////////////////////////////////////////////////////////////////////////////
inline const float* SubpixelInfoProvider::SmoothUnchecked(SubpixelInfo* ioInfo, 
	const BYTE* inRawData)
{
	float* result = ioInfo->colors;
	if (!ioInfo->isColorsReady)
	{
		const LevelPoints* curPoint = mCurrentAveragingMatrix->levelPoints;
		float r, g, b;
		r = g = b = 0;
		for (int i = mCurrentAveragingMatrix->n; i--; ++curPoint)
		{
			int intR, intG, intB;
			intR = intG = intB = 0;
			const Offsets* offs = curPoint->offsets;
			const Offsets* offsetsEnd = offs + curPoint->pointCount;
			for (; offs < offsetsEnd; ++offs)
			{
				const BYTE *p = &inRawData[offs->byteOffset];
				intB += p[0];
				intG += p[1];
				intR += p[2];
			}
			r += intR * curPoint->k;
			g += intG * curPoint->k;
			b += intB * curPoint->k;
		}
		result[0] = b;
		result[1] = g;
		result[2] = r;
		ioInfo->isColorsReady = true;
	}
	return result;
}

inline const float* SubpixelInfoProvider::SmoothWithBoundsCheck(SubpixelInfo* ioInfo, 
	const BYTE* inRawData)
{
	float* result = ioInfo->colors;
	if (!ioInfo->isColorsReady)
	{
		const LevelPoints* curPoint = mCurrentAveragingMatrix->levelPoints;
		float r, g, b, Q;
		r = g = b = Q = 0;
		for (int i = mCurrentAveragingMatrix->n; i--; ++curPoint)
		{
			int intR, intG, intB, count;
			intR = intG = intB = count = 0;
			const Offsets* offs = curPoint->offsets;
			const Offsets* offsetsEnd = offs + curPoint->pointCount;
			for (; offs < offsetsEnd; ++offs)
			{
				if (mMinDx <= offs->dx && offs->dx <= mMaxDx &&
					mMinDy <= offs->dy && offs->dy <= mMaxDy)
				{
					const BYTE *p = &inRawData[offs->byteOffset];
					intB += p[0];
					intG += p[1];
					intR += p[2];
					++count;
				}
			}
			r += intR * curPoint->k;
			g += intG * curPoint->k;
			b += intB * curPoint->k;
			Q += count * curPoint->k;
		}
		if (Q > 0)
		{
			result[0] = b / Q;
			result[1] = g / Q;
			result[2] = r / Q;
		}
		ioInfo->isColorsReady = true;
	}
	return result;
}

#endif