#ifndef TruthMatchTool_h
#define TruthMatchTool_h

#include "xAODJet/JetAuxContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"

const xAOD::TruthParticle* GetHadronicWParticle(const xAOD::TruthParticle* inputParticle);
const xAOD::TruthParticle* GetHadronicZParticle(const xAOD::TruthParticle* inputParticle);
const xAOD::TruthParticle* GetHadronicTopParticle(const xAOD::TruthParticle* inputParticle);
const xAOD::TruthParticle* GetParticleAfterFSR(const xAOD::TruthParticle* inputParticle);

int QuarkGluonLabelJet(const xAOD::TruthParticleContainer* truthparticles,
        const xAOD::Jet* fatjet, double dRmax);

int CharacterizeHadronicTopJet(const xAOD::TruthParticle* truthtop,
        const xAOD::Jet* fatjet, double dRmax);

class TruthMatchTool {
    public:
        TruthMatchTool();
        virtual ~TruthMatchTool(void) { };

        int execute(
                const xAOD::JetContainer* reco_jets,
                const xAOD::JetContainer* truth_jets,
                const xAOD::TruthParticleContainer* truth_particles
                );

    private:
        // cuts
        float dRmax;
        float truthjet_ptmin;
        float truthjet_etamax;
        float truthparticle_ptmin;
        float truthparticle_etamax;

};

#endif // #ifdef TruthMatchTool_h
