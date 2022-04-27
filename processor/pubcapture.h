#pragma once

#include <map>
#include <optional>

#include <boost/property_tree/ptree.hpp>
#include <boost/log/trivial.hpp>

#include "../dmd/maskeddmdframe.h"
#include "frameprocessor.h"

class PubCapture : public DMDFrameProcessor {

public:

	virtual bool configureFromPtree(boost::property_tree::ptree pt_general, boost::property_tree::ptree pt_source);
	bool loadTriggers(int bitsperpixel, string directory, std::optional <DMDPalette> palette);

	virtual DMDFrame processFrame(DMDFrame& f);

private:
	DMDPalette palette;
	map<int, MaskedDMDFrame> trigger_frames;
};