#include "frameprocessor.h"

DMDFrame DMDFrameProcessor::process_frame(DMDFrame& f)
{
	return f;
}

bool DMDFrameProcessor::configure_from_ptree(boost::property_tree::ptree pt_general, boost::property_tree::ptree pt_source) {
	return false;
};
