#ifndef IMAGE_PIXELARRAY_HPP
#define IMAGE_PIXELARRAY_HPP

#include <cstdint>

namespace KozyLibrary {

/*

image_data			:	is an array of RGBA pixel data

image_pixel_count	:	size of image_data / 4


Width must be equal to height.
Recommended dimensions are 16x16, 32x32 and 48x48

*/
template<uint_fast8_t BYTE_PER_PIXEL>
struct Image_PixelArray {
    inline static constexpr uint_fast8_t get_BytePerPixel() noexcept { return BYTE_PER_PIXEL;}

	Image_PixelArray(const unsigned char* image_data, uint_fast16_t h, uint_fast16_t w): 
        data(nullptr), 
        height(h), 
        width(w)
    {
		if (image_data){
			data = new unsigned char[h*w*BYTE_PER_PIXEL];

			for(uint_fast64_t pos = 0; pos != h*w*BYTE_PER_PIXEL; ++pos)
				data[pos] = image_data[pos];
		}
	}


	Image_PixelArray(const Image_PixelArray& cpy) : 
        data(nullptr), 
        height(cpy.height), 
        width(cpy.width)
    {
		if(cpy.data){
			data = new unsigned char[height*width*BYTE_PER_PIXEL];

			for(uint_fast64_t pos = 0; pos != height*width*BYTE_PER_PIXEL; ++pos)
				data[pos] = cpy.data[pos];
		}

	}


	Image_PixelArray(Image_PixelArray&& mv) : 
        data(mv.data), 
        height(mv.height), 
        width(mv.width) 
    {
		mv.data = nullptr;
	}


	Image_PixelArray& operator=(const Image_PixelArray& cpy) {
		if (this == &cpy)
			return *this;

		height = cpy.height;
		width = cpy.width;

		delete[] data;

		if (cpy.data){
			data = new unsigned char[height*width*BYTE_PER_PIXEL];

			for(uint_fast64_t pos = 0; pos != height*width*BYTE_PER_PIXEL; ++pos)
				data[pos] = cpy.data[pos];
		} else {
            data = nullptr;
        } 

		return *this;
	}

	Image_PixelArray& operator=(Image_PixelArray&& mv){
		if (this == &mv)
			return *this;

		height = mv.height;
		width = mv.width;

		delete[] data;
		data = mv.data;
		mv.data = nullptr;
		

		return *this;
	}

	~Image_PixelArray(){
		delete[] data;
	}

	unsigned char* data;
	uint_fast16_t height, width; 


};

using Image_RGB = Image_PixelArray<3>;
using Image_RGBA = Image_PixelArray<4>;

}

#endif