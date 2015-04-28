/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3 - Scene Analysis Sample                                *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/

#ifndef XNV_POINT_DRAWER_H_
#define XNV_POINT_DRAWER_H_

#include <XnCppWrapper.h>

void DrawDepthMap(const xn::DepthMetaData& dmd, const xn::SceneMetaData& smd);
void DrawFrameID(XnUInt32 nFrameID);

#endif
