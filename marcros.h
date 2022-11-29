#ifndef MACROS_H
#define MACROS_H

//IMAGING
#define  FULL_HD_IMAGE_WIDTH 1920
#define  FULL_HD_IMAGE_HEIGHT 1080

//DISTANCES
//convert um-1 to mm constant
#define C_MM_TO_UMx0p1 10000;

//LEICA STEREO MIC IMAGE SIZES BY MAGNIFICATION
//um-1
#define  IMG_W_M5p5x 27426 //100 nm   1pxl 14,28 *0.1um
#define  IMG_H_M5p5x 15421            //

#define IMG_W_M5X 30162
#define IMG_H_M5X 16959

#define IMG_W_M4X 37703
#define IMG_H_M4X 21199

#define IMG_W_M3X 50270
#define IMG_H_M3X 28266

#define IMG_W_M2X 75405
#define IMG_H_M2X 42398

#define IMG_W_M1X 150819
#define IMG_H_M1X 84797

#define IMG_W_M0p61X 247469
#define IMG_H_M0p61X 139145

//STAGE

#define STAGE_CENTER_X 751431
#define STAGE_CENTER_Y 501665

#define STAGE_FIRST_WELL_LEFT_X 0
#define STAGE_FIRST_WELL_TOP_Y 0


//PLATE DISTANCES

#define DIA_96_WELLPLATE 90000 //um-1
#define DX_96_WELLPLATE 0
#define DY_96_WELLPLATE 0

#define DIA_384_WELLPLATE 45000 //um-1 test value

// Target PLATE

//96 well plate
#define STAGE_FIRST_T_WELL_LEFT_X 877396
#define STAGE_FIRST_T_WELL_TOP_Y 1820

#define STAGE_FIRST_T_WELL_LEFT_X 877396
#define STAGE_FIRST_T_WELL_TOP_Y 1820

// Akos HCS plate

#define STAGE_FIRST_3DHCS_T_WELL_C_X 905946
#define STAGE_FIRST_3DHCS_T_WELL_C_Y 249424

#define STAGE_HCS_D_Y 109000

// petri 3mm dist
#define STAGE_FIRST_PETRI_C_X 905946
#define STAGE_FIRST_PETRI_C_Y 250000





#endif // MACROS_H

