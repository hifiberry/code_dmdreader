#pragma once

#include <iostream>
#include <fstream>
#include <unordered_set>
#include <queue>
#include <utility>
#include <chrono>

#include <boost/property_tree/ptree.hpp>

#include "genericwriter.hpp"
#include "frameprocessor.hpp"
#include "../dmd/dmdframe.hpp"
#include "palettecolorizer.hpp"

class PNGWriter : public GenericWriter {

public:
	PNGWriter();

	virtual bool configureFromPtree(boost::property_tree::ptree pt_general, boost::property_tree::ptree pt_source) override;

protected:
	string directory;
	void writeFrameToFile(DMDFrame& f, uint32_t timestamp);

	PaletteColorizer colorizer;
};