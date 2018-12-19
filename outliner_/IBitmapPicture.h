#ifndef IBitmapPicture_Head_H
#define IBitmapPicture_Head_H


#include "c24bitmap.h"

class IBitmapPicture
{
public:
	 int   GetRawWidth()  ;
	 int   GetRawHeight()  ;
	 int   GetBytesInRow() ;
	 BYTE* GetData()  ;
	 int   GetOffsetX()  ;
	 int   GetOffsetY()  ;
     C24BitMap Pic;
};

int IBitmapPicture::GetRawWidth()
{
 return Pic.Width;
}

int IBitmapPicture::GetRawHeight()
{
 return Pic.Height;
}

int IBitmapPicture::GetBytesInRow()
{
 return Pic.LineWidth;
}

BYTE* IBitmapPicture::GetData()
{
	return Pic.Buffer;
}

int  IBitmapPicture::GetOffsetX()
{
	return 3;
}

int  IBitmapPicture::GetOffsetY()
{
	return  GetBytesInRow();
}

#endif
