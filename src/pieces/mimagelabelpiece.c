
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

#include "mhotpiece.h"
#include "mlayoutpiece.h"
#include "mpairpiece.h"
#include "mimagelabelpiece.h"

#include "mstaticpiece.h"
#include "mlabelpiece.h"
#include "mimagepiece.h"

static void mImageLabelPiece_construct(mImageLabelPiece* self, DWORD add_data)
{
	mHotPiece** pieces = (mHotPiece**) add_data;
	Class(mPairPiece).construct((mPairPiece*)self, add_data);

	//create
	if(pieces)
	{
		self->first = pieces[0];
		self->second = pieces[1];
	}
	else
	{
		self->first = (mHotPiece*)NEWPIECE(mImagePiece);
		self->second = (mHotPiece*)NEWPIECE(mLabelPiece);
	}

	//set attribute
	mPairPiece_setFirstSizeType(self, NCS_PAIRPIECE_ST_PERCENT);
	mPairPiece_setFirstSize(self, 25);
	mPairPiece_setMargin(self, 5);
	mPairPiece_setSpace(self, 2);
}

static BOOL mImageLabelPiece_setProperty(mImageLabelPiece * self, int id, DWORD value)
{
	mHotPiece* label=NULL, *img = NULL;
	if((id >= PIECECOMM_PROP_ALIGN && id <= PIECECOMM_PROP_AUTOWRAP)
		|| (id >= LABELPIECE_PROP_BEGIN && id <= LABELPIECE_PROP_END))
	{
		if(mPairPiece_isVert(self))
		{
			if(id == PIECECOMM_PROP_VALIGN)
			{
				Class(mPairPiece).setProperty((mPairPiece*)self, NCSP_PAIRPIECE_SECOND_AS_FIRST,value == NCS_VALIGN_BOTTOM);
			}
		}
		else
		{
			if(id == PIECECOMM_PROP_ALIGN)
			{
				Class(mPairPiece).setProperty((mPairPiece*)self, NCSP_PAIRPIECE_SECOND_AS_FIRST,value == NCS_ALIGN_RIGHT);
			}
		}
		//get second: label
		label = self->second;
		if(label)
			return _c(label)->setProperty(label, id, value);

		return FALSE;
	}
	else if(id == PIECECOMM_PROP_DRAWMOD 
		|| (id >= IMAGEPIECE_PROP_BEGIN && id <= IMAGEPIECE_PROP_END))
	{
		img = self->first;
		if(img)
			return _c(img)->setProperty(img, id,value);
		return FALSE;
	}
	else if(id == NCSP_PAIRPIECE_DIRECTION)
	{
		int size;
		if((mPairPiece_isVert(self) && value)  
			|| (!mPairPiece_isVert(self) && !value))
			return FALSE;
		
		size = mPairPiece_getFirstSize(self);

		if(value) //vert
		{
			mPairPiece_setFirstSize(self, (100-size));
		}
		else
		{
			mPairPiece_setFirstSize(self, (100-size));
		}
		//don't return, continu call its super: mPairPiece
	}
/*	else if(id == NCSP_IMAGELABELPIECE_IMAGE_SIZETYPE)
	{
		mPairPiece_setFirstSizeType(self, value);
		return TRUE;
	}
	else if(id == NCSP_IMAGELABELPIECE_IMAGE_SIZE)
	{
		mPairPiece_setFirstSize(self, (int)value);
		return TRUE;
	}
*/


	return Class(mPairPiece).setProperty((mPairPiece*)self, id, value);
}

static DWORD mImageLabelPiece_getProperty(mImageLabelPiece * self, int id)
{
	mHotPiece* label=NULL, *img = NULL;
	if((id >= PIECECOMM_PROP_ALIGN && id <= PIECECOMM_PROP_AUTOWRAP)
		|| (id >= LABELPIECE_PROP_BEGIN && id <= LABELPIECE_PROP_END))
	{
		//get second: label
		label = mPairPiece_getSecond(self);
		if(label)
			return _c(label)->getProperty(label, id);
		return (DWORD)-1;
	}
	else if(id == PIECECOMM_PROP_DRAWMOD 
		|| (id >= IMAGEPIECE_PROP_BEGIN && id <= IMAGEPIECE_PROP_END))
	{
		img = mPairPiece_getFirst(self);
		if(img)
			return _c(img)->getProperty(img, id);
		return (DWORD)-1;
	}


	return Class(mPairPiece).getProperty((mPairPiece*)self, id);
}



BEGIN_MINI_CLASS(mImageLabelPiece, mPairPiece)
	CLASS_METHOD_MAP(mImageLabelPiece, construct  )
	CLASS_METHOD_MAP(mImageLabelPiece, getProperty )
	CLASS_METHOD_MAP(mImageLabelPiece, setProperty )
END_MINI_CLASS


