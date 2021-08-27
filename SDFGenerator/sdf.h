//
//  sdf.hpp
//  8ssedt
//
//  Created by Pujun Lun on 4/21/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#ifndef sdf_hpp
#define sdf_hpp

#include <string>

class SDF 
{
protected:
    struct Point
    {
        int dx, dy;
        
        int DistSq() const { return dx*dx + dy*dy; }
    };
    
    struct Grid
    {
        Point *points;
    };

    int gridWidth, gridHeight, numPoint;
    Grid grid1, grid2;
    Point inside = { 0, 0 };
    Point empty = { 9999, 9999 };
    virtual Point Get( Grid &g, int x, int y ) = 0;
    virtual void Put( Grid &g, int x, int y, const Point &p ) = 0;
    virtual void Compare( Grid &g, Point &p, int x, int y, int offsetx, int offsety ) = 0;
    virtual void GenerateSDF( Grid &g ) = 0;

	virtual void loadImage(const std::string& imagePath) = 0;
	virtual void run(int repeat, bool writeOutput=false) = 0;

public:
    virtual void run(const std::string& imagePath, bool writeOutput = false) = 0;

	int imageWidth, imageHeight, imageChannel;
	unsigned char* image;
	std::string imageName;
	unsigned char* sdf;

};



#endif /* sdf_hpp */
