#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

using namespace cv;
using namespace std;


void read_all_image(string filename,
		Size board_size, Size & image_size,
		vector< vector<Point2f> > & image_points_seq) {

	ifstream fin(filename);
	string image_path;

	vector<Point2f> image_points_buf;
	image_points_seq.clear();

	while (getline(fin, image_path)) {
		// read image
		Mat image_input = imread(image_path);

		// get image size
		image_size.width = image_input.cols;
		image_size.height = image_input.rows;

		// if there is no chessboard
		if (findChessboardCorners(image_input, board_size, image_points_buf) == 0) {
			cout << "Chessboard cannot be found in " << image_path << endl;
			continue;
		}

		// turn into gray
		Mat view_gray;
		cvtColor(image_input, view_gray, CV_RGB2GRAY);

		// find subpix to improve accuracy
		find4QuadCornerSubpix(view_gray, image_points_buf, Size(11, 11));

		// save
		image_points_seq.push_back(image_points_buf);

		// draw and show
		drawChessboardCorners(image_input, board_size, image_points_buf, true);
		imshow("image", image_input);
		waitKey(1000);
	}
}


void calibrate(Size board_size, Size image_size, Size square_size,
		vector< vector<Point2f> > image_points_seq, vector< vector<Point3f> > & object_points_seq,
		vector<int> & point_counts,
		vector<Mat> & r_vec, vector<Mat> & t_vec, Mat & camera_matrix, Mat & dist_coeffs) {

	// init
	for (int i = 0; i < image_points_seq.size(); i ++) {
		vector<Point3f> object_points_buf;
		for (int j = 0; j < board_size.height; j ++) {
			for (int k = 0; k < board_size.width; k ++) {
				Point3f real_point;
				real_point.x = j * square_size.width;
				real_point.y = k * square_size.height;
				real_point.z = 0;

				object_points_buf.push_back(real_point);
			}
		}

		object_points_seq.push_back(object_points_buf);
		point_counts.push_back(board_size.height * board_size.width);
	}

	// calibration
	calibrateCamera(object_points_seq, image_points_seq, image_size, camera_matrix, dist_coeffs, t_vec, r_vec, 0);
}


void evaluate_result(vector< vector<Point2f> > image_points_seq, vector< vector<Point3f> > object_points_seq,
		vector<int> point_counts,
		vector<Mat> r_vec, vector<Mat> t_vec, Mat camera_matrix, Mat dist_coeffs) {

	double err_mean = 0, err_total = 0;
	vector<Point2f> projected_image_points;

	for (int i = 0; i < image_points_seq.size(); i ++) {
		vector<Point2f> image_points_buf = image_points_seq[i];
		vector<Point3f> projected_points_buf = object_points_seq[i];

		// projection
		projectPoints(projected_points_buf, t_vec[i], r_vec[i], camera_matrix, dist_coeffs, projected_image_points);

		Mat image_points_buf_mat = Mat(1, image_points_buf.size(), CV_32FC2);
		Mat projected_image_points_mat = Mat(1, projected_image_points.size(),  CV_32FC2);

		for (int j = 0; j < image_points_buf.size(); j ++) {
			projected_image_points_mat.at<Vec2f>(0, j) = Vec2f(projected_image_points[j].x, projected_image_points[j].y);
			image_points_buf_mat.at<Vec2f>(0, j) = Vec2f(image_points_buf[j].x, image_points_buf[j].y);
		}

		err_mean = norm(projected_image_points_mat, image_points_buf_mat, NORM_L2) / point_counts[i];
		err_total += err_mean;

		cout << "image[" << i <<"] mean error is " << err_mean << "px" << endl;   
	}

	err_total /= image_points_seq.size();

	cout << "total mean error is " << err_total << endl;
}


int main() {
	vector< vector<Point2f> > image_points_seq;
	vector< vector<Point3f> > object_points_seq;
	vector<int> point_counts;
	
	vector<Mat> r_vec;                                       // 每幅图像的旋转向量
	vector<Mat> t_vec;                                       // 每幅图像的平移向量

	Mat camera_matrix = Mat(3, 3, CV_32FC1, Scalar::all(0)); // 相机内参数矩阵
	Mat dist_coeffs = Mat(1, 5, CV_32FC1, Scalar::all(0));   // 摄像机的5个畸变系数：k1,k2,p1,p2,k3

	Size board_size = Size(5, 6);                            // 棋盘格子数量
	Size square_size = Size(20, 20);                         // 棋盘格子大小 单位应该是cm
	Size image_size;

	read_all_image("imagelist.txt", 
			board_size, image_size, 
			image_points_seq);

	calibrate(board_size, image_size, square_size,
			image_points_seq, object_points_seq,
			point_counts,
			r_vec, t_vec, camera_matrix, dist_coeffs);

	evaluate_result(image_points_seq, object_points_seq,
			point_counts,
			r_vec, t_vec, camera_matrix, dist_coeffs);
}

