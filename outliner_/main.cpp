#include <stdio.h>
#include <stdlib.h>
#include "SubpixelInfoProvider.h"
#include "Calculator.h"
#include "ContourDot.h"
#include "ThresholdingIterator.h"
#include "Stroke.h"

/*	*/
C24BitMap StrokePic;


int main(int argc,char*argv[])
{

IBitmapPicture Pic;
Pic.Pic.Load(argv[1]);
StrokePic.FormatF(Pic.Pic.Width*2,Pic.Pic.Height*2);
StrokePic.CleanPic();


SubpixelInfoProvider SubPix(&Pic,1.5);
Calculator ioCalculator(&Pic,1.5);
 
int inX, inY;

ThresholdingIterator iterator(&Pic,1.5, 0.1, 0.5, 
							  10);

Calculator calculator(&Pic, 1.5);
calculator.SetMaximumDotSize(1.5 * 4);
iterator.SetSubpixelInfo(calculator.GetInfoBuffer());
 
/*mVectorSketch = new VectorSketch(float(mWidth), float(mHeight));
for (size_t key = 0; key < mSavedVectorParameters.size(); ++key)
{
	mVectorSketch->SetParameter(key, mSavedVectorParameters[key]);
}
DWORD nextShowTime = ::GetTickCount() + kUpdateIntervalInMilliseconds;*/

int x, y, n = 0, y0=0;
int i;
Stroke  newStroke;
vector<Stroke> mStrokes;


while (iterator.Next(x, y))
{
	if (y != y0)
	{
		y0 = y;	 
	}
    inX = x ;
	inY = y ;
	if (newStroke.Initialize(inX, inY, ioCalculator))
	{
		// append the next trial stroke

        StrokePic.RandPenColor();
	    double RR,GG,BB;
        RR = StrokePic.PenColor.R;
		GG = StrokePic.PenColor.G;
		BB = StrokePic.PenColor.B;

        for(i=0;i<ioCalculator.mBooker.SubInfoVec.size();i++)
		{  int dotX;
		   int dotY;

		   //StrokePic.PenColor.B = ioCalculator.mBooker.SubInfoVec[i]->colors[0];
		   //StrokePic.PenColor.G = ioCalculator.mBooker.SubInfoVec[i]->colors[1];
		   //StrokePic.PenColor.R = ioCalculator.mBooker.SubInfoVec[i]->colors[2];
		   StrokePic.PenColor.R = RR ;//*i/ioCalculator.mBooker.SubInfoVec.size();
		   StrokePic.PenColor.G = GG ;//*i/ioCalculator.mBooker.SubInfoVec.size();
		   StrokePic.PenColor.B = BB ;//*i/ioCalculator.mBooker.SubInfoVec.size();

		   dotX = ioCalculator.mBooker.SubInfoVec[i]->dXX;
		   dotY = StrokePic.Height - ioCalculator.mBooker.SubInfoVec[i]->dYY;
           StrokePic.SigDot(dotX,dotY);
		}

		mStrokes.push_back(newStroke);
		ioCalculator.UpdateBooking(kOccupiedByOthersMask);
	}
	else
	{
		ioCalculator.UpdateBooking(kOccupiedBySpeck);
	}	 
}
 
StrokePic.Save("stroke.bmp");	 

return 1;
}
