#pragma once

#include <opencv2/opencv.hpp>
#include <turbojpeg.h>

class JpegGlitcher
{
public:
    JpegGlitcher( int width, int height );
    virtual ~JpegGlitcher();

    cv::Mat glitch(cv::Mat& input, int seed_0, int seed_1, int jpegQuality, int val_min, int val_max, double bytes_ratio );

private:
    unsigned char * _jpegBuffer;
    unsigned char * _outputBuffer;
    unsigned		_width;
    unsigned		_height;
    tjhandle		_jpegCompressor;
    tjhandle		_jpegDecompressor;

    int getJpegHeaderSize( const unsigned char* data, int datalen );

protected:


};
