#include <boost/log/trivial.hpp>

#include "datdmdsource.h"

DATDMDSource::DATDMDSource()
{
}

DATDMDSource::DATDMDSource(string filename)
{
	read_file(filename);
}

DATDMDSource::~DATDMDSource()
{
	// TODO: clear
}

bool DATDMDSource::read_file(string filename)
{
	ifstream df(filename.c_str(), ios::in | ios::binary);
	if (!df) {
		BOOST_LOG_TRIVIAL(error) << "can't open file " << filename;
		return false;
	}

	df.exceptions(ifstream::failbit | ifstream::badbit);

	int framecount = 0;
	try
	{
		int rc = 0;
		while (rc == 0) {
			DMDFrame* frame = new DMDFrame();
			rc = frame->read_from_stream(df);
			if (rc == 0) {
				frames.push(frame);
				framecount++;
			}
		}
	}
	catch (int e) {
		BOOST_LOG_TRIVIAL(error) << "I/O error reading " << filename << ": " << e;
		return false;
	}

	BOOST_LOG_TRIVIAL(info) << "successfully loaded " << framecount << " from " << filename;

	return true;
}

DMDFrame* DATDMDSource::next_frame(bool blocking)
{
	DMDFrame* res = frames.front();
	frames.pop();
	return res;
}

bool DATDMDSource::finished()
{
	return frames.empty();
}

bool DATDMDSource::frame_ready()
{
	return (!finished());
}


void DATDMDSource::get_properties(SourceProperties* p) {
	DMDFrame* frame = frames.front();
	if (frame) {
		p->width = frame->get_width();
		p->height = frame->get_height();
		p->bitsperpixel = frame->get_bitsperpixel();
	}
	else {
		p->width = p->height = p->bitsperpixel = 0;
	}

}

bool DATDMDSource::configure_from_ptree(boost::property_tree::ptree pt_general, boost::property_tree::ptree pt_source) {
	return read_file(pt_source.get("name", ""));
}