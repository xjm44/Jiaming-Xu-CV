#include "../include/gcp_transformation.h"

gcp_trans::gcp_trans(){};
gcp_trans::~gcp_trans(){};


cv::Mat gcp_trans::Get3DR_TransMatrix2(std::vector<Yuan> o)
{
	double srcSumX = 0.0f;
	double srcSumY = 0.0f;
	double srcSumZ = 0.0f;

	double dstSumX = 0.0f;
	double dstSumY = 0.0f;
	double dstSumZ = 0.0f;

	int pointsNum = o.size();
	if (pointsNum < 3)
	{
		return cv::Mat();
	}


	for (int i = 0; i < pointsNum; ++i)
	{
		srcSumX += o[i].x;
		srcSumY += o[i].y;
		srcSumZ += o[i].z;

		dstSumX += o[i].rx;
		dstSumY += o[i].ry;
		dstSumZ += o[i].rz;
	}

	//pcl::PointXYZ  centerSrc, centerDst;

	double centerSrc_x = double(srcSumX / pointsNum);
	double centerSrc_y = double(srcSumY / pointsNum);
	double centerSrc_z = double(srcSumZ / pointsNum);

	double centerDst_x = double(dstSumX / pointsNum);
	double centerDst_y = double(dstSumY / pointsNum);
	double centerDst_z = double(dstSumZ / pointsNum);

	//Mat::Mat(int rows, int cols, int type)
	cv::Mat srcMat(3, pointsNum, CV_64FC1);
	cv::Mat dstMat(3, pointsNum, CV_64FC1);
	//-- - Modify
	for (int i = 0; i < pointsNum; ++i)
	{
	
		srcMat.at<double>(0, i) = o[i].x - centerSrc_x;
		srcMat.at<double>(1, i) = o[i].y - centerSrc_y;
		srcMat.at<double>(2, i) = o[i].z - centerSrc_z;

		dstMat.at<double>(0, i) = o[i].rx - centerDst_x;
		dstMat.at<double>(1, i) = o[i].ry - centerDst_y;
		dstMat.at<double>(2, i) = o[i].rz - centerDst_z;

	}

	cv::Mat matS = srcMat * dstMat.t();

	cv::Mat matU, matW, matV;
	cv::SVDecomp(matS, matW, matU, matV);

	cv::Mat matTemp = matU * matV;
	double det = cv::determinant(matTemp);

	double datM[] = { 1, 0, 0, 0, 1, 0, 0, 0, det };
	cv::Mat matM(3, 3, CV_64FC1, datM);

	cv::Mat matR = matV.t() * matM * matU.t();

	double* datR = (double*)(matR.data);
	double delta_X = centerDst_x - (centerSrc_x * datR[0] + centerSrc_y * datR[1] + centerSrc_z * datR[2]);
	double delta_Y = centerDst_y - (centerSrc_x * datR[3] + centerSrc_y * datR[4] + centerSrc_z * datR[5]);
	double delta_Z = centerDst_z - (centerSrc_x * datR[6] + centerSrc_y * datR[7] + centerSrc_z * datR[8]);


	cv::Mat R_T = (cv::Mat_<double>(4, 4) <<
		matR.at<double>(0, 0), matR.at<double>(0, 1), matR.at<double>(0, 2), delta_X,
		matR.at<double>(1, 0), matR.at<double>(1, 1), matR.at<double>(1, 2), delta_Y,
		matR.at<double>(2, 0), matR.at<double>(2, 1), matR.at<double>(2, 2), delta_Z,
		0, 0, 0, 1
		);

	return R_T;
}




int gcp_trans::trans(std::vector<Yuan> o, std::string path_cloud, std::string path_save)//01
{

	pcl::PointCloud<pcl::PointXYZI>::Ptr cloud1(new pcl::PointCloud<pcl::PointXYZI>);

	//io::loadPCDFile(path_cloud, *cloud1);
	io::loadPLYFile<pcl::PointXYZI>(path_cloud, *cloud1);

	int n = o.size();


	cv::Mat RT = Get3DR_TransMatrix2(o);

	// return empty RT **** to be done
	if (RT.empty()) {
		return -99;
	}





	//for (int r = 0; r < RT.rows; r++)
	//{
	//	for (int c = 0; c < RT.cols; c++)
	//	{
	//		//printf("%f, ", RT.at<double>(r, c));
	//	}
	//	//printf("\n");
	//}
	//printf("**************************************\n");
	//getchar();


	for (int i = 0; i < cloud1->points.size(); i++) {

		float x = cloud1->points[i].x;
		float y = cloud1->points[i].y;
		float z = cloud1->points[i].z;




		cv::Mat A = (cv::Mat_<double>(4, 1) <<
			x,
			y,
			z,
			1
			);

		cv::Mat B = RT * A;



		cloud1->points[i].x = B.at<double>(0, 0);
		cloud1->points[i].y = B.at<double>(1, 0);
		cloud1->points[i].z = B.at<double>(2, 0);


	}

	//pcl::io::savePCDFileBinary(path_save, *cloud1);
	pcl::io::savePLYFileBinary(path_save, *cloud1);
	//return cloud1;

	return 0;


}