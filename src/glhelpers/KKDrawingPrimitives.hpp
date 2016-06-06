//
//  KKDrawingPrimitives.hpp
//  KKEngine
//
//  Created by kewei on 3/14/16.
//  Copyright © 2016 kewei. All rights reserved.
//

#ifndef KKDrawingPrimitives_hpp
#define KKDrawingPrimitives_hpp

#include "KKMacros.h"
#include "KKGL.h"
#include "KKUtil.h"
#include "KKGeometry.h"

NS_KK_BEGIN

void drawLine(const KKPoint& origin, const KKPoint& dest);

void drawRect(const KKPoint&  origin, const KKPoint&  dest);

void drawSolidRect(const KKPoint& origin,const KKPoint& dest);

void drawSolidPoly(const KKPoint* poli, unsigned int numberOfPoints);

void drawPoly(const KKPoint *poli, unsigned int numberOfPoints, bool closePolygon);

void drawCircle( const KKPoint& center, float radius, float angle, unsigned int segments, bool drawLineToCenter);

void drawSolidCircle( const KKPoint& center, float radius, float angle, unsigned int segments);

void drawColor4F(GLfloat r, GLfloat g, GLfloat b, GLfloat a);

void drawColor4F(color4F color);

void drawColor4B(GLubyte r, GLubyte g, GLubyte b, GLubyte a);

void drawColor4B(color4B color);

NS_KK_END

#endif /* KKDrawingPrimitives_hpp */
