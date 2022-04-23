#pragma once

#include "../dmd/dmdframe.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/log/trivial.hpp>

class DMDFrameProcessor {

public:
	virtual DMDFrame process_frame(DMDFrame &f);

	virtual bool configure_from_ptree(boost::property_tree::ptree pt_general, boost::property_tree::ptree pt_source);
};