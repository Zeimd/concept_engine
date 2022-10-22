/*****************************************************************************
*
* texture-manager.cpp
*
* By Jari Korkala 3/11/2015
*
*****************************************************************************/

#include <IL/il.h>
#include <IL/ilu.h>
#include "texture-manager.h"
#include "env-map.h"

using namespace CEngine;

// Used when diffuse texture is missing
const Ceng::UINT8 missingDiffuseBitmap[][3] = 
{
	{200,0,0},{ 40,40,40 },{ 40,40,40 },{ 40,40,40 },{ 40,40,40 },{ 40,40,40 },{ 40,40,40 },{ 200,0,0 },
	{ 40,40,40 },{ 200,0,0 },{ 40,40,40 },{ 40,40,40 },{ 40,40,40 },{ 40,40,40 },{ 200,0,0 },{ 40,40,40 },
	{ 40,40,40 },{ 40,40,40 },{ 200,0,0 },{ 40,40,40 },{ 40,40,40 },{ 200,0,0 },{ 40,40,40 },{ 40,40,40 },
	{ 40,40,40 },{ 40,40,40 },{ 40,40,40 },{ 200,0,0 },{ 200,0,0 },{ 40,40,40 },{ 40,40,40 },{ 40,40,40 },
	{ 40,40,40 },{ 40,40,40 },{ 40,40,40 },{ 200,0,0 },{ 200,0,0 },{ 40,40,40 },{ 40,40,40 },{ 40,40,40 },
	{ 40,40,40 },{ 40,40,40 },{ 200,0,0 },{ 40,40,40 },{ 40,40,40 },{ 200,0,0 },{ 40,40,40 },{ 40,40,40 },
	{ 40,40,40 },{ 200,0,0 },{ 40,40,40 },{ 40,40,40 },{ 40,40,40 },{ 40,40,40 },{ 200,0,0 },{ 40,40,40 },
	{ 200,0,0 },{ 40,40,40 },{ 40,40,40 },{ 40,40,40 },{ 40,40,40 },{ 40,40,40 },{ 40,40,40 },{ 200,0,0 },
};

// Used when material data is missing
const Ceng::UINT8 missingMaterialDiffuse[][3] =
{
	{ 200,0,200 },{ 40,40,40 },{ 40,40,40 },{ 40,40,40 },{ 40,40,40 },{ 40,40,40 },{ 40,40,40 },{ 200,0,200 },
	{ 40,40,40 },{ 200,0,200 },{ 40,40,40 },{ 40,40,40 },{ 40,40,40 },{ 40,40,40 },{ 200,0,200 },{ 40,40,40 },
	{ 40,40,40 },{ 40,40,40 },{ 200,0,200 },{ 40,40,40 },{ 40,40,40 },{ 200,0,200 },{ 40,40,40 },{ 40,40,40 },
	{ 40,40,40 },{ 40,40,40 },{ 40,40,40 },{ 200,0,200 },{ 200,0,200 },{ 40,40,40 },{ 40,40,40 },{ 40,40,40 },
	{ 40,40,40 },{ 40,40,40 },{ 40,40,40 },{ 200,0,200 },{ 200,0,200 },{ 40,40,40 },{ 40,40,40 },{ 40,40,40 },
	{ 40,40,40 },{ 40,40,40 },{ 200,0,200 },{ 40,40,40 },{ 40,40,40 },{ 200,0,200 },{ 40,40,40 },{ 40,40,40 },
	{ 40,40,40 },{ 200,0,200 },{ 40,40,40 },{ 40,40,40 },{ 40,40,40 },{ 40,40,40 },{ 200,0,200 },{ 40,40,40 },
	{ 200,0,200 },{ 40,40,40 },{ 40,40,40 },{ 40,40,40 },{ 40,40,40 },{ 40,40,40 },{ 40,40,40 },{ 200,0,200 },
};

// Normal map for x-shape
const Ceng::UINT8 missingNormalBitmap[][3] =
{
	{ 127,127,254 },{ 127,127,254 },{ 215,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,39,153 },{ 127,127,254 },{ 127,127,254 },
	{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },
	{ 39,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,215,153 },
	{ 127,127,254 },{ 39,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,215,153 },{ 127,127,254 },
	{ 127,127,254 },{ 127,127,254 },{ 39,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,215,153 },{ 127,127,254 },{ 127,127,254 },
	{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },
	{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },
	{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },
	{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },
	{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },
	{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },
	{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },
	{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,39,153 },{ 127,127,254 },{ 127,127,254 },{ 39,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },
	{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,39,153 },{ 39,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },
	{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },
	{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },
	{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },
	{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },
	{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,215,153 },{ 39,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },
	{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,215,153 },{ 127,127,254 },{ 127,127,254 },{ 39,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },
	{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },
	{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },
	{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },
	{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },
	{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },
	{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },
	{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },
	{ 127,127,254 },{ 127,127,254 },{ 39,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,39,153 },{ 127,127,254 },{ 127,127,254 },
	{ 127,127,254 },{ 39,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,39,153 },{ 127,127,254 },
	{ 39,39,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,39,153 },
	{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 215,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },
	{ 127,127,254 },{ 127,127,254 },{ 215,215,153 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 127,127,254 },{ 39,215,153 },{ 127,127,254 },{ 127,127,254 },
};

const Ceng::UINT8 missingMaterialBitmap[][3] =
{
	{ 1,0,1 },{ 0,0,0 },{ 0,0,0 },{ 0,0,0 },{ 0,0,0 },{ 0,0,0 },{ 0,0,0 },{ 1,0,1 },
	{ 0,0,0 },{ 1,0,1 },{ 0,0,0 },{ 0,0,0 },{ 0,0,0 },{ 0,0,0 },{ 1,0,1 },{ 0,0,0 },
	{ 0,0,0 },{ 0,0,0 },{ 1,0,1 },{ 0,0,0 },{ 0,0,0 },{ 1,0,1 },{ 0,0,0 },{ 0,0,0 },
	{ 0,0,0 },{ 0,0,0 },{ 0,0,0 },{ 1,0,1 },{ 1,0,1 },{ 0,0,0 },{ 0,0,0 },{ 0,0,0 },
	{ 0,0,0 },{ 0,0,0 },{ 0,0,0 },{ 1,0,1 },{ 1,0,1 },{ 0,0,0 },{ 0,0,0 },{ 0,0,0 },
	{ 0,0,0 },{ 0,0,0 },{ 1,0,1 },{ 0,0,0 },{ 0,0,0 },{ 1,0,1 },{ 0,0,0 },{ 0,0,0 },
	{ 0,0,0 },{ 1,0,1 },{ 0,0,0 },{ 0,0,0 },{ 0,0,0 },{ 0,0,0 },{ 1,0,1 },{ 0,0,0 },
	{ 1,0,1 },{ 0,0,0 },{ 0,0,0 },{ 0,0,0 },{ 0,0,0 },{ 0,0,0 },{ 0,0,0 },{ 1,0,1 },
};

TextureManager::TextureManager() 
{
}

TextureManager::TextureManager(Ceng::RenderDevice *renderDevice)
	: renderDevice(renderDevice)
{
	Ceng::Texture2dDesc desc;

	// Configure descriptor

	desc.width = 8;
	desc.height = 8;
	desc.format = Ceng::IMAGE_FORMAT::C24_BGR;
	desc.sRGB = true;
	desc.mipLevels = 0;
	desc.arraySize = 0;
	desc.usage = Ceng::BufferUsage::gpu_read_only;
	desc.bindFlags = Ceng::BufferBinding::shader_resource;
	desc.cpuAccessFlags = 0;
	desc.multisampleDesc.count = 0;
	desc.multisampleDesc.quality = 0;

	desc.optionFlags = 0;

	Ceng::SubResourceData image;

	image.sourcePtr = &missingDiffuseBitmap[0][0];
	image.rowPitch = 3*8;
	image.depthPitch = 3*8*8;

	Ceng::Texture2D *texture;

	Ceng::CRESULT cresult = renderDevice->CreateTexture2D(desc, &image, &texture);

	if (cresult == Ceng::CE_OK)
	{
		Texture2D *temp = new Texture2D();

		if (temp != nullptr)
		{
			temp->name = "missing diffuse texture";
			temp->sRGB = true;
			temp->type = TextureType::d2;
			temp->texture = texture;

			missingDiffuse2D = std::shared_ptr<Texture>(temp);
		}
	}

	image.sourcePtr = &missingMaterialDiffuse[0][0];
	image.rowPitch = 3 * 8;
	image.depthPitch = 3 * 8 * 8;

	cresult = renderDevice->CreateTexture2D(desc, &image, &texture);

	if (cresult == Ceng::CE_OK)
	{
		Texture2D *temp = new Texture2D();

		if (temp != nullptr)
		{
			temp->name = "missing material data";
			temp->sRGB = true;
			temp->type = TextureType::d2;
			temp->texture = texture;

			missingMaterialData2D = std::shared_ptr<Texture>(temp);
			textureList.push_back(missingMaterialData2D);
		}
	}

	desc.width = 32;
	desc.height = 32;
	desc.sRGB = false;

	image.sourcePtr = &missingNormalBitmap[0][0];
	image.rowPitch = 3 * 32;
	image.depthPitch = 3 * 32 * 32;

	cresult = renderDevice->CreateTexture2D(desc, &image, &texture);

	if (cresult == Ceng::CE_OK)
	{
		Texture2D *temp = new Texture2D();

		if (temp != nullptr)
		{
			temp->name = "missing normalmap texture";
			temp->sRGB = false;
			temp->type = TextureType::d2;
			temp->texture = texture;

			missingNormal2D = std::shared_ptr<Texture>(temp);
		}
	}
}

TextureManager::~TextureManager()
{
}

const EngineResult::value TextureManager::PeekAndValidateFile(const json &filename, const TextureType::value type,
	const Ceng::BOOL sRGB, Ceng::IMAGE_FORMAT::value &out_format)
{
	return EngineResult::ok;
}

const std::shared_ptr<Texture> TextureManager::FindTexture(const Ceng::StringUtf8 &filename,
	TextureType::value type, const Ceng::BOOL sRGB)
{
	auto iter = textureList.begin();

	while (iter != textureList.end())
	{
		if ((*iter)->name == filename && (*iter)->type == type)
		{
			return (*iter);
		}

		++iter;
	}

	return nullptr;
}

const EngineResult::value TextureManager::LoadTexture2D(const Ceng::StringUtf8 &filename,
	const TextureOptions &options, std::shared_ptr<Texture> &output)
{
	auto find = FindTexture(filename, TextureType::d2, options.sRGB);

	if (find != nullptr)
	{
		output = find;
		return EngineResult::ok;
	}

	Ceng::StringUtf8 finalFile, title,extension;

	GetFileInfo(filename, finalFile, title,extension);

	//Ceng::Log::Print("Texture loader test:");

	//Ceng::Log::Print(filename);

	//Ceng::Log::Print(extension);

	Ceng::Texture2D *texture;

	EngineResult::value eresult = EngineResult::fail;

	if (extension == "ktx")
	{
		eresult = LoadTexture2D_KTX(finalFile, options, &texture);
	}
	else if (extension == "dds")
	{
		eresult = LoadTexture2D_DDS(finalFile, options, &texture);
	}
	else if (extension == "exr")
	{
		eresult = LoadTexture2D_EXR(finalFile, options, &texture);
	}
	else if (extension == "png")
	{
		eresult = LoadTexture2D_PNG(finalFile, options, &texture);
	}
	else if (extension == "bmp")
	{
		eresult = LoadTexture2D_BMP(finalFile, options, &texture);
	}

	if (eresult != EngineResult::ok)
	{
		//output = missingNormal2D;
		output = missingDiffuse2D;
		return EngineResult::ok;
	}

	Texture2D *texHandle;

	try
	{
		texHandle = new Texture2D();
	}
	catch (std::bad_alloc&)
	{
		texture->Release();
		return EngineResult::out_of_memory;
	}

	texHandle->name = filename;
	texHandle->sRGB = options.sRGB;
	texHandle->type = TextureType::d2;
	texHandle->texture = texture;

	std::shared_ptr<Texture> texPtr;

	try
	{
		texPtr = std::shared_ptr<Texture>(texHandle);
	}
	catch (std::bad_alloc&)
	{
		texture->Release();
		return EngineResult::out_of_memory;
	}

	try
	{
		textureList.push_back(texPtr);
	}
	catch (std::bad_alloc&)
	{
		return EngineResult::out_of_memory;
	}

	output = texPtr;

	return EngineResult::ok;
}

const EngineResult::value TextureManager::LoadCubemap(const Ceng::StringUtf8 &filename,
	const TextureOptions &options, std::shared_ptr<Texture> &output, std::shared_ptr<Texture>& out_irradiance)
{
	auto find = FindTexture(filename, TextureType::cubemap, options.sRGB);

	if (find != nullptr)
	{
		output = find;
		return EngineResult::ok;
	}

	auto iter = filename.FindLast('.', filename.ConstBeginIterator());

	Ceng::StringUtf8 name = filename.SubString(filename.ConstBeginIterator(), iter);

	++iter;

	Ceng::StringUtf8 extension = filename.SubString(iter, filename.ConstEndIterator());

	Ceng::StringUtf8 finalName = assetPath + filename;

	//Ceng::Log::Print(extension);

	Ceng::Cubemap *rawCubemap = nullptr;

	EngineResult::value eresult;

	if (extension == "ktx")
	{
		//eresult = LoadTexture2D_KTX(finalName, sRGB, &texture);
	}
	else if (extension == "dds")
	{
		//eresult = LoadTexture2D_DDS(finalName, sRGB, &texture);
	}
	else
	{
		eresult = LoadCubemapFromSequence(finalName, extension, options, &rawCubemap);
	}

	if (eresult != EngineResult::ok)
	{
		output = nullptr;
		out_irradiance = nullptr;
		return EngineResult::fail;
	}

	std::shared_ptr<TextureCube> cubeHandle = std::make_shared<TextureCube>();

	cubeHandle->name = filename;
	cubeHandle->sRGB = options.sRGB;
	cubeHandle->type = TextureType::cubemap;
	cubeHandle->texture = rawCubemap;
	cubeHandle->irradianceParent = nullptr;
	cubeHandle->relatedIrradiance = nullptr;

	Ceng::Cubemap* rawIrradianceMap = nullptr;

	std::shared_ptr<TextureCube> irradianceHandle = nullptr;

	if (options.generateIrradianceMap)
	{
		name = filename + "+?irradiance";

		eresult = GenerateIrradianceMapFromCube(name, options, rawCubemap, &rawIrradianceMap);

		if (eresult != EngineResult::ok)
		{
			output = nullptr;
			out_irradiance = nullptr;
			return EngineResult::fail;
		}

		irradianceHandle = std::make_shared<TextureCube>();

		irradianceHandle->name = name;
		irradianceHandle->sRGB = options.sRGB;
		irradianceHandle->type = TextureType::cubemap;
		irradianceHandle->texture = rawIrradianceMap;
		irradianceHandle->irradianceParent = cubeHandle.get();
		irradianceHandle->relatedIrradiance = nullptr;

		cubeHandle->relatedIrradiance = irradianceHandle.get();
	}

	textureList.push_back(cubeHandle);

	if (options.generateIrradianceMap)
	{
		textureList.push_back(irradianceHandle);
	}

	//texHandle->relatedIrradiance = ;
	/*
	TextureCube* cubeHandle;

	try
	{
		cubeHandle = new TextureCube();
	}
	catch (std::bad_alloc&)
	{
		if (rawIrradianceMap != nullptr)
		{
			rawIrradianceMap->Release();
		}

		rawCubemap->Release();
		return EngineResult::out_of_memory;
	}

	if 

	texHandle->name = filename;
	texHandle->sRGB = options.sRGB;
	texHandle->type = TextureType::cubemap;
	texHandle->texture = rawCubemap;
	
	//texHandle->relatedIrradiance = ;

	std::shared_ptr<Texture> texPtr;

	try
	{
		texPtr = std::shared_ptr<Texture>(texHandle);
	}
	catch (std::bad_alloc&)
	{
		if (rawIrradianceMap != nullptr)
		{
			rawIrradianceMap->Release();
		}

		rawCubemap->Release();
		return EngineResult::out_of_memory;
	}

	try
	{
		textureList.push_back(texPtr);
	}
	catch (std::bad_alloc&)
	{
		return EngineResult::out_of_memory;
	}
	*/

	output = cubeHandle;
	out_irradiance = irradianceHandle;

	return EngineResult::ok;
}

const EngineResult::value TextureManager::LoadCubemapFromSequence(const Ceng::StringUtf8 &filename,
	const Ceng::StringUtf8 &extension,
	const TextureOptions &options, Ceng::Cubemap **out_texture)
{
	auto bitmaps = std::vector<Bitmap>(6);

	auto iter = filename.FindLast('.', filename.ConstBeginIterator());

	Ceng::StringUtf8 name = filename.SubString(filename.ConstBeginIterator(), iter);

	const EngineResult::value(TextureManager::* loadFunction)(const Ceng::StringUtf8&,
		const TextureOptions &options, Bitmap&);

	Ceng::BOOL forceLinear = false;

	if (extension == "bmp")
	{
		loadFunction = &TextureManager::LoadBitmap_BMP;
	}
	else if (extension == "exr")
	{
		loadFunction = &TextureManager::LoadBitmap_EXR;
		forceLinear = true;
	}

	EngineResult::value eresult;

	for (Ceng::UINT32 k = 0; k < 6; ++k)
	{
		Ceng::StringUtf8 chainName = name + '_' + k + '.' + extension;

		eresult = (this->*loadFunction)(chainName, options, bitmaps[k]);
		if (eresult != EngineResult::ok)
		{
			return eresult;
		}

		if (bitmaps[k].width != bitmaps[0].width ||
			bitmaps[k].height != bitmaps[0].height ||
			bitmaps[k].format != bitmaps[0].format)
		{
			return EngineResult::texture_type_mismatch;
		}
	}

	Ceng::Texture2dDesc desc;

	// Configure descriptor

	desc.width = bitmaps[0].width;
	desc.height = bitmaps[0].height;
	desc.format = bitmaps[0].format;

	desc.sRGB = options.sRGB;
	if (forceLinear)
	{
		desc.sRGB = false;
	}
	
	desc.mipLevels = 0;
	desc.arraySize = 0;
	desc.bindFlags = options.bindFlags;
	desc.cpuAccessFlags = options.cpuAccessFlags;
	desc.multisampleDesc.count = 0;
	desc.multisampleDesc.quality = 0;
	desc.usage = options.usage;

	desc.optionFlags = options.options;

	Ceng::SubResourceData image[6];

	for (Ceng::UINT32 k = 0; k < 6; ++k)
	{
		image[k].sourcePtr = &bitmaps[k].imageBuffer[0];
		image[k].rowPitch = bitmaps[0].pitch;
		image[k].depthPitch = bitmaps[0].imageSize;
	}

	Ceng::CRESULT cresult = renderDevice->CreateCubemap(desc, image, out_texture);

	if (cresult != Ceng::CE_OK)
	{
		return EngineResult::fail;
	}


	return EngineResult::ok;
}

const EngineResult::value TextureManager::GenerateIrradianceMapFromCube(const Ceng::StringUtf8& name, 
	const TextureOptions& options, Ceng::Cubemap* cubemap, Ceng::Cubemap** output)
{
	*output = nullptr;

	//Ceng::Log::Print("TextureManager::GenerateIrradianceMapFromCube\n");

	Ceng::Texture2dDesc diffuseEnvDesc;

	diffuseEnvDesc.width = options.irradianceSize;
	diffuseEnvDesc.height = options.irradianceSize;

	diffuseEnvDesc.format = Ceng::IMAGE_FORMAT::CF16_ABGR;

	diffuseEnvDesc.mipLevels = 0;
	diffuseEnvDesc.arraySize = 1;

	diffuseEnvDesc.bindFlags = Ceng::BufferBinding::shader_resource;
	diffuseEnvDesc.sRGB = false;
	diffuseEnvDesc.usage = Ceng::BufferUsage::gpu_read_only;
	diffuseEnvDesc.cpuAccessFlags = Ceng::Buffer_CPU_Access::read | Ceng::Buffer_CPU_Access::write;

	diffuseEnvDesc.optionFlags = 0;
	diffuseEnvDesc.multisampleDesc.count = 0;
	diffuseEnvDesc.multisampleDesc.quality = 0;

	Ceng::CRESULT cresult;

	Ceng::Cubemap* probeIrradiance;

	cresult = renderDevice->CreateCubemap(diffuseEnvDesc, nullptr, &probeIrradiance);
	if (cresult != Ceng::CE_OK)
	{
		return EngineResult::fail;
	}

	EngineResult::value eresult;

	eresult = CEngine::CreateIrradianceMap(cubemap, probeIrradiance);
	if (eresult != CEngine::EngineResult::ok)
	{
		return EngineResult::fail;
	}

	*output = probeIrradiance;

	return EngineResult::ok;

}