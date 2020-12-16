#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include "opencv2/core/core.hpp"
#include <string>

using namespace std;
using namespace cv;



int main()
{
	float dst_size = 240.0;
	int wind_size = 40;
	Mat V2_Chern_bel, window;
	Mat frame;
	Mat bord_frame;

	VideoCapture cap("challenge.mp4");


	namedWindow("Vid sverhy", WINDOW_AUTOSIZE);
	int vverh = 72;
	int vis_planki = 77;
	int niz = 88;
	int vis_planki2 = 91;

	//Корректировка inRange

	namedWindow("Chern bel");
	int color01 = 0;
	int color02 = 0;
	int color03 = 32;
	int color11 = 233;
	int color12 = 255;
	int color13 = 255;


	while (true) {

		cap >> frame;

		if (frame.empty()) {
			cap.set(CAP_PROP_POS_FRAMES, 0);
			continue;
		}

		float rows = frame.rows;
		float cols = frame.cols;

		float t1 = cols / 100.0*niz;
		float t2 = rows / 100.0*vis_planki2;
		float t3 = cols / 100.0*(100.0 - niz);

		float h1 = cols / 100.0*vverh;
		float h2 = rows / 100.0*vis_planki;
		float h3 = cols / 100.0*(100.0 - vverh);

		vector<Point2f> dst_t;


		vector<Point> polyline;
		polyline.push_back(Point(h1, h2));
		polyline.push_back(Point(h3, h2));
		polyline.push_back(Point(t3, t2));
		polyline.push_back(Point(t1, t2));


		vector<Point2f> points;
		points.push_back(Point2f(h1, h2));
		points.push_back(Point2f(h3, h2));
		points.push_back(Point2f(t3, t2));
		points.push_back(Point2f(t1, t2));


		vector<Point2f> dst_points;
		dst_points.push_back(Point2f(dst_size, 0.0));
		dst_points.push_back(Point2f(0.0, 0.0));
		dst_points.push_back(Point2f(0.0, dst_size));
		dst_points.push_back(Point2f(dst_size, dst_size));

		Mat __trap__ = frame.clone();
		polylines(__trap__, polyline, 1, Scalar(0, 0, 255), 4);

		Mat Matrix = getPerspectiveTransform(points, dst_points);
		Mat dst;
		warpPerspective(frame, dst, Matrix, Size(240, 240), INTER_LINEAR, BORDER_CONSTANT);

		Mat dst2NC = dst.clone();
		cvtColor(dst2NC, dst2NC, COLOR_BGR2HLS);
		Mat dst_bin = dst2NC.clone();
		inRange(dst_bin, Scalar(color01, color02, color03), Scalar(color11, color12, color13), dst_bin);

		V2_Chern_bel = dst_bin.clone();
		for (int i = 0; i < dst_size / wind_size; i++) {
			for (int j = 0; j < 2 * dst_size / wind_size - 1; j++) {
				Rect rect(j*wind_size / 2, i*wind_size, wind_size, wind_size);
				window = dst_bin(rect);
				Moments mom = moments(window, true);
				if (mom.m00 > 100) {
					Point2f point(j*wind_size / 2 + float(mom.m10 / mom.m00), i*wind_size + float(mom.m01 / mom.m00));

					bool dublicate = false;
					for (size_t n = 0; n < dst_t.size(); n++) {
						if (norm(dst_t[n] - point) < 10) {
							dublicate = true;
						}
					}
					if (!dublicate) {
						dst_t.push_back(point);
						circle(V2_Chern_bel, point, 5, Scalar(128), -1);
					}
				}
			}
		}

		if (dst_t.size() > 0) {
			vector<Point2f> frame_points;
			perspectiveTransform(dst_t, frame_points, Matrix.inv());
			for (size_t i = 0; i < dst_t.size(); i++) {
				circle(__trap__, frame_points[i], 5, Scalar(0, 0, 0), -1);
			}
		}

		imshow("Gotovoe izobrazhenie", __trap__);
		imshow("Vid sverhy", dst);
		imshow("Chern bel", dst_bin);
		imshow("V2 Chern bel", V2_Chern_bel);


		char c = (char)waitKey(25);
		if (c == 27)
			break;
	}

	cap.release();
	destroyAllWindows();

	return 0;
}


