//
//  original.hpp
//  8ssedt
//
//  Created by Pujun Lun on 4/21/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#ifndef original_hpp
#define original_hpp

#include <string>
#include "sdf.h"
#include <vector>

class Original : public SDF 
{
protected:
    Point Get( Grid &g, int x, int y ) override;
    void Put( Grid &g, int x, int y, const Point &p ) override;
    void Compare( Grid &g, Point &p, int x, int y, int offsetx, int offsety ) override;
    void GenerateSDF( Grid &g ) override;

	virtual void loadImage(const std::string& imagePath) override;
	virtual void run(int repeat, bool writeOutput = false) override;

public:
    Original() = default;
	virtual void run(const std::string& imagePath, bool writeOutput = false) override;
    ~Original();
};

class SDFMerger
{
public:
	SDFMerger() = default;
	virtual void MergeImage();
	int imageWidth, imageHeight, imageChannel;
	unsigned char* image;

	std::vector<SDF*> images;
};

#endif /* original_hpp */
