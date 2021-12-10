#include "detection.hpp"

void Dilation( int, void* );
void Servo_Init();
void MyFilledCircle( cv::Mat img, cv::Point center );
double VectorAvg( std::vector<int> const& v );
void CloseWindows();
void DisplayHistogram( cv::Mat src, int height, int width, cv::Mat values, double middlepoint );

void Detection( Motors* motor ) {

    int curveArr[200] = {0};
    int curveCounter = 0;

    // Define video capture
    cv::Mat src;
	cv::VideoCapture cap(0);

    std::this_thread::sleep_for(std::chrono::milliseconds(5000));

    double t = 0;

    // Set motor speed by default
    motor->SetSpeed( 60 );

    std::cout << "Detection starting" << std::endl;

	while ( cap.isOpened() && !motor->GetControlType() )
    {
        // Check if robot is stuck
        if ( motor->robotStuck ) {

            std::cout << "In stuck" << std::endl;

            if ( curveArr[0] == 0 ) curveArr[0] = SERVO_BASE_ANGLE;
            
            // Temp variables
            int lastAverageCurve;
            int zeroesCounter = 0;

            // Get last 200 curve values and get the average
            for (int i = 0; i < 200; i++) {
                if ( curveArr[i] == 0 ) zeroesCounter++;
                else {
                    lastAverageCurve += curveArr[i];
                }
            }

            lastAverageCurve /= ( 200 - zeroesCounter ); 

            std::cout << "Last average curve was: " << lastAverageCurve << std::endl;

            if ( lastAverageCurve > 1500 ) {
                motor->SetAngle( SERVO_MAX_ANGLE );
            }
            else if ( lastAverageCurve < 1500 ) {
                motor->SetAngle( SERVO_MIN_ANGLE );
            }
            else {
                motor->SetAngle( SERVO_BASE_ANGLE );
            }

            // Wait for servo to turn
            std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
            motor->SetSpeed( 0 );
            std::this_thread::sleep_for( std::chrono::milliseconds( 300 ) );
            // Reverse for 1 second
            motor->SetSpeed( -60 );
            std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );

            // Back to default speed
            motor->SetAngle( SERVO_BASE_ANGLE );
            motor->SetSpeed( 60 );
            motor->robotStuck = 0;
            std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );

        }
        else {

            // Send VideoCapture to src cv::Mat
            cap >> src;
            if(src.empty()) {
                std::cout << "Source empty" << std::endl;
                break;
            }

            // Camera positioned upside-down
            cv::flip(src, src, -1);

            // Performance test
            t = (double) cv::getTickCount();

            // Get source image height and width
            int src_height = src.size().height;
            int src_width = src.size().width;

            cv::Mat crop;
            // Rect variable( Pos. X, Pos. Y, Width, Height )
            cv::Rect crop_region(0, src_height - 180, src_width, 180);

            crop=src(crop_region);

            // Clahe equalization
            // READ RGB color image and convert it to Lab
            cv::Mat lab_image;
            cvtColor(crop, lab_image, CV_BGR2Lab);

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
            cvtColor(lab_image, image_clahe, CV_Lab2BGR);

            cv::GaussianBlur(image_clahe, image_clahe, cv::Size(5, 5), 0);

            // Convert to HSV
            cv::Mat hsv;
            cvtColor(image_clahe, hsv, CV_BGR2HSV);

            // Paint tape black
            //cv::Mat tape;
            //cv::inRange(hsv, cv::Scalar(0, 0, 100), cv::Scalar(180, 255, 255), tape);

            //cv::bitwise_and(black, tape, tape);
            //cv::bitwise_or(hsv, tape, hsv);

            // Detect floor/tape
            cv::Mat frame_threshold;
            inRange(hsv, cv::Scalar(0, 25, 0), cv::Scalar(180, 255, 255), frame_threshold);

            //cv::bitwise_and( tape, frame_threshold, frame_threshold );
            cv::bitwise_not( frame_threshold, frame_threshold );
            //cv::imshow("TAPE", tape);

            // Sum image intensity values by columns
            cv::Mat histogramValues;
            cv::reduce( frame_threshold, histogramValues, 0, CV_REDUCE_SUM, CV_32F );

            // Find min and max values
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

            // RPi servo control
            int divider = ( src_width / 2 ) / 200;
            int curve = ( int ) ( average - ( src_width / 2 ) ) / divider;

            curve = SERVO_BASE_ANGLE - curve;

            //if ( curve < 1500 ) curve -= 50;
            //else if ( curve > 1500 ) curve += 50;

            motor->SetAngle( curve );

            std::cout << "Average: " << (src_height) << std::endl;
            
            curveArr[curveCounter] = curve;

            curveCounter++;

            if (curveCounter >= 200) curveCounter = 0;

            // Total execution time
            t = (double) cv::getTickCount() - t;
            //printf( "Total execution time = %g ms\n", t*1000/ cv::getTickFrequency());
        }

        
    }   
}

void MyFilledCircle( cv::Mat img, cv::Point center )
{
	/* Source image */
	/* Center point */
	/* Circle size  */
	/* Source image */
	/* Circle color */
	circle( img,
    	center,
    	20,
    	cv::Scalar( 0, 255, 255 ),
    	cv::FILLED,
    	cv::LINE_8 );
}

double VectorAvg(std::vector<int> const& v) {
    return 1.0 * std::accumulate(v.begin(), v.end(), 0LL) / v.size();
}

void CloseWindows() {
	if (getWindowProperty("Area", cv::WND_PROP_AUTOSIZE) != -1) cv::destroyWindow("Area");
	if (getWindowProperty("histogram", cv::WND_PROP_AUTOSIZE) != -1) cv::destroyWindow("histogram");
	if (getWindowProperty("hsv", cv::WND_PROP_AUTOSIZE) != -1) cv::destroyWindow("hsv");
	if (getWindowProperty("detectedFloor", cv::WND_PROP_AUTOSIZE) != -1) cv::destroyWindow("detectedFloor");
}

void Handler(int signo)
{
    //System Exit
    printf("\r\nHandler:Motor Stop\r\n");
    Motor_Stop(MOTORA);
    Motor_Stop(MOTORB);
    DEV_ModuleExit();
    cv::destroyAllWindows();
    exit(0);
}

void DisplayHistogram( cv::Mat src, int height, int width, cv::Mat values, double middlepoint ) {
	
	// Creating a black cv::MAT for histogram image to display
	cv::Mat histogram( height, width, CV_8UC3, cv::Scalar( 0, 0, 0 ) );

	// Variable for storing the height of a histogram line
	double intensity;

	for ( int i = 0; i < width; i++ )
	{
		// Get the intensity of the pixel at a point
		intensity = values.at<float>( 0, i );

		// Draw the line on the histogram image based on the source dimensions
		cv::line( histogram, cv::Point( i, height ), cv::Point( i, height - ( ( int ) intensity / 255 ) ), cv::Scalar( 140, 0, 255 ), 1, cv::LINE_8 );
	}

	// Draw the curvature pointer
	MyFilledCircle( histogram, cv::Point( ( int ) middlepoint, height ) );

	// Combine the histogram image with the source video 
	cv::Mat result( height, width, CV_8UC3, cv::Scalar( 0, 255, 0 ) );
	cv::Mat thefinal;
	addWeighted( src, 1, histogram, 1, 0, thefinal );

	imshow("Source and Histogram", thefinal);
	//imshow("histogram", histogram);
}