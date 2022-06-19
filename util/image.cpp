#include <cassert>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb/stb_image.h"
#include "../stb/stb_image_write.h"

#include <boost/log/trivial.hpp>

#include "image.hpp"

RGBBuffer::RGBBuffer()
{
	width = height = 0;
}

RGBBuffer::RGBBuffer(int width, int height, bool alpha)
{
	assert((width >= 0) && (height >= 0));

	this->width = width;
	this->height = height;
	this->alpha = alpha;
	if (alpha) {
		bytesPerPixel = 4;
	} else{
		bytesPerPixel = 3;
	}

	// initialize an empty picture
	data = vector<uint8_t>(width * height * bytesPerPixel);
}

RGBBuffer::~RGBBuffer()
{
}

const vector<uint8_t> &RGBBuffer::getData() const
{
	return data;
}

uint8_t* RGBBuffer::getDataPointer()
{
	return &data[0];
}

void RGBBuffer::setPixel(int x, int y, rgb rgbdata)
{
	int offset = (x + y * width) * bytesPerPixel;
	data[offset] = rgbdata.r;
	data[offset+1] = rgbdata.g;
	data[offset+2] = rgbdata.b;
}

void RGBBuffer::setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	int offset = (x + y * width) * bytesPerPixel;
	data[offset] = r;
	data[offset+1] = g;
	data[offset+2] = b;
	if (this->alpha) {
		data[offset + 3] = a;
	}
}

void RGBBuffer::setData(const vector<uint8_t> data)
{
	this->data = data;
}

RGBBuffer RGBBuffer::fromImageFile(const string filename, bool useAlpha)
{
	int width = 0;
	int height = 0;
	int bpp = 0;
	unsigned char* imageData;
	int bufflen;
	
	if (useAlpha) {
		imageData = stbi_load(filename.c_str(), &width, &height, &bpp, STBI_rgb_alpha);
		bufflen = width * height * 4;
	}
	else {
		imageData = stbi_load(filename.c_str(), &width, &height, &bpp, STBI_rgb);
		bufflen = width * height * 3;
	}

	if (imageData == nullptr) {
		return RGBBuffer();
	}

	RGBBuffer res = RGBBuffer(width, height, useAlpha);
	res.data.clear();
	for (int i = 0; i < bufflen; i++) {
		res.data.push_back((uint8_t)imageData[i]);
	}

	stbi_image_free(imageData);

	return res;
}

bool RGBBuffer::writeToFile(const string filename) const
{
	if (filename.ends_with(".png")) {
		int channelNumber = 3;
		if (alpha) {
			channelNumber = 4;
		}
		stbi_write_png(filename.c_str(), width, height, channelNumber, &data[0], width * channelNumber);
	}
	else {
		BOOST_LOG_TRIVIAL(error) << "[RGBBuffer] file type of " << filename << " not supported";
		return false;
	}

	return true;
}

bool RGBBuffer::isNull() const
{
	return ((width <= 0) || (height <= 0));
}

bool RGBBuffer::isValid() const
{
	return !isNull();
}
