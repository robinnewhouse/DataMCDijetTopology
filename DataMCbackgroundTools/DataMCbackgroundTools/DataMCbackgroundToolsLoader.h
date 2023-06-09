#ifndef DataMCbackgroundToolsLoader_H_
#define DataMCbackgroundToolsLoader_H_

#include "TopEventSelectionTools/ToolLoaderBase.h"

/**
 * This class is loaded by top-xaod when it starts.  Actually what it does is look for a class
 * that matches the name of the package with the word Loader added on to the end.  If it
 * doesn't find one, it doesn't care.
 */
namespace top {
	class DataMCbackgroundToolsLoader : public ToolLoaderBase {

	public:
		/**
		 * This is where the magic happens.  When top-xaod comes across a cut in the configuration file it tries to load it.
		 * So if it comes across the word "EVEN" it'll make and return a new EvenNumberSelector object (which
		 * you just wrote above).
		*/
		top::EventSelectorBase* initTool(const std::string& name, const std::string& line, TFile* outputFile,std::shared_ptr<top::TopConfig> config, EL::Worker* wk = nullptr);

		ClassDef(top::DataMCbackgroundToolsLoader, 0);
	};
}
#endif
