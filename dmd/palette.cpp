#include <cassert>

#include "palette.hpp"


DMDPalette::DMDPalette(const DMDColor end_color, int bitsperpixel, string name)
{
	this->bitsperpixel = bitsperpixel;
	this->name = name;
	colors.clear();

	int num_colors = 1 << bitsperpixel;

	for (int i = 0; i < num_colors; i++) {
		int fading = 256 * i / num_colors;
		colors.push_back(DMDColor::fade(end_color, fading));
	}

}

DMDPalette::DMDPalette(vector<uint32_t> colors, int bitsperpixel, string name1)
{
	assert(colors.size() >= (int(1) << bitsperpixel));

	this->colors.clear();
	for (const auto c : colors) {
		DMDColor dmdcolor = DMDColor(c);
		this->colors.push_back(c);
	}

	this->bitsperpixel = bitsperpixel;
	this->name = name;
}

int DMDPalette::getIndexOf(uint32_t color, bool ignore_alpha) const {
	for (int i = 0; i < colors.size(); i++) {
		if (colors[i].matches(color, ignore_alpha)) {
			return i;
		}
	}
	return -1;
}

int DMDPalette::getIndexOf(uint8_t r, uint8_t g, uint8_t b) const {
	for (int i = 0; i < colors.size(); i++) {
		if (colors[i].matches(r, g, b)) {
			return i;
		}
	}
	return -1;
}

bool DMDPalette::matchesImage(const RGBBuffer& buf) const
{
	const vector <uint8_t> data = buf.getData();
	int bytesPerPixel = buf.alpha ? 4 : 3;

	for (int i = 0; i < data.size(); i += bytesPerPixel) {
		uint8_t r = data[i];
		uint8_t g = data[i + 1];
		uint8_t b = data[i + 2];
		uint8_t alpha = 0xff;

		if (bytesPerPixel == 4) {
			alpha = data[i + 3];
		}

		if (alpha == 0) {
			continue;
		}

		bool color_found = false;

		for (const auto c : colors) {
			if (c.matches(r, g, b)) {
				color_found = true;
				break;
			}
		}

		if (!(color_found)) {
			return false;
		}
	}
	return true;
}

int DMDPalette::size() const
{
	return colors.size();
}



const std::optional<DMDPalette> find_matching_palette(const vector<DMDPalette> palettes, const RGBBuffer buf)
{
	for (const auto palette : palettes) {
		if (palette.matchesImage(buf)) {
			return palette;
		}
	}

	return std::nullopt;
}


vector<DMDPalette> default_palettes() {
	vector<DMDPalette> res = vector<DMDPalette>();

	// the last color in this palette is the color of the mask rectangle
	vector<uint32_t> pd_4_orange_masked_data{
		0x00000000, 0x11050000, 0x22090000, 0x330e0000,
		0x44120000, 0x55170000, 0x661C0000, 0x77200000,
		0x88000000, 0x99000000, 0xaa000000, 0xbb000000,
		0xcc370000, 0xdd3c0000, 0xee400000, 0xff450000,
		0xfd00fd00 };
	DMDPalette pd_4_orange_mask = DMDPalette(pd_4_orange_masked_data, 4, "pd_4_orange_mask");
	res.push_back(pd_4_orange_mask);

	// this is used internally as the default palette for colorisations for 4bits/pixel
	vector<uint32_t> pd_4_ffc300_data{
		0x00000000, 0x100c0000, 0x21190000, 0x32260000,
		0x43330000, 0x54400000, 0x654d0000, 0x765a0000,
		0x87670000, 0x98740000, 0xa9810000, 0xba8e0000,
		0xcb9b0000, 0xdca80000, 0xedb50000, 0xffc30000 };
	DMDPalette pd_4_ffc300 = DMDPalette(pd_4_ffc300_data, 4, "pd_4_ffc300");
	res.push_back(pd_4_ffc300);

	return res;
}