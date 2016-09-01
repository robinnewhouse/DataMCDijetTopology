#include "DataMCbackgroundTools/TruthMatchTool.h"

// returns pdgid of highest energy parton the jet can be matched to
int QuarkGluonLabelJet(const xAOD::TruthParticleContainer* truthparticles,
        const xAOD::Jet* fatjet, double dRmax)
{
    double Emax = 0;
    TLorentzVector fatjet_vector;
    fatjet_vector.SetPtEtaPhiM(fatjet->pt(), fatjet->eta(), fatjet->phi(), fatjet->m());
    int label = 0;

    for(auto parton : *truthparticles){
        if( parton->pt() < 5000) continue; // avoid errors for pt = 0 particles
        if( parton->absPdgId() > 9 && parton->absPdgId() != 21 ) continue; // not quark or gluon(9 or 21)
        if( parton->e() < Emax) continue; // want the one with the highest energy
        TLorentzVector parton_vector;
        parton_vector.SetPtEtaPhiM(parton->pt(), parton->eta(), parton->phi(), parton->m());
        if( fatjet_vector.DeltaR(parton_vector) < dRmax ){
            label = parton->pdgId();
            Emax = parton->e();
        }
    }
    return label;
}

const xAOD::TruthParticle* GetHadronicZParticle(const xAOD::TruthParticle* inputParticle)
{
    if( !inputParticle || (inputParticle->absPdgId() != 23) ) return nullptr;

    const xAOD::TruthParticle* Z_after_fsr = GetParticleAfterFSR(inputParticle);
    if(Z_after_fsr->nChildren()>0 && Z_after_fsr->child(0)->absPdgId() < 7)
        return Z_after_fsr;
    else
        return nullptr;
}

const xAOD::TruthParticle* GetHadronicWParticle(const xAOD::TruthParticle* inputParticle)
{
    if( !inputParticle || (inputParticle->absPdgId() != 24) ) return nullptr;

    const xAOD::TruthParticle* W_after_fsr = GetParticleAfterFSR(inputParticle);
    if(W_after_fsr->nChildren()>0 && W_after_fsr->child(0)->absPdgId() < 7)
        return W_after_fsr;
    else
        return nullptr;
}

const xAOD::TruthParticle* GetHadronicTopParticle(const xAOD::TruthParticle* inputParticle)
{
    // if input top: follow through FSR, test if decays hadronically, try to match
    // if input W: test if decays hadronically and follow upwards to top
    // return top after fsr or null if no top or no hadronic W
    // FIXME: someone should check this and can one do this more elegantly?
    if( !inputParticle ) return inputParticle;

    const xAOD::TruthParticle* top_after_fsr = nullptr;
    const xAOD::TruthParticle* W_after_fsr = nullptr;

    // get top after FSR
    if( inputParticle->absPdgId() == 6 ){
        top_after_fsr = GetParticleAfterFSR(inputParticle);
        // get W child
        for(unsigned int i_child=0; i_child<top_after_fsr->nChildren(); i_child++){
            if( top_after_fsr->child(i_child)->absPdgId() == 24 ){ // found W, stop
                W_after_fsr = top_after_fsr->child(i_child);
                break;
            }
        }
    }
    // check if W comes from top and get top parent
    else if( inputParticle->absPdgId() == 24){
        top_after_fsr = inputParticle;
        W_after_fsr = inputParticle;
        bool parent_loop = true;
        bool found_top = false;
        while(parent_loop){
            parent_loop = false; // only continue loop if we find suitable parent.
            for(unsigned int i_parent=0; i_parent<top_after_fsr->nParents(); i_parent++){
                if( top_after_fsr->parent(i_parent)->absPdgId() == 6 ){ // found t parent
                    parent_loop = false;
                    found_top = true;
                    top_after_fsr = top_after_fsr->parent(i_parent);
                    break;
                }
                else if(  top_after_fsr->parent(i_parent)->absPdgId() == 24 ){ // found another W, continue
                    parent_loop = true;
                    top_after_fsr = top_after_fsr->parent(i_parent);
                    break;
                }
            }
        }
        if(!found_top) top_after_fsr = nullptr;
    }

    // no good top or W found, we can already abort
    if( !top_after_fsr || !W_after_fsr ) return nullptr;

    // check if W decays hadronically
    if( GetHadronicWParticle(W_after_fsr) )
        return top_after_fsr;
    else
        return nullptr;
}

//=======================================================
//
//=======================================================
// characterize matching between truth jet and hadronic top particle
// 1. is jet dR matched to truth top particle         x 100
// 2. is jet dR matched to truth b particle           x  10
// 3. is jet dR matched to n truth W daughter quarks  x   1
// returns 100*1. + 10*2. + 3.
// to decode: 1. = (returnCode/100)%10, 2. = (returnCode/10)%10, 3. = returnCode%10
int CharacterizeHadronicTopJet(const  xAOD::TruthParticle* truthtop, const xAOD::Jet* fatjet, double dRmax)
{
    int isTopMatched=0;
    int isBMatched=0;
    int nMatchedWChildren = 0;
    int returnCode = 0;

    const xAOD::TruthParticle* truthtop_afterFSR = GetHadronicTopParticle(truthtop);
    if(!truthtop_afterFSR) // not a hadronic top...
        return 0;

    const xAOD::TruthParticle* truthW_afterFSR = nullptr;
    const xAOD::TruthParticle* truthB = nullptr;
    for(unsigned int i_child=0; i_child<truthtop_afterFSR->nChildren(); i_child++)
    {
        if( truthtop_afterFSR->child(i_child)->absPdgId() == 24) // W
            truthW_afterFSR = GetHadronicWParticle(truthtop_afterFSR->child(i_child));
        else if( truthtop_afterFSR->child(i_child)->absPdgId() ==  5 ) // b
            truthB = truthtop_afterFSR->child(i_child);
    }

    TLorentzVector fatjet_vector;
    fatjet_vector.SetPtEtaPhiM(fatjet->pt(), fatjet->eta(), fatjet->phi(), fatjet->m());

    TLorentzVector top_vector;
    top_vector.SetPtEtaPhiM(truthtop_afterFSR->pt(), truthtop_afterFSR->eta(), truthtop_afterFSR->phi(), truthtop_afterFSR->m());
    if( fatjet_vector.DeltaR(top_vector) < dRmax )
        isTopMatched = 1;

    if(truthB){ // FIXME: what to do about tops that don't decay into b but other quark (~5%?!) ?
        TLorentzVector b_vector;
        b_vector.SetPtEtaPhiM(truthB->pt(), truthB->eta(), truthB->phi(), truthB->m());
        if( fatjet_vector.DeltaR(b_vector) < dRmax )
            isBMatched = 1;
    }

    if(truthW_afterFSR){
        for(unsigned int i_child=0; i_child<truthW_afterFSR->nChildren(); i_child++){
            const xAOD::TruthParticle* truthWchild = truthW_afterFSR->child(i_child);
            TLorentzVector WChild_vector;
            WChild_vector.SetPtEtaPhiM(truthWchild->pt(), truthWchild->eta(), truthWchild->phi(), truthWchild->m());
            if( truthWchild->absPdgId() < 7 && fatjet_vector.DeltaR(WChild_vector) < dRmax )
                nMatchedWChildren += 1;
        }
    }

    returnCode = 100*isTopMatched + 10*isBMatched + nMatchedWChildren;
    return returnCode;
}

const xAOD::TruthParticle* GetParticleAfterFSR(const xAOD::TruthParticle* inputParticle){
    // follow decay chain along X -> X (+ Y) until X decays into something else, return last X
    if( !inputParticle ) return inputParticle;

    const xAOD::TruthParticle* after_fsr = inputParticle;
    int pid = inputParticle->pdgId();

    bool fsr_loop = true;
    while(fsr_loop){
        fsr_loop = false; // only continue loop if we find suitable child...
        for(unsigned int i_child=0; i_child<after_fsr->nChildren(); i_child++){
            if( after_fsr->child(i_child)->pdgId() == pid ){
                // child of same type, continue fsr loop child
                fsr_loop = true;
                after_fsr = after_fsr->child(i_child);
                break;
            }
        }
    }

    return after_fsr;
}

TruthMatchTool::TruthMatchTool(void)
{
    dRmax = 0.75;
    truthjet_ptmin = 5000;
    truthjet_etamax = 6;
    truthparticle_ptmin = 5000;
    truthparticle_etamax = 6;
}

int TruthMatchTool::execute(
        const xAOD::JetContainer* inFatJets,
        const xAOD::JetContainer* inTruthJets,
        const xAOD::TruthParticleContainer* allTruthParticles
        )
{
    static xAOD::Jet::Decorator<int> dRmatched_reco_truthDecor        ("dRmatched_reco_truth");
    static xAOD::Jet::Decorator<int> dRmatched_particle_flavorDecor   ("dRmatched_particle_flavor");
    static xAOD::Jet::Decorator<int> dRmatched_maxEParton_flavorDecor ("dRmatched_maxEParton_flavor");
    static xAOD::Jet::Decorator<int> dRmatched_topBChild_decor        ("dRmatched_topBChild");
    static xAOD::Jet::Decorator<int> dRmatched_nQuarkChildren_decor   ("dRmatched_nQuarkChildren");

    // link to matched truth jet
    const std::string link_truthjet_name = "LinkMatchedTruthJet";
    static xAOD::Jet::Decorator<ElementLink<xAOD::JetContainer> > LinkMatchedTruthJetDecor(link_truthjet_name);

    // keep track of objects matched to the two leading truthjets and ensure no double matching happens
    const xAOD::Jet* fatjet_match_ptr[2] = {nullptr, nullptr};
    const xAOD::TruthParticle* truthParticle_match_ptr[2] = {nullptr, nullptr};

    // keep track whether the two leading truthjets have matches and what their properties are
    int dRmatched_reco_truth[2] = {-1, -1};
    int dRmatched_particle_flavor[2] = {0, 0};
    int dRmatched_maxEParton_flavor[2] = {0, 0};
    int dRmatched_topBChild[2] = {0, 0};
    int dRmatched_nQuarkChildren[2] = {0, 0};

    // leading 2 truth jets (if two exist)
    int nTruthJets = std::min(2, (int) inTruthJets->size());

    for(int i_truthJet = 0; i_truthJet < nTruthJets; i_truthJet++) {
        const xAOD::Jet* truthjet_itr = inTruthJets->at(i_truthJet);
        int i_other_truthJet = (i_truthJet+1)%2;

        TLorentzVector truthjet_vector;
        truthjet_vector.SetPtEtaPhiM(truthjet_itr->pt(), truthjet_itr->eta(),
                truthjet_itr->phi(), truthjet_itr->m());

        // make sure that we can match it to (hadronic) truth particle Z,W, or top
        double dRmin_truthParticle = 100;
        for (auto truthparticle_itr : *allTruthParticles) {

            // within accepted phasespace
            if( (truthparticle_itr)->pt() <  truthparticle_ptmin || fabs( (truthparticle_itr)->eta() ) > truthparticle_etamax ) continue;
            // we're only interested in Z,W, and top
            if( fabs((truthparticle_itr) -> pdgId()) != 6 &&
                    fabs((truthparticle_itr) -> pdgId()) != 24 &&
                    fabs((truthparticle_itr) -> pdgId()) != 23) continue;

            // try to find a hadronic (i.e. W->qq) top
            const xAOD::TruthParticle* hadronic_top = GetHadronicTopParticle(truthparticle_itr);
            // try to find a hadronic W, but only if it doesn't come from top
            const xAOD::TruthParticle* hadronic_W = ( hadronic_top == nullptr ? GetHadronicWParticle(truthparticle_itr) : nullptr);
            // try to find a hadronic Z
            const xAOD::TruthParticle* hadronic_Z = GetHadronicZParticle(truthparticle_itr);

            if(hadronic_top && hadronic_top != truthParticle_match_ptr[i_other_truthJet]){
                TLorentzVector top_vector;
                top_vector.SetPtEtaPhiM(hadronic_top->pt(), hadronic_top->eta(), hadronic_top->phi(), hadronic_top->m());
                if( truthjet_vector.DeltaR(top_vector) >= dRmin_truthParticle ) continue; // only consider if closest truth particle yet
                dRmin_truthParticle = truthjet_vector.DeltaR(top_vector);
                int topMatchType      = CharacterizeHadronicTopJet(hadronic_top, truthjet_itr, dRmax);
                int isTopMatched      = (topMatchType/100)%10;
                int isBMatched        = (topMatchType/10)%10;
                int nMatchedWChildren = (topMatchType/1)%10;

                dRmatched_topBChild[i_truthJet] = isBMatched;                          // dR matched to truth b
                dRmatched_nQuarkChildren[i_truthJet] = nMatchedWChildren + isBMatched; // dR matched to n truth W child quarks
                if( isTopMatched == 1 ){ // dR matched to truth top == dR < dRmax
                    dRmatched_particle_flavor[i_truthJet] = hadronic_top->pdgId();
                    truthParticle_match_ptr[i_truthJet] = hadronic_top;
                }
            }

            else if(hadronic_W && hadronic_W != truthParticle_match_ptr[i_other_truthJet]){
                TLorentzVector W_vector;
                W_vector.SetPtEtaPhiM(hadronic_W->pt(), hadronic_W->eta(), hadronic_W->phi(), hadronic_W->m());
                if( truthjet_vector.DeltaR(W_vector) >= dRmin_truthParticle ) continue; // only consider if closest truth particle
                dRmin_truthParticle = truthjet_vector.DeltaR(W_vector);

                //count matched children
                int nMatchedWChildren = 0;
                for(unsigned int i_child=0; i_child<hadronic_W->nChildren(); i_child++){
                    const xAOD::TruthParticle* truthWchild = hadronic_W->child(i_child);
                    TLorentzVector WChild_vector;
                    WChild_vector.SetPtEtaPhiM(truthWchild->pt(), truthWchild->eta(), truthWchild->phi(), truthWchild->m());
                    if( truthWchild->absPdgId() < 7 && truthjet_vector.DeltaR(WChild_vector) < dRmax )
                        nMatchedWChildren += 1;
                }
                dRmatched_nQuarkChildren[i_truthJet] = nMatchedWChildren; // dR matched to n truth W child quarks

                if(dRmin_truthParticle < dRmax){
                    dRmatched_particle_flavor[i_truthJet] = hadronic_W->pdgId();
                    truthParticle_match_ptr[i_truthJet] = hadronic_W;
                }
            }

            else if( hadronic_Z && hadronic_Z != truthParticle_match_ptr[i_other_truthJet] ){
                TLorentzVector Z_vector;
                Z_vector.SetPtEtaPhiM(hadronic_Z->pt(), hadronic_Z->eta(), hadronic_Z->phi(), hadronic_Z->m());
                if( truthjet_vector.DeltaR(Z_vector) >= dRmin_truthParticle ) continue; // only consider if closest truth particle
                dRmin_truthParticle = truthjet_vector.DeltaR(Z_vector);

                //count matched children
                int nMatchedZChildren = 0;
                for(unsigned int i_child=0; i_child<hadronic_Z->nChildren(); i_child++){
                    const xAOD::TruthParticle* truthZchild = hadronic_Z->child(i_child);
                    TLorentzVector ZChild_vector;
                    ZChild_vector.SetPtEtaPhiM(truthZchild->pt(), truthZchild->eta(), truthZchild->phi(), truthZchild->m());
                    if( truthZchild->absPdgId() < 7 && truthjet_vector.DeltaR(ZChild_vector) < dRmax )
                        nMatchedZChildren += 1;
                }
                dRmatched_nQuarkChildren[i_truthJet] = nMatchedZChildren; // dR matched to n truth Z child quarks

                if(dRmin_truthParticle < dRmax){
                    dRmatched_particle_flavor[i_truthJet] = hadronic_Z->pdgId();
                    truthParticle_match_ptr[i_truthJet] = hadronic_Z;
                }
            }
        } // end loop over all truth particles

        // identify matching parton with highest energy
        dRmatched_maxEParton_flavor[i_truthJet] = QuarkGluonLabelJet(allTruthParticles, truthjet_itr, dRmax);

        // try to match reco fatjet to truthjet
        int nFatJets = inFatJets->size();
        for( int i_fatjet = 0; i_fatjet < nFatJets; i_fatjet++ ){
            const xAOD::Jet* fatjet_itr = inFatJets->at(i_fatjet);
            if( fatjet_itr == fatjet_match_ptr[i_other_truthJet]) continue; // fatjet already matched to other truthjet
            TLorentzVector fatjet_vector;
            fatjet_vector.SetPtEtaPhiM((fatjet_itr)->pt(), (fatjet_itr)->eta(), (fatjet_itr)->phi(), (fatjet_itr)->m());

            // try to dR match to truth jet, if match, break loop
            // ==> given jets are pt ordered, this way high pt matches are prefered
            if( fatjet_vector.DeltaR(truthjet_vector) < (dRmax * fatjet_itr->getSizeParameter()) ) {
                dRmatched_reco_truth[i_truthJet] = 1;
                fatjet_match_ptr[i_truthJet] = fatjet_itr;
                break;
            }
        }
    } // end loop over truth jets

    // decorate reco fatjets
    for( auto fatjet_itr : *inFatJets){
        bool matched_jet = false;
        for(int i_truthJet=0; i_truthJet<2; i_truthJet++){
            if(fatjet_itr == fatjet_match_ptr[i_truthJet]){ // this jet was matched to i_truthJet, we can decorate
                matched_jet = true;

                dRmatched_reco_truthDecor(*fatjet_itr) = dRmatched_reco_truth[i_truthJet];
                dRmatched_particle_flavorDecor(*fatjet_itr) = dRmatched_particle_flavor[i_truthJet];
                dRmatched_maxEParton_flavorDecor(*fatjet_itr) =  dRmatched_maxEParton_flavor[i_truthJet];
                dRmatched_nQuarkChildren_decor(*fatjet_itr) = dRmatched_nQuarkChildren[i_truthJet];
                dRmatched_topBChild_decor(*fatjet_itr) = dRmatched_topBChild[i_truthJet];
                // Associate the corresponding matched truth jet to the fatjet
                // We need element link for each fatjet otherwise the jet container will be corrupted
                ElementLink<xAOD::JetContainer> link_truthjet( *inTruthJets, (inTruthJets->at(i_truthJet))->index() );
                LinkMatchedTruthJetDecor(*fatjet_itr) = link_truthjet;
                // if (m_debug) std::cout << "truth pt in the algo: " << (inTruthJets->at(i_truthJet))->pt() << std::endl;
                // If additional reco jets given, link to jet that is matched to the same truth jet (if there is one)
                break;
            }
        }
        if(!matched_jet){
            dRmatched_reco_truthDecor(*fatjet_itr) = -1;
            dRmatched_particle_flavorDecor(*fatjet_itr) = 0;
            dRmatched_maxEParton_flavorDecor(*fatjet_itr) = 0;
            dRmatched_nQuarkChildren_decor(*fatjet_itr) = 0;
            dRmatched_topBChild_decor(*fatjet_itr) = 0;
            // new and
            // FIXME: how to handle not matched fatjets?
            // We need element link for each fatjet otherwise the jet container may be corrupted
            ElementLink<xAOD::JetContainer> link_truthjet( *inTruthJets, -1 );
            LinkMatchedTruthJetDecor(*fatjet_itr) = link_truthjet;
        }
    }

    return 0;
}



