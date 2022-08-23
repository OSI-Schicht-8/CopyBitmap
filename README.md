# CopyBitmap
Copies a whole bitmap with the Blitter

## source code
copybitmap.c can be compiled with vbcc for OS1.3
```shell
vc +kick13 copybitmap.c -o copybitmap
```

## parameters
The CopyBitmap() function uses the blitter to copy a bitmap. The following parameters are needed:
+ `UWORD *imgsrc` - a pointer to the source image
+ `UWORD *imgdest` - a pointer to the destination image
+ `int depth` - the number of bitplanes of the source image
+ `int width` - the width (in words) of the source image
+ `int height` - the height of the source images (number or rows)

## description of the routine
`OwnBlitter()` takes exclusive access to the hardware blitter.

`WaitBlit()` waits until the blitter is ready.

`BLTCON0` register contains the shift distance for source A (bits 15-12), sets the DMA channels (A-D) to be enabled (bits 11-8), sets the minterm (bits 7-0).
The minterm to be used is A = D (0xf0), where A points to the source image and B points to the destination image D.

`BLTCON1` register contains the shift distance for a possible source B (bits 15-12) and some control bits for the blitter operation mode.

`BLTAFWM` and `BLTALWM` registers contain a mask for the first and last word of each line to be copied.

`BLTAMOD` and `BLTDMOD` registers contain the modulo for blitter source A and destination D. The number in this register is added to the address at the end of each line.

`BLTAPT` and `BLTDPT` registers contain the pointers to the source image data and the destination image data.

`BLTSIZE` register contains the height and width of the blitter area to be processed. IF `BLTSIZE` register is set, the blitter starts its operation.

The function loops through all bitplanes of the source image. The target image is updated each loop until it is completed after the last loop.

`DisownBlitter()` release exclusive access to the hardware blitter.
