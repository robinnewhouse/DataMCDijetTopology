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

        void set_dRmax(int val)                { this->dRmax = val; }
        void set_truthjet_ptmin(int val)       { this->truthjet_ptmin = val; }
        void set_truthjet_etamax(int val)      { this->truthjet_etamax = val; }
        void set_truthpartile_ptmin(int val)   { this->truthparticle_ptmin = val; }
        void set_truthparticle_etamax(int val) { this->truthparticle_etamax = val; }

    private:
        // cuts
        float dRmax;
        float truthjet_ptmin;
        float truthjet_etamax;
        float truthparticle_ptmin;
        float truthparticle_etamax;

};

#endif // #ifdef TruthMatchTool_h
