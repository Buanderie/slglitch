
#include <iostream>
#include <random>

#include <unistd.h>

#include <opencv2/opencv.hpp>

#include "jpegglitcher.h"

using namespace std;
using namespace cv;

#define CVUI_IMPLEMENTATION
#include "cvui.h"

#define PROCESS_IMAGE
#define WINDOW_NAME "CVUI Hello World!"

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

    /*
    cv::Mat frame = cv::imread( argv[1] );
    imshow( "frame", frame );
    JpegGlitcher glitcher( frame.cols, frame.rows );
    cv::Mat gframe = frame.clone();
    while(true)
    {

        if( isPlaying )
        {
            gframe = glitcher.glitch( frame, rand(), rand(), 50, 0, rand()%255, 0.00005 );
        }

        cv::Mat rgframe;
        cv::resize( gframe, rgframe, cv::Size(), 0.5, 0.5 );

        imshow( "gframe", rgframe );

        char c = cv::waitKey(0);
        if( c == ' ' )
        {
            isPlaying = !isPlaying;
        }

    }
    */

    cv::Mat input = cv::imread( argv[1] );
    // imshow( "frame", frame );
    JpegGlitcher glitcher( input.cols, input.rows );
    cv::Mat gframe = input.clone();

    // Create a frame where components will be rendered to.
    cv::Mat frame = cv::Mat(200, 500, CV_8UC3);

    // Init cvui and tell it to create a OpenCV window, i.e. cv::namedWindow(WINDOW_NAME).
    cvui::init(WINDOW_NAME);

    int seed0 = 0;
    int seed1 = 0;
    int amount = 1;
    int quality = 50;

    bool needUpdate = false;

    while (true) {
        // Fill the frame with a nice color
        frame = cv::Scalar(49, 52, 49);

        // Render UI components to the frame
        if( cvui::trackbar(frame, 5, 10, 240, &seed0, (int)0., (int)1024.) )
        {
            needUpdate = true;
        }

        if( cvui::trackbar(frame, 5, 60, 240, &seed1, (int)0., (int)1024.) )
        {
            needUpdate = true;
        }

        if( cvui::trackbar(frame, 5, 110, 240, &amount, (int)0., (int)100.) )
        {
            needUpdate = true;
        }

        if( cvui::trackbar(frame, 5, 160, 240, &quality, (int)10., (int)100.) )
        {
            needUpdate = true;
        }

        if (cvui::button(frame, 300, 80, "&Quit")) {
            break;
        }

        if( needUpdate )
        {
            cerr << "update. seed0=" << seed0 << " seed1=" << seed1 << " amount=" << amount << endl;
            gframe = glitcher.glitch( input, seed0, seed1, quality, 0, rand()%255, amount );
            needUpdate = false;
        }

        // Update cvui stuff and show everything on the screen
        cvui::imshow(WINDOW_NAME, frame);

        cv::Mat rgframe;
        cv::resize( gframe, rgframe, cv::Size(), 0.5, 0.5 );
        imshow( "gframe", rgframe );

        if (cv::waitKey(20) == 27) {
            break;
        }

    }

#endif

    return 0;

}
