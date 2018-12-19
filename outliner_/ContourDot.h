#ifndef ContourDot_Head_H
#define ContourDot_Head_H

#include <math.h>
#include "Calculator.h"

// Forward declaration 
//class Calculator;


class ContourDot
{
	float mCenterX, mCenterY; // the center of the contour dot

	// sizes of the outer and inner shapes
	float mOuterHalfsize, mMinXi, mMaxXi;

	// tangent vector in forward direction, the square sides are parallel to it
	float mVx, mVy;	

	float mA, mB, mM;
	float mApproximationPrecision, mBrightness, mLineWidth;

public:
	// constructor for the first dot in a new stroke
	ContourDot(int x, int y);

	bool PlaceAtInitialPosition(Calculator&);
	bool PlaceAtNeighborPosition(Calculator&, bool moveForward);
	void Finalize(Calculator&);
			
	float GetX() const;
	float GetY() const;
	float GetVx() const;
	float GetVy() const;
	float GetOuterHalfsize() const;
	float GetApproximationPrecision() const;
	float GetBrightness() const;
	float GetLineWidth() const;
	
	float GetMinXi() const;
	float GetMaxXi() const;
    short int mSideColorsComponents[6];

private:
	bool FitCenterline(Calculator&);
	float ApproachCenterline(Calculator&, bool& outSuccess);
	bool CalculateTangentVector();
	
	// changes position of center in the direction transversal to line of contour.
	void MoveByXi(float xi);
};

////////////////////////////////////////////////
// Constants
const double kPrecision = 0.01;
const double kApproximationPrecision = 0.8;


//////////////////////////////////////////////

ContourDot::ContourDot(int inX, int inY)
	: mCenterX(float(inX)), mCenterY(float(inY)), 
	mVx(0.0f), mVy(1.0f)
{
}
bool ContourDot::PlaceAtInitialPosition(Calculator& ioCalculator)
{
	bool result = false;
	const SubpixelInfo* p = ioCalculator.GetInfo(int(mCenterX), int(mCenterY));
	mA = p->a;
	mB = p->b;
	if (mA >= 0 && mB < 0)
	{
		mVx = -1; mVy = 0;
	}
	CalculateTangentVector();
	ioCalculator.SetInitialDefaultBounds();
	if (ioCalculator.UpdateCache(mCenterX, mCenterY, mVx, mVy, true) &&
		ioCalculator.CalculateF(mA, mB))
	{
		ioCalculator.AdjustRectExceptFront();
		if (!ioCalculator.ApproximateByParabola(true) ||
			kApproximationPrecision < ioCalculator.GetLastApproximationEpsilon())
		{
			ioCalculator.SummarizeAB(mA, mB);
			CalculateTangentVector();
			ioCalculator.SetInitialDefaultBounds();
			if (!ioCalculator.UpdateCache(mCenterX, mCenterY, mVx, mVy) ||
				!ioCalculator.CalculateF(mA, mB))
			{
				return false;
			}
			ioCalculator.AdjustRectExceptFront();
			if (!ioCalculator.ApproximateByParabola(true))
			{
				return false;
			}
		}
		if (mApproximationPrecision < kApproximationPrecision)
		{
			ioCalculator.SummarizeAB(mA, mB);
			MoveByXi(ioCalculator.GetDeltaXi());
			CalculateTangentVector();

			// one more iteration
			mOuterHalfsize = ioCalculator.GetDotHalfsize();
			if (0 >= mOuterHalfsize)
			{
				return false;
			}
			ioCalculator.SetInitialBounds(mOuterHalfsize);
			if (ioCalculator.UpdateCache(mCenterX, mCenterY, mVx, mVy) &&
				ioCalculator.CalculateF(mA, mB))
			{
				ioCalculator.AdjustRectExceptFront();
				ioCalculator.ApproximateByParabola(false);
				ioCalculator.SummarizeAB(mA, mB);
				MoveByXi(ioCalculator.GetDeltaXi());
				CalculateTangentVector();
				return FitCenterline(ioCalculator);
			}
		}
	}
	return false;
}

bool ContourDot::PlaceAtNeighborPosition(Calculator& ioCalculator, 
	bool inMoveForward)
{
	float deltaEta = inMoveForward ? 2 * mOuterHalfsize : -2 * mOuterHalfsize;
	mCenterX += mVx * deltaEta;
	mCenterY += mVy * deltaEta;
	ioCalculator.SetInitialBounds(mOuterHalfsize);
	if (!ioCalculator.UpdateCache(mCenterX, mCenterY, mVx, mVy) ||
		!ioCalculator.CalculateF(mA, mB))
	{
		return false;
	}
	if (!ioCalculator.CheckAlignmentOfMaxPixel(-deltaEta))
	{
		// ?
		// probably jump between parallel nearby contour lines
		if (!ioCalculator.FindAlternativeXiBounds())
		{
			return false;
		}
	}
	if (inMoveForward)
	{
		ioCalculator.AdjustRectExceptBack();
	}
	else
	{
		ioCalculator.AdjustRectExceptFront();
	}
	if (!ioCalculator.ApproximateByParabola(true))
	{
		return false;
	}
	if (kApproximationPrecision < ioCalculator.GetLastApproximationEpsilon())
	{
		// rotate vector
		ioCalculator.SummarizeAB(mA, mB);
		CalculateTangentVector();
		ioCalculator.SetInitialBounds(mOuterHalfsize);
		if (!ioCalculator.UpdateCache(mCenterX, mCenterY, mVx, mVy) ||
			!ioCalculator.CalculateF(mA, mB))
		{
			return false;
		}
		if (inMoveForward)
		{
			ioCalculator.AdjustRectExceptBack();
		}
		else
		{
			ioCalculator.AdjustRectExceptFront();
		}
		if (!ioCalculator.ApproximateByParabola(true) ||
			kApproximationPrecision < ioCalculator.GetLastApproximationEpsilon())
		{
			return false;
		}
	}
	ioCalculator.SummarizeAB(mA, mB);
	float newHalfsize = ioCalculator.GetDotHalfsize();
	if (0 >= newHalfsize)
	{
		return false;
	}
	MoveByXi(ioCalculator.GetDeltaXi());

	// the size of dot have changed, we should accomodate eta-position
	deltaEta = inMoveForward ? 
		newHalfsize - mOuterHalfsize : mOuterHalfsize - newHalfsize;

	mCenterX += deltaEta * mVx;
	mCenterY += deltaEta * mVy;
	mOuterHalfsize = newHalfsize;
	CalculateTangentVector();

	ioCalculator.SetInitialBounds(mOuterHalfsize);
	if (!ioCalculator.UpdateCache(mCenterX, mCenterY, mVx, mVy) ||
		!ioCalculator.CalculateF(mA, mB))
	{
		return false;
	}
	if (inMoveForward)
	{
		ioCalculator.AdjustRectExceptBack();
	}
	else
	{
		ioCalculator.AdjustRectExceptFront();
	}
	ioCalculator.ApproximateByParabola(false);
	MoveByXi(ioCalculator.GetDeltaXi());
	ioCalculator.SummarizeAB(mA, mB);
	CalculateTangentVector();

	return FitCenterline(ioCalculator);
}

///////////////////////////////////////////////////////////////////

bool ContourDot::FitCenterline(Calculator& ioCalculator)
{
	bool result = false;

	const float kWanderLimit = 0.5f * mOuterHalfsize;
	float wander = 0;

	const int kLimitAgainstEternalLoop = 10;
	float moveDistance = 0;
	float previousMoveDistance = 0;

	for (int i = kLimitAgainstEternalLoop; i--;)
	{
		bool success;

		previousMoveDistance = moveDistance;
		moveDistance = ApproachCenterline(ioCalculator, success);
		wander += moveDistance;
		if (!success || ::fabs(wander) > kWanderLimit)
		{
			return false;
		}
	
		if (::fabs(moveDistance) < kPrecision)
		{
			ioCalculator.GetXiBounds(mMinXi, mMaxXi);	
			result = true;
			break;
		}
	}

	// in the case of etenal loop the dot position oscilates, and 
	// we have approximately: moveDistance = -previousMoveDistance
	if (!result &&
		::fabs(moveDistance + previousMoveDistance) < kPrecision)
	{
		float halfMove = 0.5f * moveDistance;
		MoveByXi(-halfMove);
		ioCalculator.GetXiBounds(mMinXi, mMaxXi);
		mMinXi += halfMove;
		mMaxXi += halfMove;
		result = true;
	}

	if (result)
	{
		// get the most accurate statistics
		ioCalculator.ApproximateByParabola(true);
		mApproximationPrecision = ioCalculator.GetLastApproximationEpsilon();
		mLineWidth = ioCalculator.GetLineWidth();
		float xiShift = 0.5f * (mVx - mVy);	
	    ioCalculator.SummarizeSideColors(xiShift, mSideColorsComponents);
	}

	return result;
}



float ContourDot::ApproachCenterline(Calculator& ioCalculator, bool& outSuccess)
{
	float deltaXi = 0;
	bool success = false;

	ioCalculator.ExtendXiBounds();
	if (ioCalculator.UpdateCache(mCenterX, mCenterY, mVx, mVy) && 
		ioCalculator.CalculateF(mA, mB))
	{
		ioCalculator.AdjustXiBounds();
		if (ioCalculator.ApproximateByParabola(false))
		{
			deltaXi = ioCalculator.GetDeltaXi();
			ioCalculator.SummarizeAB(mA, mB);
			MoveByXi(deltaXi);
			CalculateTangentVector();
			success = true;
		}
	}
	outSuccess = success;
	return deltaXi;
}

//////////////////////////////////////////////////////////////////
float ContourDot::GetX() const			{ return mCenterX; }
float ContourDot::GetY() const			{ return mCenterY; }
float ContourDot::GetVx() const		{ return mVx; }
float ContourDot::GetVy() const		{ return mVy; }
float ContourDot::GetOuterHalfsize() const { return mOuterHalfsize; }
float ContourDot::GetApproximationPrecision() const	{ return mApproximationPrecision; }
float ContourDot::GetBrightness() const {return mBrightness;}
float ContourDot::GetLineWidth() const { return mLineWidth; }
float ContourDot::GetMinXi() const { return mMinXi; }
float ContourDot::GetMaxXi() const { return mMaxXi; }

//////////////////////////////////////////////////////////////////
void ContourDot::Finalize(Calculator&)
{
	// the last chance to extract any useful information from gradient
	mBrightness = ::sqrt(mM);
	// ....

	// adjust coordinates - the gradient is shifted by the half pixel 
	// against the original bitmap
	mCenterX += 0.5;
	mCenterY += 0.5;
}


bool ContourDot::CalculateTangentVector()
{
	mM = ::sqrt(mA * mA + mB * mB);

	if (mM == 0)
	{
		return false;
	}
	if (mA >= 0)
	{
		mVx = mVx >= 0 ? ::sqrt(0.5f * (mM + mA) / mM) : 
			-::sqrt(0.5f * (mM + mA) / mM);
		mVy = mVx * mB / (mM + mA);
	}
	else
	{
		mVy = mVy >= 0 ? ::sqrt(0.5f * (mM - mA) / mM) :
			-::sqrt(0.5f * (mM - mA) / mM);
		mVx = mVy * mB / (mM - mA);
	}
	return true;
}


////////////////////////////////////////////////////////////////////////////////////
// new functions
void ContourDot::MoveByXi(float inXi)
{
	mCenterX -= inXi * mVy;
	mCenterY += inXi * mVx;
}

#endif