//
//  KKGeometry.h
//  KKEngine
//
//  Created by kewei on 1/9/14.
//  Copyright (c) 2014 KK. All rights reserved.
//

#ifndef KKEngine_KKGeometry_h
#define KKEngine_KKGeometry_h

#include "KKMacros.h"
#include <math.h>

NS_KK_BEGIN

class KKPoint {
    
public:
    float x = {0};
    float y = {0};
public:
    KKPoint():x(0),y(0){}
    KKPoint(float x, float y):x(x),y(y){}
    KKPoint operator+(const KKPoint &point)
    {
        return KKPoint(x+point.x, y+point.y);
    }
    
    KKPoint operator+(const KKPoint &point) const
    {
        return KKPoint(x+point.x, y+point.y);
    }
    
    KKPoint operator-(const KKPoint &point)
    {
        return KKPoint(x-point.x, y-point.y);
    }
    
    KKPoint operator-(const KKPoint &point) const
    {
        return KKPoint(x-point.x, y-point.y);
    }
    
    KKPoint operator-() const
    {
        return KKPoint(-x, -y);
    }
    
    KKPoint operator*(const float &s) const
    {
        return KKPoint(x*s, y*s);
    }
    
    KKPoint operator/(const float &s) const
    {
        return KKPoint(x/s, y/s);
    }
    
    KKPoint & operator*=(const float &s)
    {
        x *= s;
        y *= s;
        
        return *this;
    }
    
    KKPoint & operator/=(const float &s)
    {
        x /= s;
        y /= s;
        
        return *this;
    }
    
    KKPoint & operator+=(const KKPoint &point)
    {
        x += point.x;
        y += point.y;
        
        return *this;
    }
    
    KKPoint & operator+=(const float &s)
    {
        x += s;
        y += s;
        
        return *this;
    }
    
    bool operator==(const KKPoint &point)
    {
        return this->x == point.x && this->y == point.y;
    }
    
    bool operator!=(const KKPoint &point)
    {
        return this->x != point.x || this->y != point.y;
    }
    
    bool operator==(const KKPoint &point) const
    {
        return x == point.x && y == point.y;
    }
    
    bool operator!=(const KKPoint &point) const
    {
        return !(*this == point);
    }
    
    bool isFuzzyEqualToPoint(const KKPoint &point, const float &var) const
    {
        if (x - var <= point.x && point.x <= x + var)
            if (y - var <= point.y && point.y <= y + var)
                return true;
        return false;
    }
    
    //向量x，y之间的点积
    float dot(const KKPoint &point) const
    {
        return x*point.x+y*point.y;
    }
    
    //向量x，y之间的叉积
    float cross(const KKPoint &point) const
    {
        return x*point.x-y*point.y;
    }
    
    KKPoint perp() const
    {
        return KKPoint(-y, x);
    }
    
    KKPoint rpery() const
    {
        return KKPoint(y, -x);
    }
    
    KKPoint project(const KKPoint &point) const
    {
        return point*(dot(point)/point.dot(point));
    }
    
    KKPoint rotate(const KKPoint &point) const
    {
        return KKPoint(x*point.x-y*point.y, x*point.y+y*point.x);
    }
    
    KKPoint unrotate(const KKPoint &point) const
    {
        return KKPoint(x*point.x+y*point.y, x*point.y-y*point.x);
    }
    
    float length() const
    {
        return sqrtf(dot(*this));
    }
    
    //标准化向量，返回一个方向和x相同但长度为1的向量
    KKPoint normalize() const
    {
        return (*this)*(1.0f/length());
    }
    
    static float distance(const KKPoint &v1, const KKPoint &v2)
    {
        return (v1-v2).length();
    }
    
    static KKPoint midpoint(const KKPoint &v1, const KKPoint &v2)
    {
        return (v1+v2)/2;
    }
    
    static float angle(const KKPoint& a, const KKPoint& b)
    {
        float angle = acosf(a.normalize().dot(b.normalize()));
        if (fabs(angle) < FLT_EPSILON) {
            return 0.f;
        }
        return angle;
    }
    
};

class KKSize {
public:
    float width = {0};
    float height = {0};
public:
    KKSize(){}
    KKSize(float width, float height):width(width),height(height){}
    
    bool operator==(const KKSize &size) const
    {
        return width == size.width && height == size.height;
    }
    
    bool operator!=(const KKSize &size) const
    {
        return !(*this==size);
    }
    
    bool operator>(const KKSize &size) const
    {
        return width*size.width > height*size.height;
    }
    
    bool operator<(const KKSize &size) const
    {
        return width*size.width < height*size.height;
    }
    
    KKSize operator*(const float &s) const
    {
        return KKSize(width*s, height*s);
    }
    
    KKSize operator/(const float &s) const
    {
        return KKSize(width/s, height/s);
    }
    
    KKSize & operator*=(const float &s)
    {
        width *= s;
        height *= s;
        return *this;
    }
};

class KKRect {
    
public:
    KKPoint origin = {0,0};
    KKSize size = {0,0};
    
public:
    KKRect(){}
    KKRect(float x, float y, float width, float height):origin(x,y), size(width, height){}
    
    bool operator==(const KKRect &rect)
    {
        return origin==rect.origin && size==rect.size;
    }
    
    bool operator!=(const KKRect &rect)
    {
        return !(*this==rect);
    }
    
    bool containsPoint(const KKPoint &point)
    {
        if (point.x >= origin.x && point.x <= (origin.x+size.width)
            && point.y >= origin.y && point.y <= origin.y+size.height)
        {
            return true;
        }
        return false;
    }
};

const KKPoint KKPointZero(0,0);
const KKSize  KKSizeZero(0,0);
const KKRect  KKRectZero(0,0,0,0);


NS_KK_END


#endif
