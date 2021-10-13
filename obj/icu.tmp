#include "opencv2/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/highgui/highgui_c.h"
#include <stdlib.h>
#include <stdio.h>
#include <numeric>

#define SERVO_PIN 18 // BCM hardware PWM servo pin

class Driving {
public:
	int y = 1;

private:
	int x = 5;
};

void Dilation( int, void* );
void MyFilledCircle( cv::Mat& img, cv::Point center );
static double VectorAvg( std::vector<int> const& v );
void CloseWindows();
void DisplayHistogram( int height, int width, cv::Mat& values, double middlepoint );

cv::Mat src;

cv::Mat ClaheEqualization(cv::Mat& source) {
	// Clahe equalization
	// READ RGB color image and convert it to Lab
    cv::Mat lab_image;
    cv::cvtColor(source, lab_image, CV_BGR2Lab);

    // Extract the L channel
    std::vector<cv::Mat> lab_planes(3);
    split(lab_image, lab_planes);  // now we have the L image in lab_planes[0]

    // apply the CLAHE algorithm to the L channel
    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
    clahe->setClipLimit(3);
	clahe->setTilesGridSize( cv::Size (8,8) );
    cv::Mat dst;
    clahe->apply(lab_planes[0], dst);

    // Merge the the color planes back into an Lab image
    dst.copyTo(lab_planes[0]);
    cv::merge(lab_planes, lab_image);

	// convert back to RGB
   	cv::Mat image_clahe;
   	cv::cvtColor(lab_image, image_clahe, CV_Lab2BGR);

	return image_clahe;
	
}

// Timer functionality for code performance testing
double t = 0;

int main() {



	// Define manual driving control
	bool detect = true;

	// Define video capture
	cv::VideoCapture cap("../test_footage/output.avi");

	while ( cap.isOpened() )
    {
		// Send VideoCapture to src Mat
		cap >> src;

		char key = (char) cv::waitKey(1);

		//if (key == 'p') detect = true;
		//else if (key == 'o') detect = false;

		if (detect)
		{
			// Performance test
			t = (double)cv::getTickCount();

			// Get source image height and width
			int src_height = src.size().height;
			int src_width = src.size().width;

			// Select crop region
			cv::Rect crop_region(0, 120, src_width, 120);

			// Crop the image
			cv::Mat crop;
			crop=src(crop_region);

			// Apply CLAHE to capture
			cv::Mat image_clahe = ClaheEqualization(crop);
			
			// Convert to HSV
			cv::Mat hsv;
			cv::cvtColor(crop, hsv, CV_BGR2HSV);

			// Detect floor
			cv::Mat frame_threshold;
			inRange(hsv, cv::Scalar(37, 0, 0), cv::Scalar(179, 255, 255), frame_threshold);

			// Sum image intensity values by columns
			cv::Mat histogramValues;
        	cv::reduce( frame_threshold, histogramValues, 0, CV_REDUCE_SUM, CV_32F );

			// CHECK WHAT VALUES ARE STORED IN histogramVAlues WHEN NO FLOOR DETECTED
	
			// Find min and max valuess
			double min, max;
			cv::minMaxIdx( histogramValues, &min, &max );
			min = 0.8 * max;

			// Filter noise
			std::vector<int> arr;
			size_t c = 0;

			for(int i = 0; i < histogramValues.cols; i++)
			{
				if (histogramValues.at<float>(0, i) >= min) 
				{
					arr.push_back( i );
					c++;
				}
			}

			// Find the average value
        	double average = VectorAvg(arr);

			std::printf("Average: %.3f\n", average);

			DisplayHistogram( src_height, src_width, histogramValues, average);

        	imshow("hsv", hsv);
			imshow("detectedFloor", frame_threshold);
		}	
		else 
		{
			CloseWindows();

		}

		// Total execution time
		std::printf( "Total execution time = %g ms\n", ((double)cv::getTickCount() - t)*1000/cv::getTickFrequency());
    }   

	cv::destroyAllWindows();

    return 0;
} 

void MyFilledCircle( cv::Mat& img, cv::Point center )
{
	/* Source image */
	/* Center point */
	/* Circle size  */
	/* Source image */
	/* Circle color */
	cv::circle( img,
    	center,
    	20,
    	cv::Scalar( 0, 255, 255 ),
    	cv::FILLED,
    	cv::LINE_8 );
}

static double VectorAvg(std::vector<int> const& v) {
    return 1.0 * std::accumulate(v.begin(), v.end(), 0LL) / v.size();
}

void CloseWindows() {
	if (cv::getWindowProperty("Area", cv::WND_PROP_AUTOSIZE) != -1) cv::destroyWindow("Area");
	if (cv::getWindowProperty("histogram", cv::WND_PROP_AUTOSIZE) != -1) cv::destroyWindow("histogram");
	if (cv::getWindowProperty("hsv", cv::WND_PROP_AUTOSIZE) != -1) cv::destroyWindow("hsv");
	if (cv::getWindowProperty("detectedFloor", cv::WND_PROP_AUTOSIZE) != -1) cv::destroyWindow("detectedFloor");
}

void DisplayHistogram( int height, int width, cv::Mat& values, double middlepoint ) {
	
	// Creating a black MAT for histogram image to display
	cv::Mat histogram( height, width, CV_8UC3, cv::Scalar( 0, 0, 0 ) );

	// Variable for storing the height of a histogram line
	double intensity;

	for ( int i = 0; i < width; i++ )
	{
		// Get the intensity of the pixel at a point
		intensity = values.at<float>( 0, i );

		// Draw the line on the histogram image based on the source dimensions
		cv::line( histogram, cv::Point( i, height ),
				  cv::Point( i, height - ( ( int ) intensity / 255 ) ), 
				  cv::Scalar( 140, 0, 255 ), 1, cv::LINE_8 );
	}

	// Draw the curvature pointer
	MyFilledCircle( histogram, cv::Point( ( int ) middlepoint, height ) );

	// Combine the histogram image with the source video 
	cv::Mat result( height, width, CV_8UC3, cv::Scalar( 0, 255, 0 ) );
	cv::Mat thefinal;
	addWeighted( src, 1, histogram, 1, 0, thefinal );

	cv::imshow("Source and Histogram", thefinal);
	cv::imshow("histogram", histogram);
}