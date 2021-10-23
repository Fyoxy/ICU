#include "detection.hpp"


using namespace std;
using namespace cv;

void Dilation( int, void* );
void Servo_Init();
void MyFilledCircle( Mat img, Point center );
double VectorAvg( std::vector<int> const& v );
void CloseWindows();
void DisplayHistogram( int height, int width, Mat values, double middlepoint );

Mat src;

void Detection( Motors* motor ) {
    // Define video capture
	VideoCapture cap(0);

    double t = 0;

    // Set motor speed by default
    motor->SetSpeed( 80 );

    cout << "ControlType:: " << motor->GetControlType() << endl;

	while ( cap.isOpened() && motor->GetControlType() )
    {
        cout << "Looping" << endl;

		// Send VideoCapture to src Mat
		cap >> src;
        cout << "Check -1" << endl;
        if(src.empty()) {
			cout << "Source empty" << endl;
			break;
		}

        cout << "Check 0" << endl;

		// Camera positioned upside-down
		flip(src, src, -1);

		// Performance test
		t = (double)getTickCount();

		// Get source image height and width
		int src_height = src.size().height;
		int src_width = src.size().width;

		//char key = (char) waitKey(1);

        cout << "Check 1" << endl;

        Mat crop;
        // X[0] and Y[0], X[1] and Y[1]
        Rect crop_region(0, 120, src_width, 120);

        crop=src(crop_region);

        // Clahe equalization
        // READ RGB color image and convert it to Lab
        Mat lab_image;
        cvtColor(crop, lab_image, CV_BGR2Lab);

        cout << "Check 2" << endl;

        // Extract the L channel
        vector<Mat> lab_planes(3);
        split(lab_image, lab_planes);  // now we have the L image in lab_planes[0]

        // apply the CLAHE algorithm to the L channel
        Ptr<CLAHE> clahe = createCLAHE();
        clahe->setClipLimit(3);
        clahe->setTilesGridSize( Size (8,8) );
        Mat dst;
        clahe->apply(lab_planes[0], dst);

        cout << "Check 3" << endl;

        // Merge the the color planes back into an Lab image
        dst.copyTo(lab_planes[0]);
        cv::merge(lab_planes, lab_image);

        // convert back to RGB
        Mat image_clahe;
        cvtColor(lab_image, image_clahe, CV_Lab2BGR);

        // Convert to HSV
        Mat hsv;
        cvtColor(image_clahe, hsv, CV_BGR2HSV);

        cout << "Check 4" << endl;

        // Detect floor
        Mat frame_threshold;
        inRange(hsv, Scalar(37, 0, 0), Scalar(179, 255, 255), frame_threshold);

        // Sum image intensity values by columns
        Mat histogramValues;
        reduce( frame_threshold, histogramValues, 0, CV_REDUCE_SUM, CV_32F );

        // Find min and max values
        double min, max;
        minMaxIdx( histogramValues, &min, &max );
        min = 0.8 * max;

        // Filter noise
        vector<int> arr;
        size_t c = 0;

        cout << "Check 5" << endl;

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

        //DisplayHistogram( src_height, src_width, histogramValues, average);

        //imshow("hsv", hsv);
        //imshow("detectedFloor", frame_threshold);

        // RPi servo control
        int curve = (int) (average - 320) / 21;

        std::cout << "Curve: " << curve << std::endl;
        
        //if (curve) curve *= -1.3;
        //else if (!curve) curve *= -1.3;
        //curve += angle;

        //if (average > 320) angle
        //if (angle > 15) angle += 15;
        //else if (angle < -15) angle += -15;

        //motor->SetAngle( curve )

		//imshow("source", src);

		// Total execution time
		t = (double)getTickCount() - t;
		printf( "Total execution time = %g ms\n", t*1000/getTickFrequency());
    }   
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

void Handler(int signo)
{
    //System Exit
    printf("\r\nHandler:Motor Stop\r\n");
    Motor_Stop(MOTORA);
    Motor_Stop(MOTORB);
    DEV_ModuleExit();
    destroyAllWindows();
    exit(0);
}

void DisplayHistogram( int height, int width, Mat values, double middlepoint ) {
	
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
	addWeighted( src, 1, histogram, 1, 0, thefinal );

	imshow("Source and Histogram", thefinal);
	imshow("histogram", histogram);
}