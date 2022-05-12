#pragma once

#include <vector>

#include "../drm/videoplayer.hpp"
#include "../services/service.hpp"


class PUPPlayer : public Service {

public:
	PUPPlayer(int screenNumber = 0);
	~PUPPlayer();

	virtual bool configureFromPtree(boost::property_tree::ptree pt_general, boost::property_tree::ptree pt_source) override;

	void playEvent(int event);

private:
	vector <std::shared_ptr<VideoPlayer>> players;
};