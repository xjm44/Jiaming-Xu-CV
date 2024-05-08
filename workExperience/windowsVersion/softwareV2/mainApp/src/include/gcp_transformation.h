#pragma once
//include

#include <iostream>
#include<opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "Yuan.h"
#include "gcp.h"



class gcp_trans {
public:
	gcp_trans();
	~gcp_trans();

	cv::Mat Get3DR_TransMatrix2(std::vector<Yuan> o);

	int trans(std::vector<Yuan> o, std::string path_cloud, std::string path_save);//01



};





