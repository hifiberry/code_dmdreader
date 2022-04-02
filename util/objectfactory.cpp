#include "../dmdsource/dmdsource.h"
#include "../dmdsource/datdmdsource.h"
#include "../dmdsource/txtdmdsource.h"
#include "../dmd/pubcapture.h"
#include "../dmd/frameprocessor.h"
#include "../dmd/frameinfologger.h"
#include "../render/framerenderer.h"
#include "../render/raylibrenderer.h"

#if __has_include("../colorize/config.h")
#include "../colorize/config.h"
#include "../colorize/vnicolorisation.h"
#endif

DMDSource* createSource(string name) {

	if (name == "dat") {
		return (DMDSource*)(new DATDMDSource());
	} else if (name == "txt") {
		return (DMDSource*)(new TXTDMDSource());
	}
#ifdef VNICOLORING
	else if (name == "vni") {
		return (DMDSource*)(new VniColorisation());
	}
#endif
	else {
		BOOST_LOG_TRIVIAL(error) << "source name " << name << " unknown";
		return NULL;
	}

}

DMDFrameProcessor* createProcessor(string name) {

	if (name == "pubcapture") {
		return (DMDFrameProcessor*)(new PubCapture());
	}
#ifdef VNICOLORING
	else if (name == "vni") {
		return (DMDFrameProcessor*)(new VniColorisation());
	}
#endif
	else if (name == "frameinfo") {
		return (DMDFrameProcessor*)(new FrameInfoLogger());
	}
	else {
		BOOST_LOG_TRIVIAL(error) << "processor name " << name << "unknown";
		return NULL;
	}

}


FrameRenderer* createRenderer(string name) {
	if (name == "raylib") {
		return (FrameRenderer*)(new RaylibRenderer());
	}
	else {
		BOOST_LOG_TRIVIAL(error) << "renderer name " << name << "unknown";
		return NULL;
	}

}
