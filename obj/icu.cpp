#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/highgui/highgui_c.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <numeric>
#include <pthread.h>

#define SERVO_PIN 18 // BCM hardware PWM servo pin

using namespace std;
using namespace cv;

struct thread_data {
	int height;
	int width;
	cv::Mat values;
	double middlepoint;
};


void Dilation( int, void* );
void MyFilledCircle( Mat img, Point center );
double VectorAvg( std::vector<int> const& v );
void CloseWindows();
void *DisplayHistogram( void *threadarg );

Mat src, crop, finalImage;

void *ClaheEqualization( void * ) {
	// Clahe equalization
	// READ RGB color image and convert it to Lab
    Mat lab_image;
    cvtColor(crop, lab_image, CV_BGR2Lab);

    // Extract the L channel
    vector<Mat> lab_planes(3);
    split(lab_image, lab_planes);  // now we have the L image in lab_planes[0]

    // apply the CLAHE algorithm to the L channel
    Ptr<CLAHE> clahe = createCLAHE();
    clahe->setClipLimit(3);
	clahe->setTilesGridSize( Size (8,8) );
    Mat dst;
    clahe->apply(lab_planes[0], dst);

    // Merge the the color planes back into an Lab image
    dst.copyTo(lab_planes[0]);
    cv::merge(lab_planes, lab_image);

	// convert back to RGB
   	Mat image_clahe;
   	cvtColor(lab_image, crop, CV_Lab2BGR);

	pthread_exit(NULL);
}

// Timer functionality for code performance testing
double t = 0;

int main(int argc,char** argv){

	pthread_t threads[3];
	int thread;
	struct thread_data td;

	// Define manual driving control
	bool detect = true;

	// Define video capture
	VideoCapture cap("output.avi");


	while ( cap.isOpened() )
    {
		// Send VideoCapture to src Mat
		cap >> src;
        if(src.empty()) break;

		// Performance test
		t = (double)getTickCount();

		// Get source image height and width
		int src_height = src.size().height;
		int src_width = src.size().width;

		char key = (char) waitKey(1);
		//char key = 'p'; 

		if (key == 'p') detect = true;
		else if (key == 'o') detect = false;

		if (detect)
		{
			//Mat crop;
			Rect crop_region(0, 120, src_width, 120);

			crop=src(crop_region);			

			thread = pthread_create(&threads[1], NULL, ClaheEqualization, NULL);

			//Mat image_clahe = ClaheEqualization(crop);
			
			// Convert to HSV
			Mat hsv;
			cvtColor(crop, hsv, CV_BGR2HSV);

			// Detect floor
			Mat frame_threshold;
			inRange(hsv, Scalar(37, 0, 0), Scalar(179, 255, 255), frame_threshold);

			// Sum image intensity values by columns
			Mat histogramValues;
        	reduce( frame_threshold, histogramValues, 0, CV_REDUCE_SUM, CV_32F );

			// CHECK WHAT VALUES ARE STORED IN histogramVAlues WHEN NO FLOOR DETECTED
	
			// Find min and max valuess
			double min, max;
			minMaxIdx( histogramValues, &min, &max );
			min = 0.8 * max;

			// Filter noise
			vector<int> arr;
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

			cout << average << endl;

			td.height = src_height;
			td.width = src_width;
			td.values = histogramValues;
			td.middlepoint = average;

			thread = pthread_create(&threads[0], NULL, DisplayHistogram, (void *)&td);

			//DisplayHistogram( src_height, src_width, histogramValues, average);

        	//imshow("hsv", hsv);
			//imshow("detectedFloor", frame_threshold);
		}	
		else 
		{
			CloseWindows();

		}
		
		if(!finalImage.empty()) imshow("source", finalImage);

		// Total execution time
		printf( "Total execution time = %g ms\n", ((double)getTickCount() - t)*1000/getTickFrequency());
    }   

	destroyAllWindows();
	pthread_exit(NULL);

    return 0;
} 

void MyFilledCircle( Mat img, Point center )
{
	/* Source image */
	/* Center point */
	/* Circle size  */
	/* Source image */
	/* Circle color */
	circle( img,
    	center,
    	20,
    	Scalar( 0, 255, 255 ),
    	FILLED,
    	LINE_8 );
}

double VectorAvg(std::vector<int> const& v) {
    return 1.0 * std::accumulate(v.begin(), v.end(), 0LL) / v.size();
}

void CloseWindows() {
	if (getWindowProperty("Area", WND_PROP_AUTOSIZE) != -1) destroyWindow("Area");
	if (getWindowProperty("histogram", WND_PROP_AUTOSIZE) != -1) destroyWindow("histogram");
	if (getWindowProperty("hsv", WND_PROP_AUTOSIZE) != -1) destroyWindow("hsv");
	if (getWindowProperty("detectedFloor", WND_PROP_AUTOSIZE) != -1) destroyWindow("detectedFloor");
}

void *DisplayHistogram( void *threadarg ) {

	struct thread_data *data;

	data = (struct thread_data *) threadarg;

	int height = data->height;
	int width = data->width;
	cv::Mat values = data->values;
	double middlepoint = data->middlepoint;
	
	// Creating a black MAT for histogram image to display
	Mat histogram( height, width, CV_8UC3, Scalar( 0, 0, 0 ) );

	// Variable for storing the height of a histogram line
	double intensity;

	for ( int i = 0; i < width; i++ )
	{
		// Get the intensity of the pixel at a point
		intensity = values.at<float>( 0, i );

		// Draw the line on the histogram image based on the source dimensions
		line( histogram, Point( i, height ), Point( i, height - ( ( int ) intensity / 255 ) ), Scalar( 140, 0, 255 ), 1, LINE_8 );
	}

	// Draw the curvature pointer
	MyFilledCircle( histogram, Point( ( int ) middlepoint, height ) );

	// Combine the histogram image with the source video 
	Mat result( height, width, CV_8UC3, cv::Scalar( 0, 255, 0 ) );
	Mat thefinal;
	addWeighted( src, 1, histogram, 1, 0, finalImage );

	//cv::imshow("Source and Histogram", thefinal);
	//cv::imshow("histogram", histogram);

	pthread_exit(NULL);
}