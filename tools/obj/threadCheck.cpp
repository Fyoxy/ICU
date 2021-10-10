#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/highgui/highgui_c.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

using namespace std;
using namespace cv;

int main()
{
    if(cv::getNumThreads()==4 && cv::checkHardwareSupport(CV_CPU_NEON)==1)
        std::cout << "OpenCV is optimized" << std::endl;
	else cout << "no" << endl;

	cout << getNumThreads() << endl;
	cout << checkHardwareSupport(CV_CPU_NEON) << endl;



    return 0;
}