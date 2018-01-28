#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <dirent.h>
#include <sys/stat.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

struct mem_bitmap_info {
	PBITMAP *pbmps;
	int count;
};

static void addFrame(mMemAnimateFrames* self, PBITMAP pBmp)
{
	mMemAnimateFrame *frame, *current = NULL;

	current = self->cur_frame;	
	frame = (mMemAnimateFrame*) calloc(1, sizeof(mMemAnimateFrame));
	frame->next = NULL;

	frame->pbmp = pBmp;	

	if(self->frames == NULL)
	{
		self->frames = frame;
		current = frame;
		current->prev = NULL;
	}
	else
	{
		frame->prev = current;
		current->next = frame;
		current = current->next;
	}	
	self->cur_frame = current;	
	self->nr_frames++;
}

static void maxFrameSize(mMemAnimateFrames* self)
{
	mMemAnimateFrame *frame;
	int max_width = 0, max_height = 0;

	frame = self->frames;
	
	max_width = frame->pbmp->bmWidth;
	max_height = frame->pbmp->bmHeight;

	frame = frame->next;
	while(frame != NULL)
	{
		if(frame->pbmp->bmWidth > max_width)
			max_width = frame->pbmp->bmWidth;
		if(frame->pbmp->bmHeight > max_height)
			max_height = frame->pbmp->bmHeight;

		frame = frame->next;
	}

	self->max_width = max_width;
	self->max_height = max_height;
}

static void mMemAnimateFrames_construct(mMemAnimateFrames *self, DWORD addData)
{
	int i;
	struct mem_bitmap_info *pbmi = (struct mem_bitmap_info*)addData;

	Class(mObject).construct((mObject*)self, 0);

	for (i = 0; i < pbmi->count; i ++)
	{
		addFrame(self, pbmi->pbmps[i]);
	}
	
	if (self->frames == NULL)
	{
		fprintf(stderr, "no pic\n");
		return;
	}

	self->cur_frame = self->frames;
	maxFrameSize(self);
}			

BOOL mMemAnimateFrames_getMaxFrameSize(mMemAnimateFrames* self, int *pwidth, int *pheight)
{
	*pwidth = self->max_width;
	*pheight = self->max_height;
	
	return TRUE;
}

static void mMemAnimateFrames_destroy (mMemAnimateFrames* self)
{
	mMemAnimateFrame *tmp, *frame;
	
	frame = self->frames;
	while(frame) {
		tmp = frame->next;
		free(frame);
		frame = tmp;
	}
	Class(mAnimateFrames).destroy((mAnimateFrames*)self);
}

/*  
mMemAnimateFrames* ncsCreateAnimateFramesFromMEM(PBITMAP * bmps, int count)
{
	mMemAnimateFrames* maf;
	struct mem_bitmap_info mbi;

	if(!bmps && count <= 0)
		return NULL;

	mbi.pbmps = bmps;
	mbi.count = count;
	if(count <= 0)
	{
		int i = 0;
		PBITMAP* pbmp = bmps;
		while(pbmp[i]) i++;
		mbi.count = i;
	}
	
	maf = NEWEX(mMemAnimateFrames, (DWORD)&mbi);
	
	return maf;
}*/

int mMemAnimateFrames_drawFrame(mMemAnimateFrames* self, HDC hdc, BOOL bScaled, RECT *pRect, BOOL bScale)
{
	if(self->frames == NULL)
		return NCSR_ANIMATEFRAME_FAILED;

	if(self->cur_frame == NULL)
	{
		self->cur_frame = self->frames;
	}

	if(!self->cur_frame)
		return NCSR_ANIMATEFRAME_FAILED;
	
	if(bScale)
	{	
		FillBoxWithBitmap(hdc, pRect->left, pRect->top, pRect->right - pRect->left, pRect->bottom - pRect->top, (((mMemAnimateFrame*)(self->cur_frame))->pbmp));
	} 
	else
	{
		FillBoxWithBitmap(hdc, pRect->left, pRect->top, ((mMemAnimateFrame*)(self->cur_frame))->pbmp->bmWidth, ((mMemAnimateFrame*)(self->cur_frame))->pbmp->bmHeight, (((mMemAnimateFrame*)(self->cur_frame))->pbmp));
	}
	
	return NCSR_ANIMATEFRAME_OK;

}

static int mMemAnimateFrames_nextFrame(mMemAnimateFrames* self)
{
	if(!self->cur_frame)
	{
		self->cur_frame = self->frames;
		return NCSR_ANIMATEFRAME_OK;
	}
	
	self->cur_frame = ((mMemAnimateFrame*)(self->cur_frame))->next;

	return self->cur_frame ? NCSR_ANIMATEFRAME_OK : NCSR_ANIMATEFRAME_LASTFRAME;
}
	
BEGIN_MINI_CLASS(mMemAnimateFrames, mAnimateFrames)
	CLASS_METHOD_MAP(mMemAnimateFrames, construct)
	CLASS_METHOD_MAP(mMemAnimateFrames, destroy)
	CLASS_METHOD_MAP(mMemAnimateFrames, getMaxFrameSize)
	CLASS_METHOD_MAP(mMemAnimateFrames, drawFrame)
	CLASS_METHOD_MAP(mMemAnimateFrames, nextFrame)
END_MINI_CLASS
