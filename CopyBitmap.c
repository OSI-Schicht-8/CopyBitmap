#include <proto/exec.h>
#include <proto/graphics.h>
#include <hardware/custom.h>
#include <stdio.h>

#define custom ((struct Custom *) (0xdff000))

struct GfxBase *GfxBase;

void CopyBitmap(UWORD *imgsrc, UWORD *imgdest, int depth, int width, int height);

int main()
{
	int width = 1; /* image width in words */
	int height = 3; /* image height in rows */
	int depth = 3; /* number of bitplanes */

	UWORD *imgsrc = AllocMem(depth*2*width*height, MEMF_CHIP|MEMF_CLEAR); /* allocate memory for source image */
	UWORD *imgdest = AllocMem(depth*2*width*height, MEMF_CHIP|MEMF_CLEAR); /* allocate memory for target image */

	GfxBase = (struct GfxBase *)OpenLibrary ("graphics.library", 0L);
	
	/* example image data */
	
	/* bitplane 0 */
	imgsrc[0] = 0x8001; /* row 0 */
	imgsrc[1] = 0x0000; /* row 1 */
	imgsrc[2] = 0x0000; /* row 2 */
	
	/* bitplane 1 */
	imgsrc[3] = 0x0000; /* row 0 */
	imgsrc[4] = 0x4002; /* row 1 */
	imgsrc[5] = 0x0000; /* row 2 */
	
	/* bitplane 2 */
	imgsrc[6] = 0x0000; /* row 0 */
	imgsrc[7] = 0x0000; /* row 1 */
	imgsrc[8] = 0x2004; /* row 2 */
	
	CopyBitmap(imgsrc, imgdest, depth, width, height); /* copy image from imgsrc to imgdest */
	
	printf("bitplane 0\nrow 0: %x\nrow 1: %x\nrow 2: %x\n\n",imgdest[0],imgdest[1],imgdest[2]); /* print copied image data */
	printf("bitplane 1\nrow 0: %x\nrow 1: %x\nrow 2: %x\n\n",imgdest[3],imgdest[4],imgdest[5]);
	printf("bitplane 2\nrow 0: %x\nrow 1: %x\nrow 2: %x\n",imgdest[6],imgdest[7],imgdest[8]);
	
	CloseLibrary ((struct Library *) GfxBase);
}

void CopyBitmap(UWORD *imgsrc, UWORD *imgdest, int depth, int width, int height)
{
	int planesize;
	int blittersize;
	
	OwnBlitter(); /* take control over blitter */
	WaitBlit();
	
	custom->bltcon0 = 0x09f0; /* use source A and destination D (0x09); minterm 0xf0: A = D */
	custom->bltcon1 = 0x0000; /* no shift */
	custom->bltafwm = 0xffff; /* full first word mask */
	custom->bltalwm = 0xffff; /* full last word mask */
	custom->bltamod = 0x0000; /* no modulus for source A */
	custom->bltdmod = 0x0000; /* no modulus for destination D */
	
	planesize = width * height; /* size of one bitplane in words */
	
	height = height & 0x03ff; /* only for special case height = 1024 */
	width = width & 0x003f; /* only for special case width = 64 */
	
	blittersize = height<<6; /* shift height to bits 15 - 6 */
	blittersize = blittersize + width; /* width in bits 5 - 0 */
	do
	{
		custom->bltapt = imgsrc; /* source A points to source image bitplane */
		custom->bltdpt = imgdest; /* destination D points to target image bitplane */
		
		custom->bltsize = blittersize; /* start blitter operation */
		WaitBlit(); /* wait until blitter has finished operation */
		
		depth = depth - 1; /* loop counter */
		imgsrc = imgsrc + planesize; /* set source image pointer to next bitplane address */
		imgdest = imgdest + planesize; /* set target image pointer to next bitplane address */
				
	} while (depth); /* loop through all source image bitplanes */
	
	DisownBlitter(); /* free blitter */
}