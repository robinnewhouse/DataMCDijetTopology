#ifndef MISCTOOLS_H_
#define MISCTOOLS_H_

#include <xAODRootAccess/TEvent.h>
#include <xAODRootAccess/TStore.h>
//my include for objects and containers 
#include <xAODTruth/TruthParticleContainer.h>
#include <xAODTruth/TruthParticle.h>
#include <xAODJet/JetContainer.h>
#include <xAODJet/JetAuxContainer.h>
#include <xAODJet/Jet.h>
#include <TopEvent/Event.h>
#include "DataMCbackgroundTools/classification.h"
#include "JetMatchingTools/DeltaRJetMatchingTool.h"
#include <PATCore/TAccept.h>
#include <string>

// b-tagging thresholds
extern std::string bTagWP;
extern bool useTrackJets;
extern std::unique_ptr<DeltaRJetMatchingTool> jetMatchingTool; // new truth-matching tool from MC optimisation


// /// Shortcut for creation of unique pointers, until we start using C++14 ;-)
// template<typename T, typename ...Args> std::unique_ptr<T> make_unique(Args&& ...args)
// {
//     return std::unique_ptr<T>( new T( std::forward<Args>(args)... ) );
// }


/** check if larget jet has W-like topology
 * top-like: found b-tagged R=0.4 jet within large jet cone
 * W-like: found b-tagged R=0.4 jet outside of large jet cone
 * 'inside'/'outside' is determined according to dR between axes of the jets */
bool WSelected(xAOD::JetContainer::const_iterator jet_itr, const top::Event& event);
bool WSelected(const xAOD::Jet* LJet, const top::Event& event);


/** check if larget jet has W-like topology
 * top-like: found b-tagged R=0.4 jet within large jet cone
  * W-like: found b-tagged R=0.4 jet outside of large jet cone
  * 'inside'/'outside' is determined according to dR between axes of the jets */
bool TopSelected(xAOD::JetContainer::const_iterator jet_itr, const top::Event& event);
bool TopSelected(const xAOD::Jet* LJet, const top::Event& event);


/** general function to evaluate the minimal distance from the largeR jet 
 * to a losse b-tagged calo jet from event.m_jets */ 
double getMinDistToLooseB(const xAOD::Jet* LJet, const top::Event& event, bool useTrackJets=true);


void matchTruthToReco(const xAOD::Jet *reco_jet, const xAOD::JetContainer *truth_jets, const xAOD::TruthParticleContainer* partons, float dRjet, float dRparton);

/** Return an integer representing type of event toplogy:
	* 1==top-like
	* 2==W-like
	* 3==qb-like
	* 4==b-like
	* 5==other (everything not falling into above)
    * 6==notop (truth top and/or truth jet not matched to reco jet)
	* 0==does not pass cuts -> should not happen if event selection cuts are OK */
// int LargeJetTopology( xAOD::JetContainer::const_iterator jet_itr, const xAOD::TruthParticleContainer* partons, bool ttbar);
// int LargeJetTopology( const xAOD::Jet* LJet, const xAOD::TruthParticleContainer* partons, bool ttbar);
int LargeJetTopology(const xAOD::Jet* reco_jet, const xAOD::Jet* truth_jet, const xAOD::TruthParticle* truth_matched_particle, float dRmax);

// check if a fat jet is the leading isolated jet
bool isLeadingIsoJet( const xAOD::Jet* LJet );

// return the highest-pT IParticle from a vector of IParticles
const xAOD::IParticle* leading_pT_particle(const std::vector<const xAOD::IParticle*> &particleVector);

// convert result object of SmootherTopTagger into our own unified enum result interface
simpleTaggerPass convertSmoothedTopTaggerResult(const Root::TAccept &res, const smoothedTopTaggerConfig &cfg);

// convert result object of SmootherWZTagger into our own unified enum result interface
simpleTaggerPass convertSmoothedWZTaggerResult(const Root::TAccept &res);

// convert result object of JSSWTopTaggerDNN and JSSWTopTaggerBDT into our own unified enum result interface
simpleTaggerPass convertMVATaggerResult(const Root::TAccept &res, std::string var2_cut="PassScore");

#endif
