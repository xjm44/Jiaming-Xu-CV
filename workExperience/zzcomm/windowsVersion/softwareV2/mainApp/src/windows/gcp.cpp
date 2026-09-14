#define PCL_NO_PRECOMPILE 

#include "../include/gcp.h"

std::vector<std::string> split(const std::string& str, char delim) {
	std::stringstream ss(str);
	std::string item;
	std::vector<std::string> elems;
	while (std::getline(ss, item, delim)) {
		if (!item.empty()) {
			elems.push_back(item);
		}
	}
	return elems;
}




int gcp::gcp_save(int line_key, vector<Yuan> o, std::string path_txt) {

	ofstream dataFile;
	dataFile.open(path_txt, ofstream::app);
	fstream file(path_txt, ios::out);

	dataFile << line_key << endl;

	for (int i = 0; i < o.size(); i++) {

		dataFile << o[i].name << " " << o[i].x << " " << o[i].y << " " << o[i].z << " " << o[i].r << " " << std::fixed << setprecision(6) << o[i].t << std::fixed << setprecision(10) << " " << o[i].rx << " " << o[i].ry << " " << o[i].rz << endl;
	}

	return 0;
}


vector<Yuan> gcp::gcp_load(std::string path_txt) {
	vector<Yuan> o;

	std::string line_tmp;
	vector<string> lines;
	int line_key = -1;

	ifstream ifs;

	ifs.open(path_txt);
	if (!ifs)
	{

		return o;
	}

	while (getline(ifs, line_tmp))   
	{
		lines.push_back(line_tmp);  
		//cout << line_tmp << endl;   
	}
	ifs.close();


	//lines.resize(lines.size() - 1);

	line_key = stoi(lines[0]);

	for (int i = 1; i < lines.size(); i++)
	{
		vector<string> Yuan_params = split(lines[i], ' ');

		//cout << Yuan_params.size() << "\n\n" << endl;
		//cout << Yuan_params[0] << " " << Yuan_params[1] << " " << Yuan_params[2] << " " << Yuan_params[3] << " " << Yuan_params[4] << Yuan_params[5] << "\n" << endl;
		Yuan y0;

		y0.name = stoi(Yuan_params[0]);
		y0.x = stof(Yuan_params[1]);
		y0.y = stof(Yuan_params[2]);
		y0.z = stof(Yuan_params[3]);
		y0.r = stof(Yuan_params[4]);
		y0.t = stof(Yuan_params[5]);
		y0.rx = stof(Yuan_params[6]);
		y0.ry = stof(Yuan_params[7]);
		y0.rz = stof(Yuan_params[8]);
		//cout << y0.name << " " << y0.x << " " << y0.y << " " << y0.z << " " << y0.r << " " << y0.t << "\n" << endl;
		o.emplace_back(y0);
	}
	//oi
	//cout << "oi" << endl;    
	/*for (int i = 0; i < o.size(); i++)
	{
		cout << o[0].x << " " << o[i].y << " " << o[i].z << " " << o[i].r << " " << o[i].t << "\n" << endl;
	}*/
	return o;
}


bool cmpt(Yuan s1, Yuan s2) {  
	return s1.t < s2.t; 
}


gcp::gcp() {

}
gcp::~gcp() {

}


pcl::visualization::PCLVisualizer::Ptr gcp::simpleVis(pcl::PointCloud<pcl::PointXYZI>::ConstPtr cloud)
{
	pcl::visualization::PCLVisualizer::Ptr viewer(new pcl::visualization::PCLVisualizer("3D Viewer"));
	viewer->setBackgroundColor(0, 0, 0);
	viewer->addPointCloud<pcl::PointXYZI>(cloud, "sample cloud");
	viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 3, "sample cloud");
	viewer->initCameraParameters();
	return (viewer);
}



pcl::PointCloud<pcl::PointXYZI>::Ptr gcp::vox_dsp(pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_zhen, float x) {



	pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_filtered(new pcl::PointCloud<pcl::PointXYZI>);


	pcl::VoxelGrid<pcl::PointXYZI> sor;
	sor.setInputCloud(cloud_zhen);            
	sor.setLeafSize(x, x, x); 
	sor.filter(*cloud_filtered);        

	return cloud_filtered;

}



pcl::PointCloud<pcl::PointXYZI>::Ptr gcp::uni_dsp(pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_zhen, float x) {


	pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_sampling(new pcl::PointCloud<pcl::PointXYZI>());

	pcl::UniformSampling<pcl::PointXYZI> sor;


	sor.setInputCloud(cloud_zhen);
	sor.setRadiusSearch(x);
	sor.filter(*cloud_sampling);

	//pcl::PLYWriter writer;
	//writer.write<pcl::PointXYZRGB>("table_scene_lms400_inliers.ply", *cloud_sampling, false);


	return cloud_sampling;

}

//sor
pcl::PointCloud<pcl::PointXYZI>::Ptr gcp::sor(pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_zhen, int n, float e) {



	pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_out(new pcl::PointCloud<pcl::PointXYZI>);


	pcl::StatisticalOutlierRemoval<pcl::PointXYZI> outrem;
	outrem.setInputCloud(cloud_zhen);
	outrem.setMeanK(n);
	outrem.setStddevMulThresh(e);

	outrem.filter(*cloud_out);

	return cloud_out;

}

//MLS  
pcl::PointCloud<pcl::PointXYZI>::Ptr  gcp::mls(pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_zhen, float r, int n, float g) {



	pcl::PointCloud<pcl::PointXYZI>::Ptr mls_points{ new pcl::PointCloud<pcl::PointXYZI>() };

	pcl::MovingLeastSquares<pcl::PointXYZI, pcl::PointXYZI> MLS;
	pcl::search::KdTree<pcl::PointXYZI>::Ptr tree(new pcl::search::KdTree<pcl::PointXYZI>());
	MLS.setSearchMethod(tree);
	MLS.setSearchRadius(r);//0.1 
	MLS.setComputeNormals(false);

	MLS.setInputCloud(cloud_zhen);
	MLS.setPolynomialOrder(n);//2
	MLS.setSqrGaussParam(g);//0.005 
	MLS.process(*mls_points);

	return mls_points;
}


pcl::PointCloud<pcl::PointXYZI>::Ptr  gcp::pth_filter(pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_zhen, float i_min, int i_max) {

	pcl::PointCloud<pcl::PointXYZI>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZI>);

	pcl::PassThrough< pcl::PointXYZI >  pass;

	pass.setInputCloud(cloud_zhen);

	pass.setFilterFieldName("intensity");

	pass.setFilterLimits(i_min, i_max);

	pass.setNegative(0);

	pass.filter(*cloud);

	return cloud;
}


pcl::PointCloud<pcl::PointXYZI>::Ptr gcp::del(pcl::PointCloud<pcl::PointXYZI>::Ptr cloud, vector<int> index)
{
	int n = (*cloud).size();
	vector<int>I = {};
	//I.reserve(n);
	I.resize(n);
	for (int i = 0; i < n; i++)
	{
		I[i] = i;
	}

	int n1 = index.size() - 1;

	for (int i = n1; i >= 0; i--)
	{
		//int a = index[i];
		//std::cout << I.size() << " " << index[i] << std::endl;
		I.erase(I.begin() + index[i]);
	}

	pcl::PointCloud<pcl::PointXYZI>::Ptr cloud2(new PointCloud<PointXYZI>);
	pcl::copyPointCloud(*cloud, I, *cloud2);
	cloud = cloud2;

	return cloud;
}


pcl::PointCloud<pcl::PointXYZI>::Ptr gcp::del2(pcl::PointCloud<pcl::PointXYZI>::Ptr cloud, vector<int> index)
{
	int n0 = (*cloud).size();
	int n1 = index.size();
	int n2 = n0 - n1;



	for (int i = 0; i < n1; i++) {
		int k = n0 - 1 - i;
		int j = index[n1 - 1 - i];
		swap(cloud->points[j], cloud->points[k]);


	}

	//cloud->points.size,width = n2;
	cloud->points.resize(n2);
	(*cloud).width = n2;


	return cloud;
}


bool gcp::find_neigbour_radius(pcl::KdTreeFLANN<pcl::PointXYZI> kd_tree, pcl::PointXYZI search_pt, float radius, std::vector<int>& pt_idx_search, std::vector<float>& pt_sqr_dis)
{
	int num_neighbours = kd_tree.radiusSearch(search_pt, radius, pt_idx_search, pt_sqr_dis);
	return num_neighbours;
}

bool gcp::find_neigbour_radius2(pcl::KdTreeFLANN<pcl::PointXYZI> kd_tree, pcl::PointXYZI search_pt, float radius, Indices& pt_idx_search, std::vector<float>& pt_sqr_dis)
{
	int num_neighbours = kd_tree.radiusSearch(search_pt, radius, pt_idx_search, pt_sqr_dis);
	return num_neighbours;
}


bool gcp::find_k_nearest_neigbour(pcl::KdTreeFLANN<pcl::PointXYZI> kd_tree, pcl::PointXYZI search_pt, int k, std::vector<int>& pt_idx_search, std::vector<float>& pt_sqr_dis)
{

	int result = kd_tree.nearestKSearch(search_pt, k, pt_idx_search, pt_sqr_dis);
	return result;
}


bool gcp::find_neigbour_radius_t(pcl::KdTreeFLANN<pcl::PointXYZIT> kd_tree, pcl::PointXYZIT search_pt, float radius, std::vector<int>& pt_idx_search, std::vector<float>& pt_sqr_dis)
{
	int num_neighbours = kd_tree.radiusSearch(search_pt, radius, pt_idx_search, pt_sqr_dis);
	return num_neighbours;

}

bool gcp::find_k_nearest_neigbour_t(pcl::KdTreeFLANN<pcl::PointXYZIT> kd_tree, pcl::PointXYZIT search_pt, int k, std::vector<int>& pt_idx_search, std::vector<float>& pt_sqr_dis)
{

	int result = kd_tree.nearestKSearch(search_pt, k, pt_idx_search, pt_sqr_dis);
	return result;
}


pair<float, float> gcp::get_time_t(float x0, float y0, float z0, pcl::PointCloud<PointXYZIT>::Ptr cloud) {

	float min_dis;
	//int minIndex , tempIndex;
	//int x0 = 0, y0 = 0, z0 = 0;
	//minDistance = 1000000000;

	pcl::KdTreeFLANN<pcl::PointXYZIT> kd_tree;

	kd_tree.setInputCloud(cloud);


	pcl::PointXYZIT search_pt;
	search_pt.x = x0;
	search_pt.y = y0;
	search_pt.z = z0;


	//float x = search_pt.x;
	//float y = search_pt.y;
	//float z = search_pt.z;



	int k = 1;
	std::vector<int> pt_idx_search; 
	std::vector<float> pt_sqr_dis; 


	find_k_nearest_neigbour_t(kd_tree, search_pt, k, pt_idx_search, pt_sqr_dis);

	//if (find_k_nearest_neigbour(kd_tree, search_pt, k, pt_idx_search, pt_sqr_dis) > 0)
	//{
	//	for (size_t i = 0; i < pt_idx_search.size(); i++)  //   ӡ   еĽ  ڵ     ;   
	//	{
	//		std::cout << "(" << cloud->points[pt_idx_search[i]].x << "  "
	//			<< cloud->points[pt_idx_search[i]].y << "  "
	//			<< cloud->points[pt_idx_search[i]].z << ")"
	//			<< "dis: " << pt_sqr_dis[i] << "\n" << std::endl;
	//	}
	//}

	int j = pt_idx_search[0];
	min_dis = pt_sqr_dis[0];

	//float min_dis = pow((x - cloud->points[j].x), 2) + pow((y - cloud->points[j].y), 2) + pow((z - cloud->points[j].z), 2);

	float t = cloud->points[j].time;

	return make_pair(t, min_dis);

}

pair<float, float> gcp::get_time(float x0, float y0, float z0, pcl::PointCloud<PointXYZI>::Ptr cloud, pcl::PointCloud<PointXYZIT>::Ptr cloud_t) {

	float min_dis;
	//int minIndex , tempIndex;
	//int x0 = 0, y0 = 0, z0 = 0;
	//minDistance = 1000000000;

	pcl::KdTreeFLANN<pcl::PointXYZI> kd_tree;

	kd_tree.setInputCloud(cloud);


	pcl::PointXYZI search_pt;
	search_pt.x = x0;
	search_pt.y = y0;
	search_pt.z = z0;


	//float x = search_pt.x;
	//float y = search_pt.y;
	//float z = search_pt.z;


	int k = 1;
	std::vector<int> pt_idx_search; 
	std::vector<float> pt_sqr_dis;  


	find_k_nearest_neigbour(kd_tree, search_pt, k, pt_idx_search, pt_sqr_dis);

	//if (find_k_nearest_neigbour(kd_tree, search_pt, k, pt_idx_search, pt_sqr_dis) > 0)
	//{
	//	for (size_t i = 0; i < pt_idx_search.size(); i++)  
	//	{
	//		std::cout << "(" << cloud->points[pt_idx_search[i]].x << "  "
	//			<< cloud->points[pt_idx_search[i]].y << "  "
	//			<< cloud->points[pt_idx_search[i]].z << ")"
	//			<< "dis: " << pt_sqr_dis[i] << "\n" << std::endl;
	//	}
	//}

	int j = pt_idx_search[0];
	min_dis = pt_sqr_dis[0];

	//float min_dis = pow((x - cloud->points[j].x), 2) + pow((y - cloud->points[j].y), 2) + pow((z - cloud->points[j].z), 2);

	float t = cloud_t->points[j].time;

	return make_pair(t, min_dis);

}

pair<float, float> gcp::get_time2(float x0, float y0, float z0, pcl::PointCloud<PointXYZIT>::Ptr pathCloud) {

	float minDistance, tempDistance;
	int minIndex, tempIndex;
	//int x0 = 0, y0 = 0, z0 = 0;
	int x1 = 0, y1 = 0, z1 = 0;
	//minDistance = 1e10;
	minDistance = 1000000000;

	for (int j = 0; j < pathCloud->size(); j++)
	{
		x1 = pathCloud->points[j].x;
		y1 = pathCloud->points[j].y;
		z1 = pathCloud->points[j].z;
		tempDistance = pow((x0 - x1), 2) + pow((y0 - y1), 2) + pow((z0 - z1), 2);

		if (j == 0)
		{
			minDistance = tempDistance;
			continue;
		}

		if (tempDistance < minDistance)
		{
			minDistance = tempDistance;
			minIndex = j;
		}
	}
	float t = pathCloud->points[minIndex].time;
	return make_pair(t, minDistance);

}


pair< std::vector<Yuan>, std::vector<std::string> > gcp::get_gcp(std::string path_cloud, std::string path,   
	std::string path_cloudline, int a, float r1, float r2, int switch_key, float i_min, float i_max, int min_circle,          
	int min_break, int d_max, int d, float rd, float r_index)	                                  
{

	int diedai = d * a;
	int f = 0;
	int line_key = 0;


	if (path_cloudline.length() > 1) {
		line_key = 1;
	}

	if (switch_key == 1) {
		d_max = 0.5;
	}

	pcl::PointCloud<pcl::PointXYZI>::Ptr cloud0(new pcl::PointCloud<pcl::PointXYZI>);


	//io::loadPCDFile(path_cloud, *cloud0);
	io::loadPLYFile(path_cloud, *cloud0);

	//pcl::PointCloud<pcl::PointXYZI>::Ptr cloudline(new pcl::PointCloud<pcl::PointXYZI>);

	pcl::PointCloud<pcl::PointXYZIT>::Ptr cloudline_t(new pcl::PointCloud<pcl::PointXYZIT>);

	if (line_key != 0) {

		//io::loadPCDFile(path_cloudline, *cloudline);
		//io::loadPLYFile(path_cloudline, *cloudline);
		//io::loadPCDFile(path_cloudline, *cloudline);
		io::loadPLYFile(path_cloudline, *cloudline_t);
	}


	vector <Yuan>  o;


	std::vector<pcl::PointCloud<pcl::PointXYZI>::Ptr> cloudlist0;
	std::vector<pcl::PointCloud<pcl::PointXYZI>::Ptr> cloudlist1;


	std::vector<float> circlelist;
	std::vector<std::string> pathlist;


	pcl::PointCloud<pcl::PointXYZI>::Ptr cloud(new       pcl::PointCloud<pcl::PointXYZI>);

	cloud = cloud0;

	cloud = pth_filter(cloud, i_min, i_max);

	//cloud = uni_dsp(cloud, 0.005);

	//cout << cloud->points.size() << "\n\n";

	cloud = sor(cloud, 10, 1);//sor

	cloud = mls(cloud, 0.3, 1, 0.005);//mls

	for (int k = 0; k < diedai; k++) {

		//for(int k=0; k<1;k++){

		std::cout << "=====" << "\n\n";

		pcl::PointCloud<pcl::PointXYZI>::Ptr final(new pcl::PointCloud<pcl::PointXYZI>);

		std::vector<int> inliers;

		pcl::SampleConsensusModelCircle3D<pcl::PointXYZI>::Ptr
			model_circle3D(new pcl::SampleConsensusModelCircle3D<pcl::PointXYZI>(cloud));

		pcl::RandomSampleConsensus<pcl::PointXYZI> ransac(model_circle3D);
		//ransac.setMaxIterations(15000);
		ransac.setDistanceThreshold(0.001);
		ransac.computeModel();
		ransac.getInliers(inliers);

		Eigen::VectorXf model;

		ransac.getModelCoefficients(model);
		std::cout << model << "\n\n";

		std::cout << inliers.size() << "\n\n";
		//if (inliers.size() == 0)  
		//if (inliers.size() < min_circle && f >= a)
		if (inliers.size() < min_circle && f >= a)

		{
			std::cout << "=====break=====" << "\n\n";
			break;
		}

		if (inliers.size() < min_break)

		{
			std::cout << "=====break=====" << "\n\n";
			break;
		}
		float x0 = model[0];
		float y0 = model[1];
		float z0 = model[2];

		float r = model[3];

		float nx = model[4];
		float ny = model[5];
		float nz = model[6];

		pcl::KdTreeFLANN<pcl::PointXYZI> kd_tree;

		kd_tree.setInputCloud(cloud);

		pcl::PointXYZI search_pt;
		search_pt.x = x0;
		search_pt.y = y0;
		search_pt.z = z0;

		std::vector<int> pt_idx_search;  
		std::vector<float> pt_sqr_dis;  
		float rr = r + rd;
		float radius = rr;


		int num_neighbours = find_neigbour_radius(kd_tree, search_pt, radius, pt_idx_search, pt_sqr_dis);


		std::vector<int> remove;

		for (size_t i = 0; i < pt_idx_search.size(); i++)  
		{
			float x1 = cloud->points[pt_idx_search[i]].x;
			float y1 = cloud->points[pt_idx_search[i]].y;
			float z1 = cloud->points[pt_idx_search[i]].z;
			float d = ((x1 - x0) * nx + (y1 - y0) * ny + (z1 - z0) * nz);
			d = abs(d);

			if (d < 0.02)
			{

				remove.push_back(pt_idx_search[i]);

			}
		}

		sort(remove.begin(), remove.end());

		pcl::copyPointCloud<pcl::PointXYZI>(*cloud, remove, *final);

		std::cout << final->points.size() << "\n\n";

		//inlier
		int nn = remove.size();

		cloud = del2(cloud, remove);

		if (r < r1 || r2 < r)
		{
			std::cout << "tiaoguo" << "\n\n";
			continue;
		}

		if (inliers.size() < min_circle)
		{
			std::cout << "tiaoguo" << "\n\n";
			continue;
		}

		float t = 0;

		if (line_key != 0) {


			//pair<float, float> p1 = get_time(x0, y0, z0, cloudline, cloudline_t);
			pair<float, float> p1 = get_time_t(x0, y0, z0,cloudline_t);

			t = p1.first;
			float distance = p1.second;;

			if (distance > d_max)
			{
				std::cout << "tiaoguo" << "\n\n";
				continue;
			}
		}

		Yuan yu;
		yu.x = x0;
		yu.y = y0;
		yu.z = z0;
		yu.r = r;

		yu.t = t;
		yu.nx = nx;
		yu.ny = ny;
		yu.nz = nz;

		cloudlist0.emplace_back(final);

		o.emplace_back(yu);

		f++;

		std::cout << f << "\n\n";
	}

	std::cout << f << "\n\n";

	if (line_key == 0) {

		for (int i = 0; i < o.size(); i++) {
			std::cout << "\n\n\n\n";
			
			pcl::KdTreeFLANN<pcl::PointXYZI> kd_tree;
			kd_tree.setInputCloud(cloud0);

			pcl::PointXYZI search_pt;
			search_pt.x = o[i].x;
			search_pt.y = o[i].y;
			search_pt.z = o[i].z;

			Indices pt_idx_search;
			std::vector<float> pt_sqr_dis; 
			float radius = r_index;

			int num_neighbours = find_neigbour_radius2(kd_tree, search_pt, radius, pt_idx_search, pt_sqr_dis);
			float avg = 0;
			std::cout << "index=======" << "\n\n";
			for (size_t i = 0; i < pt_idx_search.size(); i++) 
			{
				avg = avg + pt_idx_search[i] / pt_idx_search.size();
			}

			std::cout << "=========================";
			std::cout << avg;
			o[i].t = int(avg);
		}
	}

	for (int i = 0; i < o.size(); i++) {
		o[i].name = i;

	}
	sort(o.begin(), o.end(), cmpt);  


	for (int i = 0; i < o.size(); i++) {

		//cloudlist1[i] = cloudlist0[o[i].name];

		cloudlist1.emplace_back(cloudlist0[o[i].name]);
	}


	for (int i = 0; i < o.size(); i++) {
		o[i].name = i + 1;

	}


	ofstream dataFile;
	std::string path_txt = path + "xyzt.txt";
	dataFile.open(path_txt, ofstream::app);
	fstream file(path_txt, ios::out);

	for (int i = 0; i < o.size(); i++) {

		dataFile << o[i].x << ' ' << o[i].y << ' ' << o[i].z << ' ' << std::fixed << setprecision(6) << o[i].t << "\n" << endl;
	}

	dataFile.close();

	pathlist.emplace_back(path_txt);

	ofstream dataFile2;
	std::string path_txt2 = path + "name+xyzr.txt";
	dataFile2.open(path_txt2, ofstream::app);
	fstream file2(path_txt2, ios::out);

	dataFile2 << "name" << "     " << "x" << "       " << "y" << "       " << "z" << "      " << "r"
		<< "     " << "real_x" << "     " << "real_y" << "     " << "real_z" << "\n" << endl;

	for (int i = 0; i < o.size(); i++) {

		dataFile2 << "gcp_" << i + 1 << ' ' << std::fixed << setprecision(4) << o[i].x << ' ' << std::fixed << setprecision(4) << o[i].y << ' ' << std::fixed << setprecision(4) << o[i].z
			<< ' ' << std::fixed << setprecision(4) << o[i].r << "\n" << endl;
	}

	dataFile2.close();


	//std::string path_shengyu = path + "shengyu.ply";

	cout << "===============0" << "\n";

	//pcl::io::savePCDFileBinary(path_shengyu, *cloud);
	//pcl::io::savePLYFileBinary(path_shengyu, *cloud);

	cout << "===============1" << "\n";

	if (f != 0) {

		for (int u = 0; u < f; u++) {
			std::string file = "gcp_" + std::to_string(u + 1) + ".ply";
			file = path + file;
			pathlist.emplace_back(file);

			std::cout << "===============" << "\n\n";

			std::cout << o[u].x << "\n\n";
			std::cout << o[u].y << "\n\n";
			std::cout << o[u].z << "\n\n";
			std::cout << o[u].r << "\n\n";

			circlelist.emplace_back(o[u].x);
			circlelist.emplace_back(o[u].y);
			circlelist.emplace_back(o[u].z);
			circlelist.emplace_back(o[u].r);

			std::cout << std::fixed << setprecision(6) << o[u].t << "\n\n";

			pcl::io::savePLYFileBinary(file, *(cloudlist1[u]));

		}
		//pathlist.emplace_back(path_shengyu);
	}


	//if (f != 0) {
	//	for (int u = 0; u < f; u++) {
	//		float xs = o[u].x;
	//		float ys = o[u].y;
	//		float zs = o[u].z;
	//		float rs = o[u].r;
	//		float nxs = o[u].nx;
	//		float nys = o[u].ny;
	//		float nzs = o[u].nz;

	//		pcl::KdTreeFLANN<pcl::PointXYZI> kd_tree2;

	//		kd_tree2.setInputCloud(cloud0);

	//		pcl::PointXYZI search_pt2;
	//		search_pt2.x = xs;
	//		search_pt2.y = ys;
	//		search_pt2.z = zs;

	//		std::vector<int> pt_idx_search2;  
	//		std::vector<float> pt_sqr_dis2;  
	//		float rrr = rs + rd;
	//		float radius2 = rrr;

	//		int num_neighbours = find_neigbour_radius(kd_tree2, search_pt2, radius2, pt_idx_search2, pt_sqr_dis2);


	//		//===================================================

	//		std::vector<int> remove2;

	//		for (size_t i = 0; i < pt_idx_search2.size(); i++) 
	//		{
	//			float x3 = cloud0->points[pt_idx_search2[i]].x;
	//			float y3 = cloud0->points[pt_idx_search2[i]].y;
	//			float z3 = cloud0->points[pt_idx_search2[i]].z;
	//			float d2 = ((x3 - xs) * nxs + (y3 - ys) * nys + (z3 - zs) * nzs);
	//			d2 = abs(d2);

	//			if (d2 < 0.04)
	//			{
	//				remove2.push_back(pt_idx_search2[i]);
	//			}
	//		}

	//		sort(remove2.begin(), remove2.end());
	//		cloud0 = del2(cloud0, remove2);
	//	}

	//	std::string path_0shengyu = path + "cloud0_shengyu.ply";

	//	pathlist.emplace_back(path_0shengyu);

	//	//pcl::io::savePCDFileBinary(path_0shengyu, *cloud0);
	//	pcl::io::savePLYFileBinary(path_0shengyu, *cloud0);

	//}

	std::cout << "\n\n" << " *****  happy ending  *****" << "\n\n";

	vec_yuan = o;

	return make_pair(o, pathlist);

}


Yuan gcp::add(float x, float y, float z, std::string path_cloud, std::string path, int cloudName,
	std::string path_cloudline, int name, int diedai,  int switch_key)
{

	Yuan test; // return null
	if (!x || !y || !z)
		return test;


	//std::string path_cloudline;

	//float x = 0, y = 0, z = 0;
	// 
	//int switch_key = 0;

	//std::string path = "";

	//int diedai = 1;
	int i_min = 240;
	int i_max = 255;
	int d_max = 100;

	float r_index = 1;

	int line_key = 0;

	int ff = 0;

	Yuan yyy0;


	if (path_cloudline.length() > 1) {
		line_key = 1;

	}


	if (switch_key == 1) {
		d_max = 0.5;
	}



	pcl::PointCloud<pcl::PointXYZI>::Ptr cloud0(new pcl::PointCloud<pcl::PointXYZI>);

	//io::loadPCDFile(path_cloud, *cloud0);
	io::loadPLYFile(path_cloud, *cloud0);

	pcl::PointCloud<pcl::PointXYZIT>::Ptr cloudline_t(new pcl::PointCloud<pcl::PointXYZIT>);

	if (line_key != 0) {
		io::loadPLYFile(path_cloudline, *cloudline_t);

	}

	pcl::PointCloud<pcl::PointXYZI>::Ptr final(new pcl::PointCloud<pcl::PointXYZI>);

	pcl::KdTreeFLANN<pcl::PointXYZI> kd_tree;
	kd_tree.setInputCloud(cloud0);

	pcl::PointXYZI search_pt;
	search_pt.x = x;
	search_pt.y = y;
	search_pt.z = z;

	std::vector<int> pt_idx_search;  
	std::vector<float> pt_sqr_dis; 
	float r = 0.4;
	int num_neighbours = find_neigbour_radius(kd_tree, search_pt, r, pt_idx_search, pt_sqr_dis);

	pcl::PointCloud<pcl::PointXYZI>::Ptr cloud1(new pcl::PointCloud<pcl::PointXYZI>);

	pcl::copyPointCloud<pcl::PointXYZI>(*cloud0, pt_idx_search, *cloud1);

	pcl::PointCloud<pcl::PointXYZI>::Ptr cloud2(new pcl::PointCloud<pcl::PointXYZI>);

	cloud2 = pth_filter(cloud1, i_min, i_max);

	//cloud = uni_dsp(cloud, 0.005);

	//cout << cloud->points.size() << "\n\n";

	cloud2 = sor(cloud2, 10, 1);//sor

	cloud2 = mls(cloud2, 0.3, 1, 0.005);//mls

	for (int k = 0; k < diedai; k++) {


		std::cout << "=====" << "\n\n";

		std::cout << cloud2->points.size() << "\n\n\n\n";

		pcl::PointCloud<pcl::PointXYZI>::Ptr f(new pcl::PointCloud<pcl::PointXYZI>);


		std::vector<int> inliers;

		pcl::SampleConsensusModelCircle3D<pcl::PointXYZI>::Ptr
			model_circle3D(new pcl::SampleConsensusModelCircle3D<pcl::PointXYZI>(cloud2));

		pcl::RandomSampleConsensus<pcl::PointXYZI> ransac(model_circle3D);
		//ransac.setMaxIterations(15000);
		ransac.setDistanceThreshold(0.001);
		ransac.computeModel();
		ransac.getInliers(inliers);

		Eigen::VectorXf model;

		ransac.getModelCoefficients(model);
		std::cout << model << "\n\n";

		std::cout << inliers.size() << "\n\n";

		if (inliers.size() < 20)

		{
			std::cout << "=====break=====" << "\n\n";
			break;
		}

		float x0 = model[0];
		float y0 = model[1];
		float z0 = model[2];

		float r = model[3];

		float nx = model[4];
		float ny = model[5];
		float nz = model[6];

		//float dn = pow(xn, 2) + pow(yn, 2) + pow(zn, 2);
		//std::cout << "dn"<<dn << "\n\n";
		//std::cout << "\n" << x << "\n" << y << "\n" << z << "\n" << r <<  "\n\n";
		//=================================================================
		pcl::KdTreeFLANN<pcl::PointXYZI> kd_tree;

		kd_tree.setInputCloud(cloud2);


		pcl::PointXYZI search_pt;
		search_pt.x = x0;
		search_pt.y = y0;
		search_pt.z = z0;

		std::vector<int> pt_idx_search;  
		std::vector<float> pt_sqr_dis;
		float rr = r + 0.04;
		float radius = rr;

		int num_neighbours = find_neigbour_radius(kd_tree, search_pt, radius, pt_idx_search, pt_sqr_dis);


		//for (size_t i = 0; i < pt_idx_search.size(); i++)  
		//{
		//	std::cout << pt_idx_search[i] << "(" << cloud->points[pt_idx_search[i]].x << ","
		//		<< cloud->points[pt_idx_search[i]].y << ","
		//		<< cloud->points[pt_idx_search[i]].z << ")"
		//		<< "dis: " << pt_sqr_dis[i] << "; radius: " << radius << std::endl;
		//}

		//pcl::PointCloud<pcl::PointXYZI>::iterator r_index = cloud->begin();

		std::vector<int> remove;

		for (size_t i = 0; i < pt_idx_search.size(); i++)  
		{
			float x1 = cloud2->points[pt_idx_search[i]].x;
			float y1 = cloud2->points[pt_idx_search[i]].y;
			float z1 = cloud2->points[pt_idx_search[i]].z;
			float d = ((x1 - x0) * nx + (y1 - y0) * ny + (z1 - z0) * nz);
			d = abs(d);

			//std::cout << pt_idx_search[i] << "..";

			if (d < 0.02)
			{

				remove.push_back(pt_idx_search[i]);
				//cloud->points[pt_idx_search[i]].x = 0;
				//cloud->points[pt_idx_search[i]].y = 0;
				//cloud->points[pt_idx_search[i]].z = 0;
			}

		}


		sort(remove.begin(), remove.end());


		pcl::copyPointCloud<pcl::PointXYZI>(*cloud2, remove, *f);

		//inlier
		std::cout << f->points.size() << "\n\n";


		//int nn = remove.size();
		/*for(int i = 0; i < nn;i++ ){
			std::cout << remove[i] << "\n\n";

		}*/


		cloud2 = del2(cloud2, remove);

		//pcl::io::savePCDFileBinary("F:/PCD_data/gcp/f.pcd", *final);
		//del2(cloud, inliers);
		//pcl::io::savePCDFileBinary("F:/PCD_data/gcp/san.pcd", *cloud);

		if (r < 0.1 || 0.2 < r)
		{
			std::cout << "tiaoguo" << "\n\n";
			continue;
		}

		if (inliers.size() < 20)
		{
			std::cout << "tiaoguo" << "\n\n";
			continue;
		}

		float t = 0;
		if (line_key != 0) {
			//pair<float, float> p1 = get_time(x0, y0, z0, cloudline, cloudline_t);
			pair<float, float> p1 = get_time_t(x0, y0, z0, cloudline_t);


			t = p1.first;
			float distance = p1.second;;

			if (distance > d_max)
			{
				std::cout << "tiaoguo" << "\n\n";
				continue;
			}

		}


		// index avg 
		if (line_key == 0) {

			pcl::KdTreeFLANN<pcl::PointXYZI> kd_tree;
			kd_tree.setInputCloud(cloud0);

			pcl::PointXYZI search_pt;
			search_pt.x = x0;
			search_pt.y = y0;
			search_pt.z = z0;

			//std::vector<int> pt_idx_search;  
			Indices pt_idx_search;
			std::vector<float> pt_sqr_dis;  
			float radius = r_index;

			int num_neighbours = find_neigbour_radius2(kd_tree, search_pt, radius, pt_idx_search, pt_sqr_dis);
			//for (size_t i = 0; i < pt_idx_search.size(); i++) 
			//{
			//	std::cout << pt_idx_search[i] << "(" << cloud->points[pt_idx_search[i]].x << ","
			//		<< cloud->points[pt_idx_search[i]].y << ","
			//		<< cloud->points[pt_idx_search[i]].z << ")"
			//		<< "dis: " << pt_sqr_dis[i] << "; radius: " << radius << std::endl;
			//}
			float avg = 0;
			std::cout << "index=======" << "\n\n";
			for (size_t i = 0; i < pt_idx_search.size(); i++) 
			{
				avg = avg + pt_idx_search[i] / pt_idx_search.size();
				/*if (pt_idx_search[i]<0) {
					std::cout << pt_idx_search[i] << "..";
					std::cout << "===============fule";
					return 0;
				}*/
				//std::cout << avg << "..";
				//std::cout << pt_idx_search[i] << "..";
			}
			//avg = avg / pt_idx_search.size();
			std::cout << "=========================";
			std::cout << avg << "\n\n";
			t = int(avg);


		}



		Yuan yu;
		yu.x = x0;
		yu.y = y0;
		yu.z = z0;
		yu.r = r;

		yu.t = t;
		yu.n = inliers.size();


		/*if (ff == 0) {

			o[0] = yu;

		}

		if (ff > 0) {*/

		if (yu.n > yyy0.n) {
			//o[0] = yu;

			yyy0 = yu;
			final = f;
		}

		//}
		//o.emplace_back(yu);


		ff++;

		std::cout << ff << "\n\n";


		std::cout << "size" << cloud2->points.size() << "\n\n\n\n";

		if (cloud2->points.size() == 0) {
			break;
		}


	}

	if (ff == 0) {

		return yyy0;
	}
	else if (ff == 1) {

	}
	else if (ff > 1) {
	}
	yyy0.name = cloudName;


	std::string path2 = path + "gcp_" + to_string(cloudName) + ".ply";
	pcl::io::savePLYFileBinary(path2, *final);


	return yyy0;

}