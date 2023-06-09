#include "DataMCbackgroundTools/DataMCbackgroundToolsLoader.h"
#include "DataMCbackgroundTools/NLargeJetSelector.h"
#include "DataMCbackgroundTools/GammaJetSelector.h"
#include "DataMCbackgroundTools/VVJJSelector.h"
#include "TopAnalysis/Tools.h" //for the loadLibraries function
#include "TopConfiguration/TopConfig.h"
#include <iostream>
#include "TFile.h"


namespace top {

top::EventSelectorBase* DataMCbackgroundToolsLoader::initTool(const std::string& name, const std::string& line, TFile* outputFile, std::shared_ptr<top::TopConfig> config, EL::Worker* wk)
{
	std::istringstream iss(line);
	std::string toolname;
	getline(iss, toolname, ' ');

	// any parameters?
	std::string param;
	if(line.size() > toolname.size())
		param = line.substr(toolname.size() + 1);

  if (toolname == "LARGEJET_N") {
    return new top::NLargeJetSelector(param);
  } else if (toolname == "GAMMAJET") {
    return new top::GammaJetSelector();
  } else if (toolname == "VVJJ") {
    return new top::VVJJSelector();
  }

	// we need it to fall through at the end in case the tool is in someone else's library (not yours)
	// don't worry the main code checks for a nullptr
	return nullptr;
}

}
