#include "TFile.h"
#include "TH1D.h"
#include "THStack.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TRandom3.h"
#include "TGraphErrors.h"
#include "TSystem.h"
#include "TGraphAsymmErrors.h"
#include "TFractionFitter.h"
#include "TCanvas.h"

#include "atlasstyle-00-03-05/AtlasStyle.C"
#include "atlasstyle-00-03-05/AtlasLabels.C"
#include "atlasstyle-00-03-05/AtlasUtils.C"

#include "RooRealVar.h"
#include "RooCategory.h"
#include "RooSimultaneous.h"
#include "RooChebychev.h"
#include "RooMCStudy.h"
using namespace RooFit;

#include <iomanip>
#include <iostream>
#include <fstream>
#include <map>
using namespace std;

enum class Channel {
    Electron,
    Muon,
    Combined,
    Dijet,
    GammaJet
};

const map<Channel,TString> CHANNEL_TO_TSTRING_MAP = {
    { Channel::Electron , "elec" } ,
    { Channel::Muon     , "muon" } ,
    { Channel::Combined , "comb" } ,
    { Channel::Dijet    , "dijet" } ,
    { Channel::GammaJet , "gammajet" }
};

enum class Tagger {
    SmoothD2,
    SmoothTau32
};

const map<Tagger,TString> TAGGER_TO_TSTRING = {
    { Tagger::SmoothD2, "D2" },
    { Tagger::SmoothTau32, "Tau32" }
};

class HistoFitSet {
public:
    HistoFitSet(void) {};
    HistoFitSet(const HistoFitSet* hfs, const TString suffix);

    TH1D* data_pass;
    TH1D* data_pass_mw;
    TH1D* signal_matched_pass;
    TH1D* signal_matched_pass_mw;
    TH1D* signal_unmatched_pass;
    TH1D* signal_unmatched_pass_mw;

    TH1D* data_fail;
    TH1D* data_fail_mw;
    TH1D* signal_matched_fail;
    TH1D* signal_matched_fail_mw;
    TH1D* signal_unmatched_fail;
    TH1D* signal_unmatched_fail_mw;
};

HistoFitSet::HistoFitSet(const HistoFitSet* hfs, const TString suffix)
{
    this->data_pass    = (TH1D*) hfs->data_pass->Clone(data_pass->GetName() + suffix);
    this->data_pass_mw = (TH1D*) hfs->data_pass_mw->Clone(data_pass_mw->GetName() + suffix);

    this->signal_matched_pass      = (TH1D*) hfs->signal_matched_pass->Clone(signal_matched_pass->GetName() + suffix);
    this->signal_matched_pass_mw   = (TH1D*) hfs->signal_matched_pass_mw->Clone(signal_matched_pass_mw->GetName() + suffix);
    this->signal_unmatched_pass    = (TH1D*) hfs->signal_unmatched_pass->Clone(signal_unmatched_pass->GetName() + suffix);
    this->signal_unmatched_pass_mw = (TH1D*) hfs->signal_unmatched_pass_mw->Clone(signal_unmatched_pass_mw->GetName() + suffix);

    this->data_fail    = (TH1D*) hfs->data_fail->Clone(data_fail->GetName() + suffix);
    this->data_fail_mw = (TH1D*) hfs->data_fail_mw->Clone(data_fail_mw->GetName() + suffix);

    this->signal_matched_fail      = (TH1D*) hfs->signal_matched_fail->Clone(signal_matched_fail->GetName() + suffix);
    this->signal_matched_fail_mw   = (TH1D*) hfs->signal_matched_fail_mw->Clone(signal_matched_fail_mw->GetName() + suffix);
    this->signal_unmatched_fail    = (TH1D*) hfs->signal_unmatched_fail->Clone(signal_unmatched_fail->GetName() + suffix);
    this->signal_unmatched_fail_mw = (TH1D*) hfs->signal_unmatched_fail_mw->Clone(signal_unmatched_fail_mw->GetName() + suffix);
}

vector<TString> get_systematics_directories(Channel channel) {
    vector <TString> x = {};
    return x;
};

struct HistoFitSet*
get_histogram_set(
        TFile* f,
        const Channel chan,
        const Tagger tagger,
        const TString directory,
        int nsmooth
        )
{
    HistoFitSet* histo_set = new HistoFitSet();

    if (chan == Channel::Dijet) {
        if (tagger == Tagger::SmoothD2) {
            const TString passed_jss_cut      = "h_rljet0_m_w_prerec_50eff_d2cut";
            const TString passed_jss_mass_cut = "h_rljet0_m_w_prerec_50eff_d2masscut";
            const TString failed_jss_cut      = passed_jss_cut + "_VETO";
            const TString failed_jss_mass_cut = passed_jss_mass_cut + "_VETO";

            auto load_dijet_histo = [&] (TString sample, TString tagged_histogram_name) {
                TDirectory* sample_dir = (TDirectory*) f->Get(sample);
                TDirectory* sys_dir = (TDirectory*) sample_dir->Get(directory);
                TH1D* h_tmp = (TH1D*) sys_dir->Get(tagged_histogram_name)->Clone();
                h_tmp->Smooth(nsmooth);
                if (h_tmp == nullptr) throw;
                return h_tmp;
            };

            histo_set->data_pass    = load_dijet_histo("data", passed_jss_cut);
            histo_set->data_pass_mw = load_dijet_histo("data", passed_jss_mass_cut);
            histo_set->data_fail    = load_dijet_histo("data", failed_jss_cut);
            histo_set->data_fail_mw = load_dijet_histo("data", failed_jss_mass_cut);

            histo_set->signal_matched_pass    = load_dijet_histo("pythia_wjets", passed_jss_cut);
            histo_set->signal_matched_pass_mw = load_dijet_histo("pythia_wjets", passed_jss_mass_cut);
            histo_set->signal_matched_fail    = load_dijet_histo("pythia_wjets", failed_jss_cut);
            histo_set->signal_matched_fail_mw = load_dijet_histo("pythia_wjets", failed_jss_mass_cut);

            histo_set->signal_matched_pass->Scale(1.2670);
            histo_set->signal_matched_pass_mw->Scale(1.2670);
            histo_set->signal_matched_fail->Scale(1.2670);
            histo_set->signal_matched_fail_mw->Scale(1.2670);

            histo_set->signal_unmatched_pass    = load_dijet_histo("pythia_dijet", passed_jss_cut);
            histo_set->signal_unmatched_pass_mw = load_dijet_histo("pythia_dijet", passed_jss_mass_cut);
            histo_set->signal_unmatched_fail    = load_dijet_histo("pythia_dijet", failed_jss_cut);
            histo_set->signal_unmatched_fail_mw = load_dijet_histo("pythia_dijet", failed_jss_mass_cut);

            histo_set->signal_unmatched_pass->Scale(0.6);
            histo_set->signal_unmatched_pass_mw->Scale(0.6);
            histo_set->signal_unmatched_fail->Scale(0.6);
            histo_set->signal_unmatched_fail_mw->Scale(0.6);

            const vector<string> non_signal_samples = { "pythia_zjets" , "ttbar_allhad" };
            for (auto const& sample_str : non_signal_samples)
            {
                TH1D* tmp_unmatched_pass    = load_dijet_histo(sample_str, passed_jss_cut);
                TH1D* tmp_unmatched_pass_mw = load_dijet_histo(sample_str, passed_jss_mass_cut);
                TH1D* tmp_unmatched_fail    = load_dijet_histo(sample_str, failed_jss_cut);
                TH1D* tmp_unmatched_fail_mw = load_dijet_histo(sample_str, failed_jss_mass_cut);

                if (sample_str.compare("ttbar_allhad") == 0)  {
                    tmp_unmatched_pass->Scale(1e7);
                    tmp_unmatched_pass_mw->Scale(1e7);
                    tmp_unmatched_fail->Scale(1e7);
                    tmp_unmatched_fail_mw->Scale(1e7);
                } else if (sample_str.compare("pythia_zjets") == 0) {
                    tmp_unmatched_pass->Scale(1.2043);
                    tmp_unmatched_pass_mw->Scale(1.2043);
                    tmp_unmatched_fail->Scale(1.2043);
                    tmp_unmatched_fail_mw->Scale(1.2043);
                }

                histo_set->signal_matched_pass->Add( tmp_unmatched_pass );
                histo_set->signal_matched_pass_mw->Add( tmp_unmatched_pass_mw );
                histo_set->signal_matched_fail->Add( tmp_unmatched_fail );
                histo_set->signal_matched_fail_mw->Add( tmp_unmatched_fail_mw );
            }

        } // end SmoothD2 W-tagger histogram loading
        else {
            throw "tau32 tagging not yet implemented for dijet selection.";
        }

    } else { // end Dijet histogram loading
        throw;
    }

    return histo_set;
}

// {{{ PLOTTING

void
DrawSignalHistFitPlot(
        TString systtype,
        TString vname,
        TString ptbin,
        float mlow,
        float mhigh,
        TH1D* signal_matched,
        TH1D* signal_matched_mw,
        TString outputdir,
        TString plottype,
        TString label
        )
{

    cout<<"DrawSignalHistFitPlot: "<<endl;

    TString ptbintag="";
    TString vnametag="";
    TString chanlab="";

    if(label.Contains("muon")) chanlab="Muon Channel";
    else if(label.Contains("elec")) chanlab="Electron Channel";
    else if(label.Contains("comb")) chanlab="Electron+Muon Channel";
    else if(label.Contains("dijet")) chanlab="Dijet Selection";

    //not studying in bins of pT yet, keep for future
    if(ptbin=="200ex") ptbintag = "200 GeV < p_{T} < 250 GeV";
    if(ptbin=="250ex") ptbintag = "250 GeV < p_{T} < 350 GeV";
    if(ptbin=="350ex") ptbintag = "350 GeV < p_{T} < 500 GeV";

    if(vname=="tau21_ptdep_medium"  ) vnametag="#tau_{21} (Medium p_{T} Dep.)";
    if(vname=="tau21_ptdep_tight"   ) vnametag="#tau_{21} (Tight p_{T} Dep.)";
    if(vname=="d2beta1_ptdep_medium") vnametag="D_{2}^{#beta=1} (Medium p_{T} Dep.)";
    if(vname=="d2beta1_ptdep_tight" ) vnametag="D_{2}^{#beta=1} (Tight p_{T} Dep.)";

    TCanvas *c = new TCanvas("c","c",300,300);

    int lb = signal_matched->GetXaxis()->FindBin(mlow);
    int hb = signal_matched->GetXaxis()->FindBin(mhigh);

    cout<<"Bounds: "<<mlow<<"  "<<mhigh<<"  "<<lb<<"  "<<hb<<endl;

    c->cd();

    signal_matched     ->SetLineColor(4);
    signal_matched_mw  ->SetLineColor(2);
    signal_matched     ->SetLineWidth(4);
    signal_matched_mw  ->SetLineWidth(2);

    signal_matched->GetXaxis()->SetRangeUser(mlow,mhigh);
    float max=signal_matched->GetBinContent(signal_matched->GetMaximumBin());
    signal_matched->SetMaximum(1.5*max);
    signal_matched->GetXaxis()->SetTitle("m [GeV]");
    signal_matched->GetYaxis()->SetTitle("Events / 5 GeV");

    signal_matched->Draw("hist");
    signal_matched_mw->Draw("histsame");

    ATLASLabel(  0.20,0.87,0.035,"Internal",1);
    myText(      0.20,0.82,1, 0.035,"#sqrt{s} = 13 TeV");
    myText(      0.20,0.77,1, 0.035,"L = 3 fb^{-1}");
    //myText(      0.20,0.72,1, 0.035,ptbintag);
    myText(      0.20,0.67,1, 0.035,vnametag);
    myText(      0.20,0.62,1, 0.035,"Variation : "+systtype);
    myBoxText(   0.65,0.83,0.06, 4,3001,4,1,0.035,"Matched");
    myBoxText(   0.65,0.78,0.06, 2,3001,2,1,0.035,"Matched-MW");
    myText(      0.60,0.68,1, 0.035,chanlab);

    c->SaveAs(outputdir+plottype+"_"+label+".eps");

    cout<<"Signal Integrals: "<<signal_matched->Integral(lb,hb)<<"  "<<signal_matched_mw->Integral(lb,hb)<<endl;

}

void
DrawHistFitPlot(
        TString systtype,
        TString vname,
        TString ptbin,
        float mlow,
        float mhigh,
        TH1D* data,
        TH1D* signal_matched,
        TH1D* signal_unmatched,
        TString outputdir,
        TString plottype,
        TString label
        )
{

    cout<<"DrawHistFitPlot: "<<endl;

    TString ptbintag="";
    TString vnametag="";
    TString chanlab="";

    if(label.Contains("muon")) chanlab="Muon Channel";
    if(label.Contains("elec")) chanlab="Electron Channel";
    if(label.Contains("comb")) chanlab="Electron+Muon Channel";

    if(ptbin=="200ex") ptbintag = "200 GeV < p_{T} < 250 GeV";
    if(ptbin=="250ex") ptbintag = "250 GeV < p_{T} < 350 GeV";
    if(ptbin=="350ex") ptbintag = "350 GeV < p_{T} < 500 GeV";

    if(vname=="tau21_ptdep_medium"  ) vnametag="#tau_{21} (Medium p_{T} Dep.)";
    if(vname=="tau21_ptdep_tight"   ) vnametag="#tau_{21} (Tight p_{T} Dep.)";
    if(vname=="d2beta1_ptdep_medium") vnametag="D_{2}^{#beta=1} (Medium p_{T} Dep.)";
    if(vname=="d2beta1_ptdep_tight" ) vnametag="D_{2}^{#beta=1} (Tight p_{T} Dep.)";

    TCanvas *c = new TCanvas("c","c",300,300);

    TH1D *tot = (TH1D*)signal_matched->Clone("tot_pass");
    tot->Add(signal_unmatched);

    int lb = tot->GetXaxis()->FindBin(mlow);
    int hb = tot->GetXaxis()->FindBin(mhigh);

    cout<<"Bounds: "<<mlow<<"  "<<mhigh<<"  "<<lb<<"  "<<hb<<endl;

    c->cd();

    signal_matched    ->SetLineColor(4);
    signal_unmatched  ->SetLineColor(2);
    tot               ->SetLineColor(1);

    TH1D* signal_matched_line = (TH1D*)signal_matched->Clone("signal_matched_line");
    TH1D* signal_unmatched_line = (TH1D*)signal_unmatched->Clone("signal_unmatched_line");
    TH1D* tot_line = (TH1D*)tot->Clone("tot_line");

    data              ->SetLineColor(1);
    data              ->SetMarkerStyle(20);
    data              ->SetMarkerSize(0.1);

    signal_matched               ->SetFillColor(4);
    signal_matched               ->SetFillStyle(3001);
    signal_matched               ->SetMarkerSize(0);

    signal_unmatched               ->SetFillColor(2);
    signal_unmatched               ->SetFillStyle(3001);
    signal_unmatched               ->SetMarkerSize(0);

    tot               ->SetFillColor(1);
    tot               ->SetFillStyle(3001);
    tot               ->SetMarkerSize(0);


    signal_matched_line               ->SetFillColor(0);
    signal_matched_line               ->SetFillStyle(0);
    signal_matched_line               ->SetMarkerSize(0);

    signal_unmatched_line               ->SetFillColor(0);
    signal_unmatched_line               ->SetFillStyle(0);
    signal_unmatched_line               ->SetMarkerSize(0);

    tot_line               ->SetFillColor(0);
    tot_line               ->SetFillStyle(0);
    tot_line               ->SetMarkerSize(0);

    tot->GetXaxis()->SetRangeUser(mlow,mhigh);
    float max=data->GetBinContent(data->GetMaximumBin());
    tot->SetMaximum(1.5*max);
    tot->SetMinimum(0.1);
    tot->GetXaxis()->SetTitle("m [GeV]");
    tot->GetYaxis()->SetTitle("Events / 5 GeV");

    tot->Draw("E2");
    tot_line->Draw("histsame");
    signal_unmatched->Draw("E2same");
    signal_unmatched_line->Draw("histsame");
    signal_matched->Draw("E2same");
    signal_matched_line->Draw("histsame");

    data->Draw("p,same");

    ATLASLabel(  0.20,0.87,0.035,"Internal",1);
    myText(      0.20,0.82,1, 0.035,"#sqrt{s} = 13 TeV");
    myText(      0.20,0.77,1, 0.035,"L = 3 fb^{-1}");
    //myText(      0.20,0.72,1, 0.035,ptbintag);
    //myText(      0.20,0.67,1, 0.035,vnametag);
    myText(      0.20,0.62,1, 0.035,"Variation : "+systtype);
    myMarkerText(0.65,0.88,1,20,0.06,0.035,"Data",0.6);
    myBoxText(   0.65,0.83,0.06, 4,3001,4,1,0.035,"top-Matched");
    myBoxText(   0.65,0.78,0.06, 2,3001,2,1,0.035,"Non-top");
    myBoxText(   0.65,0.73,0.06, 1,3001,1,1,0.035,"Sum MC");
    myText(      0.60,0.68,1, 0.035,chanlab);

    //c->SaveAs(outputdir+plottype+"_"+label+".eps");

    TCanvas *cv2 = new TCanvas("cv2","cv2",600,600);
    cv2->Draw();
    cv2->SetLogy(0);
    cv2->Divide(1,2);
    TPad* top = (TPad*)cv2->cd(1);
    top->SetLogy(0);
    top->SetPad(0.0,0.3,1.0,1.0);
    top->SetBottomMargin(0);
    TPad* bot = (TPad*)cv2->cd(2);
    bot->SetPad(0.0,0.0,1.0,0.3);
    bot->SetFillColor(0);
    bot->SetTopMargin(0);
    bot->SetBottomMargin(0.3);

    for(int i=0; i<(int)tot_line->GetNbinsX(); i++){
        tot_line->SetBinError(i,0.0);
    }

    data->SetMarkerSize(0.1);

    TH1D *ratio_dat = (TH1D*)data->Clone("ratio");
    TH1D *ratio_mc  = (TH1D*)tot->Clone("ratio");
    ratio_dat->Divide(tot_line);
    ratio_mc->Divide(tot_line);

    top->cd();
    tot->Draw("E2");
    tot_line->Draw("histsame");
    signal_unmatched->Draw("E2same");
    signal_unmatched_line->Draw("histsame");
    signal_matched->Draw("E2same");
    signal_matched_line->Draw("histsame");

    data->Draw("psame");

    ATLASLabel(  0.20,0.87,0.05,"Internal",1);
    myText(      0.20,0.81,1, 0.05,"#sqrt{s} = 13 TeV");
    myText(      0.20,0.75,1, 0.05,"L = 3 fb^{-1}");
    //myText(      0.20,0.69,1, 0.05,ptbintag);
    //myText(      0.20,0.63,1, 0.05,vnametag);
    myText(      0.20,0.57,1, 0.05,"Variation : "+systtype);
    myMarkerText(0.70,0.89,1,20,0.06,0.05,"Data",0.8);
    myBoxText(   0.70,0.83,0.06, 4,3001,4,1,0.05,"W Matched");
    myBoxText(   0.70,0.77,0.06, 2,3001,2,1,0.05,"Non-W");
    myBoxText(   0.70,0.71,0.06, 1,3001,1,1,0.05,"Sum MC");
    myText(      0.60,0.62,1, 0.05,chanlab);

    bot->cd();
    float labsize=0.12;
    ratio_mc->SetMinimum(0.5);
    ratio_mc->SetMaximum(1.5);
    ratio_mc->GetXaxis()->SetTitleSize(labsize);
    ratio_mc->GetXaxis()->SetLabelSize(labsize);
    ratio_mc->GetXaxis()->SetTitleOffset(1.1);
    ratio_mc->GetYaxis()->SetTitle("Data/MC");
    ratio_mc->GetYaxis()->SetTitleOffset(0.55);
    ratio_mc->GetYaxis()->CenterTitle();
    ratio_mc->GetYaxis()->SetTitleSize(labsize);
    ratio_mc->GetYaxis()->SetLabelSize(labsize);
    ratio_mc->GetYaxis()->SetNdivisions(405);

    TH1D* ratio_mc_line = (TH1D*)ratio_mc->Clone("ratio_mc_line");
    ratio_mc_line->SetFillColor(0);

    ratio_mc->Draw("E2");
    ratio_mc_line->Draw("histsame");
    ratio_dat->Draw("psame");

    cv2->SaveAs(outputdir+plottype+"_"+label+"_withratio.eps");

    cout<<"Integrals: data="<<data->Integral(lb,hb)<<"  totmc="<<tot->Integral(lb,hb)<<"  bkgmc="<<signal_unmatched->Integral(lb,hb)<<endl;
    cout<<"Subtr: "<<data->Integral(lb,hb)-signal_unmatched->Integral(lb,hb)<<endl;
}

void
DrawPostFitPlot(
        RooCategory sample,
        TString category,
        RooRealVar *myMass,
        RooDataHist* data,
        RooSimultaneous totalPdf,
        TString outputdir,
        TString plottype,
        TString label
        )
{
    cout<<"DrawPostFitPlot: "<<category<<endl;

    TCanvas *c = new TCanvas("c","c",300,300);

    RooPlot* frame1 = myMass->frame();
    data->plotOn(frame1,Cut("sample_tag==sample_tag::"+category), MarkerSize(0.4));
    totalPdf.plotOn(frame1,Slice(sample,category),ProjWData(sample,*data), LineColor(kBlack)) ;
    // double chiS1=frame1->chiSquare();
    // cout << chiS1 << endl;
    totalPdf.plotOn(frame1,Slice(sample,category),Components("pdf_matched_"+category),ProjWData(sample,*data), LineColor(kBlue) ) ;
    totalPdf.plotOn(frame1,Slice(sample,category),Components("pdf_unmatched_"+category),ProjWData(sample,*data), LineColor(kRed) ) ;

    const double max_y = frame1->GetMaximum();
    frame1->SetMaximum(1.5 * max_y);

    c->cd();
    frame1->Draw();

    ATLASLabel(  0.20,0.87,0.04,"Internal",1);
    myText(      0.20,0.82,1, 0.04,"#sqrt{s} = 13 TeV");
    myText(      0.20,0.77,1, 0.04,"L = 3 fb^{-1}");
    myMarkerText(0.70,0.88,1,20,0.06,0.04,"Data",0.8);
    myBoxText(   0.70,0.83,0.06, 4,3001,4,1,0.04,"W Matched");
    myBoxText(   0.70,0.78,0.06, 2,3001,2,1,0.04,"Non-W");
    myBoxText(   0.70,0.73,0.06, 1,3001,1,1,0.04,"Sum MC");
    c->SaveAs(outputdir+plottype+"_"+label+".eps");
}

// }}}

// {{{ EFFICIENCIES

void
GetSignalEfficiencies(
        TString outputdir,
        TString label,
        int lb,int hb,
        TH1D* hist_matched_pass_mw,
        TH1D* hist_matched_pass,
        TH1D* hist_matched_fail_mw,
        TH1D* hist_matched_fail,
        TH1D* hist_unmatched_pass_mw,
        TH1D* hist_unmatched_pass,
        TH1D* hist_unmatched_fail_mw,
        TH1D* hist_unmatched_fail,
        TH1D* hist_pass_mw,
        TH1D* hist_pass,
        TH1D* hist_fail_mw,
        TH1D* hist_fail
        )
{

    cout<<"\nGetting Efficiencies: "<<label<<endl;

    double mc_matched_swPass_err=0.0;
    double mc_matched_totPass_err=0.0;
    double mc_matched_swFail_err=0.0;
    double mc_matched_totFail_err=0.0;
    double mc_unmatched_swPass_err=0.0;
    double mc_unmatched_totPass_err=0.0;
    double mc_unmatched_swFail_err=0.0;
    double mc_unmatched_totFail_err=0.0;
    double data_swPass_err=0.0;
    double data_totPass_err=0.0;
    double data_swFail_err=0.0;
    double data_totFail_err=0.0;

    double mc_matched_swPass      = hist_matched_pass_mw  ->IntegralAndError(lb, hb, mc_matched_swPass_err   );
    double mc_matched_totPass     = hist_matched_pass     ->IntegralAndError(lb, hb, mc_matched_totPass_err  );
    double mc_matched_swFail      = hist_matched_fail_mw  ->IntegralAndError(lb, hb, mc_matched_swFail_err   );
    double mc_matched_totFail     = hist_matched_fail     ->IntegralAndError(lb, hb, mc_matched_totFail_err  );
    double mc_unmatched_swPass    = hist_unmatched_pass_mw->IntegralAndError(lb, hb, mc_unmatched_swPass_err );
    double mc_unmatched_totPass   = hist_unmatched_pass   ->IntegralAndError(lb, hb, mc_unmatched_totPass_err);
    double mc_unmatched_swFail    = hist_unmatched_fail_mw->IntegralAndError(lb, hb, mc_unmatched_swFail_err );
    double mc_unmatched_totFail   = hist_unmatched_fail   ->IntegralAndError(lb, hb, mc_unmatched_totFail_err);
    double data_swPass            = hist_pass_mw          ->IntegralAndError(lb, hb, data_swPass_err         );
    double data_totPass           = hist_pass             ->IntegralAndError(lb, hb, data_totPass_err        );
    double data_swFail            = hist_fail_mw          ->IntegralAndError(lb, hb, data_swFail_err         );
    double data_totFail           = hist_fail             ->IntegralAndError(lb, hb, data_totFail_err        );

    //use histograms for error propogation;
    TH1D* h_mc_matched_swPass    = new TH1D("h_mc_matched_swPass   ","h_mc_matched_swPass   ",1,0,1);
    TH1D* h_mc_matched_totPass   = new TH1D("h_mc_matched_totPass  ","h_mc_matched_totPass  ",1,0,1);
    TH1D* h_mc_matched_swFail    = new TH1D("h_mc_matched_swFail   ","h_mc_matched_swFail   ",1,0,1);
    TH1D* h_mc_matched_totFail   = new TH1D("h_mc_matched_totFail  ","h_mc_matched_totFail  ",1,0,1);
    TH1D* h_mc_unmatched_swPass  = new TH1D("h_mc_unmatched_swPass ","h_mc_unmatched_swPass ",1,0,1);
    TH1D* h_mc_unmatched_totPass = new TH1D("h_mc_unmatched_totPass","h_mc_unmatched_totPass",1,0,1);
    TH1D* h_mc_unmatched_swFail  = new TH1D("h_mc_unmatched_swFail ","h_mc_unmatched_swFail ",1,0,1);
    TH1D* h_mc_unmatched_totFail = new TH1D("h_mc_unmatched_totFail","h_mc_unmatched_totFail",1,0,1);
    TH1D* h_data_swPass          = new TH1D("h_data_swPass         ","h_data_swPass         ",1,0,1);
    TH1D* h_data_totPass         = new TH1D("h_data_totPass        ","h_data_totPass        ",1,0,1);
    TH1D* h_data_swFail          = new TH1D("h_data_swFail         ","h_data_swFail         ",1,0,1);
    TH1D* h_data_totFail         = new TH1D("h_data_totFail        ","h_data_totFail        ",1,0,1);

    h_mc_matched_swPass   ->SetBinContent(1, mc_matched_swPass   );
    h_mc_matched_totPass  ->SetBinContent(1, mc_matched_totPass  );
    h_mc_matched_swFail   ->SetBinContent(1, mc_matched_swFail   );
    h_mc_matched_totFail  ->SetBinContent(1, mc_matched_totFail  );
    h_mc_unmatched_swPass ->SetBinContent(1, mc_unmatched_swPass );
    h_mc_unmatched_totPass->SetBinContent(1, mc_unmatched_totPass);
    h_mc_unmatched_swFail ->SetBinContent(1, mc_unmatched_swFail );
    h_mc_unmatched_totFail->SetBinContent(1, mc_unmatched_totFail);
    h_data_swPass         ->SetBinContent(1, data_swPass         );
    h_data_totPass        ->SetBinContent(1, data_totPass        );
    h_data_swFail         ->SetBinContent(1, data_swFail         );
    h_data_totFail        ->SetBinContent(1, data_totFail        );

    h_mc_matched_swPass   ->SetBinError(1, mc_matched_swPass_err   );
    h_mc_matched_totPass  ->SetBinError(1, mc_matched_totPass_err  );
    h_mc_matched_swFail   ->SetBinError(1, mc_matched_swFail_err   );
    h_mc_matched_totFail  ->SetBinError(1, mc_matched_totFail_err  );
    h_mc_unmatched_swPass ->SetBinError(1, mc_unmatched_swPass_err );
    h_mc_unmatched_totPass->SetBinError(1, mc_unmatched_totPass_err);
    h_mc_unmatched_swFail ->SetBinError(1, mc_unmatched_swFail_err );
    h_mc_unmatched_totFail->SetBinError(1, mc_unmatched_totFail_err);
    h_data_swPass         ->SetBinError(1, data_swPass_err         );
    h_data_totPass        ->SetBinError(1, data_totPass_err        );
    h_data_swFail         ->SetBinError(1, data_swFail_err         );
    h_data_totFail        ->SetBinError(1, data_totFail_err        );



    //MASS WINDOW ONLY;
    TH1D* h_eff_MassWindow_mc_num = (TH1D*)h_mc_matched_swPass->Clone("h_eff_MassWindow_mc_num");
    h_eff_MassWindow_mc_num->Add(h_mc_matched_swFail);
    TH1D* h_eff_MassWindow_mc_den = (TH1D*)h_mc_matched_totPass->Clone("h_eff_MassWindow_mc_den");
    h_eff_MassWindow_mc_den->Add(h_mc_matched_totFail);
    TH1D* h_eff_MassWindow_mc     = (TH1D*)h_eff_MassWindow_mc_num->Clone("h_eff_MassWindow_mc");
    h_eff_MassWindow_mc->Divide(h_eff_MassWindow_mc_den);

    cout<<"HistEffMassWindow(MC):   "<<h_eff_MassWindow_mc_num->GetBinContent(1)<<" ("<<h_eff_MassWindow_mc_num->GetBinError(1)<<") "
        <<"  "<<h_eff_MassWindow_mc_den->GetBinContent(1)<<" ("<<h_eff_MassWindow_mc_den->GetBinError(1)<<") "
        <<"  "<<h_eff_MassWindow_mc->GetBinContent(1)<<" ("<<h_eff_MassWindow_mc->GetBinError(1)<<") "<<endl;


    TH1D* h_eff_MassWindow_data_num = (TH1D*)h_data_swPass->Clone("h_eff_MassWindow_data_num");
    h_eff_MassWindow_data_num->Add(h_data_swFail);
    h_eff_MassWindow_data_num->Add(h_mc_unmatched_swPass,-1);
    h_eff_MassWindow_data_num->Add(h_mc_unmatched_swFail,-1);
    ;
    TH1D* h_eff_MassWindow_data_den = (TH1D*)h_data_totPass->Clone("h_eff_MassWindow_data_den");
    h_eff_MassWindow_data_den->Add(h_data_totFail);
    h_eff_MassWindow_data_den->Add(h_mc_unmatched_totPass,-1);
    h_eff_MassWindow_data_den->Add(h_mc_unmatched_totFail,-1);
    ;
    TH1D* h_eff_MassWindow_data     = (TH1D*)h_eff_MassWindow_data_num->Clone("h_eff_MassWindow_data");
    h_eff_MassWindow_data->Divide(h_eff_MassWindow_data_den);

    cout<<"HistEffMassWindow(DATA): "<<h_eff_MassWindow_data_num->GetBinContent(1)<<" ("<<h_eff_MassWindow_data_num->GetBinError(1)<<") "
        <<"  "<<h_eff_MassWindow_data_den->GetBinContent(1)<<" ("<<h_eff_MassWindow_data_den->GetBinError(1)<<") "
        <<"  "<<h_eff_MassWindow_data->GetBinContent(1)<<" ("<<h_eff_MassWindow_data->GetBinError(1)<<") "<<endl;




    //MASS WINDOW AND TAGGER
    TH1D* h_eff_MassWindowTag_mc_num = (TH1D*)h_mc_matched_swPass->Clone("h_eff_MassWindowTag_mc_num");
    TH1D* h_eff_MassWindowTag_mc_den = (TH1D*)h_mc_matched_totPass->Clone("h_eff_MassWindowTag_mc_den");
    h_eff_MassWindowTag_mc_den->Add(h_mc_matched_totFail);
    TH1D* h_eff_MassWindowTag_mc     = (TH1D*)h_eff_MassWindowTag_mc_num->Clone("h_eff_MassWindowTag_mc");
    h_eff_MassWindowTag_mc->Divide(h_eff_MassWindowTag_mc_den);

    cout<<"HistEffMassWindowTag(MC):   "<<h_eff_MassWindowTag_mc_num->GetBinContent(1)<<" ("<<h_eff_MassWindowTag_mc_num->GetBinError(1)<<") "
        <<"  "<<h_eff_MassWindowTag_mc_den->GetBinContent(1)<<" ("<<h_eff_MassWindowTag_mc_den->GetBinError(1)<<") "
        <<"  "<<h_eff_MassWindowTag_mc->GetBinContent(1)<<" ("<<h_eff_MassWindowTag_mc->GetBinError(1)<<") "<<endl;


    TH1D* h_eff_MassWindowTag_data_num = (TH1D*)h_data_swPass->Clone("h_eff_MassWindowTag_data_num");
    h_eff_MassWindowTag_data_num->Add(h_mc_unmatched_swPass,-1);
    ;
    TH1D* h_eff_MassWindowTag_data_den = (TH1D*)h_data_totPass->Clone("h_eff_MassWindowTag_data_den");
    h_eff_MassWindowTag_data_den->Add(h_data_totFail);
    h_eff_MassWindowTag_data_den->Add(h_mc_unmatched_totPass,-1);
    h_eff_MassWindowTag_data_den->Add(h_mc_unmatched_totFail,-1);
    ;
    TH1D* h_eff_MassWindowTag_data     = (TH1D*)h_eff_MassWindowTag_data_num->Clone("h_eff_MassWindowTag_data");
    h_eff_MassWindowTag_data->Divide(h_eff_MassWindowTag_data_den);

    cout<<"HistEffMassWindowTag(DATA): "<<h_eff_MassWindowTag_data_num->GetBinContent(1)<<" ("<<h_eff_MassWindowTag_data_num->GetBinError(1)<<") "
        <<"  "<<h_eff_MassWindowTag_data_den->GetBinContent(1)<<" ("<<h_eff_MassWindowTag_data_den->GetBinError(1)<<") "
        <<"  "<<h_eff_MassWindowTag_data->GetBinContent(1)<<" ("<<h_eff_MassWindowTag_data->GetBinError(1)<<") "<<endl;


    TH1D *htemp = new TH1D();
    htemp->Fill("MassWindow_mc",     h_eff_MassWindow_mc->GetBinContent(1));
    htemp->SetBinError(1,            h_eff_MassWindow_mc->GetBinError(1));
    htemp->Fill("MassWindow_data",   h_eff_MassWindow_data->GetBinContent(1));
    htemp->SetBinError(2,            h_eff_MassWindow_data->GetBinError(1));
    htemp->Fill("MassWindowTag_mc",  h_eff_MassWindowTag_mc->GetBinContent(1));
    htemp->SetBinError(3,            h_eff_MassWindowTag_mc->GetBinError(1));
    htemp->Fill("MassWindowTag_data",h_eff_MassWindowTag_data->GetBinContent(1));
    htemp->SetBinError(4,            h_eff_MassWindowTag_data->GetBinError(1));

    TFile *fout = new TFile(outputdir+"Eff_"+label+".root","RECREATE");
    htemp->Write("heff");
    fout->Close();

    cout<<endl;

    h_mc_matched_swPass   ->Delete();
    h_mc_matched_totPass  ->Delete();
    h_mc_matched_swFail   ->Delete();
    h_mc_matched_totFail  ->Delete();
    h_mc_unmatched_swPass ->Delete();
    h_mc_unmatched_totPass->Delete();
    h_mc_unmatched_swFail ->Delete();
    h_mc_unmatched_totFail->Delete();
    h_data_swPass         ->Delete();
    h_data_totPass        ->Delete();
    h_data_swFail         ->Delete();
    h_data_totFail        ->Delete();
}


// }}}

void FitScaleFactorSystematics(void) {

    SetAtlasStyle ();
    TH1::SetDefaultSumw2();

    /********************/
    /* MACRO PARAMETERS */
    /********************/

    const TString INPUT_FILEPATH = "/home/zmeadows/ana/TopBosonTagBackground/DataMCbackground/plotting/raw/dijet/08012016.nominal_only.root";
    TFile* INPUT_FILE = TFile::Open(INPUT_FILEPATH);

    const Channel CHANNEL = Channel::Dijet;

    const TString OUTPUT_DIR = "/home/zmeadows/ana/TopBosonTagBackground/DataMCbackground/fitting/08012016/";
    gSystem->MakeDirectory(OUTPUT_DIR);

    vector<TString> directories = { "nominal" };
    for (TString sys_dir : get_systematics_directories(CHANNEL)) {
        directories.push_back(sys_dir);
    }

    const vector<Tagger> TAGGERS_TO_USE = { Tagger::SmoothD2 };

    const int REBIN_AMOUNT      = 5;
    const int SMOOTH_AMOUNT     = 1;
    const float MASS_LOW_BOUND  = 50.;
    const float MASS_HIGH_BOUND = 250.;

    /************************/
    /* END MACRO PARAMETERS */
    /************************/

    string label;

    for (auto const TAGGER : TAGGERS_TO_USE) {
        for (auto const DIRECTORY : directories) {
            HistoFitSet* histo_set = get_histogram_set(INPUT_FILE, CHANNEL, TAGGER, DIRECTORY, SMOOTH_AMOUNT);

            TString systtype = "DEF";
            TString vname = TAGGER_TO_TSTRING.at(TAGGER);
            TString ptBin = "500ex";


            label = "TESTTEST";

            DrawHistFitPlot(systtype, vname, "Full Pt range",
                    MASS_LOW_BOUND, MASS_HIGH_BOUND,
                    histo_set->data_pass,
                    histo_set->signal_matched_pass,
                    histo_set->signal_unmatched_pass,
                    OUTPUT_DIR, "SpectraPreFit", label);

            DrawSignalHistFitPlot(systtype, vname, "Full Pt range",
                    MASS_LOW_BOUND, MASS_HIGH_BOUND,
                    histo_set->signal_matched_pass,
                    histo_set->signal_matched_pass_mw,
                    OUTPUT_DIR, "SignalSpectraPreFit", label);

            const int LOW_MASS_BIN  = histo_set->signal_matched_pass->FindBin(MASS_LOW_BOUND);
            const int HIGH_MASS_BIN = histo_set->signal_matched_pass->FindBin(MASS_HIGH_BOUND);
            const double BIN_WIDTH  = histo_set->signal_matched_pass->GetBinWidth(2) ;
            const double NUM_BINS   = (MASS_HIGH_BOUND - MASS_LOW_BOUND) / BIN_WIDTH;

            cout << "LOWEST MASS BIN NUMBER: "       << LOW_MASS_BIN  << endl;
            cout << "HIGHEST MASS BIN NUMBER: "      << HIGH_MASS_BIN << endl;
            cout << "BIN WIDTH: "                    << BIN_WIDTH     << endl;
            cout << "NUMBER OF BINS IN MASS RANGE: " << NUM_BINS      << endl;

            RooRealVar* myMass = new RooRealVar("Mass","m_{J}",MASS_LOW_BOUND,MASS_HIGH_BOUND,"GeV");
            myMass->setBins(NUM_BINS);
            myMass->Print();

            RooRealVar Mass = *myMass;
            Mass.Print();

            //////////////////////////////////////////////
            //convert Histograms into RooDataHists

            //pass monte carlo
            RooDataHist* rdh_matched_pass   = new RooDataHist("rdh_matched_pass"      , ""                   , Mass            , histo_set->signal_matched_pass); // TODO: fill in empty quotes?
            RooHistPdf*  pdf_matched_pass   = new RooHistPdf("pdf_matched_pass_tag"   , ""                   , RooArgSet(Mass) , *rdh_matched_pass);
            RooDataHist* rdh_unmatched_pass = new RooDataHist("rdh_unmatched_pass"    , "rdh_unmatched_pass" , Mass            , histo_set->signal_unmatched_pass);
            RooHistPdf*  pdf_unmatched_pass = new RooHistPdf("pdf_unmatched_pass_tag" , "rdh_unmatched_pass" , Mass            , *rdh_unmatched_pass);

            //fail monte carlo
            RooDataHist* rdh_matched_fail   = new RooDataHist("rdh_matched_fail"      , ""                   , Mass            , histo_set->signal_matched_fail);
            RooHistPdf*  pdf_matched_fail   = new RooHistPdf("pdf_matched_fail_tag"   , ""                   , RooArgSet(Mass) , *rdh_matched_fail); // TODO: RooArgSet needed?
            RooDataHist* rdh_unmatched_fail = new RooDataHist("rdh_unmatched_fail"    , "rdh_unmatched_fail" , Mass            , histo_set->signal_unmatched_fail);
            RooHistPdf*  pdf_unmatched_fail = new RooHistPdf("pdf_unmatched_fail_tag" , "pdf_unmatched_fail" , Mass            , *rdh_unmatched_fail);

            //total data
            RooDataHist* rdh_data_pass = new RooDataHist("rdh_data_pass" , "rdh_data_pass" , RooArgList(Mass) , histo_set->data_pass);
            RooDataHist* rdh_data_fail = new RooDataHist("rdh_data_fail" , "rdh_data_fail" , RooArgList(Mass) , histo_set->data_fail);

            //------ MC Efficiency ----
            const double passN       = histo_set->signal_matched_pass->Integral(LOW_MASS_BIN, HIGH_MASS_BIN);
            const double failN       = histo_set->signal_matched_fail->Integral(LOW_MASS_BIN, HIGH_MASS_BIN);
            const double totBckgFail = histo_set->signal_unmatched_fail->Integral(LOW_MASS_BIN, HIGH_MASS_BIN);
            const double totBckgPass = histo_set->signal_unmatched_pass->Integral(LOW_MASS_BIN, HIGH_MASS_BIN);

            const double dataPassN = histo_set->data_pass->Integral(LOW_MASS_BIN, HIGH_MASS_BIN);
            const double dataFailN = histo_set->data_fail->Integral(LOW_MASS_BIN, HIGH_MASS_BIN);

            // Now define some efficiency/yield variables
            RooRealVar* eff = new RooRealVar("eff","eff", 0.5, 0., 1. );

            RooRealVar* numSignal   = new RooRealVar("numSignal"   , "numSignal"           , passN+failN     , 100.0           , 15000.0);
            RooFormulaVar* nSigPass = new RooFormulaVar("nSigPass" , "eff*numSignal"       , RooArgList(*eff , *numSignal));
            RooFormulaVar* nSigFail = new RooFormulaVar("nSigFail" , "(1.0-eff)*numSignal" , RooArgList(*eff , *numSignal));

            RooRealVar* nBkgPass    = new RooRealVar("nBkgPass"    , "nBkgPass"            , totBckgPass     , 0.75 * dataPassN , 1.25 * dataPassN);
            RooRealVar* nBkgFail    = new RooRealVar("nBkgFail"    , "nBkgFail"            , totBckgFail     , 0.75 * dataFailN , 1.25 * dataFailN);

            //  RooArgList componentsPass(*signalShapePdf,*bkgShapePdf)
            RooArgList componentsPass(*pdf_matched_pass,*pdf_unmatched_pass);
            RooArgList componentsFail(*pdf_matched_fail,*pdf_unmatched_fail);
            RooArgList yieldsPass(*nSigPass, *nBkgPass);
            RooArgList yieldsFail(*nSigFail, *nBkgFail);
            RooAddPdf pdfPass("pdfPass","extended sum pdf", componentsPass, yieldsPass);
            RooAddPdf pdfFail("pdfFail","extended sum pdf", componentsFail, yieldsFail);

            //The total simultaneous fit
            RooCategory sample_tag("sample_tag","");
            sample_tag.defineType("pass_tag", 1);
            sample_tag.defineType("fail_tag", 2);

            //total dataset to fit
            RooDataHist* fit_data_total = new RooDataHist(
                    "fit_data_total","fit_data_total",
                    RooArgList(Mass), RooFit::Index(sample_tag),
                    RooFit::Import("pass_tag", *histo_set->data_pass),
                    RooFit::Import("fail_tag", *histo_set->data_fail)
                    );

            RooSimultaneous totalPdf("totalPdf","totalPdf", sample_tag);
            totalPdf.addPdf(pdfPass,"pass_tag");
            totalPdf.addPdf(pdfFail,"fail_tag");
            //totalPdf.Print();

            //********* Do the Actual Fit **********
            RooFitResult *fitResult = totalPdf.fitTo(*fit_data_total,
                    RooFit::Save(true), RooFit::Extended(true), RooFit::Minos(true),
                    RooFit::SumW2Error(kTRUE)
                    );

            // fitResult->Print("v");

            RooPlot* frame = myMass->frame();

            //fail plot of post fit
            label=("D2_ptdep_medium_PostFitfail_"+string(DIRECTORY)).c_str();
            totalPdf.plotOn(frame,Slice(sample_tag,"fail_tag"),ProjWData(sample_tag,*fit_data_total)) ;
            DrawPostFitPlot( sample_tag, "fail_tag", myMass, fit_data_total, totalPdf, OUTPUT_DIR, "RooFitPostFit", label);

            label=("D2_ptdep_medium_PostFitpass_"+string(DIRECTORY)).c_str();
            //pass plot of post fit
            totalPdf.plotOn(frame,Slice(sample_tag,"pass_tag"),ProjWData(sample_tag,*fit_data_total)) ;
            DrawPostFitPlot( sample_tag, "pass_tag", myMass, fit_data_total, totalPdf, OUTPUT_DIR, "RooFitPostFit", label);

            /////////////////////////
            // Scale histograms and calculate efficiencies
            double num = nSigPass->getVal() + nSigFail->getVal();
            double den = histo_set->signal_matched_pass->Integral(LOW_MASS_BIN, HIGH_MASS_BIN) +
                         histo_set->signal_matched_fail->Integral(LOW_MASS_BIN, HIGH_MASS_BIN);

            const double scalesig = num / den;

            num = nBkgPass->getVal() + nBkgFail->getVal();

            den = histo_set->signal_unmatched_pass->Integral(LOW_MASS_BIN, HIGH_MASS_BIN) +
                  histo_set->signal_unmatched_fail->Integral(LOW_MASS_BIN, HIGH_MASS_BIN);
            const double scalebkg = num / den;

            cout << "Scales(matched)  :" << endl
                 << "postfit:          " << nSigPass->getVal()<<" + "<<nSigFail->getVal()<<endl
                 << "prefit :          " << histo_set->signal_matched_pass->Integral(LOW_MASS_BIN,HIGH_MASS_BIN)<<" + "<<histo_set->signal_matched_fail->Integral(LOW_MASS_BIN,HIGH_MASS_BIN)<<endl
                 << "scale(pre/post):  " << scalesig<<endl;
            cout << "Scales(unmatched):" << endl
                 << "postfit:          " << nBkgPass->getVal()<<" + "<<nBkgFail->getVal()<<endl
                 << "prefit :          " << histo_set->signal_unmatched_pass->Integral(LOW_MASS_BIN,HIGH_MASS_BIN)<<" + "<<histo_set->signal_unmatched_fail->Integral(LOW_MASS_BIN,HIGH_MASS_BIN)<<endl
                 << "scale(pre/post):  " << scalebkg<<endl;

            histo_set->signal_matched_pass->Scale(scalesig);
            histo_set->signal_matched_pass_mw->Scale(scalesig);
            histo_set->signal_unmatched_pass->Scale(scalebkg);
            histo_set->signal_unmatched_pass_mw->Scale(scalebkg);

            histo_set->signal_matched_pass->Draw("hist");

            label=("D2_SpectraPostFitpass_"+string(DIRECTORY)).c_str();
            DrawHistFitPlot(systtype, vname, ptBin,
                    MASS_LOW_BOUND, MASS_HIGH_BOUND,
                    histo_set->data_pass,
                    histo_set->signal_matched_pass,
                    histo_set->signal_unmatched_pass,
                    OUTPUT_DIR,
                    "SpectraPostFit", label);

            histo_set->signal_matched_fail->Scale(scalesig);
            histo_set->signal_matched_fail_mw->Scale(scalesig);
            histo_set->signal_unmatched_fail->Scale(scalebkg);
            histo_set->signal_unmatched_fail_mw->Scale(scalebkg);


            label=("D2_SpectraPostFitfail_"+string(DIRECTORY)).c_str();
            DrawHistFitPlot(systtype, vname, ptBin,
                    MASS_LOW_BOUND, MASS_HIGH_BOUND,
                    histo_set->data_fail,
                    histo_set->signal_matched_fail,
                    histo_set->signal_unmatched_fail,
                    OUTPUT_DIR,
                    "SpectraPostFit",
                    label
                    );

            GetSignalEfficiencies(OUTPUT_DIR, label,
                    LOW_MASS_BIN, HIGH_MASS_BIN,
                    histo_set->signal_matched_pass_mw,
                    histo_set->signal_matched_pass,
                    histo_set->signal_matched_fail_mw,
                    histo_set->signal_matched_fail,
                    histo_set->signal_unmatched_pass_mw,
                    histo_set->signal_unmatched_pass,
                    histo_set->signal_unmatched_fail_mw,
                    histo_set->signal_unmatched_fail,
                    histo_set->data_pass_mw,
                    histo_set->data_pass,
                    histo_set->data_fail_mw,
                    histo_set->data_fail
                    );

            num = histo_set->signal_matched_pass_mw->Integral();
            den = nSigPass->getVal()+nSigFail->getVal();
            float sf  = num/den;
            cout<<"DirectFitting Method: "<<num<<"  "<<den<<"  "<<sf<<endl;

            num = histo_set->signal_matched_pass_mw->Integral(LOW_MASS_BIN, HIGH_MASS_BIN);
            den = histo_set->signal_matched_pass->Integral(LOW_MASS_BIN, HIGH_MASS_BIN) +
                  histo_set->signal_matched_fail->Integral(LOW_MASS_BIN, HIGH_MASS_BIN);
            sf  = num/den;
            cout<<"Integrating Method:   "<<num<<"  "<<den<<"  "<<sf<<endl;

        } // end loop over nominal + systematic directories
    } // end loop over taggers

    INPUT_FILE->Close();
}

