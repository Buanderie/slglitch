
#include "jpegglitcher.h"

#include <random>
#include <iostream>

using namespace std;
using namespace cv;

JpegGlitcher::JpegGlitcher(int width, int height)
    :_width(width), _height(height)
{
    _jpegCompressor = tjInitCompress();

    // Allocate (w*h*3)
    _jpegBuffer = new unsigned char[ _width * _height * 3 ];

    // Allocate (w*h*3)
    _outputBuffer = new unsigned char[ _width * _height * 3 ];
}

JpegGlitcher::~JpegGlitcher()
{
    tjDestroy(_jpegCompressor);
    tjDestroy(_jpegDecompressor);
    delete[] _jpegBuffer;
    delete[] _outputBuffer;
}

cv::Mat JpegGlitcher::glitch(cv::Mat &input, int seed_0, int seed_1, int jpegQuality, int val_min, int val_max, int amount )
{

    // ASSERT IMAGE SIZE
    assert( input.cols == _width && input.rows == _height );

    // preps
    unsigned char* input_img = (unsigned char*)input.data;
    long unsigned int _jpegSize = input.cols * input.rows * 3;

    // encoding
    tjCompress2(_jpegCompressor, input_img, _width, 0, _height, TJPF_RGB,
                &_jpegBuffer, &_jpegSize, TJSAMP_444, jpegQuality,
                TJFLAG_FASTDCT);

    // RNG 0
    std::mt19937 rng0(seed_0);

    // RNG 1
    std::mt19937 rng1(seed_1);

    int headerSize = getJpegHeaderSize( (const unsigned char*)_jpegBuffer, _width*_height*3 );
    int datalen = _jpegSize;// _width*_height*3;

    int max_index = datalen - headerSize - 4;
    int px_min = headerSize + 4;
    int px_max = max_index;
    int delta = px_max - px_min;

    std::uniform_int_distribution<int> uniform_dist_idx(px_min, px_max);
    std::uniform_int_distribution<int> uniform_dist_val(val_min, val_max);

    int iterations = amount;//(int)((double)(delta) * bytes_ratio);
    // cerr << "iterations=" << iterations << endl;
    for( int k = 0; k < iterations; ++k )
    {
        // Select location from rng0
        int idx = uniform_dist_idx( rng0 );

        // Select new value from rng1
        int val = uniform_dist_val( rng1 );

        _jpegBuffer[ idx ] = val;
    }

    cv::Mat output = cv::Mat::zeros( cv::Size(_width, _height), CV_8UC3 );

    // decoding
    _jpegDecompressor = tjInitDecompress();
    int jpegSubsamp;
    int width = _width;
    int height = _height;
    tjDecompressHeader2( _jpegDecompressor, _jpegBuffer, _jpegSize, &width, &height, &jpegSubsamp);
    tjDecompress2( _jpegDecompressor, _jpegBuffer, _jpegSize, _outputBuffer, width, 0/*pitch*/, height, TJPF_RGB, TJFLAG_FASTDCT);
    memcpy( output.data, _outputBuffer, _width * _height * 3 );

    return output;

}

int JpegGlitcher::getJpegHeaderSize(const unsigned char *data, int datalen)
{
    int result = 417;
    for ( int i = 0; i < datalen; i++ )
    {
        if ( data[i] == 0xFF && data[i + 1] == 0xDA )
        {
            result = i + 2;
            break;
        }
    }
    return result;
}
