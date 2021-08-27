//
//  original.cpp
//  8ssedt
//
//  Created by Pujun Lun on 4/21/18.
//  Copyright © 2018 Pujun Lun. All rights reserved.
//

#include <math.h>
#include <iostream>
#include <chrono>
#include "stb_image.h"
#include "stb_image_write.h"
#include "original.h"

Original::Point Original::Get(Grid& g, int x, int y)
{
	// OPTIMIZATION: you can skip the edge check code if you make your grid
	// have a 1-pixel gutter.
	if (x >= 0 && y >= 0 && x < imageWidth && y < imageHeight)
		return g.points[y * imageWidth + x];
	else
		return empty;
}

void Original::Put(Grid& g, int x, int y, const Point& p)
{
	g.points[y * imageWidth + x] = p;
}

void Original::Compare(Grid& g, Point& p, int x, int y, int offsetx, int offsety)
{
	Point other = Get(g, x + offsetx, y + offsety);
	other.dx += offsetx;
	other.dy += offsety;

	if (other.DistSq() < p.DistSq())
		p = other;
}

void Original::GenerateSDF(Grid& g)
{
	// Pass 0
	for (int y = 0; y < imageHeight; y++)
	{
		for (int x = 0; x < imageWidth; x++)
		{
			Point p = Get(g, x, y);
			Compare(g, p, x, y, -1, 0);
			Compare(g, p, x, y, 0, -1);
			Compare(g, p, x, y, -1, -1);
			Compare(g, p, x, y, 1, -1);
			Put(g, x, y, p);
		}

		for (int x = imageWidth - 1; x >= 0; x--)
		{
			Point p = Get(g, x, y);
			Compare(g, p, x, y, 1, 0);
			Put(g, x, y, p);
		}
	}

	// Pass 1
	for (int y = imageHeight - 1; y >= 0; y--)
	{
		for (int x = imageWidth - 1; x >= 0; x--)
		{
			Point p = Get(g, x, y);
			Compare(g, p, x, y, 1, 0);
			Compare(g, p, x, y, 0, 1);
			Compare(g, p, x, y, -1, 1);
			Compare(g, p, x, y, 1, 1);
			Put(g, x, y, p);
		}

		for (int x = 0; x < imageWidth; x++)
		{
			Point p = Get(g, x, y);
			Compare(g, p, x, y, -1, 0);
			Put(g, x, y, p);
		}
	}
}




void Original::loadImage(const std::string& imagePath)
{
	image = stbi_load(imagePath.c_str(), &imageWidth, &imageHeight, &imageChannel, 0);

	gridWidth = imageWidth;
	gridHeight = imageHeight;

	numPoint = gridWidth * gridHeight;
	grid1.points = (Point*)malloc(numPoint * sizeof(Point));
	grid2.points = (Point*)malloc(numPoint * sizeof(Point));
	for (int y = 0; y < imageHeight; ++y)
	{
		for (int x = 0; x < imageWidth; ++x)
		{
			// 大于0的地方是物体
			if (image[y * imageWidth * imageChannel + x * imageChannel] > 0)
			{
				Put(grid1, x, y, inside);
				Put(grid2, x, y, empty);
			}
			else
			{
				Put(grid2, x, y, inside);
				Put(grid1, x, y, empty);
			}
		}
	}
}

void Original::run(int repeat, bool writeOutput)
{
	double elaspsedTime = 0.0;
	Grid testGrid1, testGrid2;
	testGrid1.points = (Point*)malloc(numPoint * sizeof(Point));
	testGrid2.points = (Point*)malloc(numPoint * sizeof(Point));
	for (int i = 0; i < repeat; ++i)
	{
		memcpy(testGrid1.points, grid1.points, numPoint * sizeof(Point));
		memcpy(testGrid2.points, grid2.points, numPoint * sizeof(Point));

		auto start = std::chrono::system_clock::now();
		GenerateSDF(testGrid1);
		GenerateSDF(testGrid2);
		auto end = std::chrono::system_clock::now();
		std::chrono::duration<double, std::ratio<1>> elapsed = end - start;
		elaspsedTime += elapsed.count();
	}
	std::cout << "Finished after " << elaspsedTime / repeat << "s on average" << std::endl;

	sdf = (unsigned char*)malloc(imageWidth * imageHeight * sizeof(unsigned char));

	for (int y = 0; y < imageHeight; ++y)
	{
		for (int x = 0; x < imageWidth; ++x)
		{
			// calculate the actual distance from the dx/dy
			int dist1 = (int)(sqrt((double)Get(testGrid1, x, y).DistSq()));
			int dist2 = (int)(sqrt((double)Get(testGrid2, x, y).DistSq()));
			int dist = dist1 - dist2;

			// clamp and scale for display purpose
			int c = dist * 3 + 128;
			if (c < 0) c = 0;
			if (c > 255) c = 255;

			sdf[y * imageWidth + x] = c;
		}
	}

	if (writeOutput)
	{
		std::string outputPath = imageName + "_sdf_display.jpg";
		stbi_write_jpg(outputPath.c_str(), imageWidth, imageHeight, 1, sdf, 100);
	}

	if (testGrid1.points) free(testGrid1.points);
	if (testGrid2.points) free(testGrid2.points);
}

void Original::run(const std::string& imagePath, bool writeOutput)
{
	auto index = imagePath.find_last_of('.');
	if (index > 0)
	{
		imageName = imagePath.substr(0, index);
	}
	else
	{
		std::cout << "imagePath is error on: " << imagePath << std::endl;
	}

	loadImage(imagePath);
	run(1, writeOutput);
}

Original::~Original()
{
	if (image)
		stbi_image_free(image);
	if (grid1.points)
		free(grid1.points);
	if (grid2.points)
		free(grid2.points);
	std::cout << "Grids freed" << std::endl;
}

void SDFMerger::MergeImage()
{
	if (images.size() > 0)
	{
		imageWidth = images[0]->imageWidth;
		imageHeight = images[0]->imageHeight;
		imageChannel = images[0]->imageChannel;
		image = new unsigned char[imageWidth * imageHeight * imageChannel];

		memset(image, 0, imageWidth * imageHeight * imageChannel);
	}

	// simplest merge
	//float scale = 1.f / images.size();
	//for (int i = 0; i < images.size(); i++)
	//{
	//    for (int y = 0; y < imageHeight; ++y)
	//    {
	//        for (int x = 0; x < imageWidth; ++x)
	//        {
	//            int sdf_index = y * imageWidth + x;
	//            int pixel_index = sdf_index * imageChannel;
	//            for (int c = 0; c < imageChannel; ++c)
	//            {
	//                image[pixel_index + c] += images[i]->image[pixel_index + c] * scale;
	//            }
	//        }
	//    }
	//}

	float scale = 1.f / (images.size() - 1);
	for (int step = 0, i = images.size() - 1; i >= 1; --i, ++step)
	{
		for (int y = 0; y < imageHeight; ++y)
		{
			for (int x = 0; x < imageWidth; ++x)
			{
				// lerp
				int sdf_index = y * imageWidth + x;
				int pixel_index = sdf_index * imageChannel;

				int left_border_index = i;
				int right_border_index = i - 1;

				float sdf1 = images[left_border_index]->sdf[sdf_index] / 255.f;
				float sdf2 = images[right_border_index]->sdf[sdf_index] / 255.f;
				sdf1 = 2.f * sdf1 - 1.f;
				sdf2 = 2.f * sdf2 - 1.f;

				float left = step * scale;
				float right = (step + 1) * scale;

				if (sdf1 * sdf2 > 0)
				{
					if (right_border_index == 0)
					{
						if (sdf1 < 0)
						{
							for (int c = 0; c < imageChannel; ++c)
								image[pixel_index + c] = 255;
						}
					}
					else
					{
						// nothing
					}
				}
				else
				{
					// sdf1 < 0 , sdf2 > 0
					float totalDis = abs(sdf1) + abs(sdf2);
					float t = abs(sdf2) / totalDis;
					t = 1 - t;
					for (int c = 0; c < imageChannel; ++c)
					{
						float dst = left * (1 - t) + (right * t);
						// dst = std::pow(dst, 1 / 2.2f); // gamma for linear display
						image[pixel_index + c] = dst * 255;
					}
				}
			}
		}
	}

	stbi_write_jpg("../../example/merger.jpg", imageWidth, imageHeight, imageChannel, image, 100);
}
