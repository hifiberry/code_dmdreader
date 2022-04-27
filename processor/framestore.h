#pragma once

#include <iostream>
#include <fstream>
#include <unordered_set>
#include <queue>
#include <utility>
#include <chrono>

#include <boost/property_tree/ptree.hpp>

#include "../dmd/dmdframe.h"
#include "frameprocessor.h"

class FrameStore : public DMDFrameProcessor {

public:

	~FrameStore();

	virtual bool configureFromPtree(boost::property_tree::ptree pt_general, boost::property_tree::ptree pt_source) override;
	virtual DMDFrame processFrame(DMDFrame &f) override;

	virtual void close() override;

private:

	void writeFrameToFile(DMDFrame& f, uint32_t timestamp);

	ofstream outputfile;
	bool isFinished = true;
	bool ignore_duplicates = true;
	bool async = false;

	unsigned long startMillisec;

	/// <summary>
	/// Checksums of frames that have been seen already
	/// </summary>
	unordered_set<uint32_t> seen;

	/// <summary>
	/// Queue of frames that are not yet written including a timestamp
	/// </summary>
	queue<pair<DMDFrame, int>> frames_to_write;
 
	int frameno = 1;
};