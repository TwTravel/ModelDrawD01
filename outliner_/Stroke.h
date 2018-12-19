#ifndef Stroke_Head_H
#define Stroke_Head_H

#include "ContourDot.h"
#include "Calculator.h"
#include <list>

using namespace std;

// Forward declarations
class Calculator;
 
class Stroke
{
	typedef std::list<ContourDot> DotCollection;
	typedef std::list<ContourDot>::iterator DotIterator;
	//typedef std::list<ContourDot>::const_iterator ConstDotIterator;

	DotCollection mDots;
	bool mIsClosed;
	float mFrontEta, mBackEta;

	// support of mouse tracking and drawing
	DotIterator mNearestDot;
	RECT mBounds, mBoundsExtended;

	// support of serialization
	float mAverageLineWidth;
	float mAverageBrightness;
	 

public:
	Stroke();
	Stroke(const Stroke&);
	~Stroke();
	//----------------
	bool Initialize(int x, int y, Calculator&); 
	bool IsValid() const;
	bool IsClosed() const;
	void Finalize(Calculator&);


	bool FindNearestDot(float x, float y, float areaEnlargement, bool& outSelectedDotChanged);
	void UnselectNearestDot();

	const RECT& GetBounds(bool extended) const;
	bool IsIntersecting(const RECT&, bool useExtendedBounds) const;

	// getting path information
	float AverageLineWidth() const;
	float AverageBrightness() const;
	 

private:
	void InitializePath();
	  
};


 
Stroke::Stroke()
	: mDots(), mIsClosed(false), mFrontEta(0), mBackEta(0),
	mNearestDot(mDots.end()),
	mAverageLineWidth(0), mAverageBrightness(0) 
{
}
	
Stroke::Stroke(const Stroke& inOtherStroke)
	: mDots(inOtherStroke.mDots), mIsClosed(inOtherStroke.mIsClosed),
	mFrontEta(inOtherStroke.mFrontEta), mBackEta(inOtherStroke.mBackEta),
	mNearestDot(mDots.end()),
	mAverageLineWidth(inOtherStroke.mAverageLineWidth), 
	mAverageBrightness(inOtherStroke.mAverageBrightness) 
{
	 
	/*mBounds = inOtherStroke.mBounds;
	mBoundsExtended = inOtherStroke.mBoundsExtended;

	const DotCollection& otherDots = inOtherStroke.mDots;
	if (inOtherStroke.mNearestDot != otherDots.end())
	{
		mNearestDot = mDots.begin();
		std::advance(mNearestDot, 
			std::distance(otherDots.begin(), inOtherStroke.mNearestDot));
	}*/
}

Stroke::~Stroke()
{
	 
}

bool Stroke::Initialize(int inX, int inY, Calculator& ioCalculator) 
{
	mDots.clear();
	ContourDot dot(inX, inY);
	if (!dot.PlaceAtInitialPosition(ioCalculator))
	{
		return false;
	}
	// mark pixels of the just created contour dot.
	ioCalculator.MarkOccupied();
	mFrontEta = ioCalculator.GetMinEta();
	mBackEta = ioCalculator.GetMaxEta();
	mDots.push_back(dot);

	bool isStrokeExtensibleBackward = ioCalculator.IsMiddlePoint();
	mIsClosed = false;

	while (!mIsClosed)
	{
		ContourDot dot(mDots.back());
		if (fabs(dot.GetX() - 170.5) < 0.8 && ::fabs(dot.GetY() - 183.5) < 0.8)
		{
			mIsClosed = false;
		}

		if (!dot.PlaceAtNeighborPosition(ioCalculator, true))
		{
			// the dot chain can not be extended
			break;
		}
		mIsClosed = ioCalculator.IsForwardOverlapping();
		ioCalculator.MarkOccupied();
		mBackEta = ioCalculator.GetMaxEta();
		mDots.push_back(dot);	
	}

	if (!mIsClosed && isStrokeExtensibleBackward)
	{
		for (;;)
		{
			ContourDot dot(mDots.front());
			if (fabs(dot.GetX() - 174.8) < 0.8 && ::fabs(dot.GetY() - 187.0) < 0.8)
			{
				mIsClosed = false;
			}
			if (!dot.PlaceAtNeighborPosition(ioCalculator, false))
			{
				break;
			}
			ioCalculator.MarkOccupied();
			mFrontEta = ioCalculator.GetMinEta();
			mDots.push_front(dot);
		}
	}
	mNearestDot = mDots.end();

	return (1 < mDots.size());
}

bool Stroke::IsClosed() const
{
	return mIsClosed;
}

void Stroke::Finalize(Calculator& ioCalculator)
{
	RECT r, re;
	float lineWidthTotal = 0;
	float brightnessTotal = 0;
	int dotCount = 0;

	for (DotIterator it = mDots.begin(); it != mDots.end(); ++it)
	{
		it->Finalize(ioCalculator);

		lineWidthTotal += it->GetLineWidth();
		brightnessTotal += it->GetBrightness();
		++dotCount;

		// calculate stroke bounds
		LONG xCenter = LONG(it->GetX());
		LONG yCenter = LONG(it->GetY());

		double delta = it->GetOuterHalfsize() * (::fabs(it->GetVx()) + ::fabs(it->GetVy()));
		LONG left = LONG(::floor(it->GetX() - delta));
		LONG right = LONG(::ceil(it->GetX() + delta));
		LONG top = LONG(::floor(it->GetY() - delta));
		LONG bottom = LONG(::ceil(it->GetY() + delta));

		if (mDots.begin() == it)
		{
			r.left = r.right = xCenter;
			r.top = r.bottom = yCenter;
			re.left = left;
			re.right = right;
			re.top = top;
			re.bottom = bottom;
		}
		else
		{
			r.left    = MIN_(r.left, xCenter);
			r.right   = MAX_(r.right, xCenter);
			r.top     = MIN_(r.top, yCenter);
			r.bottom  = MAX_(r.bottom, yCenter);

			re.left   = MIN_(re.left, left);
			re.right  = MAX_(re.right, right);
			re.top    = MIN_(re.top, top);
			re.bottom = MAX_(re.bottom, bottom);
		}
	}
	//::InflateRect(&r, 2, 2);
	//::InflateRect(&re, 2, 2);
	mBounds = r;
	mBoundsExtended = re;

	mAverageLineWidth = lineWidthTotal / dotCount;
	mAverageBrightness = brightnessTotal / dotCount;
	InitializePath();
}

 


bool Stroke::FindNearestDot(float inX, float inY, float inAreaEnlargement,
	bool& outSelectedDotChanged)
{
	DotIterator foundDot = mDots.end();
	for (DotIterator it = mDots.begin(); it != mDots.end(); ++it)
	{
		float dx = inX - it->GetX();
		float dy = inY - it->GetY();
		float eta = dx * it->GetVx() + dy * it->GetVy();
		float xi = -dx * it->GetVy() + dy * it->GetVx();

		if (::fabs(eta) < it->GetOuterHalfsize() + inAreaEnlargement && 
			it->GetMinXi() - inAreaEnlargement < xi && 
			xi < it->GetMaxXi() + inAreaEnlargement)
		{
			foundDot = it;
			break;
		}
	}

	if (foundDot != mNearestDot)
	{
		mNearestDot = foundDot;
		outSelectedDotChanged = true;
	}

	return mDots.end() != mNearestDot;
}

void Stroke::UnselectNearestDot()
{
	mNearestDot = mDots.end();
}

const RECT& Stroke::GetBounds(bool inExtended) const
{
	return (inExtended ? mBoundsExtended : mBounds);
}

bool Stroke::IsIntersecting(const RECT& inRect, bool inUseExtendedBounds) const
{
	const RECT& bounds = GetBounds(inUseExtendedBounds);
	return bounds.left < inRect.right || bounds.right > inRect.left ||
		bounds.top < inRect.bottom || bounds.bottom > inRect.top;
}

float Stroke::AverageLineWidth() const
{
	return mAverageLineWidth;
}

float Stroke::AverageBrightness() const
{
	return mAverageBrightness;
}



//////////////////////////////////////////////////////////
void Stroke::InitializePath()
{
 
	if (mFrontEta < 0 && !IsClosed())
	{
		const ContourDot& dot = mDots.front();
		float x = dot.GetX();
		float y = dot.GetY();
		 
	}
	DotIterator it = mDots.begin();
	DotIterator prevIt = it++;
	while (mDots.end() != it)
	{
		//AddCurveBetweenDots(*mPath, *prevIt, *it);
		prevIt = it++;
	}

	if (IsClosed())
	{
	//	AddCurveBetweenDots(*mPath, mDots.back(), mDots.front());
		 
	}
	else if (0 < mBackEta)
	{
		const ContourDot& dot = mDots.back();
		float x = dot.GetX();
		float y = dot.GetY();
		 
	}
}


 
#endif
