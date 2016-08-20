#ifndef NLJETSELECTOR_H_
#define NLJETSELECTOR_H_

#include "TopEventSelectionTools/SignValueSelector.h"

namespace top {

	/**
	* @brief Does some stuff
	*/
	class NLargeJetSelector : public SignValueSelector {
	
	public:
		explicit NLargeJetSelector(const std::string& params);

		/**
		 * @brief Write a description ;)
		 *				Check the number of large jets
		 * @param event The event to perform the operations on.
		 * @return True if the event passes the selection, false otherwise.
		 */
		 bool apply(const top::Event& event) const override;
	};
}

#endif
