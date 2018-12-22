#include <stdio.h>
#include <stdlib.h>
#include "SubpixelInfoProvider.h"
#include "Calculator.h"
#include "ContourDot.h"
#include "ThresholdingIterator.h"
#include "Stroke.h"

/*	*/
C24BitMap StrokePic;

double robo_ymin = -0.03; 
double robo_ymax = 0.145;
double robo_xmin = 0.586; 
double robo_xmax = 0.855;

double zmin = 0.837;
double zmax = 0.86;

//double 
void TransformAxis( double &xx, double &yy, int imgw, int imgh )
{
   double robox, roboy;
   
   robox = xx;
   roboy = yy;
   
   double  xscale, yscale;
  
   xscale = (robo_xmax - robo_xmin) / imgw;
   yscale = (robo_ymax - robo_ymin) / imgh;
   
   robox = xx * xscale + robo_xmin;
   roboy = yy * yscale + robo_ymin;   
   
   xx = robox;
   yy = roboy;
  
  /*double temp;
  temp = xx;
  xx = yy; 
  yy = temp;
  double  xscale, yscale;
  xscale = (xmax - xmin) / imgh;
  yscale = (ymax - ymin) / imgw;
  xx *= xscale; 
  yy *= yscale;
  xx += ymin;
  yy += ymin;*/
}

void PintAxis( double &xx, double &yy, int imgw, int imgh , double zz)
{
   double robox, roboy;
   
   robox = xx;
   roboy = yy;
   
   double  xscale, yscale;
  
   xscale = (robo_xmax - robo_xmin) / imgw;
   yscale = (robo_ymax - robo_ymin) / imgh;
   
   robox = xx * xscale + robo_xmin;
   roboy = yy * yscale + robo_ymin;   
   
   xx = robox;
   yy = roboy;
  
   printf(" %.4lf, %.4lf , %.4lf, 0.0, 1.0, 0.0, -1\n" , xx, yy, zz);
}

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
        
	    double dotX;
		double dotY;
		
		dotX = ioCalculator.mBooker.SubInfoVec[0]->dXX;
		dotY = StrokePic.Height - ioCalculator.mBooker.SubInfoVec[0]->dYY;
		
		StrokePic.MoveTo(dotX, dotY);
		PintAxis( dotX, dotY, StrokePic.Width, StrokePic.Height , zmax);
		PintAxis( dotX, dotY, StrokePic.Width, StrokePic.Height , zmin);
		
        for(i=0;i<ioCalculator.mBooker.SubInfoVec.size();i+=700)
		{  

		   //StrokePic.PenColor.B = ioCalculator.mBooker.SubInfoVec[i]->colors[0];
		   //StrokePic.PenColor.G = ioCalculator.mBooker.SubInfoVec[i]->colors[1];
		   //StrokePic.PenColor.R = ioCalculator.mBooker.SubInfoVec[i]->colors[2];

		   dotX = ioCalculator.mBooker.SubInfoVec[i]->dXX;
		   dotY = StrokePic.Height - ioCalculator.mBooker.SubInfoVec[i]->dYY;
		   
		/*if(0)// StrokePic.LineToDist(dotX, dotY) > 300)
		{  StrokePic.MoveTo(dotX, dotY);
	       PintAxis( StrokePic.tempX, StrokePic.tempY, 
		                StrokePic.Width, StrokePic.Height , zmax);
	       PintAxis( dotX, dotY, 
		                StrokePic.Width, StrokePic.Height , zmax);
		}
		else*/
		{  StrokePic.LineTo(dotX, dotY);
	       PintAxis( dotX, dotY, StrokePic.Width, StrokePic.Height , zmin);
		}
           //StrokePic.SigDot(dotX,dotY);
		}
        
		dotX = ioCalculator.mBooker.SubInfoVec[ioCalculator.mBooker.SubInfoVec.size()-1]->dXX;
		dotY = StrokePic.Height - 
		ioCalculator.mBooker.SubInfoVec[ioCalculator.mBooker.SubInfoVec.size()-1]->dYY;
		
		/*if( StrokePic.LineToDist(dotX, dotY) >30)
		{  
	       StrokePic.MoveTo(dotX, dotY);
		   PintAxis( StrokePic.tempX, StrokePic.tempY, 
		                StrokePic.Width, StrokePic.Height , zmax);
	       PintAxis( dotX, dotY, StrokePic.Width, StrokePic.Height , zmax);
		}
		else*/
		{ 
           StrokePic.LineTo(dotX, dotY);
	       PintAxis( dotX, dotY, StrokePic.Width, StrokePic.Height , zmin);
		   PintAxis( dotX, dotY, StrokePic.Width, StrokePic.Height , zmax);
		   //PintAxis( StrokePic.tempX, StrokePic.tempY, 
		   //             StrokePic.Width, StrokePic.Height , zmax);
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
