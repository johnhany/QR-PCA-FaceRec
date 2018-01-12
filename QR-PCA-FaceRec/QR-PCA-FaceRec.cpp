/************************************************************************/
/*                   QR-PCA-FaceRec  by John Hany                       */
/*																		*/
/*	A face recognition algorithm using QR based PCA.              		*/
/*																		*/
/*	Released under MIT license.											*/
/*																		*/
/*	Contact me at johnhany@163.com										*/
/*																		*/
/*	Welcome to my blog http://johnhany.net/, if you can read Chinese:)	*/
/*																		*/
/************************************************************************/

#include <opencv2/opencv.hpp>
#include <armadillo>
#include <iostream>

using namespace std;

int component_num = 7;

string orl_path = "G:\\Datasets\\orl_faces";

enum distance_type {ECULIDEAN = 0, MANHATTAN, MAHALANOBIS};
//double distance_criterion[3] = { 10.0, 30.0, 3.0};
double distance_criterion[3] = { 1000.0, 1000.0, 1000.0};

bool compDistance(pair<int, double> a, pair<int, double> b);
double calcuDistance(const arma::vec vec1, const arma::vec vec2, distance_type dis_type);
double calcuDistance(const arma::vec vec1, const arma::vec vec2, const arma::mat cov2, distance_type dis_type);

int main(int argc, const char *argv[]) {
	
	int class_num = 40;
	int sample_num = 10;

	int img_cols = 92;
	int img_rows = 112;
	cv::Size sample_size(img_cols, img_rows);

	arma::mat mat_sample(img_rows*img_cols, sample_num*class_num);

	//Load samples in one matrix `mat_sample`.

	for(int class_idx = 0; class_idx < class_num; class_idx++) {
		for(int sample_idx = 0; sample_idx < sample_num; sample_idx++) {

			string filename = orl_path + "\\s" + to_string(class_idx+1) + "\\" + to_string(sample_idx+1) + ".pgm";
			cv::Mat img_frame = cv::imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
			cv::Mat img_sample;
			cv::resize(img_frame, img_sample, sample_size);

			for(int i = 0; i < img_rows; i++) {
				uchar* pframe = img_sample.ptr<uchar>(i);
				for(int j = 0; j < img_cols; j++) {
					mat_sample(i*img_cols+j, class_idx*sample_num+sample_idx) = (double)pframe[j]/255.0;
				}
			}
		}
	}
//	cout <<	mat_sample.n_rows << endl << mat_sample.n_cols << endl << mat_sample(img_rows*img_cols/2, 0) << endl;

	//Calculate PCA transform matrix `mat_pca`.

	arma::mat H = mat_sample;
	arma::mat mean_x = arma::mean(mat_sample, 1);

	for(int j = 0; j < class_num * sample_num; j++) {
		H.col(j) -= mean_x.col(0);
	}
	H *= 1.0/sqrt(sample_num-1);

	arma::mat Q, R;
	arma::qr_econ(Q, R, H);

	arma::mat U, V;
	arma::vec d;
	arma::svd_econ(U, d, V, R.t());

//	cout << "d" << endl << d << endl;

//	arma::rowvec vec_eigen = d.head(component_num).t();
//	cout << "vec_eigen" << endl << vec_eigen << endl;

	arma::mat V_h(V.n_rows, component_num);
	if(component_num == 1) {
		V_h = V.col(0);
	}else {
		V_h = V.cols(0, component_num-1);
	}

	arma::mat mat_pca = Q * V_h;

	//Calculate eigenfaces `mat_eigen_vec`.

	arma::mat mat_eigen = mat_pca.t() * mat_sample;
//	cout << "mat_eigen" << endl << mat_eigen << endl;
	arma::mat mat_eigen_vec(component_num, class_num, arma::fill::zeros);
	vector<arma::mat> mat_cov_list;

	for(int class_idx = 0; class_idx < class_num; class_idx++) {

		arma::vec eigen_sum(component_num, arma::fill::zeros);
		for(int sample_idx = 0; sample_idx < sample_num; sample_idx++) {
			eigen_sum += mat_eigen.col(class_idx*sample_num+sample_idx);
		}
		eigen_sum /= (double)sample_num;
		mat_eigen_vec.col(class_idx) = eigen_sum;

		mat_cov_list.push_back(arma::cov((mat_eigen.cols(class_idx*sample_num, class_idx*sample_num+sample_num-1)).t()));

//		cout << mat_cov_list[class_idx] << endl;

	}

//	cout << "mat_eigen_vec" << endl << mat_eigen_vec << endl;

/*
	cout << "dis within class" << endl;
	for(int class_idx = 0; class_idx < class_num; class_idx++) {
		for(int sample_idx = 0; sample_idx < sample_num; sample_idx++) {
			double dis = calcuDistance(mat_eigen.col(class_idx*sample_num+sample_idx), mat_eigen_vec.col(class_idx), mat_cov_list[class_idx], distance_type::MAHALANOBIS);
			cout << dis << " ";
		}
		cout << endl;
	}

	cout << "dis between classes" << endl;
	for(int class_idx = 0; class_idx < class_num; class_idx++) {
		for(int sample_idx = 0; sample_idx < class_num; sample_idx++) {
			double dis = calcuDistance(mat_eigen.col(sample_idx*sample_num), mat_eigen_vec.col(class_idx), mat_cov_list[class_idx], distance_type::MAHALANOBIS);
			cout << dis << " ";
		}
		cout << endl;
	}
*/

	//Classify new sample.

	int correct_count = 0;

	double max_dis = 0.0;

	for(int class_idx = 0; class_idx < class_num; class_idx++){
		for(int sample_idx = 0; sample_idx < sample_num; sample_idx++) {
			arma::mat mat_new_sample(img_rows*img_cols, 1);

			string filename = orl_path + "\\s" + to_string(class_idx+1) + "\\" + to_string(sample_idx+1) + ".pgm";
			cv::Mat img_new_frame = cv::imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
			cv::Mat img_new_sample;
			cv::resize(img_new_frame, img_new_sample, sample_size);

			for(int i = 0; i < img_rows; i++) {
				uchar* pframe = img_new_sample.ptr<uchar>(i);
				for(int j = 0; j < img_cols; j++) {
					mat_new_sample(i*img_cols+j, 0) = (double)pframe[j]/255.0;
				}
			}

			arma::mat mat_new_eigen = mat_pca.t() * mat_new_sample;

			vector<pair<int, double>> dis_list;
			for(int new_class_idx = 0; new_class_idx < class_num; new_class_idx++) {
				double dis = calcuDistance(mat_new_eigen.col(0), mat_eigen_vec.col(new_class_idx), mat_cov_list[new_class_idx], distance_type::MAHALANOBIS);
				dis_list.push_back(make_pair(new_class_idx, dis));
			}
			sort(dis_list.begin(), dis_list.end(), compDistance);

			if(dis_list[0].first == class_idx && dis_list[0].second <= distance_criterion[distance_type::MAHALANOBIS]) {
				correct_count++;
			}

			if(dis_list.back().second > max_dis) {
				max_dis = dis_list.back().second;
			}
		}
	}

	cout << "Maximum distance: " << max_dis << endl;

	double correct_ratio = (double)correct_count / (class_num * sample_num);
	cout << "Correctness ratio: " << correct_ratio * 100.0 << "%" << endl;

	cin.get();

	return 0;
}

bool compDistance(pair<int, double> a, pair<int, double> b) {
	return (a.second < b.second);
}

double calcuDistance(const arma::vec vec1, const arma::vec vec2, distance_type dis_type) {

	if(dis_type == ECULIDEAN) {
		return arma::norm(vec1-vec2, 2);
	}else if(dis_type == MANHATTAN) {
		return arma::norm(vec1-vec2, 1);
	}else if(dis_type == MAHALANOBIS) {
		arma::mat tmp = (vec1-vec2).t() * (vec1 - vec2);
		return sqrt(tmp(0,0));
	}

	return -1.0;
}

double calcuDistance(const arma::vec vec1, const arma::vec vec2, const arma::mat cov2, distance_type dis_type) {

	if(dis_type == ECULIDEAN) {
		return arma::norm(vec1-vec2, 2);
	}else if(dis_type == MANHATTAN) {
		return arma::norm(vec1-vec2, 1);
	}else if(dis_type == MAHALANOBIS) {
		arma::mat tmp = (vec1-vec2).t() * cov2.i() * (vec1 - vec2);
		return sqrt(tmp(0,0));
	}

	return -1.0;
}
