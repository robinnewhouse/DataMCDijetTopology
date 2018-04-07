#include "DataMCbackgroundTools/MiscTools.h"
#include "JetMatchingTools/JetMatchingUtils.h"
#include "TTreeFormula.h"
#include "TopEvent/EventTools.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"
#include "TopCorrections/ScaleFactorCalculator.h"

#include "TopSystematicObjectMaker/ObjectCollectionMaker.h"

#include "TopAnalysis/EventSelectionManager.h"
#include "TopAnalysis/Tools.h"
#include "TopAnalysis/ObjectLoaderBase.h"
#include "TopAnalysis/EventSaverBase.h"
#include "TopEvent/EventTools.h"
#include "TopParticleLevel/TruthTools.h"
#include "JetMatchingTools/DeltaRJetMatchingTool.h"


#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/Jet.h"


#include <memory>
#include <iostream>

using std::cout;
using std::endl;

std::string MV2_name;
bool useTrackJets = false;
std::string bTagWP = "FixedCutBEff_70";
std::unique_ptr<DeltaRJetMatchingTool> jetMatchingTool;


bool WSelected (xAOD::JetContainer::const_iterator jet_itr, const top::Event& event)
{
    return WSelected(*jet_itr, event);
}


bool WSelected (const xAOD::Jet* LJet, const top::Event& event)
{
    double dist_trk  = getMinDistToLooseB(LJet, event, true);
    double dist_calo = getMinDistToLooseB(LJet, event, false);
    double dist = (useTrackJets ? dist_trk : dist_calo);

    if( dist > 1.)
        return true;
    else
        return false;
}


bool TopSelected(xAOD::JetContainer::const_iterator jet_itr, const top::Event& event)
{
    return TopSelected(*jet_itr, event);
}


bool TopSelected(const xAOD::Jet* LJet, const top::Event& event)
{
    // if(LJet->pt() < 300e3) // top has to be boosted at least 300GeV
    //  return false;

    double dist_trk  = getMinDistToLooseB(LJet, event, true);
    double dist_calo = getMinDistToLooseB(LJet, event, false);
    double dist = (useTrackJets ? dist_trk : dist_calo);

    if( dist < 1. * LJet->getSizeParameter())
        return true;
    else
        return false;
}


double getMinDistToLooseB(const xAOD::Jet* LJet, const top::Event& event, bool useTrackJets)
{
    if ( !isLeadingIsoJet(LJet) && LJet->getAlgorithmType() == xAOD::JetAlgorithmType::ID::antikt_algorithm) {
        cout << "ERROR-TWSELECTED:: Only leading jet could pass selection" << endl;
        return false;
    }

    // looking for the distance to the nearest loose b jet
    double min_dist=10000;
    auto *smallJets = &event.m_jets;
    if(useTrackJets) {
        smallJets = &event.m_trackJets;
    }
    for(auto jet : *smallJets) {
        const bool hasMV2 = jet->isAvailable<char>("isbtagged_"+bTagWP);
        if(!hasMV2) {
            if(useTrackJets)
                cout << "ERROR: Missing b-tagging configuration for track jet! BTag WP: " << bTagWP << endl;
            else
                cout << "ERROR: Missing b-tagging configuration for small-R jet! BTag WP: " << bTagWP << endl;
            continue;
        }
        if(!jet->auxdataConst<char>("isbtagged_"+bTagWP))
            continue;

        double dist=top::deltaR(*jet, *LJet);
        if(dist<min_dist) {
            min_dist = dist;
        }
    }
    if(useTrackJets)
        LJet->auxdecor<double>("DR_looseBtrk") = min_dist;
    else
        LJet->auxdecor<double>("DR_looseBcalo") = min_dist;
    return min_dist;
}


void matchTruthToReco(const xAOD::Jet *reco_jet, const xAOD::JetContainer *truth_jets, const xAOD::TruthParticleContainer* partons, float dRjet, float dRparton)
{
    if(partons == nullptr) cout << "Partons are NULL!!" << endl;
    if(truth_jets == nullptr) cout << "Truth jets are NULL!!" << endl;
    // match truth jets to reco jets
    // first make separate xAOD::JetContainer with just the selected leading large-R jet
    // this is for performance -- don't want to match all reco jets
    xAOD::JetContainer *leading_reco_jets = new xAOD::JetContainer();
    xAOD::AuxContainerBase *leading_reco_jets_aux = new xAOD::AuxContainerBase();
    leading_reco_jets->setStore(leading_reco_jets_aux);
    xAOD::Jet *reco_jet_copy = new xAOD::Jet();
    // copy the leading jet (FIXME not sure how to do this without making a copy)
    *reco_jet_copy = *reco_jet;
    leading_reco_jets->push_back(reco_jet_copy);

    bool truthMatchingAfterFSR = true;

    jetMatchingTool->DecorateMatchJets(*truth_jets, *leading_reco_jets, dRjet, "dRmatched_recoJet", "dRmatched_truthJet");
    // take the truth large-R jet matched to reco large-R jet
    const xAOD::Jet *truth_ljet = reco_jet_copy->auxdata<const xAOD::Jet*>("dRmatched_truthJet");
    if(truth_ljet != nullptr) {
        // make separate xAOD::JetContainer with the matched truth large-R jet
        xAOD::JetContainer *matched_truth_jets = new xAOD::JetContainer();
        xAOD::AuxContainerBase *matched_truth_jets_aux = new xAOD::AuxContainerBase();
        matched_truth_jets->setStore(matched_truth_jets_aux);
        xAOD::Jet *truth_ljet_copy = new xAOD::Jet();
        // copy the matched truth jet (FIXME not sure how to do this without making a copy)
        *truth_ljet_copy = *truth_ljet;
        matched_truth_jets->push_back(truth_ljet_copy);
        // match truth wtruth particle to truth jet
        jetMatchingTool->DecorateMatchTruth(*matched_truth_jets, *partons, 6, dRparton, "dRmatched_truthTop", truthMatchingAfterFSR);
        jetMatchingTool->DecorateMatchTruth(*matched_truth_jets, *partons, 24, dRparton, "dRmatched_truthW", truthMatchingAfterFSR);
        // decorate the original matched truth jet
        truth_ljet->auxdecor<const xAOD::TruthParticle*>("dRmatched_truthTop") = truth_ljet_copy->auxdata<const xAOD::TruthParticle*>("dRmatched_truthTop");
        truth_ljet->auxdecor<const xAOD::TruthParticle*>("dRmatched_truthW") = truth_ljet_copy->auxdata<const xAOD::TruthParticle*>("dRmatched_truthW");

        delete matched_truth_jets;
        delete matched_truth_jets_aux;
    }

    // match truth particle quark to reco jet
    jetMatchingTool->DecorateMatchTruth(*leading_reco_jets, *partons, 6, dRparton, "dRmatched_truthTop", truthMatchingAfterFSR);
    jetMatchingTool->DecorateMatchTruth(*leading_reco_jets, *partons, 24, dRparton, "dRmatched_truthW", truthMatchingAfterFSR);

    // decorate the original jet
    reco_jet->auxdecor<const xAOD::Jet*>("dRmatched_truthJet") = truth_ljet;
    reco_jet->auxdecor<const xAOD::TruthParticle*>("dRmatched_truthTop") = reco_jet_copy->auxdata<const xAOD::TruthParticle*>("dRmatched_truthTop");
    reco_jet->auxdecor<const xAOD::TruthParticle*>("dRmatched_truthW") = reco_jet_copy->auxdata<const xAOD::TruthParticle*>("dRmatched_truthW");

    delete leading_reco_jets_aux;
    delete leading_reco_jets;
}


int LargeJetTopology(const xAOD::Jet* reco_jet, const xAOD::Jet* truth_jet, const xAOD::TruthParticle* truth_matched_particle, float dRmax)
{
    topo output = topo::unknown;
     
    // determine leading reco large-R jet containment
    bool save_parton_jet_dR = true;
    if(truth_matched_particle && truth_jet) {
        CharacterizeTopWZJet(truth_matched_particle, reco_jet, dRmax, save_parton_jet_dR);

        // only consider cases when the truth particle (top/W/etc) is hadronically decaying!
        if(reco_jet->auxdata<bool>("isHadronic")) {
            bool b_matched    = reco_jet->auxdata<bool>("isBMatched");
            int  n_w_children = reco_jet->auxdata<int>("nMatchedZWChildren");

            bool qqb = b_matched && (n_w_children == 2);
            bool qq = !b_matched && (n_w_children == 2);
            bool qb = b_matched && (n_w_children == 1);
            bool b = b_matched && (n_w_children == 0);

            if(qqb) { // top topology
                output = topo::t;       
            } else if(qq) { // W topology
                output = topo::W;
            } else if(qb) { // b + quark topology
                output = topo::bq;
            } else if(b) { // b topology
                output = topo::b;
            } else { // others
                output = topo::other; // everything else, when a truth particle is in truth jet matched to reco jet
            }
        } else {
            output = topo::notruth; // didn't match hadronically deacying truth particle (top/W/etc)
        }
    } else {
        output = topo::notruth; // hadronic truth particle (top/W/etc) not matched to truth jet or truth jet not matched to reco jet (or both)
    }

    reco_jet->auxdecor<int>("topology") = static_cast<int>(output);
    return static_cast<int>(output);
}


bool isLeadingIsoJet( const xAOD::Jet* LJet )
{
    if( LJet->isAvailable<int>("largeIsolated") &&  LJet->auxdata<int>("largeIsolated")==1 )
        return true;
    else 
        return false;
}


const xAOD::IParticle* leading_pT_particle(const std::vector<const xAOD::IParticle*> &particleVector)
{
    double pT = -999.;
    const xAOD::IParticle* return_p = nullptr;
    for(const xAOD::IParticle* p : particleVector) {
        if(!p) continue;
        if(p->pt() > pT) {
            return_p = p;
            pT = p->pt();
        }
    }
    return return_p;
}


simpleTaggerPass convertSmoothedTopTaggerResult(const Root::TAccept &res, const smoothedTopTaggerConfig &cfg)
{
    // check invalid and/or out-of-range jets
    if(res.getCutResult("ValidPtRangeHigh")
            && res.getCutResult("ValidPtRangeLow")
            && res.getCutResult("ValidEtaRange")
            && res.getCutResult("ValidJetContent")) {
        // check if jet passed both var1 and var2 cut
        if(res) return simpleTaggerPass::both;

        bool passV1 = false, passV2 = false;

        // var1==Tau32 & var2==Split23
        if(cfg == smoothedTopTaggerConfig::Tau32Split23) {
            passV1 = res.getCutResult("PassTau32");
            passV2 = res.getCutResult("PassSplit23");

        // var1==Mass & var2==Tau32
        } else if(cfg == smoothedTopTaggerConfig::MassTau32) {
            passV1 = res.getCutResult("PassMass");
            passV2 = res.getCutResult("PassTau32");

        // var1==Qw & var2==Tau32
        } else if(cfg == smoothedTopTaggerConfig::QwTau32) {
            passV1 = res.getCutResult("PassQw");
            passV2 = res.getCutResult("PassTau32");
        }

        if(passV1 && !passV2) return simpleTaggerPass::var1;
        if(!passV1 && passV2) return simpleTaggerPass::var2;

    } else {
        return simpleTaggerPass::invalid;
    }

    return simpleTaggerPass::none;
}


simpleTaggerPass convertSmoothedWZTaggerResult(const Root::TAccept &res)
{
    // check invalid and/or out-of-range jets
    if(res.getCutResult("ValidPtRangeHigh")
            && res.getCutResult("ValidPtRangeLow")
            && res.getCutResult("ValidEtaRange")
            && res.getCutResult("ValidJetContent")) {
        // check if jet passed both var1 and var2 cut
        if(res) return simpleTaggerPass::both;

        bool passV1 = false, passV2 = false;

        passV1 = (res.getCutResult("PassMassLow") && res.getCutResult("PassMassHigh"));
        passV2 = res.getCutResult("PassD2");

        if(passV1 && !passV2) return simpleTaggerPass::var1;
        if(!passV1 && passV2) return simpleTaggerPass::var2;

    } else {
        return simpleTaggerPass::invalid;
    }
}


simpleTaggerPass convertMVATaggerResult(const Root::TAccept &res, std::string var2_cut)
{
    // check invalid and/or out-of-range jets
    if(res.getCutResult("ValidPtRangeHigh")
            && res.getCutResult("ValidPtRangeLow")
            && res.getCutResult("ValidEtaRange")
            && res.getCutResult("ValidJetContent")) {
        // check if jet passed both mass and score cut
        if(res) return simpleTaggerPass::both;

        bool passMass = false, passScore = false;

        passMass = (res.getCutResult("PassMassLow") && res.getCutResult("PassMassHigh"));
        passScore = res.getCutResult(var2_cut);

        if(passMass && !passScore) return simpleTaggerPass::var1;
        if(!passMass && passScore) return simpleTaggerPass::var2;
        if(!passMass && !passScore) return simpleTaggerPass::none;

    } else {
        return simpleTaggerPass::invalid;
    }
}