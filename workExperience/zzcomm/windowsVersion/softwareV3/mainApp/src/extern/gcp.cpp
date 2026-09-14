#define PCL_NO_PRECOMPILE 

#include "../include/include_gui/gcp.h"
#include "../include/include_gui/widget_newProcess.h"

gcp::gcp(widget_newProcess* parent)
    : QWidget(parent)
    //, gcpIndex(0)

{
    parent_ = dynamic_cast<widget_newProcess*>(parent);
    //init();
}

gcp::~gcp()
{}

void gcp::closeEvent(QCloseEvent* event) {

}
//
//void gcp::init()
//{
//	//std::string path_0shengyu = "E:\\200\\qq.ply";
//	//pcl::PointCloud<pcl::PointXYZI>::Ptr cloud0(new pcl::PointCloud<pcl::PointXYZI>);//最初始电云
//	//io::loadPCDFile("E:\\200\\1.pcd", *cloud0);
//	//pcl::io::savePLYFileBinary(path_0shengyu, *cloud0);
//
//	// set param
//	this->manualCheck = parent_->getManualCheck();
//	this->radMax = parent_->getRadMax();
//	this->radMin = parent_->getRadMin();
//	this->no_of_gcp = parent_->getNoOfGcp();
//	this->intensityMax = parent_->getIntensityMax();
//	this->intensityMin = parent_->getIntensityMin();
//	this->iteration = parent_->getIteration();
//	this->circlePts = parent_->getCirclePts();
//	this->filterDist = parent_->getFilterDist();
//	this->filterDistance = parent_->getFilterDistance();
//	this->filterZCoor = parent_->getFilterZCoor();
//	this->filterZCoorMax = parent_->getFilterZCoorMax();
//	this->filterZCoorMin = parent_->getFilterZCoorMin();
//	//std::cout << "gcp ppp ================= " << std::endl;
//	//std::cout << "r max " << std::to_string(this->radMax) << std::endl;
//	//std::cout << "r min " << std::to_string(this->radMin) << std::endl;
//	//std::cout << "gcps " << std::to_string(this->no_of_gcp) << std::endl;
//	//std::cout << "i max " << std::to_string(this->intensityMax) << std::endl;
//	//std::cout << "i min " << std::to_string(this->intensityMin) << std::endl;
//	//std::cout << "d " << std::to_string(this->iteration) << std::endl;
//	//std::cout << "min circle " << std::to_string(this->circlePts) << std::endl;
//	//std::cout << "min break " << std::to_string(this->circlePts / 4) << std::endl;
//	//std::cout << "switch " << std::to_string(this->filterDist) << std::endl;
//	//std::cout << "d max1 " << std::to_string(this->filterDistance) << std::endl;
//	//std::cout << "zk " << std::to_string(this->filterZCoor) << std::endl;
//	//std::cout << "z max " << std::to_string(this->filterZCoorMax) << std::endl;
//	//std::cout << "z min " << std::to_string(this->filterZCoorMin) << std::endl;
//}
//
//void gcp::mergeFiles(QStringList l, QString path) {
//	
//	pcl::PointCloud<pcl::PointXYZI>::Ptr cloudadd(new pcl::PointCloud<pcl::PointXYZI>);	
//	pcl::io::loadPLYFile(l.at(0).toStdString(), *cloudadd); // at first ply file
//
//
//	pcl::PointCloud<pcl::PointXYZI>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZI>);
//
//	for (int i = 1; i<l.size(); i++) {	
//		pcl::io::loadPLYFile(l.at(i).toStdString(), *cloud);	
//		*cloudadd += *cloud;
//		cloud->clear();
//	}
//
//	pcl::io::savePLYFileBinary(path.toStdString() + "/"+"gcp_output.ply", *cloudadd);
//	std::cout << "merge files done" << std::endl;
//
//}
//
//
//
//bool cmpt(Yuan s1, Yuan s2) {  // 自定义排序
//    return s1.t < s2.t; // 按照时间升序排列
//}
//bool cmpn(Yuan s1, Yuan s2) {  // 自定义排序
//	return s1.n > s2.n; // 按照n降序排列
//}
//bool cmp_float(const double& a, const double& b)
//{
//    return a < b;// 按照时间升序排列
//}
//
//int gcp::gcp_save(int line_key, vector<Yuan> o, std::string path_txt) {
//
//	ofstream dataFile;
//	dataFile.open(path_txt, ofstream::app);
//	fstream file(path_txt, ios::out);
//	dataFile << line_key << endl;
//	for (int i = 0; i < o.size(); i++) {
//
//		dataFile << o[i].name << " " << o[i].x << " " << o[i].y << " " << o[i].z << " " << o[i].r << " " << std::fixed << setprecision(6) << o[i].t << " " <<
//			o[i].x_path << " " << o[i].y_path << " " << o[i].z_path << " " << o[i].rx << " " << o[i].ry << " " << o[i].rz << endl;
//	}
//	return 0;
//}
//
//vector<Yuan> gcp::gcp_load(std::string path_txt) {
//	vector<Yuan> o;
//	//Yuan y1;
//
//	std::string line_tmp;
//	vector<string> lines;
//	int line_key = -1;
//	//从文档读数据
//	ifstream ifs;
//
//	ifs.open(path_txt);//打开文件
//	if (!ifs)
//	{
//		cout << "无法打开文件！" << endl;
//		exit(1);
//	}
//
//	while (getline(ifs, line_tmp))    //获取每一行数据
//	{
//		lines.push_back(line_tmp);   //将每一行依次存入到vector中
//		cout << line_tmp << endl;    //顺便打印一下这一行
//	}
//	ifs.close();
//
//	//文档读完了
//
//	//处理文档数据
//
//	//有时候最后一个好像是多的 要报错  需要给删了
//	//lines.resize(lines.size() - 1);
//
//	cout << "y0" << "\n\n" << endl;
//	line_key = stoi(lines[0]);
//
//	cout << "一共有几行===" << lines.size() << "\n\n" << endl;
//
//	for (int i = 1; i < lines.size(); i++)
//	{
//		vector<string> Yuan_params = split(lines[i], ' ');
//
//		cout << Yuan_params.size() << "\n\n" << endl;
//		cout << Yuan_params[0] << " " << Yuan_params[1] << " " << Yuan_params[2] << " " << Yuan_params[3] << " " << Yuan_params[4] << " " << Yuan_params[5] << " " << Yuan_params[6] << " " << Yuan_params[7] << " " << Yuan_params[8] << "\n" << endl;
//		Yuan y0;
//
//		y0.name = stoi(Yuan_params[0]);
//		y0.x = stof(Yuan_params[1]);
//		y0.y = stof(Yuan_params[2]);
//		y0.z = stof(Yuan_params[3]);
//		y0.r = stof(Yuan_params[4]);
//		y0.t = stof(Yuan_params[5]);
//
//		y0.x_path = stof(Yuan_params[6]);
//		y0.y_path = stof(Yuan_params[7]);
//		y0.z_path = stof(Yuan_params[8]);
//
//
//		y0.rx = stof(Yuan_params[9]);
//		y0.ry = stof(Yuan_params[10]);
//		y0.rz = stof(Yuan_params[11]);
//		cout << y0.name << " " << y0.x << " " << y0.y << " " << y0.z << " " << y0.r << " " << y0.t <<
//			" " << y0.x_path << " " << y0.y_path << " " << y0.z_path << "\n" << endl;
//
//		o.emplace_back(y0);
//	}
//
//	cout << "oi" << endl;    //顺便打印一下这一行
//	for (int i = 0; i < o.size(); i++)
//	{
//
//		cout << o[0].x << " " << o[i].y << " " << o[i].z << " " << o[i].r << " " << o[i].t << " " << o[i].x_path << " " << o[i].y_path << " " << o[i].z_path << "\n" << endl;
//
//	}
//
//	return o;
//}
////可视化
//pcl::visualization::PCLVisualizer::Ptr gcp::simpleVis(pcl::PointCloud<pcl::PointXYZI>::ConstPtr cloud)
//{
//	pcl::visualization::PCLVisualizer::Ptr viewer(new pcl::visualization::PCLVisualizer("3D Viewer"));
//	viewer->setBackgroundColor(0, 0, 0);
//	viewer->addPointCloud<pcl::PointXYZI>(cloud, "sample cloud");
//	viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 3, "sample cloud");
//	viewer->initCameraParameters();
//	return (viewer);
//}
////体素滤波下采样
//pcl::PointCloud<pcl::PointXYZI>::Ptr gcp::vox_dsp(pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_zhen, float x) {// cloud_zhen 为指针变量，output也是指针变量
//	pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_filtered(new pcl::PointCloud<pcl::PointXYZI>);//体素滤波下采样
//	pcl::VoxelGrid<pcl::PointXYZI> sor;
//	sor.setInputCloud(cloud_zhen);             //输入点云指针
//	sor.setLeafSize(x, x, x); //体素滤波器，单位m
//	sor.filter(*cloud_filtered);          //滤波后的点云
//	return cloud_filtered;
//}
////均匀滤波下采样
//pcl::PointCloud<pcl::PointXYZI>::Ptr gcp::uni_dsp(pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_zhen, float x) {// cloud_zhen 为指针变量，output也是指针变量
//
//
//	pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_sampling(new pcl::PointCloud<pcl::PointXYZI>());//均匀滤波下采样
//
//	pcl::UniformSampling<pcl::PointXYZI> sor;
//
//
//	sor.setInputCloud(cloud_zhen);
//	sor.setRadiusSearch(x);
//	sor.filter(*cloud_sampling);
//
//
//	//结果保存
//	//pcl::PLYWriter writer;
//	//writer.write<pcl::PointXYZRGB>("table_scene_lms400_inliers.ply", *cloud_sampling, false);
//
//
//	return cloud_sampling;
//
//}
//sor
pcl::PointCloud<pcl::PointXYZI>::Ptr gcp::sor(pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_zhen, int n, float e) {//  n=附近点数量，e=标准差系数
	pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_out(new pcl::PointCloud<pcl::PointXYZI>);//统计滤波去离群点
	pcl::StatisticalOutlierRemoval<pcl::PointXYZI> outrem;//创建统计滤波对象
	outrem.setInputCloud(cloud_zhen);
	outrem.setMeanK(n);//设置查询点的半径范围
	outrem.setStddevMulThresh(e);//设置判断是否为离群点的阈值，即半径内至少包括的点数
	outrem.filter(*cloud_out);
	return cloud_out;
}
////MLS  
//pcl::PointCloud<pcl::PointXYZI>::Ptr gcp::mls(pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_zhen, float r, int n, float g) {// input-->cloud_zhen 为指针变量，output是点云（XYZRGB）
//	pcl::PointCloud<pcl::PointXYZI>::Ptr mls_points{ new pcl::PointCloud<pcl::PointXYZI>() };
//	pcl::MovingLeastSquares<pcl::PointXYZI, pcl::PointXYZI> MLS;
//	pcl::search::KdTree<pcl::PointXYZI>::Ptr tree(new pcl::search::KdTree<pcl::PointXYZI>());
//	MLS.setSearchMethod(tree);
//	MLS.setSearchRadius(r);//0.1 初始
//	MLS.setComputeNormals(false);
//
//	MLS.setInputCloud(cloud_zhen);
//	MLS.setPolynomialOrder(n);//2 初始
//	MLS.setSqrGaussParam(g);//0.005 初始
//	MLS.process(*mls_points);
//
//	return mls_points;
//}
////直通滤波
//pcl::PointCloud<pcl::PointXYZI>::Ptr gcp::pth_filter(pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_zhen, float i_min, int i_max) {// input-->cloud_zhen 为指针变量，output是点云（XYZRGB）
//
//	pcl::PointCloud<pcl::PointXYZI>::Ptr cloud(new       pcl::PointCloud<pcl::PointXYZI>);
//
//
//
//	/* 声明直通滤波 的 类实例 */
//	pcl::PassThrough< pcl::PointXYZI >  pass;
//	/* 设置输入点云 */
//	pass.setInputCloud(cloud_zhen);
//	/* 设置滤波的维度 */
//	pass.setFilterFieldName("intensity");
//	/* 设置滤波的范围 */
//	pass.setFilterLimits(i_min, i_max);
//	/* 设置去掉的 是 范围内（true） 还是 范围外（false）   */
//	pass.setNegative(0);
//	/* 执行滤波 返回 滤波后 的 点云 */
//	pass.filter(*cloud);
//
//
//	return cloud;
//}
////直通滤波z
//pcl::PointCloud<pcl::PointXYZI>::Ptr gcp::pth_filter_z(pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_zhen, float z_min, int z_max) {// input-->cloud_zhen 为指针变量，output是点云（XYZRGB）
//
//	pcl::PointCloud<pcl::PointXYZI>::Ptr cloud(new       pcl::PointCloud<pcl::PointXYZI>);
//
//
//
//	/* 声明直通滤波 的 类实例 */
//	pcl::PassThrough< pcl::PointXYZI >  pass;
//	/* 设置输入点云 */
//	pass.setInputCloud(cloud_zhen);
//	/* 设置滤波的维度 */
//	pass.setFilterFieldName("z");
//	/* 设置滤波的范围 */
//	pass.setFilterLimits(z_min, z_max);
//	/* 设置去掉的 是 范围内（true） 还是 范围外（false）   */
//	pass.setNegative(0);
//	/* 执行滤波 返回 滤波后 的 点云 */
//	pass.filter(*cloud);
//
//
//	return cloud;
//}
////vector删除点云
//pcl::PointCloud<pcl::PointXYZI>::Ptr gcp::del(pcl::PointCloud<pcl::PointXYZI>::Ptr cloud, vector<int> index)
//{
//	int n = (*cloud).size();
//	vector<int>I = {};
//	//I.reserve(n);
//	I.resize(n);
//	for (int i = 0; i < n; i++)
//	{
//		I[i] = i;
//	}
//
//	int n1 = index.size() - 1;
//
//	for (int i = n1; i >= 0; i--)
//	{
//		//int a = index[i];
//		//std::cout << I.size() << " " << index[i] << std::endl;
//		I.erase(I.begin() + index[i]);
//	}
//
//	pcl::PointCloud<pcl::PointXYZI>::Ptr cloud2(new PointCloud<PointXYZI>);
//	pcl::copyPointCloud(*cloud, I, *cloud2);
//	cloud = cloud2;
//
//	return cloud;
//}
////vector删除点云,swap
//pcl::PointCloud<pcl::PointXYZI>::Ptr gcp::del2(pcl::PointCloud<pcl::PointXYZI>::Ptr cloud, vector<int> index)
//{
//	int n0 = (*cloud).size();
//	int n1 = index.size();
//	int n2 = n0 - n1;
//
//
//
//	for (int i = 0; i < n1; i++) {
//		int k = n0 - 1 - i;
//		int j = index[n1 - 1 - i];
//		swap(cloud->points[j], cloud->points[k]);
//
//
//	}
//
//	//cloud->points.size,width = n2;
//	cloud->points.resize(n2);
//	(*cloud).width = n2;
//
//
//	return cloud;
//}
////1 查询所有的在范围r内近邻点
//bool gcp::find_neigbour_radius(pcl::KdTreeFLANN<pcl::PointXYZI> kd_tree, pcl::PointXYZI search_pt, float radius, std::vector<int>& pt_idx_search, std::vector<float>& pt_sqr_dis)
//{
//	int num_neighbours = kd_tree.radiusSearch(search_pt, radius, pt_idx_search, pt_sqr_dis);
//	return num_neighbours;
//}
//bool gcp::find_neigbour_radius2(pcl::KdTreeFLANN<pcl::PointXYZI> kd_tree, pcl::PointXYZI search_pt, float radius, Indices& pt_idx_search, std::vector<float>& pt_sqr_dis)
//{
//	int num_neighbours = kd_tree.radiusSearch(search_pt, radius, pt_idx_search, pt_sqr_dis);
//	return num_neighbours;
//}
////kd树搜索最近点
//bool gcp::find_k_nearest_neigbour(pcl::KdTreeFLANN<pcl::PointXYZI> kd_tree, pcl::PointXYZI search_pt, int k, std::vector<int>& pt_idx_search, std::vector<float>& pt_sqr_dis)
//{
//	// 执行k近邻搜索
//	int result = kd_tree.nearestKSearch(search_pt, k, pt_idx_search, pt_sqr_dis);
//	return result;
//}
////xyzit的
//bool gcp::find_neigbour_radius_t(pcl::KdTreeFLANN<pcl::PointXYZIT> kd_tree, pcl::PointXYZIT search_pt, float radius, std::vector<int>& pt_idx_search, std::vector<float>& pt_sqr_dis)
//{
//	int num_neighbours = kd_tree.radiusSearch(search_pt, radius, pt_idx_search, pt_sqr_dis);
//	return num_neighbours;
//
//}
//bool gcp::find_k_nearest_neigbour_t(pcl::KdTreeFLANN<pcl::PointXYZIT> kd_tree, pcl::PointXYZIT search_pt, int k, std::vector<int>& pt_idx_search, std::vector<float>& pt_sqr_dis)
//{
//	// 执行k近邻搜索
//	int result = kd_tree.nearestKSearch(search_pt, k, pt_idx_search, pt_sqr_dis);
//	return result;
//}
////找t
////kd树
//pair<double, float> gcp::get_time0(float x0, float y0, float z0, pcl::PointCloud<PointXYZI>::Ptr cloud, pcl::PointCloud<PointXYZIT>::Ptr cloud_t) {
//
//	float min_dis;
//	//int minIndex , tempIndex;
//	//int x0 = 0, y0 = 0, z0 = 0;
//	//minDistance = 1000000000;
//
//	// 2,创建kd-tree对象，并把上面的点云设置成输入，然后创建一个查询点，查询该点
//	pcl::KdTreeFLANN<pcl::PointXYZI> kd_tree;
//
//	kd_tree.setInputCloud(cloud);
//
//
//
//	// 3,创建一个查询点
//	pcl::PointXYZI search_pt;
//	search_pt.x = x0;
//	search_pt.y = y0;
//	search_pt.z = z0;
//
//
//	//float x = search_pt.x;
//	//float y = search_pt.y;
//	//float z = search_pt.z;
//
//
//	// 4, 查询所有的在半径r内邻居点
//	int k = 1;
//	std::vector<int> pt_idx_search;  // 存储查询到的最近邻的k个点的索引
//	std::vector<float> pt_sqr_dis;  //  存储近邻点对应的距离平方
//
//
//
//	//执行k近邻搜索
//	find_k_nearest_neigbour(kd_tree, search_pt, k, pt_idx_search, pt_sqr_dis);
//
//	//if (find_k_nearest_neigbour(kd_tree, search_pt, k, pt_idx_search, pt_sqr_dis) > 0)
//	//{
//	//	for (size_t i = 0; i < pt_idx_search.size(); i++)  // 打印所有的近邻点坐标和距离
//	//	{
//	//		std::cout << "(" << cloud->points[pt_idx_search[i]].x << "  "
//	//			<< cloud->points[pt_idx_search[i]].y << "  "
//	//			<< cloud->points[pt_idx_search[i]].z << ")"
//	//			<< "dis: " << pt_sqr_dis[i] << "\n" << std::endl;
//	//	}
//	//}
//
//	int j = pt_idx_search[0];
//	min_dis = pt_sqr_dis[0];
//
//	//float min_dis = pow((x - cloud->points[j].x), 2) + pow((y - cloud->points[j].y), 2) + pow((z - cloud->points[j].z), 2);
//
//	double t = cloud_t->points[j].time;
//
//	return make_pair(t, min_dis);
//
//}
////xyzit kd树
//pair<double, vector<float>> gcp::get_time_t(float x0, float y0, float z0, pcl::PointCloud<PointXYZIT>::Ptr cloud) {
//
//	vector<float> f;
//
//	float min_dis;
//	//int minIndex , tempIndex;
//	//int x0 = 0, y0 = 0, z0 = 0;
//	//minDistance = 1000000000;
//
//	// 2,创建kd-tree对象，并把上面的点云设置成输入，然后创建一个查询点，查询该点
//	pcl::KdTreeFLANN<pcl::PointXYZIT> kd_tree;
//
//	kd_tree.setInputCloud(cloud);
//
//
//
//	// 3,创建一个查询点
//	pcl::PointXYZIT search_pt;
//	search_pt.x = x0;
//	search_pt.y = y0;
//	search_pt.z = z0;
//
//
//	//float x = search_pt.x;
//	//float y = search_pt.y;
//	//float z = search_pt.z;
//
//
//	// 4, 查询所有的在半径r内邻居点
//	int k = 1;
//	std::vector<int> pt_idx_search;  // 存储查询到的最近邻的k个点的索引
//	std::vector<float> pt_sqr_dis;  //  存储近邻点对应的距离平方
//
//
//
//	//执行k近邻搜索
//	find_k_nearest_neigbour_t(kd_tree, search_pt, k, pt_idx_search, pt_sqr_dis);
//
//	//if (find_k_nearest_neigbour(kd_tree, search_pt, k, pt_idx_search, pt_sqr_dis) > 0)
//	//{
//	//	for (size_t i = 0; i < pt_idx_search.size(); i++)  // 打印所有的近邻点坐标和距离
//	//	{
//	//		std::cout << "(" << cloud->points[pt_idx_search[i]].x << "  "
//	//			<< cloud->points[pt_idx_search[i]].y << "  "
//	//			<< cloud->points[pt_idx_search[i]].z << ")"
//	//			<< "dis: " << pt_sqr_dis[i] << "\n" << std::endl;
//	//	}
//	//}
//
//	int j = pt_idx_search[0];
//	min_dis = pt_sqr_dis[0];
//
//	//float min_dis = pow((x - cloud->points[j].x), 2) + pow((y - cloud->points[j].y), 2) + pow((z - cloud->points[j].z), 2);
//
//	double t = cloud->points[j].time;
//	float xp = cloud->points[j].x;
//	float yp = cloud->points[j].y;
//	float zp = cloud->points[j].z;
//
//	f.emplace_back(min_dis);
//	f.emplace_back(xp);
//	f.emplace_back(yp);
//	f.emplace_back(zp);
//
//	return make_pair(t, f);
//
//}
//
////循环
//pair<double, float> gcp::get_time2(float x0, float y0, float z0, pcl::PointCloud<PointXYZIT>::Ptr pathCloud) {
//
//	float minDistance, tempDistance;
//	int minIndex, tempIndex;
//	//int x0 = 0, y0 = 0, z0 = 0;
//	int x1 = 0, y1 = 0, z1 = 0;
//	//minDistance = 1e10;
//	minDistance = 1000000000;
//
//	for (int j = 0; j < pathCloud->size(); j++)
//	{
//		x1 = pathCloud->points[j].x;
//		y1 = pathCloud->points[j].y;
//		z1 = pathCloud->points[j].z;
//		tempDistance = pow((x0 - x1), 2) + pow((y0 - y1), 2) + pow((z0 - z1), 2);
//
//		if (j == 0)
//		{
//			minDistance = tempDistance;
//			continue;
//		}
//
//		if (tempDistance < minDistance)
//		{
//			minDistance = tempDistance;
//			minIndex = j;
//		}
//	}
//	double t = pathCloud->points[minIndex].time;
//	return make_pair(t, minDistance);
//
//}
////识别重复靶点
//vector<double> gcp::chongfubadian(float x, float y, float z, pcl::PointCloud<pcl::PointXYZIT>::Ptr cloudline, float d_max) {
//
//	//float d_max = 0.5;
//
//	//pcl::PointCloud<pcl::PointXYZIT>::Ptr cloudline(new pcl::PointCloud<pcl::PointXYZIT>);//路径点云xyzit
//
//	pcl::PointCloud<pcl::PointXYZIT>::Ptr cloud1(new pcl::PointCloud<pcl::PointXYZIT>);//U点云，从进到出
//
//	//std::string path = "F:/PCD_data/0/line_1.pcd";
//
//	//io::loadPCDFile(path, *cloudline);
//	//io::loadPLYFile(path, *cloudline);
//
//	//float x = 77;
//	//float y = 74;
//	//float z = 0.2;
//
//	int key = 0;
//	int s = 0;
//	int w = 0;
//	vector<int> a;
//	vector<double> time;
//	for (int i = 0; i < cloudline->points.size(); i++) {
//
//
//
//
//		float d = pow((cloudline->points[i].x - x), 2) + pow((cloudline->points[i].y - y), 2) + pow((cloudline->points[i].z - z), 2);
//
//		if (key == 0 && d < d_max) {
//			key = 1;
//
//			s = i;
//		}
//
//
//		if (key == 1 && d > d_max) {
//			key = 0;
//			w = i;
//
//			for (int i = s; i < w; i++) {
//				a.emplace_back(i);
//			}
//
//			pcl::copyPointCloud<pcl::PointXYZIT>(*cloudline, a, *cloud1);
//			a.clear();
//
//			time.emplace_back(get_time_t(x, y, z, cloud1).first);
//
//
//		}
//
//
//
//	}
//
//	sort(time.begin(), time.end(), cmp_float);
//
//	cout << "共识别到的靶点个数   " << time.size() << "\n\n";
//	for (int i = 0; i < time.size(); i++)
//	{
//
//		cout << time[i] << "\n\n";
//
//	}
//
//
//	return time;
//
//}
////字符串分割
//std::vector<std::string> gcp::split(const std::string& str, char delim) {
//	std::stringstream ss(str);
//	std::string item;
//	std::vector<std::string> elems;
//	while (std::getline(ss, item, delim)) {
//		if (!item.empty()) {
//			elems.push_back(item);
//		}
//	}
//	return elems;
//}
////================================================================================================================================================
////输出  o vector<Yuan>  Yuan class的向量  ,pathlist 第0个是txt的路径，然后按顺序存靶点点云，然后cloudshengyu（强度过滤后的）和cloud0shengyu
//pair< std::vector<Yuan>, std::vector<std::string> > gcp::get_gcp(std::string path_cloud, std::string path, int a,   //一定需要客户手动输入的 
//	std::string path_cloudline, float r1, float r2 , int switch_key, float i_min, float  i_max, //可能需要客户输入的
//	int zk, float z_min, float z_max , int min_circle, float d_max_1, int auto_key, float d_min,//可能需要客户输入的
//	int min_break, float zr, int d_max, int d , float rd, float r_index,
//	int gcpIndex) {
//
//	// for merging
//	gcpIndex = this->gcpIndex;
//
//	// params from parent
//	r2 = this->radMax;
//	r1 = this->radMin;
//	a = this->no_of_gcp;
//	i_max = this->intensityMax;
//	i_min = this->intensityMin;
//	d = this->iteration;
//	min_circle = this->circlePts;
//	min_break = min_circle / 4;
//	switch_key = this->filterDist;
//	d_max_1 = this->filterDistance;
//	zk = this->filterZCoor;
//	z_max = this->filterZCoorMax;
//	z_min = this->filterZCoorMin;
//	// ==================
//	std::cout << "gcp ================= " << std::endl;
//	std::cout << "r max " << std::to_string(r2) << std::endl;
//	std::cout << "r min " << std::to_string(r1) << std::endl;
//	std::cout << "gcps " << std::to_string(a) << std::endl;
//	std::cout << "i max " << std::to_string(i_max) << std::endl;
//	std::cout << "i min " << std::to_string(i_min) << std::endl;
//	std::cout << "d " << std::to_string(d) << std::endl;
//	std::cout << "min circle " << std::to_string(min_circle) << std::endl;
//	std::cout << "min break " << std::to_string(min_break) << std::endl;
//	std::cout << "switch " << std::to_string(switch_key) << std::endl;
//	std::cout << "d max1 " << std::to_string(d_max_1) << std::endl;
//	std::cout << "zk " << std::to_string(zk) << std::endl;
//	std::cout << "z max " << std::to_string(z_max) << std::endl;
//	std::cout << "z min " << std::to_string(z_min) << std::endl;
//
//	//预设参数
//	//一定需要客户手动输入的
//	//std::string path_cloud = "F:/PCD_data/ceshi/h5_lvbo.pcd";//导入点云路径
//	//std::string path_cloudline = "F:/PCD_data/ceshi/h5_lvbo.ply";//xyzit 带时间的路径点云
//	//std::string path = "F:/PCD_data/gaofanshe/cunjieguo/";//大的保存路径
//	//int a = 5;//真实靶点个数
//	//可能需要客户输入的
//	//float r1 = 0.1, r2 = 0.14;
//	// int switch_key = 0 // 离路径过远的靶点不识别的开关，默认为关，0位关，赋值1为开。
//	//float i_min = 240, i_max = 255;
//	//int zk = 0; //是否开启z轴直通过滤
//	//float z_min = -1;
//	//float z_max = 0;
//	//int min_circle = 80;//圆上点最小个数
//	//auto_key; //自动去除多余靶点功能开关
//	//d_min = 5; //当开启自动识别  两靶点间的最小间距
//	//大概率不需要客户输入的
//	//int min_break = 20;//圆上最少点单条件结束
//	//int d_max = 100;//可接受的  靶点离路径的距离  如果不开启功能的值, 暂时建议为5
//	// int d_max_1 = 5;//靶点离路径的距离  开启功能的值, 建议为5
//	//int d = 10;//识别循环次数上限系数
//	//float rd = 0.06;//要删除的圆环半径差（+容错值）
//	//float r_index = 1;//求平均索引的半径
//	//float zr =0.02; // 删识别圆时的纵轴范围
//
//	int diedai = d * a;//识别循环次数上限
//	int f = 0;//满足条件（并经过保存)的圆
//	int line_key = 0;//是否有路径点云
//	float d_min2 = pow(d_min, 2);
//
//	if (path_cloudline.length() > 1) {
//		line_key = 1;
//
//	}
//
//	if (path_cloudline.length() <= 1) {
//		switch_key = 0;
//
//	}
//	//if (switch_key == 0) {//路径筛选条件打开，离路径4m外的靶点不识别
//	//	d_max = 100;
//	//}
//	if (switch_key == 1) {//路径筛选条件打开，离路径4m外的靶点不识别
//		d_max = d_max_1;
//	}
//
//	pcl::PointCloud<pcl::PointXYZI>::Ptr cloud0(new pcl::PointCloud<pcl::PointXYZI>);//最初始电云
//	//io::loadPCDFile(path_cloud, *cloud0);
//	io::loadPLYFile(path_cloud, *cloud0);
//	pcl::PointCloud<pcl::PointXYZIT>::Ptr cloudline_t(new pcl::PointCloud<pcl::PointXYZIT>);//最初始电云
//
//	//有路径点云才load相关点云
//	if (line_key != 0) {
//		//io::loadPCDFile(path_cloudline, *cloudline_t);
//		io::loadPLYFile(path_cloudline, *cloudline_t);
//
//	}
//
//
//	vector <Yuan>  o;
//
//	//单独存点云的
//	std::vector<pcl::PointCloud<pcl::PointXYZI>::Ptr> cloudlist0;
//	std::vector<pcl::PointCloud<pcl::PointXYZI>::Ptr> cloudlist1;
//
//	// 两个结果第一阶段存输出的
//	std::vector<float> circlelist;
//	std::vector<std::string> pathlist;
//
//	//强度直通滤波
//
///*  直通滤波 后 的点云 */
//	pcl::PointCloud<pcl::PointXYZI>::Ptr cloud(new       pcl::PointCloud<pcl::PointXYZI>);
//
//	cloud = cloud0;
//
//	//直通滤波
//
//	cloud = pth_filter(cloud, i_min, i_max);
//
//	if (zk != 0) {
//
//		cloud = pth_filter_z(cloud, z_min, z_max);
//
//	}
//
//	/*std::string path0 = path + "filter.ply";
//	pcl::io::savePLYFileBinary(path0, *cloud);*/
//	//强度过滤后两步步预处理（要用下采样只能用均匀，最好不用下采样）
//	//cloud = uni_dsp(cloud, 0.005);//调用下采样
//	//cout << cloud->points.size() << "\n\n";
//
//
//	cloud = sor(cloud, 10, 1);//sor
//	cloud = mls(cloud, 0.3, 1, 0.005);//mls
//
//
//
//	std::cout << "=== 预处理结束 ==" << "\n\n";
//
//	//pcl::io::savePCDFileBinary("F:/PCD_data/gaofanshe/cunjieguo/2.pcd", *cloud);
//	//迭代识别
//
//	for (int k = 0; k < diedai; k++) {
//
//		//for(int k=0; k<1;k++){
//		std::cout << "=====" << "\n\n";
//
//		pcl::PointCloud<pcl::PointXYZI>::Ptr final(new pcl::PointCloud<pcl::PointXYZI>);
//
//
//		std::vector<int> inliers;
//
//		pcl::SampleConsensusModelCircle3D<pcl::PointXYZI>::Ptr
//			model_circle3D(new pcl::SampleConsensusModelCircle3D<pcl::PointXYZI>(cloud));
//
//		pcl::RandomSampleConsensus<pcl::PointXYZI> ransac(model_circle3D);
//		//ransac.setMaxIterations(15000);
//		ransac.setDistanceThreshold(0.001);
//		ransac.computeModel();
//		ransac.getInliers(inliers);
//
//		Eigen::VectorXf model;
//
//		ransac.getModelCoefficients(model);
//		std::cout << model << "\n\n";
//
//
//		//如果找不到圆了就结束循环
//
//		std::cout << inliers.size() << "\n\n";
//		//if (inliers.size() == 0)   //或者改成小于阈值 
//		//if (inliers.size() < min_circle && f >= a)//并且，就算找到足够数量靶点也会把圆上点数量的条件循环跑完
//		if (inliers.size() < min_circle && f >= a)
//
//		{
//			std::cout << "=====break=====" << "\n\n";
//			break;
//		}
//
//		if (inliers.size() < min_break)
//
//		{
//			std::cout << "=====break=====" << "\n\n";
//			break;
//		}
//
//
//
//		//找圆结束
//
//		float x0 = model[0];
//		float y0 = model[1];
//		float z0 = model[2];
//
//		float r = model[3];
//		int n = inliers.size();
//
//		float xp = 0;
//		float yp = 0;
//		float zp = 0;
//
//		float nx = model[4];
//		float ny = model[5];
//		float nz = model[6];
//
//		//float dn = pow(xn, 2) + pow(yn, 2) + pow(zn, 2);
//		//std::cout << "dn"<<dn << "\n\n";
//		//std::cout << "\n" << x << "\n" << y << "\n" << z << "\n" << r <<  "\n\n";
//		//=================================================================
//		//邻近搜索, 从cloud中 删除搜索到的圆的点云
//
//
//		// 2,创建kd-tree对象，并把上面的点云设置成输入，然后创建一个查询点，查询该点
//		pcl::KdTreeFLANN<pcl::PointXYZI> kd_tree;
//
//		kd_tree.setInputCloud(cloud);
//
//
//		// 3,创建一个查询点
//		pcl::PointXYZI search_pt;
//		search_pt.x = x0;
//		search_pt.y = y0;
//		search_pt.z = z0;
//
//		// 4, 查询所有的在半径r内邻居点
//		std::vector<int> pt_idx_search;  // 存储查询到的最近邻的k个点的索引
//		std::vector<float> pt_sqr_dis;  //  存储近邻点对应的距离平方
//		float rr = r + rd;//多删除的（每个靶点只会被识别到一次，识别到内圆的话也会把整个靶点删掉）
//		float radius = rr;
//
//		// 执行k近邻搜索
//		int num_neighbours = find_neigbour_radius(kd_tree, search_pt, radius, pt_idx_search, pt_sqr_dis);
//
//		// 打印所有的近邻点坐标和距离
//
//		//for (size_t i = 0; i < pt_idx_search.size(); i++)  
//		//{
//		//	std::cout << pt_idx_search[i] << "(" << cloud->points[pt_idx_search[i]].x << ","
//		//		<< cloud->points[pt_idx_search[i]].y << ","
//		//		<< cloud->points[pt_idx_search[i]].z << ")"
//		//		<< "dis: " << pt_sqr_dis[i] << "; radius: " << radius << std::endl;
//		//}
//
//
//		// 在球内点云中，用投影判断,删除圆内的点
//
//		//pcl::PointCloud<pcl::PointXYZI>::iterator r_index = cloud->begin();
//
//		std::vector<int> remove;
//
//
//		for (size_t i = 0; i < pt_idx_search.size(); i++)  // 打印所有的近邻点坐标和距离
//		{
//			float x1 = cloud->points[pt_idx_search[i]].x;
//			float y1 = cloud->points[pt_idx_search[i]].y;
//			float z1 = cloud->points[pt_idx_search[i]].z;
//			float d = ((x1 - x0) * nx + (y1 - y0) * ny + (z1 - z0) * nz);
//			d = abs(d);
//
//			//std::cout << pt_idx_search[i] << "..";
//
//
//
//			if (d < zr)
//			{
//
//				remove.push_back(pt_idx_search[i]);
//
//				//点xyz=0代替删除
//
//				//cloud->points[pt_idx_search[i]].x = 0;
//				//cloud->points[pt_idx_search[i]].y = 0;
//				//cloud->points[pt_idx_search[i]].z = 0;
//
//
//			}
//
//		}
//
//
//		sort(remove.begin(), remove.end());
//
//
//
//
//		pcl::copyPointCloud<pcl::PointXYZI>(*cloud, remove, *final);
//
//		//
//		std::cout << final->points.size() << "\n\n";
//
//		//inlier
//
//		int nn = remove.size();
//		/*for(int i = 0; i < nn;i++ ){
//			std::cout << remove[i] << "\n\n";
//
//		}*/
//
//
//		cloud = del2(cloud, remove);
//
//
//
//		//保存提取的圆cloud
//
//		//pcl::io::savePCDFileBinary("F:/PCD_data/gcp/f.pcd", *final);
//		//del2(cloud, inliers);
//		//pcl::io::savePCDFileBinary("F:/PCD_data/gcp/san.pcd", *cloud);
//
//
//		//如果圆不符合提前设置的要求，就只删除圆和圆内点不保存进靶点列表
//		if (r < r1 || r2 < r)
//		{
//			std::cout << "tiaoguo" << "\n\n";
//			continue;
//		}
//
//		/*if (inliers.size() < min_circle)
//		{
//			std::cout << "tiaoguo" << "\n\n";
//			continue;
//		}*/
//
//		double t = 0;
//		//如果有路径点云，从点云路径里找t 
//		if (line_key != 0) {
//
//			pair<double, vector<float>> p1 = get_time_t(x0, y0, z0, cloudline_t);
//			t = p1.first;
//
//			vector<float> f = p1.second;
//			float distance = f[0];
//			xp = f[1];
//			yp = f[2];
//			zp = f[3];
//
//			if (distance > d_max)
//			{
//				std::cout << "tiaoguo" << "\n\n";
//				continue;
//			}
//
//		}
//
//		//如果没有路径点云，找index avg
//		if (line_key == 0) {
//
//			//邻近搜索
//			std::cout << "\n\n\n\n";
//			// 2,创建kd-tree对象，并把上面的点云设置成输入，然后创建一个查询点，查询该点
//			pcl::KdTreeFLANN<pcl::PointXYZI> kd_tree;
//			kd_tree.setInputCloud(cloud0);
//			// 3,创建一个查询点
//			pcl::PointXYZI search_pt;
//			search_pt.x = x0;
//			search_pt.y = y0;
//			search_pt.z = z0;
//			// 4, 查询所有的在半径r内邻居点
//			//std::vector<int> pt_idx_search;  // 存储查询到的最近邻的k个点的索引
//			Indices pt_idx_search;
//			std::vector<float> pt_sqr_dis;  //  存储近邻点对应的距离平方
//			float radius = r_index;
//			// 执行k近邻搜索
//			int num_neighbours = find_neigbour_radius2(kd_tree, search_pt, radius, pt_idx_search, pt_sqr_dis);
//			//for (size_t i = 0; i < pt_idx_search.size(); i++)  // 打印所有的近邻点坐标和距离
//			//{
//			//	std::cout << pt_idx_search[i] << "(" << cloud->points[pt_idx_search[i]].x << ","
//			//		<< cloud->points[pt_idx_search[i]].y << ","
//			//		<< cloud->points[pt_idx_search[i]].z << ")"
//			//		<< "dis: " << pt_sqr_dis[i] << "; radius: " << radius << std::endl;
//			//}
//			double avg = 0;
//			std::cout << "index=======" << "\n\n";
//			for (size_t i = 0; i < pt_idx_search.size(); i++)  // 打印所有的近邻点坐标和距离
//			{
//				avg = avg + pt_idx_search[i] / pt_idx_search.size();
//				/*if (pt_idx_search[i]<0) {
//					std::cout << pt_idx_search[i] << "..";
//					std::cout << "===============fule";
//					return 0;
//				}*/
//				//std::cout << avg << "..";
//				//std::cout << pt_idx_search[i] << "..";
//			}
//			//avg = avg / pt_idx_search.size();
//			std::cout << "=========================";
//			std::cout << avg;
//			t = int(avg);
//
//
//		}
//
//
//
//		//把点云和圆参数存下来
//		/*cloudlist.emplace_back(final);
//		circlelist.emplace_back(x0);
//		circlelist.emplace_back(y0);
//		circlelist.emplace_back(z0);
//		circlelist.emplace_back(r);
//		circlelist.emplace_back(nx);
//		circlelist.emplace_back(ny);
//		circlelist.emplace_back(nz);*/
//
//		Yuan yu;
//		yu.x = x0;
//		yu.y = y0;
//		yu.z = z0;
//		yu.r = r;
//
//		yu.t = t;
//		yu.n = n;
//
//		yu.x_path = xp;
//		yu.y_path = yp;
//		yu.z_path = zp;
//
//		yu.nx = nx;
//		yu.ny = ny;
//		yu.nz = nz;
//
//
//		//去除过近的靶点
//
//		int c_key = 0;
//		if (auto_key == 1 && f > 0) {
//			float dis = 0;
//			for (int i = 0; i < f; i++) {
//				dis = pow((o[i].x - yu.x), 2) + pow((o[i].y - yu.y), 2) + pow((o[i].z - yu.z), 2);
//				if (dis < d_min2) {
//					c_key = 1;
//					cout << " 识别到过近靶点 " << "\n\n";
//					cout << " tiaoguo " << "\n\n";
//					if (yu.n > o[i].n) {
//						o[i] = yu;
//					}
//					break;
//				}
//			}
//		}
//		if (c_key == 1) {
//			continue;
//		}
//
//
//		cloudlist0.emplace_back(final);
//
//		o.emplace_back(yu);
//
//
//		f++;
//
//		std::cout << f << "\n\n";
//
//		if (f >= 1.2 * a) {
//
//			cout << " 已识别到过多靶点 " << "\n\n";
//			cout << " break " << "\n\n";
//			break;
//
//		}
//
//		/*for (auto i : remove)
//		{
//			cout << i << "..";
//		}
//		cout << endl;*/
//		//pcl::io::savePCDFileBinary("F:/PCD_data/gcp/f.pcd", *cloud);
//		//end
//		// 
//		// 
//		// 
//		//可视化
//		//pcl::visualization::PCLVisualizer::Ptr viewer;
//		//if ( pcl::console::find_argument(argc, argv, "-c3d") >= 0)
//		//	viewer = simpleVis(final);
//		//else
//		//	viewer = simpleVis(cloud);
//		//while (!viewer->wasStopped())
//		//{
//		//	viewer->spinOnce(100);
//		//	std::this_thread::sleep_for(100ms);
//		//}
//
//	}
//
//	std::cout << f << "\n\n";
//
//	// 识别过程结束 ==========================================
//	//如果没有路径点云，找index avg
//
//	//if (line_key == 0) {
//
//	//	for (int i = 0; i < o.size(); i++) {
//	//		//邻近搜索
//	//		std::cout << "\n\n\n\n";
//	//		// 2,创建kd-tree对象，并把上面的点云设置成输入，然后创建一个查询点，查询该点
//	//		pcl::KdTreeFLANN<pcl::PointXYZI> kd_tree;
//	//		kd_tree.setInputCloud(cloud0);
//	//		// 3,创建一个查询点
//	//		pcl::PointXYZI search_pt;
//	//		search_pt.x = o[i].x;
//	//		search_pt.y = o[i].y;
//	//		search_pt.z = o[i].z;
//	//		// 4, 查询所有的在半径r内邻居点
//	//		//std::vector<int> pt_idx_search;  // 存储查询到的最近邻的k个点的索引
//	//		Indices pt_idx_search;
//	//		std::vector<float> pt_sqr_dis;  //  存储近邻点对应的距离平方
//	//		float radius = r_index;
//	//		// 执行k近邻搜索
//	//		int num_neighbours = find_neigbour_radius2(kd_tree, search_pt, radius, pt_idx_search, pt_sqr_dis);
//	//		//for (size_t i = 0; i < pt_idx_search.size(); i++)  // 打印所有的近邻点坐标和距离
//	//		//{
//	//		//	std::cout << pt_idx_search[i] << "(" << cloud->points[pt_idx_search[i]].x << ","
//	//		//		<< cloud->points[pt_idx_search[i]].y << ","
//	//		//		<< cloud->points[pt_idx_search[i]].z << ")"
//	//		//		<< "dis: " << pt_sqr_dis[i] << "; radius: " << radius << std::endl;
//	//		//}
//	//		double avg = 0;
//	//		std::cout << "index=======" << "\n\n";
//	//		for (size_t i = 0; i < pt_idx_search.size(); i++)  // 打印所有的近邻点坐标和距离
//	//		{
//	//			avg = avg + pt_idx_search[i] / pt_idx_search.size();
//	//			/*if (pt_idx_search[i]<0) {
//	//				std::cout << pt_idx_search[i] << "..";
//	//				std::cout << "===============fule";
//	//				return 0;
//	//			}*/
//	//			//std::cout << avg << "..";
//	//			//std::cout << pt_idx_search[i] << "..";
//	//		}
//	//		//avg = avg / pt_idx_search.size();
//	//		std::cout << "=========================";
//	//		std::cout << avg;
//	//		o[i].t = int(avg);
//	//	}
//	//}
//
//
//	//从点云路径里找t 
//
//	//if (line_key != 0) {
//	//	for (int i = 0; i < o.size(); i++) {
//	//		
//	//		o[i].t = get_time(o[i].x, o[i].y, o[i].z,cloudline,cloudline_t).first;
//	//		//float b = get_time(o[i].x, o[i].y, o[i].z, cloudline);
//	//		//o[i].t = b;
//	//		//cout << b << "\n";
//	//		//cout << std::fixed << setprecision(6) << o[i].t << "\n";
//	//	}
//	//}
//
//
//
//	//开启auto_key,自动去除多余靶点
//	if (auto_key == 1 && f > a) {
//
//		cout << "===============" << "\n";
//		std::cout << "有多余靶点，开始去除" << "\n\n";
//
//		////第1步  去掉距离<d_min的靶点
//
//		//float d_min2 = pow(d_min, 2);
//
//		//float dis = 0;
//		//
//		//for (int i = 0; i < f - 1;i++) {
//		//	//cout << "0000" << "\n";
//
//		//	cout << i << "\n";
//		//	for (int j = i+1; j < f; j++) {
//		//		cout << j << "\n";
//		//		dis = pow((o[i].x - o[j].x), 2) + pow((o[i].y - o[j].y), 2) + pow((o[i].z - o[j].z), 2);
//
//		//		cout << dis << "\n";
//
//		//		if (dis <  d_min2) {
//		//			if (o[i].n >= o[j].n) {
//		//				//std::cout << "情况1" << "\n\n";
//		//				o.erase(o.begin()+j);
//		//				cloudlist0.erase(cloudlist0.begin() + j);
//
//		//			}
//		//			else {
//		//				//std::cout << "情况2" << "\n\n";
//		//				o.erase(o.begin() + i);
//		//				cloudlist0.erase(cloudlist0.begin() + i);
//
//		//			}
//		//		}
//		//	}
//		//		
//		//}
//		//f = o.size();
//		//第2步  如果还有多的靶点，按n顺序排列，去掉末尾多余的
//		//if ( f > a) {
//		//if (f > 1000) {
//
//			//先给Yuan赋 name  记录sort排序，让cloudlist同样排序  (从0开始)
//		for (int i = 0; i < o.size(); i++) {
//			o[i].name = i;
//
//		}
//		sort(o.begin(), o.end(), cmpn);  // 将靶点根据n排序
//
//		//根据Yuan排序后的name结果    在点云  cloudlist复刻排序
//		for (int i = 0; i < o.size(); i++) {
//
//			//cloudlist1[i] = cloudlist0[o[i].name];
//
//			cloudlist1.emplace_back(cloudlist0[o[i].name]);
//
//		}
//		cloudlist0.swap(cloudlist1);
//
//		std::vector<pcl::PointCloud<pcl::PointXYZI>::Ptr> cloudlist_kong;
//
//		cloudlist1.swap(cloudlist_kong);
//
//		o.resize(a);
//		cloudlist0.resize(a);
//
//		f = a;
//		//}
//	}
//
//
//	//把o和cloudlist0 根据t排序， o先排序然后 cloudlist0 复制顺序
//
//	//先给Yuan赋 name  记录sort排序，让cloudlist同样排序  (从0开始)
//	for (int i = 0; i < o.size(); i++) {
//		o[i].name = i;
//
//	}
//	sort(o.begin(), o.end(), cmpt);  // 将靶点根据index排序匹配上序号
//
//	//根据Yuan排序后的name结果    在点云  cloudlist复刻排序
//	for (int i = 0; i < o.size(); i++) {
//
//		//cloudlist1[i] = cloudlist0[o[i].name];
//
//		cloudlist1.emplace_back(cloudlist0[o[i].name]);
//	}
//
//	//给Yuan重新赋name  （从1开始）
//	for (int i = 0; i < o.size(); i++) {
//		o[i].name = i + 1;
//
//	}
//
//	//// 后续process需要的文档  向txt文档中写入数据
//	//ofstream dataFile;
//	//std::string path_txt = path + "xyzt.txt";
//	//dataFile.open(path_txt, ofstream::app);
//	//fstream file(path_txt, ios::out);
//	//for (int i = 0; i < o.size(); i++) {
//	//	dataFile << o[i].x << ' ' << o[i].y << ' ' << o[i].z << ' ' << std::fixed << setprecision(6) << o[i].t << "\n" << endl;
//	//}
//	//dataFile.close();
//
//	//pathlist.emplace_back(path_txt);
//
//
//	//// 暂时替代前端显示的文档   向txt文档中写入数据
//	//ofstream dataFile2;
//	//std::string path_txt2 = path + "name+xyzr.txt";
//	//dataFile2.open(path_txt2, ofstream::app);
//	//fstream file2(path_txt2, ios::out);
//	////dataFile2 << "name" << ' ' << "x" << ' ' << "y" << ' ' << "z" << ' ' << "r" << ' ' << "real_x" << ' ' << "real_y" << ' ' << "real_z" << "\n" << endl;
//	//dataFile2 << "name" << "     " << "x" << "       " << "y" << "       " << "z" << "      " << "r" << "      " << "n"
//	//	<< "     " << "real_x" << "     " << "real_y" << "     " << "real_z" << "\n" << endl;
//	//for (int i = 0; i < o.size(); i++) {
//	//	dataFile2 << "gcp_" << i + 1 << ' ' << std::fixed << setprecision(4) << o[i].x << ' ' << std::fixed << setprecision(4) << o[i].y << ' ' << std::fixed << setprecision(4) << o[i].z
//	//		<< ' ' << std::fixed << setprecision(4) << o[i].r << ' ' << o[i].n << "\n" << endl;
//	//}
//	//dataFile2.close();
//
//
//
//	//保存强度过滤后 cloud 删剩下的点云
//
//	//std::string path_shengyu = path + "shengyu.ply";
//
//	cout << "===============0" << "\n";
//
//	//pcl::io::savePCDFileBinary(path_shengyu, *cloud);
//
//
//	//pcl::io::savePLYFileBinary(path_shengyu, *cloud);
//
//	cout << "===============1" << "\n";
//	//从 class  o的元素.cloud里 保存所有提取出的靶点点云
//	if (f != 0) {
//
//		for (int u = 0; u < f; u++) {
//			std::string file = "gcp_" + std::to_string(u + 1+ gcpIndex) + ".ply";
//			file = path + file;
//			pathlist.emplace_back(file);
//
//			//std::cout << u << "\n\n";
//
//			//保存所有提取出的靶点点云
//
//			std::cout << "===============" << "\n\n";
//
//			std::cout << o[u].x << "\n\n";
//			std::cout << o[u].y << "\n\n";
//			std::cout << o[u].z << "\n\n";
//			std::cout << "xyz_path" << "\n\n";
//			std::cout << o[u].x_path << "\n\n";
//			std::cout << o[u].y_path << "\n\n";
//			std::cout << o[u].z_path << "\n\n";
//			std::cout << "-----" << "\n\n";
//			std::cout << o[u].r << "\n\n";
//			std::cout << o[u].n << "\n\n";
//
//			circlelist.emplace_back(o[u].x);
//			circlelist.emplace_back(o[u].y);
//			circlelist.emplace_back(o[u].z);
//			circlelist.emplace_back(o[u].r);
//
//			std::cout << std::fixed << setprecision(6) << o[u].t << "\n\n";
//			//pcl::io::savePCDFileBinary(file, *(o[u].cloud));
//
//
//			pcl::io::savePLYFileBinary(file, *(cloudlist1[u]));
//
//		}
//		//pathlist.emplace_back(path_shengyu);
//	}
//
//
//	std::cout << "\n\n" << " *****  happy ending  *****" << "\n\n";
//
//	return make_pair(o, pathlist);
//
//}
//
//
//
//
//
////单点加靶点
//Yuan gcp::add(float x, float y, float z, std::string path_cloud, std::string path, float r1, float r2,//一定需要客户手动输入的
//	int name, int diedai, std::string path_cloudline, int i_min, int i_max, int switch_key, int d_max_1, int min_break)
//{
//	//std::string path_cloud = "F:/PCD_data/0/surfel/8.pcd";
//	//std::string path_cloud = "F:/PCD_data/0/surfel/7.pcd";
//	//std::string path_cloudline;
//	//float x = 0, y = 0, z = 0;
//	//int switch_key = 0;//是否打开路径距离判断筛选
//	//std::string path = "";
//	//int diedai = 1;//是否多次识别  可改5
//	//int i_min = 240;
//	//int i_max = 255;
//	int d_max = 100;
//	float r_index = 1;//索引半径0.0
//	int line_key = 0;//是否有路径点云
//	int ff = 0;
//
//
//	// params from parent
//	r2 = this->radMax;
//	r1 = this->radMin;
//	i_max = this->intensityMax;
//	i_min = this->intensityMin;
//	switch_key = this->filterDist;
//	d_max_1 = this->filterDistance;
//	min_break = this->circlePts / 4;
//	// ==================
//	std::cout << "add ================= " << std::endl;
//	std::cout << "r max " << std::to_string(r2) << std::endl;
//	std::cout << "r min " << std::to_string(r1) << std::endl;
//	std::cout << "i max " << std::to_string(i_max) << std::endl;
//	std::cout << "i min " << std::to_string(i_min) << std::endl;
//	std::cout << "min circle " << std::to_string(this->circlePts) << std::endl;
//	std::cout << "min break " << std::to_string(min_break) << std::endl;
//	std::cout << "switch " << std::to_string(switch_key) << std::endl;
//	std::cout << "d max1 " << std::to_string(d_max_1) << std::endl;
//
//
//	Yuan yyy0;
//
//	if (path_cloudline.length() > 1) {
//		line_key = 1;
//
//	}
//
//
//	if (path_cloudline.length() <= 1) {
//		switch_key = 0;
//
//	}
//
//
//	if (switch_key == 1) {//路径筛选条件打开，离路径0.5m外的靶点不识别
//		d_max = d_max_1;
//	}
//
//
//
//
//	pcl::PointCloud<pcl::PointXYZI>::Ptr cloud0(new pcl::PointCloud<pcl::PointXYZI>);//最初始电云
//
//	//io::loadPCDFile(path_cloud, *cloud0);
//	io::loadPLYFile(path_cloud, *cloud0);
//
//
//	//pcl::PointCloud<pcl::PointXYZI>::Ptr cloudline(new pcl::PointCloud<pcl::PointXYZI>);//路径点云xyzi
//	pcl::PointCloud<pcl::PointXYZIT>::Ptr cloudline_t(new pcl::PointCloud<pcl::PointXYZIT>);//路径点云xyzit
//
//
//
//	//有路径点云才load相关点云
//	if (line_key != 0) {
//
//		//io::loadPCDFile(path_cloudline, *cloudline);
//		io::loadPLYFile(path_cloudline, *cloudline_t);
//
//		//io::loadPCDFile(path_cloudline, *cloudline);
//		//io::loadPLYFile(path_cloudline, *cloudline);
//
//
//
//	}
//
//	pcl::PointCloud<pcl::PointXYZI>::Ptr final(new pcl::PointCloud<pcl::PointXYZI>);//最终保存的对应靶点的点云
//
//	//kd树 半径提取球状点云
//
//	// 2,创建kd-tree对象，并把上面的点云设置成输入，然后创建一个查询点，查询该点
//	pcl::KdTreeFLANN<pcl::PointXYZI> kd_tree;
//	kd_tree.setInputCloud(cloud0);
//	// 3,创建一个查询点
//	pcl::PointXYZI search_pt;
//	search_pt.x = x;
//	search_pt.y = y;
//	search_pt.z = z;
//	// 4, 查询所有的在半径r内邻居点
//	std::vector<int> pt_idx_search;  // 存储查询到的最近邻的k个点的索引
//	std::vector<float> pt_sqr_dis;  //  存储近邻点对应的距离平方
//	float r = 2 * r2;//提取的球的半径
//	// 执行k近邻搜索
//	int num_neighbours = find_neigbour_radius(kd_tree, search_pt, r, pt_idx_search, pt_sqr_dis);
//
//
//
//
//	pcl::PointCloud<pcl::PointXYZI>::Ptr cloud1(new pcl::PointCloud<pcl::PointXYZI>);//球状点云
//
//
//
//	pcl::copyPointCloud<pcl::PointXYZI>(*cloud0, pt_idx_search, *cloud1);
//
//	//提取出的球状点云的预处理
//
//		//强度直通滤波
//
//
///*  直通滤波 后 的点云 */
//	pcl::PointCloud<pcl::PointXYZI>::Ptr cloud2(new pcl::PointCloud<pcl::PointXYZI>);//预处理后的点云
//
//
//
//
//	//直通滤波
//
//	cloud2 = pth_filter(cloud1, i_min, i_max);
//
//
//	//pcl::io::savePCDFileBinary("F:/PCD_data/gaofanshe/cunjieguo/1.pcd", *cloud2);
//
//
//	//强度过滤后两步步预处理（要用下采样只能用均匀，最好不用下采样）
//
//
//	//cloud = uni_dsp(cloud, 0.005);//调用下采样
//
//	//cout << cloud->points.size() << "\n\n";
//
//	cloud2 = sor(cloud2, 10, 1);//sor
//
//	cloud2 = mls(cloud2, 0.3, 1, 0.005);//mls
//
//
//	//pcl::io::savePCDFileBinary("F:/PCD_data/gaofanshe/cunjieguo/2.pcd", *cloud);
//
//	//开始循环识别
//	for (int k = 0; k < diedai; k++) {
//
//
//
//
//
//
//
//		std::cout << "=====" << "\n\n";
//
//		std::cout << cloud2->points.size() << "\n\n\n\n";
//
//		pcl::PointCloud<pcl::PointXYZI>::Ptr f(new pcl::PointCloud<pcl::PointXYZI>);
//
//
//		std::vector<int> inliers;
//
//		pcl::SampleConsensusModelCircle3D<pcl::PointXYZI>::Ptr
//			model_circle3D(new pcl::SampleConsensusModelCircle3D<pcl::PointXYZI>(cloud2));
//
//		pcl::RandomSampleConsensus<pcl::PointXYZI> ransac(model_circle3D);
//		//ransac.setMaxIterations(15000);
//		ransac.setDistanceThreshold(0.001);
//		ransac.computeModel();
//		ransac.getInliers(inliers);
//
//		Eigen::VectorXf model;
//
//		ransac.getModelCoefficients(model);
//		std::cout << model << "\n\n";
//
//
//		//如果找不到圆了就结束循环
//
//		std::cout << inliers.size() << "\n\n";
//		//if (inliers.size() == 0)   //或者改成小于阈值 
//		//if (inliers.size() < min_circle && f >= a)//并且，就算找到足够数量靶点也会把圆上点数量的条件循环跑完
//
//
//		if (inliers.size() < min_break)
//
//		{
//			std::cout << "=====break=====" << "\n\n";
//			break;
//		}
//
//
//
//		//找圆结束
//
//		float x0 = model[0];
//		float y0 = model[1];
//		float z0 = model[2];
//
//		float r = model[3];
//		//int n = inliers.size();
//
//		float xp = 0;
//		float yp = 0;
//		float zp = 0;
//
//		float nx = model[4];
//		float ny = model[5];
//		float nz = model[6];
//
//
//		//float dn = pow(xn, 2) + pow(yn, 2) + pow(zn, 2);
//		//std::cout << "dn"<<dn << "\n\n";
//
//
//		//std::cout << "\n" << x << "\n" << y << "\n" << z << "\n" << r <<  "\n\n";
//
//
//
//
//
//
//
//		//=================================================================
//		//邻近搜索, 从cloud中 删除搜索到的圆的点云
//
//
//
//
//		// 2,创建kd-tree对象，并把上面的点云设置成输入，然后创建一个查询点，查询该点
//		pcl::KdTreeFLANN<pcl::PointXYZI> kd_tree;
//
//		kd_tree.setInputCloud(cloud2);
//
//
//
//		// 3,创建一个查询点
//		pcl::PointXYZI search_pt;
//		search_pt.x = x0;
//		search_pt.y = y0;
//		search_pt.z = z0;
//
//		// 4, 查询所有的在半径r内邻居点
//		std::vector<int> pt_idx_search;  // 存储查询到的最近邻的k个点的索引
//		std::vector<float> pt_sqr_dis;  //  存储近邻点对应的距离平方
//		float rr = r + 0.04;//多删除的（每个靶点只会被识别到一次，识别到内圆的话也会把整个靶点删掉）
//		float radius = rr;
//
//		// 执行k近邻搜索
//		int num_neighbours = find_neigbour_radius(kd_tree, search_pt, radius, pt_idx_search, pt_sqr_dis);
//
//		// 打印所有的近邻点坐标和距离
//
//		//for (size_t i = 0; i < pt_idx_search.size(); i++)  
//		//{
//		//	std::cout << pt_idx_search[i] << "(" << cloud->points[pt_idx_search[i]].x << ","
//		//		<< cloud->points[pt_idx_search[i]].y << ","
//		//		<< cloud->points[pt_idx_search[i]].z << ")"
//		//		<< "dis: " << pt_sqr_dis[i] << "; radius: " << radius << std::endl;
//		//}
//
//
//		// 在球内点云中，用投影判断,删除圆内的点
//
//		//pcl::PointCloud<pcl::PointXYZI>::iterator r_index = cloud->begin();
//
//		std::vector<int> remove;
//
//
//		for (size_t i = 0; i < pt_idx_search.size(); i++)  // 打印所有的近邻点坐标和距离
//		{
//			float x1 = cloud2->points[pt_idx_search[i]].x;
//			float y1 = cloud2->points[pt_idx_search[i]].y;
//			float z1 = cloud2->points[pt_idx_search[i]].z;
//			float d = ((x1 - x0) * nx + (y1 - y0) * ny + (z1 - z0) * nz);
//			d = abs(d);
//
//			//std::cout << pt_idx_search[i] << "..";
//
//
//
//			if (d < 0.02)
//			{
//
//				remove.push_back(pt_idx_search[i]);
//
//				//点xyz=0代替删除
//
//				//cloud->points[pt_idx_search[i]].x = 0;
//				//cloud->points[pt_idx_search[i]].y = 0;
//				//cloud->points[pt_idx_search[i]].z = 0;
//
//
//			}
//
//		}
//
//
//		sort(remove.begin(), remove.end());
//
//
//
//
//		pcl::copyPointCloud<pcl::PointXYZI>(*cloud2, remove, *f);
//
//		//inlier
//		std::cout << f->points.size() << "\n\n";
//
//
//
//		//int nn = remove.size();
//		/*for(int i = 0; i < nn;i++ ){
//			std::cout << remove[i] << "\n\n";
//
//		}*/
//
//
//		cloud2 = del2(cloud2, remove);
//
//
//
//
//
//
//		//保存提取的圆cloud
//
//		//pcl::io::savePCDFileBinary("F:/PCD_data/gcp/f.pcd", *final);
//		//del2(cloud, inliers);
//		//pcl::io::savePCDFileBinary("F:/PCD_data/gcp/san.pcd", *cloud);
//
//
//		//如果圆不符合提前设置的要求，就只删除圆和圆内点不保存进靶点列表
//
//
//
//
//		if (r < r1 || r2 < r)
//		{
//			std::cout << "tiaoguo" << "\n\n";
//			continue;
//		}
//
//		/*if (inliers.size() <  min_break)
//		{
//			std::cout << "tiaoguo" << "\n\n";
//			break;
//		}*/
//
//		double t = 0;
//		//如果有路径点云，从点云路径里找t 
//		if (line_key != 0) {
//
//
//			pair<double, vector<float>> p1 = get_time_t(x0, y0, z0, cloudline_t);
//			t = p1.first;
//
//			vector<float> f = p1.second;
//			float distance = f[0];
//			xp = f[1];
//			yp = f[2];
//			zp = f[3];
//
//			if (distance > d_max)
//			{
//				std::cout << "tiaoguo" << "\n\n";
//				continue;
//			}
//
//		}
//
//
//		//如果没有路径点云，找 index avg 
//
//		if (line_key == 0) {
//
//
//
//			//邻近搜索
//			std::cout << "\n\n\n\n";
//			// 2,创建kd-tree对象，并把上面的点云设置成输入，然后创建一个查询点，查询该点
//			pcl::KdTreeFLANN<pcl::PointXYZI> kd_tree;
//			kd_tree.setInputCloud(cloud0);
//			// 3,创建一个查询点
//			pcl::PointXYZI search_pt;
//			search_pt.x = x0;
//			search_pt.y = y0;
//			search_pt.z = z0;
//			// 4, 查询所有的在半径r内邻居点
//			//std::vector<int> pt_idx_search;  // 存储查询到的最近邻的k个点的索引
//			Indices pt_idx_search;
//			std::vector<float> pt_sqr_dis;  //  存储近邻点对应的距离平方
//			float radius = r_index;
//			// 执行k近邻搜索
//			int num_neighbours = find_neigbour_radius2(kd_tree, search_pt, radius, pt_idx_search, pt_sqr_dis);
//			//for (size_t i = 0; i < pt_idx_search.size(); i++)  // 打印所有的近邻点坐标和距离
//			//{
//			//	std::cout << pt_idx_search[i] << "(" << cloud->points[pt_idx_search[i]].x << ","
//			//		<< cloud->points[pt_idx_search[i]].y << ","
//			//		<< cloud->points[pt_idx_search[i]].z << ")"
//			//		<< "dis: " << pt_sqr_dis[i] << "; radius: " << radius << std::endl;
//			//}
//			float avg = 0;
//			std::cout << "index=======" << "\n\n";
//			for (size_t i = 0; i < pt_idx_search.size(); i++)  // 打印所有的近邻点坐标和距离
//			{
//				avg = avg + pt_idx_search[i] / pt_idx_search.size();
//				/*if (pt_idx_search[i]<0) {
//					std::cout << pt_idx_search[i] << "..";
//					std::cout << "===============fule";
//					return 0;
//				}*/
//				//std::cout << avg << "..";
//				//std::cout << pt_idx_search[i] << "..";
//			}
//			//avg = avg / pt_idx_search.size();
//			std::cout << "=========================";
//			std::cout << avg << "\n\n";
//			t = int(avg);
//
//
//		}
//
//
//
//
//
//
//		Yuan yu;
//		yu.x = x0;
//		yu.y = y0;
//		yu.z = z0;
//		yu.r = r;
//
//		yu.x_path = xp;
//		yu.y_path = yp;
//		yu.z_path = zp;
//
//		yu.t = t;
//		yu.n = inliers.size();
//
//
//
//
//		/*if (ff == 0) {
//
//			o[0] = yu;
//
//		}
//
//		if (ff > 0) {*/
//
//		if (yu.n > yyy0.n) {
//			//o[0] = yu;
//
//			yyy0 = yu;
//			final = f;
//
//			//std::cout << "换" << "\n\n";
//
//		}
//
//		//}
//		//o.emplace_back(yu);
//
//
//		ff++;
//
//
//		std::cout << ff << "\n\n";
//
//
//		std::cout << "size" << cloud2->points.size() << "\n\n\n\n";
//
//		//if (ff == 1) {
//		//	std::cout << "找到了" << "\n\n";
//		//	break;
//		//}
//
//		if (cloud2->points.size() < min_break) {
//			std::cout << "点云空了" << "\n\n";
//			break;
//		}
//
//
//	}
//
//
//
//	if (ff == 0) {
//
//		cout << "未识别到任何靶点" << "\n\n";
//		return yyy0;//如果yyy0.r==0  或yyy0.n==0 那么说明没识别出来
//	}
//	else if (ff == 1) {
//		cout << "识别到唯一靶点" << "\n\n";
//
//
//	}
//	else if (ff > 1) {
//		cout << "识别到多个靶点" << "\n\n";
//		//按照圆上点排倒序后，取第一个 数量最多的的作为输出 
//
//	}
//
//
//
//
//
//
//	yyy0.name = name;
//
//	std::string path2 = path + "gcp_" + to_string(name) + ".ply";
//	pcl::io::savePLYFileBinary(path2, *final);
//
//
//	return yyy0;
//
//
//}