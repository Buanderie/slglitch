
#include <iostream>
#include <random>

#include <unistd.h>

#include <opencv2/opencv.hpp>

#include "jpegglitcher.h"

using namespace std;
using namespace cv;

#define PROCESS_IMAGE

int main( int argc, char** argv )
{

    srand(time(NULL));
    bool isPlaying = true;

#ifndef PROCESS_IMAGE
    namedWindow( "frame" );
    cv::VideoCapture cap( argv[1] );

    cv::Mat frame;

    for(;;)
    {
        if( isPlaying )
        {
            cap >> frame;
            if( frame.cols <= 0 || frame.rows <= 0 )
                continue;

        }

        imshow("frame", frame);

        char c = cv::waitKey(5);
        if( c == ' ' )
        {
            isPlaying = !isPlaying;
        }
        else if( c == 'q' )
        {
            break;
        }

    }
#else

    cv::Mat frame = cv::imread( argv[1] );
    imshow( "frame", frame );

    JpegGlitcher glitcher( frame.cols, frame.rows );
    cv::Mat gframe = frame.clone();
    while(true)
    {

        if( isPlaying )
        {
            gframe = glitcher.glitch( frame, rand(), rand(), 50, 0, rand()%255, 0.0005 );
        }

        imshow( "gframe", gframe );

        char c = cv::waitKey(25);
        if( c == ' ' )
        {
            isPlaying = !isPlaying;
        }

    }

#endif

    return 0;

}
