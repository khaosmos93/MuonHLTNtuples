#include "TROOT.h"
#include "TStyle.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TTree.h"
#include "TBranch.h"

#include <iostream>
#include <algorithm>
#include <vector>
#include <map>

#include "DataFormats/Math/interface/deltaR.h"
#include "MuonHLTNtuples/Analyzers/src/MuonTree.h"
#include "TLorentzVector.h"

#include <ctime>
#include <TStopwatch.h>
#include <TTimeStamp.h>

using namespace std;

double muonmass = 0.10565837;

double offlineIsoCut = 0.15;

double pt_bins[14]  = { 20 ,  24 ,  27 ,   30,   35,   40,   45,   50,  60, 70 ,  90, 150, 250, 500 };
double eta_bins[16] = {-2.4, -2.1, -1.6, -1.2, -1.04, -0.9, -0.3, -0.2,  0.2, 0.3, 0.9, 1.04, 1.2, 1.6, 2.1, 2.4};

bool matchMuon              (MuonCand, std::vector<HLTObjCand>, std::string);
bool selectTagMuon          (MuonCand, double );
bool selectProbeMuonLoose   (MuonCand, HLTObjCand, double, TH1D*, TH1D* );
bool selectProbeMuonTight   (MuonCand, HLTObjCand, double, TH1D* );
bool isSameMuon             (MuonCand, MuonCand);
static inline void loadBar(int x, int n, int r, int w);

/*======================================================================*/

void ReadNtuples_Purity_OffMatching_v2(TString Period, TString Menu, std::string hltname, bool verbose) {

  // -- General Info & Filter assign -- //
    TTimeStamp ts_start;
    cout << "[Start Time(local time): " << ts_start.AsString("l") << "]" << endl;

    TStopwatch totaltime;
    totaltime.Start();

    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,sizeof(buffer),"%Y%m%d",timeinfo);
    TString theDate(buffer);

    double OnlinePtCut  = 99999;
    double OfflinePtCut = 99999;
    std::string filter;
    if (hltname.find("HLT_IsoMu27_v") != std::string::npos ) {
      filter = "hltL3crIsoL1sMu22Or25L1f0L2f10QL3f27QL3trkIsoFiltered0p07";
      OnlinePtCut  = 27;
      OfflinePtCut = 29;
    }
    else if (hltname.find("HLT_Mu50_v") != std::string::npos ) {
      filter = "hltL3fL1sMu22Or25L1f0L2f10QL3Filtered50Q";
      OnlinePtCut  = 50;
      OfflinePtCut = 53;
    }

    if(Period == "Old") {
      if (hltname.find("HLT_IsoMu27_v") != std::string::npos ) {
        filter = "hltL3crIsoL1sMu22Or25L1f0L2f10QL3f27QL3trkIsoFiltered0p09";
        OnlinePtCut  = 27;
        OfflinePtCut = 29;
      }
      else if (hltname.find("HLT_Mu50_v") != std::string::npos ) {
        filter = "hltL3fL1sMu22Or25L1f0L2f10QL3Filtered50Q";
        OnlinePtCut  = 50;
        OfflinePtCut = 53;
      }
    }

  // -- Histograms -- //
    TH1D *h_mass_TnP                    = new TH1D("h_mass_TnP", "h_mass_TnP", 200, 0, 200);
    TH1D *h_mass_TnP_Loose              = new TH1D("h_mass_TnP_Loose", "h_mass_TnP_Loose", 200, 0, 200);
    TH1D *h_mass_TnP_Tight              = new TH1D("h_mass_TnP_Tight", "h_mass_TnP_Tight", 200, 0, 200);

    TH1D *h_LoSel_hltPt                 = new TH1D("h_LoSel_hltPt", "h_LoSel_hltPt", 13,  pt_bins);
    TH1D *h_LoSel_hltEta                = new TH1D("h_LoSel_hltEta", "h_LoSel_hltEta", 24, -2.4, 2.4);
    TH1D *h_LoSel_hltEta_bin            = new TH1D("h_LoSel_hltEta_bin", "h_LoSel_hltEta_bin", 15, eta_bins);
    TH1D *h_LoSel_hltPhi                = new TH1D("h_LoSel_hltPhi", "h_LoSel_hltPhi", 40, -3.2, 3.2);
    TH2D *h_LoSel_hltEtaPhi             = new TH2D("h_LoSel_hltEtaPhi", "h_LoSel_hltEtaPhi", 15, eta_bins, 40, -3.2, 3.2);

    TH1D *h_LoSel_recoPt                = new TH1D("h_LoSel_recoPt", "h_LoSel_recoPt", 13,  pt_bins);
    TH1D *h_LoSel_recoEta               = new TH1D("h_LoSel_recoEta", "h_LoSel_recoEta", 24, -2.4, 2.4);
    TH1D *h_LoSel_recoEta_bin           = new TH1D("h_LoSel_recoEta_bin", "h_LoSel_recoEta_bin", 15, eta_bins);
    TH1D *h_LoSel_recoPhi               = new TH1D("h_LoSel_recoPhi", "h_LoSel_recoPhi", 40, -3.2, 3.2);
    TH2D *h_LoSel_recoEtaPhi            = new TH2D("h_LoSel_recoEtaPhi", "h_LoSel_recoEtaPhi", 15, eta_bins, 40, -3.2, 3.2);

    TH1D *h_LoSel_recoTightPt           = new TH1D("h_LoSel_recoTightPt", "h_LoSel_recoTightPt", 13,  pt_bins);
    TH1D *h_LoSel_recoTightEta          = new TH1D("h_LoSel_recoTightEta", "h_LoSel_recoTightEta", 24, -2.4, 2.4);
    TH1D *h_LoSel_recoTightEta_bin      = new TH1D("h_LoSel_recoTightEta_bin", "h_LoSel_recoTightEta_bin", 15, eta_bins);
    TH1D *h_LoSel_recoTightPhi          = new TH1D("h_LoSel_recoTightPhi", "h_LoSel_recoTightPhi", 40, -3.2, 3.2);
    TH2D *h_LoSel_recoTightEtaPhi       = new TH2D("h_LoSel_recoTightEtaPhi", "h_LoSel_recoTightEtaPhi", 15, eta_bins, 40, -3.2, 3.2);

    TH1D *h_LoSel_recoMediumPt          = new TH1D("h_LoSel_recoMediumPt", "h_LoSel_recoMediumPt", 13,  pt_bins);
    TH1D *h_LoSel_recoMediumEta         = new TH1D("h_LoSel_recoMediumEta", "h_LoSel_recoMediumEta", 24, -2.4, 2.4);
    TH1D *h_LoSel_recoMediumEta_bin     = new TH1D("h_LoSel_recoMediumEta_bin", "h_LoSel_recoMediumEta_bin", 15, eta_bins);
    TH1D *h_LoSel_recoMediumPhi         = new TH1D("h_LoSel_recoMediumPhi", "h_LoSel_recoMediumPhi", 40, -3.2, 3.2);
    TH2D *h_LoSel_recoMediumEtaPhi      = new TH2D("h_LoSel_recoMediumEtaPhi", "h_LoSel_recoMediumEtaPhi", 15, eta_bins, 40, -3.2, 3.2);

    TH1D *h_LoSel_recoLoosePt           = new TH1D("h_LoSel_recoLoosePt", "h_LoSel_recoLoosePt", 13,  pt_bins);
    TH1D *h_LoSel_recoLooseEta          = new TH1D("h_LoSel_recoLooseEta", "h_LoSel_recoLooseEta", 24, -2.4, 2.4);
    TH1D *h_LoSel_recoLooseEta_bin      = new TH1D("h_LoSel_recoLooseEta_bin", "h_LoSel_recoLooseEta_bin", 15, eta_bins);
    TH1D *h_LoSel_recoLoosePhi          = new TH1D("h_LoSel_recoLoosePhi", "h_LoSel_recoLoosePhi", 40, -3.2, 3.2);
    TH2D *h_LoSel_recoLooseEtaPhi       = new TH2D("h_LoSel_recoLooseEtaPhi", "h_LoSel_recoLooseEtaPhi", 15, eta_bins, 40, -3.2, 3.2);

    TH1D *h_TiSel_hltPt                 = new TH1D("h_TiSel_hltPt", "h_TiSel_hltPt", 13,  pt_bins);
    TH1D *h_TiSel_hltEta                = new TH1D("h_TiSel_hltEta", "h_TiSel_hltEta", 24, -2.4, 2.4);
    TH1D *h_TiSel_hltEta_bin            = new TH1D("h_TiSel_hltEta_bin", "h_TiSel_hltEta_bin", 15, eta_bins);
    TH1D *h_TiSel_hltPhi                = new TH1D("h_TiSel_hltPhi", "h_TiSel_hltPhi", 40, -3.2, 3.2);
    TH2D *h_TiSel_hltEtaPhi             = new TH2D("h_TiSel_hltEtaPhi", "h_TiSel_hltEtaPhi", 15, eta_bins, 40, -3.2, 3.2);

    TH1D *h_TiSel_recoPt                = new TH1D("h_TiSel_recoPt", "h_TiSel_recoPt", 13,  pt_bins);
    TH1D *h_TiSel_recoEta               = new TH1D("h_TiSel_recoEta", "h_TiSel_recoEta", 24, -2.4, 2.4);
    TH1D *h_TiSel_recoEta_bin           = new TH1D("h_TiSel_recoEta_bin", "h_TiSel_recoEta_bin", 15, eta_bins);
    TH1D *h_TiSel_recoPhi               = new TH1D("h_TiSel_recoPhi", "h_TiSel_recoPhi", 40, -3.2, 3.2);
    TH2D *h_TiSel_recoEtaPhi            = new TH2D("h_TiSel_recoEtaPhi", "h_TiSel_recoEtaPhi", 15, eta_bins, 40, -3.2, 3.2);

    TH1D *h_TiSel_recoTightPt           = new TH1D("h_TiSel_recoTightPt", "h_TiSel_recoTightPt", 13,  pt_bins);
    TH1D *h_TiSel_recoTightEta          = new TH1D("h_TiSel_recoTightEta", "h_TiSel_recoTightEta", 24, -2.4, 2.4);
    TH1D *h_TiSel_recoTightEta_bin      = new TH1D("h_TiSel_recoTightEta_bin", "h_TiSel_recoTightEta_bin", 15, eta_bins);
    TH1D *h_TiSel_recoTightPhi          = new TH1D("h_TiSel_recoTightPhi", "h_TiSel_recoTightPhi", 40, -3.2, 3.2);
    TH2D *h_TiSel_recoTightEtaPhi       = new TH2D("h_TiSel_recoTightEtaPhi", "h_TiSel_recoTightEtaPhi", 15, eta_bins, 40, -3.2, 3.2);

    TH1D *h_TiSel_recoMediumPt          = new TH1D("h_TiSel_recoMediumPt", "h_TiSel_recoMediumPt", 13,  pt_bins);
    TH1D *h_TiSel_recoMediumEta         = new TH1D("h_TiSel_recoMediumEta", "h_TiSel_recoMediumEta", 24, -2.4, 2.4);
    TH1D *h_TiSel_recoMediumEta_bin     = new TH1D("h_TiSel_recoMediumEta_bin", "h_TiSel_recoMediumEta_bin", 15, eta_bins);
    TH1D *h_TiSel_recoMediumPhi         = new TH1D("h_TiSel_recoMediumPhi", "h_TiSel_recoMediumPhi", 40, -3.2, 3.2);
    TH2D *h_TiSel_recoMediumEtaPhi      = new TH2D("h_TiSel_recoMediumEtaPhi", "h_TiSel_recoMediumEtaPhi", 15, eta_bins, 40, -3.2, 3.2);

    TH1D *h_TiSel_recoLoosePt           = new TH1D("h_TiSel_recoLoosePt", "h_TiSel_recoLoosePt", 13,  pt_bins);
    TH1D *h_TiSel_recoLooseEta          = new TH1D("h_TiSel_recoLooseEta", "h_TiSel_recoLooseEta", 24, -2.4, 2.4);
    TH1D *h_TiSel_recoLooseEta_bin      = new TH1D("h_TiSel_recoLooseEta_bin", "h_TiSel_recoLooseEta_bin", 15, eta_bins);
    TH1D *h_TiSel_recoLoosePhi          = new TH1D("h_TiSel_recoLoosePhi", "h_TiSel_recoLoosePhi", 40, -3.2, 3.2);
    TH2D *h_TiSel_recoLooseEtaPhi       = new TH2D("h_TiSel_recoLooseEtaPhi", "h_TiSel_recoLooseEtaPhi", 15, eta_bins, 40, -3.2, 3.2);

  // -- Input & Output files -- //
    TString outputDir = "outputs/Purity_OffMatching_v2/v"+theDate;
    if (gSystem->mkdir(outputDir,kTRUE) != -1)
      gSystem->mkdir(outputDir,kTRUE);

    TString outputPath = outputDir+"/Purity_OffMatching_v2_"+Period+"_"+Menu+"_"+hltname+"_"+theDate+".root";
    TFile* outfile = new TFile(outputPath,"RECREATE");

    const Int_t nRuns = 8;
    TString Runs[nRuns] = {"Run2017Bv1", "Run2017Bv2", "Run2017Cv1", "Run2017Cv2", "Run2017Cv3", "Run2017Dv1", "Run2017Ev1", "Run2017Fv1"};

    const Int_t nPeriods = 5 +1;
    TString  Periods[nPeriods] =   {"Period2", "Period3", "Period4", "Period5", "Period6", "END"};
    Double_t RunRanges[nPeriods] = { 297050,    297557,    299368,    300262,    305388,    999999};
    /*vector< vector<TString> > vec_Menu;  // vec_Menu[iPeriod][iMenu]
    vector< TString > vec_Period2;
      vec_Period2.push_back("v1p1");
        vec_Menu.push_back(vec_Period2);

    vector< TString > vec_Period3;
      vec_Period3.push_back("v1p2");
        vec_Menu.push_back(vec_Period3);

    vector< TString > vec_Period4;
      vec_Period4.push_back("v2p0");
      vec_Period4.push_back("v2p1");
        vec_Menu.push_back(vec_Period4);

    vector< TString > vec_Period5;
      vec_Period5.push_back("v2p1");
      vec_Period5.push_back("v2p2");
      vec_Period5.push_back("v3p0");
      vec_Period5.push_back("v3p1");
      vec_Period5.push_back("v3p2");
        vec_Menu.push_back(vec_Period5);

    vector< TString > vec_Period6;
      vec_Period6.push_back("v4p0");
        vec_Menu.push_back(vec_Period6);*/


    Int_t iPeriod = -1;
    for(Int_t iP=0; iP<nPeriods; ++iP) {
      if( Period == Periods[iP] )
        iPeriod = iP;
    }
    if(iPeriod<0 || iPeriod>=nPeriods-1) {
      cout << "WARNING : Wrong period" << endl;
      return;
    }

    TChain* inputchain = new TChain("muonNtuples/muonTree");
    for(Int_t iRun=0; iRun<nRuns; ++iRun) {
      TString NtuplePath = "/u/user/msoh/SE_UserHome/RateStudy2017_v3/v20171117/SingleMuon/crab_RateStudy2017_v3_SingleMuon_"+Runs[iRun]+"_"+Menu+"_20171117";
      if( gSystem->cd(NtuplePath) ) {
        cout << "Adding...." << NtuplePath << endl;
        inputchain -> Add(NtuplePath+"/muonNtuple_*.root");
      }
    }

    MuonEvent* ev      = new MuonEvent();
    TBranch*  evBranch = inputchain->GetBranch("event");
    inputchain -> SetBranchAddress("event",&ev);

    cout << "\n====================================================" << endl;
    cout << "====================================================" << endl;
    cout << "||  Period       : " << Period << "( " << RunRanges[iPeriod] << " - " << RunRanges[iPeriod+1]-1 << " )" << endl;
    cout << "||  Menus        : " << Menu << endl;
    cout << "||  HLT name     : " << hltname << endl;
    cout << "||  OnlinePtCut  : " << OnlinePtCut << endl;
    cout << "||  OfflinePtCut : " << OfflinePtCut << endl;
    cout << "====================================================" << endl;
    cout << "====================================================" << endl;

  // -- Events loop -- //
  int nentries = inputchain->GetEntries();    //GetEntriesFast();
  cout << "Number of entries = " << nentries << endl;

  int nEvents = 0;
  int nTagMuons = 0;
  int nLoSelProbes = 0;
  int nLoSelRECOMuons = 0;
  int nLoSelRECOTightMuons = 0;
  int nLoSelRECOMediumMuons = 0;
  int nLoSelRECOLooseMuons = 0;
  int nTiSelProbes = 0;
  int nTiSelRECOMuons = 0;
  int nTiSelRECOTightMuons = 0;
  int nTiSelRECOMediumMuons = 0;
  int nTiSelRECOLooseMuons = 0;

  int nTagsPerEvent = 0;
  int nLoSelProbesPerTag = 0;
  int nTiSelProbesPerTag = 0;

  bool isRecoMatched, isTightMatched, isMediumMatched, isLooseMatched;

  //nentries = 20000;
  for (Int_t eventNo=0; eventNo < nentries; eventNo++) {
    if (!verbose) loadBar(eventNo+1, nentries, 100, 100);
    Int_t IgetEvent   = inputchain   -> GetEvent(eventNo);

    if( !( (ev->runNumber >= RunRanges[iPeriod]) && (ev->runNumber < RunRanges[iPeriod+1]) ) ) continue;

    nEvents +=1;
    nTagsPerEvent = 0;

    unsigned int nmuons = ev->muons.size();
    if (verbose) cout<< "\n\t# Offline muons : " << nmuons << endl;
    if (nmuons < 1) continue;

    if ( !( ev-> hltTag.find(hltname) ) ) continue;

    for (Int_t imu = 0; imu < nmuons; imu++) {
      // Tag selection
      if ( !selectTagMuon(ev -> muons.at(imu), OfflinePtCut) )                continue;
      if ( !matchMuon(ev -> muons.at(imu), ev -> hltTag.objects, filter) )    continue;
      nLoSelProbesPerTag = 0;
      nTiSelProbesPerTag = 0;
      nTagMuons +=1;
      nTagsPerEvent +=1;
      if (verbose) cout << "\t\t" << nTagsPerEvent << "th Tag in this event" << endl;

      for (std::vector<HLTObjCand>::const_iterator it_mu = ev->hlt.objects.begin(); it_mu != ev->hlt.objects.end(); ++it_mu) {
        if(it_mu->filterTag.find(filter)!=std::string::npos) {
          if (verbose) cout << "\t\t\t" << hltname << " : " <<it_mu->filterTag<<endl;

          // Probe selection
          double minDR = 0.1;
          double theDR = 999;
          int jmu = 0;
          //if (verbose) cout << "\t\t\tselectProbeMuonLoose = " << selectProbeMuonLoose(ev -> muons.at(imu), *it_mu, OnlinePtCut, h_mass_TnP, h_mass_TnP_Loose) << endl;
          if ( selectProbeMuonLoose(ev -> muons.at(imu), *it_mu, OnlinePtCut, h_mass_TnP, h_mass_TnP_Loose) ) {
            isRecoMatched = false;
            isTightMatched = false;
            isMediumMatched = false;
            isLooseMatched = false;

            // HLT variables
            nLoSelProbes +=1;
            nLoSelProbesPerTag +=1;
            h_LoSel_hltPt             -> Fill(it_mu->pt);
            h_LoSel_hltEta            -> Fill(it_mu->eta);
            h_LoSel_hltEta_bin        -> Fill(it_mu->eta);
            h_LoSel_hltPhi            -> Fill(it_mu->phi);
            h_LoSel_hltEtaPhi         -> Fill(it_mu->eta, it_mu->phi);

            // jmu : RECO muon
            minDR = 0.1;
            theDR = 999;
            for (jmu = 0; jmu < nmuons; ++jmu) {
              if ( isSameMuon(ev->muons.at(imu), ev->muons.at(jmu)) )  continue;

              theDR = deltaR(it_mu->eta,it_mu->phi, ev->muons.at(jmu).eta,ev->muons.at(jmu).phi);
              if (theDR < minDR) {
                minDR = theDR;
                isRecoMatched = false;
                isTightMatched = false;
                isMediumMatched = false;
                isLooseMatched = false;
                if (                                     (ev->muons.at(jmu).pt > OnlinePtCut) )    isRecoMatched   = true;
                if ( (ev->muons.at(jmu).isTight  ==1) && (ev->muons.at(jmu).pt > OnlinePtCut) )    isTightMatched  = true;
                if ( (ev->muons.at(jmu).isMedium ==1) && (ev->muons.at(jmu).pt > OnlinePtCut) )    isMediumMatched = true;
                if ( (ev->muons.at(jmu).isLoose  ==1) && (ev->muons.at(jmu).pt > OnlinePtCut) )    isLooseMatched  = true;
              }
            }

            if (isRecoMatched) {
              nLoSelRECOMuons +=1;
              h_LoSel_recoPt             -> Fill(it_mu->pt);
              h_LoSel_recoEta            -> Fill(it_mu->eta);
              h_LoSel_recoEta_bin        -> Fill(it_mu->eta);
              h_LoSel_recoPhi            -> Fill(it_mu->phi);
              h_LoSel_recoEtaPhi         -> Fill(it_mu->eta, it_mu->phi);
            }

            if (isTightMatched) {
              nLoSelRECOTightMuons +=1;
              h_LoSel_recoTightPt             -> Fill(it_mu->pt);
              h_LoSel_recoTightEta            -> Fill(it_mu->eta);
              h_LoSel_recoTightEta_bin        -> Fill(it_mu->eta);
              h_LoSel_recoTightPhi            -> Fill(it_mu->phi);
              h_LoSel_recoTightEtaPhi         -> Fill(it_mu->eta, it_mu->phi);
            }

            if (isMediumMatched) {
              nLoSelRECOMediumMuons +=1;
              h_LoSel_recoMediumPt             -> Fill(it_mu->pt);
              h_LoSel_recoMediumEta            -> Fill(it_mu->eta);
              h_LoSel_recoMediumEta_bin        -> Fill(it_mu->eta);
              h_LoSel_recoMediumPhi            -> Fill(it_mu->phi);
              h_LoSel_recoMediumEtaPhi         -> Fill(it_mu->eta, it_mu->phi);
            }

            if (isLooseMatched) {
              nLoSelRECOLooseMuons +=1;
              h_LoSel_recoLoosePt             -> Fill(it_mu->pt);
              h_LoSel_recoLooseEta            -> Fill(it_mu->eta);
              h_LoSel_recoLooseEta_bin        -> Fill(it_mu->eta);
              h_LoSel_recoLoosePhi            -> Fill(it_mu->phi);
              h_LoSel_recoLooseEtaPhi         -> Fill(it_mu->eta, it_mu->phi);
            }

          } // selectProbeMuonLoose

          //if (verbose) cout << "\t\t\tselectProbeMuonTight = " << selectProbeMuonTight(ev -> muons.at(imu), *it_mu, OnlinePtCut, h_mass_TnP_Tight) << endl;
          if ( selectProbeMuonTight(ev -> muons.at(imu), *it_mu, OnlinePtCut, h_mass_TnP_Tight) ) {
            isRecoMatched = false;
            isTightMatched = false;
            isMediumMatched = false;
            isLooseMatched = false;

            // HLT variables
            nTiSelProbes +=1;
            nTiSelProbesPerTag +=1;
            h_TiSel_hltPt             -> Fill(it_mu->pt);
            h_TiSel_hltEta            -> Fill(it_mu->eta);
            h_TiSel_hltEta_bin        -> Fill(it_mu->eta);
            h_TiSel_hltPhi            -> Fill(it_mu->phi);
            h_TiSel_hltEtaPhi         -> Fill(it_mu->eta, it_mu->phi);

            // jmu : RECO muon
            minDR = 0.1;
            theDR = 999;
            for (jmu = 0; jmu < nmuons; ++jmu) {
              if ( isSameMuon(ev->muons.at(imu), ev->muons.at(jmu)) )  continue;

              theDR = deltaR(it_mu->eta,it_mu->phi, ev->muons.at(jmu).eta,ev->muons.at(jmu).phi);
              if (theDR < minDR) {
                minDR = theDR;
                isRecoMatched = false;
                isTightMatched = false;
                isMediumMatched = false;
                isLooseMatched = false;
                if (                                     (ev->muons.at(jmu).pt > OnlinePtCut) )    isRecoMatched   = true;
                if ( (ev->muons.at(jmu).isTight  ==1) && (ev->muons.at(jmu).pt > OnlinePtCut) )    isTightMatched  = true;
                if ( (ev->muons.at(jmu).isMedium ==1) && (ev->muons.at(jmu).pt > OnlinePtCut) )    isMediumMatched = true;
                if ( (ev->muons.at(jmu).isLoose  ==1) && (ev->muons.at(jmu).pt > OnlinePtCut) )    isLooseMatched  = true;
              }
            }

            if (isRecoMatched) {
              nTiSelRECOMuons +=1;
              h_TiSel_recoPt             -> Fill(it_mu->pt);
              h_TiSel_recoEta            -> Fill(it_mu->eta);
              h_TiSel_recoEta_bin        -> Fill(it_mu->eta);
              h_TiSel_recoPhi            -> Fill(it_mu->phi);
              h_TiSel_recoEtaPhi         -> Fill(it_mu->eta, it_mu->phi);
            }

            if (isTightMatched) {
              nTiSelRECOTightMuons +=1;
              h_TiSel_recoTightPt             -> Fill(it_mu->pt);
              h_TiSel_recoTightEta            -> Fill(it_mu->eta);
              h_TiSel_recoTightEta_bin        -> Fill(it_mu->eta);
              h_TiSel_recoTightPhi            -> Fill(it_mu->phi);
              h_TiSel_recoTightEtaPhi         -> Fill(it_mu->eta, it_mu->phi);
            }

            if (isMediumMatched) {
              nTiSelRECOMediumMuons +=1;
              h_TiSel_recoMediumPt             -> Fill(it_mu->pt);
              h_TiSel_recoMediumEta            -> Fill(it_mu->eta);
              h_TiSel_recoMediumEta_bin        -> Fill(it_mu->eta);
              h_TiSel_recoMediumPhi            -> Fill(it_mu->phi);
              h_TiSel_recoMediumEtaPhi         -> Fill(it_mu->eta, it_mu->phi);
            }

            if (isLooseMatched) {
              nTiSelRECOLooseMuons +=1;
              h_TiSel_recoLoosePt             -> Fill(it_mu->pt);
              h_TiSel_recoLooseEta            -> Fill(it_mu->eta);
              h_TiSel_recoLooseEta_bin        -> Fill(it_mu->eta);
              h_TiSel_recoLoosePhi            -> Fill(it_mu->phi);
              h_TiSel_recoLooseEtaPhi         -> Fill(it_mu->eta, it_mu->phi);
            }

          } // selectProbeMuonTight


        } // if(it_mu->filterTag.compare(filter) == 0)
      } // for (std::vector<HLTObjCand>::const_iterator it_mu = ev->hlt.objects.begin(); it_mu != ev->hlt.objects.end(); ++it_mu)
      if (verbose) cout << "\t\tnLoSelProbesPerTag = " << nLoSelProbesPerTag << endl;
      if (verbose) cout << "\t\tnTiSelProbesPerTag = " << nTiSelProbesPerTag << endl;
    } // for (Int_t imu = 0; imu < nmuons; imu++)
  } // for (Int_t eventNo=0; eventNo < nentries; eventNo++)

  cout << "\n====================================================" << endl;
  cout << "====================================================" << endl;
  cout << "\t  nEvents : " << nEvents << endl;
  cout << "\t  nTagMuons : " << nTagMuons << endl;
  cout << endl;
  cout << "\t  nLoSelProbes : " << nLoSelProbes << endl;
  cout << "\t  nLoSelRECOMuons : " << nLoSelRECOMuons << endl;
  cout << "\t  nLoSelRECOLooseMuons : " << nLoSelRECOLooseMuons << endl;
  cout << "\t  nLoSelRECOMediumMuons : " << nLoSelRECOMediumMuons << endl;
  cout << "\t  nLoSelRECOTightMuons : " << nLoSelRECOTightMuons << endl;
  cout << endl;
  cout << "\t  nTiSelProbes : " << nTiSelProbes << endl;
  cout << "\t  nTiSelRECOMuons : " << nTiSelRECOMuons << endl;
  cout << "\t  nTiSelRECOLooseMuons : " << nTiSelRECOLooseMuons << endl;
  cout << "\t  nTiSelRECOMediumMuons : " << nTiSelRECOMediumMuons << endl;
  cout << "\t  nTiSelRECOTightMuons : " << nTiSelRECOTightMuons << endl;
  cout << "====================================================" << endl;
  cout << "====================================================" << endl;

  h_LoSel_hltPt             -> Sumw2();
  h_LoSel_hltEta            -> Sumw2();
  h_LoSel_hltEta_bin        -> Sumw2();
  h_LoSel_hltPhi            -> Sumw2();
  h_LoSel_hltEtaPhi         -> Sumw2();

  h_LoSel_recoPt             -> Sumw2();
  h_LoSel_recoEta            -> Sumw2();
  h_LoSel_recoEta_bin        -> Sumw2();
  h_LoSel_recoPhi            -> Sumw2();
  h_LoSel_recoEtaPhi         -> Sumw2();

  h_LoSel_recoTightPt             -> Sumw2();
  h_LoSel_recoTightEta            -> Sumw2();
  h_LoSel_recoTightEta_bin        -> Sumw2();
  h_LoSel_recoTightPhi            -> Sumw2();
  h_LoSel_recoTightEtaPhi         -> Sumw2();

  h_LoSel_recoMediumPt             -> Sumw2();
  h_LoSel_recoMediumEta            -> Sumw2();
  h_LoSel_recoMediumEta_bin        -> Sumw2();
  h_LoSel_recoMediumPhi            -> Sumw2();
  h_LoSel_recoMediumEtaPhi         -> Sumw2();

  h_LoSel_recoLoosePt             -> Sumw2();
  h_LoSel_recoLooseEta            -> Sumw2();
  h_LoSel_recoLooseEta_bin        -> Sumw2();
  h_LoSel_recoLoosePhi            -> Sumw2();
  h_LoSel_recoLooseEtaPhi         -> Sumw2();

  h_TiSel_hltPt             -> Sumw2();
  h_TiSel_hltEta            -> Sumw2();
  h_TiSel_hltEta_bin        -> Sumw2();
  h_TiSel_hltPhi            -> Sumw2();
  h_TiSel_hltEtaPhi         -> Sumw2();

  h_TiSel_recoPt             -> Sumw2();
  h_TiSel_recoEta            -> Sumw2();
  h_TiSel_recoEta_bin        -> Sumw2();
  h_TiSel_recoPhi            -> Sumw2();
  h_TiSel_recoEtaPhi         -> Sumw2();

  h_TiSel_recoTightPt             -> Sumw2();
  h_TiSel_recoTightEta            -> Sumw2();
  h_TiSel_recoTightEta_bin        -> Sumw2();
  h_TiSel_recoTightPhi            -> Sumw2();
  h_TiSel_recoTightEtaPhi         -> Sumw2();

  h_TiSel_recoMediumPt             -> Sumw2();
  h_TiSel_recoMediumEta            -> Sumw2();
  h_TiSel_recoMediumEta_bin        -> Sumw2();
  h_TiSel_recoMediumPhi            -> Sumw2();
  h_TiSel_recoMediumEtaPhi         -> Sumw2();

  h_TiSel_recoLoosePt             -> Sumw2();
  h_TiSel_recoLooseEta            -> Sumw2();
  h_TiSel_recoLooseEta_bin        -> Sumw2();
  h_TiSel_recoLoosePhi            -> Sumw2();
  h_TiSel_recoLooseEtaPhi         -> Sumw2();

  outfile           -> cd();

  h_mass_TnP                -> Write();
  h_mass_TnP_Loose          -> Write();
  h_mass_TnP_Tight          -> Write();

  h_LoSel_hltPt             -> Write();
  h_LoSel_hltEta            -> Write();
  h_LoSel_hltEta_bin        -> Write();
  h_LoSel_hltPhi            -> Write();
  h_LoSel_hltEtaPhi         -> Write();

  h_LoSel_recoPt             -> Write();
  h_LoSel_recoEta            -> Write();
  h_LoSel_recoEta_bin        -> Write();
  h_LoSel_recoPhi            -> Write();
  h_LoSel_recoEtaPhi         -> Write();

  h_LoSel_recoTightPt             -> Write();
  h_LoSel_recoTightEta            -> Write();
  h_LoSel_recoTightEta_bin        -> Write();
  h_LoSel_recoTightPhi            -> Write();
  h_LoSel_recoTightEtaPhi         -> Write();

  h_LoSel_recoMediumPt             -> Write();
  h_LoSel_recoMediumEta            -> Write();
  h_LoSel_recoMediumEta_bin        -> Write();
  h_LoSel_recoMediumPhi            -> Write();
  h_LoSel_recoMediumEtaPhi         -> Write();

  h_LoSel_recoLoosePt             -> Write();
  h_LoSel_recoLooseEta            -> Write();
  h_LoSel_recoLooseEta_bin        -> Write();
  h_LoSel_recoLoosePhi            -> Write();
  h_LoSel_recoLooseEtaPhi         -> Write();

  h_TiSel_hltPt             -> Write();
  h_TiSel_hltEta            -> Write();
  h_TiSel_hltEta_bin        -> Write();
  h_TiSel_hltPhi            -> Write();
  h_TiSel_hltEtaPhi         -> Write();

  h_TiSel_recoPt             -> Write();
  h_TiSel_recoEta            -> Write();
  h_TiSel_recoEta_bin        -> Write();
  h_TiSel_recoPhi            -> Write();
  h_TiSel_recoEtaPhi         -> Write();

  h_TiSel_recoTightPt             -> Write();
  h_TiSel_recoTightEta            -> Write();
  h_TiSel_recoTightEta_bin        -> Write();
  h_TiSel_recoTightPhi            -> Write();
  h_TiSel_recoTightEtaPhi         -> Write();

  h_TiSel_recoMediumPt             -> Write();
  h_TiSel_recoMediumEta            -> Write();
  h_TiSel_recoMediumEta_bin        -> Write();
  h_TiSel_recoMediumPhi            -> Write();
  h_TiSel_recoMediumEtaPhi         -> Write();

  h_TiSel_recoLoosePt             -> Write();
  h_TiSel_recoLooseEta            -> Write();
  h_TiSel_recoLooseEta_bin        -> Write();
  h_TiSel_recoLoosePhi            -> Write();
  h_TiSel_recoLooseEtaPhi         -> Write();

  //outfile           -> Close();

  TH1D *h_LoSel_purityPt                = (TH1D*) h_LoSel_recoPt->Clone("h_LoSel_purityPt");
  h_LoSel_purityPt->SetTitle("h_LoSel_purityPt");
  h_LoSel_purityPt->Divide(h_LoSel_hltPt);
  TH1D *h_LoSel_purityEta                = (TH1D*) h_LoSel_recoEta->Clone("h_LoSel_purityEta");
  h_LoSel_purityEta->SetTitle("h_LoSel_purityEta");
  h_LoSel_purityEta->Divide(h_LoSel_hltEta);
  TH1D *h_LoSel_purityEta_bin                = (TH1D*) h_LoSel_recoEta_bin->Clone("h_LoSel_purityEta_bin");
  h_LoSel_purityEta_bin->SetTitle("h_LoSel_purityEta_bin");
  h_LoSel_purityEta_bin->Divide(h_LoSel_hltEta_bin);
  TH1D *h_LoSel_purityPhi                = (TH1D*) h_LoSel_recoPhi->Clone("h_LoSel_purityPhi");
  h_LoSel_purityPhi->SetTitle("h_LoSel_purityPhi");
  h_LoSel_purityPhi->Divide(h_LoSel_hltPhi);
  TH2D *h_LoSel_purityEtaPhi                = (TH2D*) h_LoSel_recoEtaPhi->Clone("h_LoSel_purityEtaPhi");
  h_LoSel_purityEtaPhi->SetTitle("h_LoSel_purityEtaPhi");
  h_LoSel_purityEtaPhi->Divide(h_LoSel_hltEtaPhi);

  TH1D *h_LoSel_purityTightPt                = (TH1D*) h_LoSel_recoTightPt->Clone("h_LoSel_purityTightPt");
  h_LoSel_purityTightPt->SetTitle("h_LoSel_purityTightPt");
  h_LoSel_purityTightPt->Divide(h_LoSel_hltPt);
  TH1D *h_LoSel_purityTightEta                = (TH1D*) h_LoSel_recoTightEta->Clone("h_LoSel_purityTightEta");
  h_LoSel_purityTightEta->SetTitle("h_LoSel_purityTightEta");
  h_LoSel_purityTightEta->Divide(h_LoSel_hltEta);
  TH1D *h_LoSel_purityTightEta_bin                = (TH1D*) h_LoSel_recoTightEta_bin->Clone("h_LoSel_purityTightEta_bin");
  h_LoSel_purityTightEta_bin->SetTitle("h_LoSel_purityTightEta_bin");
  h_LoSel_purityTightEta_bin->Divide(h_LoSel_hltEta_bin);
  TH1D *h_LoSel_purityTightPhi                = (TH1D*) h_LoSel_recoTightPhi->Clone("h_LoSel_purityTightPhi");
  h_LoSel_purityTightPhi->SetTitle("h_LoSel_purityTightPhi");
  h_LoSel_purityTightPhi->Divide(h_LoSel_hltPhi);
  TH2D *h_LoSel_purityTightEtaPhi                = (TH2D*) h_LoSel_recoTightEtaPhi->Clone("h_LoSel_purityTightEtaPhi");
  h_LoSel_purityTightEtaPhi->SetTitle("h_LoSel_purityTightEtaPhi");
  h_LoSel_purityTightEtaPhi->Divide(h_LoSel_hltEtaPhi);

  TH1D *h_LoSel_purityMediumPt                = (TH1D*) h_LoSel_recoMediumPt->Clone("h_LoSel_purityMediumPt");
  h_LoSel_purityMediumPt->SetTitle("h_LoSel_purityMediumPt");
  h_LoSel_purityMediumPt->Divide(h_LoSel_hltPt);
  TH1D *h_LoSel_purityMediumEta                = (TH1D*) h_LoSel_recoMediumEta->Clone("h_LoSel_purityMediumEta");
  h_LoSel_purityMediumEta->SetTitle("h_LoSel_purityMediumEta");
  h_LoSel_purityMediumEta->Divide(h_LoSel_hltEta);
  TH1D *h_LoSel_purityMediumEta_bin                = (TH1D*) h_LoSel_recoMediumEta_bin->Clone("h_LoSel_purityMediumEta_bin");
  h_LoSel_purityMediumEta_bin->SetTitle("h_LoSel_purityMediumEta_bin");
  h_LoSel_purityMediumEta_bin->Divide(h_LoSel_hltEta_bin);
  TH1D *h_LoSel_purityMediumPhi                = (TH1D*) h_LoSel_recoMediumPhi->Clone("h_LoSel_purityMediumPhi");
  h_LoSel_purityMediumPhi->SetTitle("h_LoSel_purityMediumPhi");
  h_LoSel_purityMediumPhi->Divide(h_LoSel_hltPhi);
  TH2D *h_LoSel_purityMediumEtaPhi                = (TH2D*) h_LoSel_recoMediumEtaPhi->Clone("h_LoSel_purityMediumEtaPhi");
  h_LoSel_purityMediumEtaPhi->SetTitle("h_LoSel_purityMediumEtaPhi");
  h_LoSel_purityMediumEtaPhi->Divide(h_LoSel_hltEtaPhi);

  TH1D *h_LoSel_purityLoosePt                = (TH1D*) h_LoSel_recoLoosePt->Clone("h_LoSel_purityLoosePt");
  h_LoSel_purityLoosePt->SetTitle("h_LoSel_purityLoosePt");
  h_LoSel_purityLoosePt->Divide(h_LoSel_hltPt);
  TH1D *h_LoSel_purityLooseEta                = (TH1D*) h_LoSel_recoLooseEta->Clone("h_LoSel_purityLooseEta");
  h_LoSel_purityLooseEta->SetTitle("h_LoSel_purityLooseEta");
  h_LoSel_purityLooseEta->Divide(h_LoSel_hltEta);
  TH1D *h_LoSel_purityLooseEta_bin                = (TH1D*) h_LoSel_recoLooseEta_bin->Clone("h_LoSel_purityLooseEta_bin");
  h_LoSel_purityLooseEta_bin->SetTitle("h_LoSel_purityLooseEta_bin");
  h_LoSel_purityLooseEta_bin->Divide(h_LoSel_hltEta_bin);
  TH1D *h_LoSel_purityLoosePhi                = (TH1D*) h_LoSel_recoLoosePhi->Clone("h_LoSel_purityLoosePhi");
  h_LoSel_purityLoosePhi->SetTitle("h_LoSel_purityLoosePhi");
  h_LoSel_purityLoosePhi->Divide(h_LoSel_hltPhi);
  TH2D *h_LoSel_purityLooseEtaPhi                = (TH2D*) h_LoSel_recoLooseEtaPhi->Clone("h_LoSel_purityLooseEtaPhi");
  h_LoSel_purityLooseEtaPhi->SetTitle("h_LoSel_purityLooseEtaPhi");
  h_LoSel_purityLooseEtaPhi->Divide(h_LoSel_hltEtaPhi);

  TH1D *h_TiSel_purityPt                = (TH1D*) h_TiSel_recoPt->Clone("h_TiSel_purityPt");
  h_TiSel_purityPt->SetTitle("h_TiSel_purityPt");
  h_TiSel_purityPt->Divide(h_TiSel_hltPt);
  TH1D *h_TiSel_purityEta                = (TH1D*) h_TiSel_recoEta->Clone("h_TiSel_purityEta");
  h_TiSel_purityEta->SetTitle("h_TiSel_purityEta");
  h_TiSel_purityEta->Divide(h_TiSel_hltEta);
  TH1D *h_TiSel_purityEta_bin                = (TH1D*) h_TiSel_recoEta_bin->Clone("h_TiSel_purityEta_bin");
  h_TiSel_purityEta_bin->SetTitle("h_TiSel_purityEta_bin");
  h_TiSel_purityEta_bin->Divide(h_TiSel_hltEta_bin);
  TH1D *h_TiSel_purityPhi                = (TH1D*) h_TiSel_recoPhi->Clone("h_TiSel_purityPhi");
  h_TiSel_purityPhi->SetTitle("h_TiSel_purityPhi");
  h_TiSel_purityPhi->Divide(h_TiSel_hltPhi);
  TH2D *h_TiSel_purityEtaPhi                = (TH2D*) h_TiSel_recoEtaPhi->Clone("h_TiSel_purityEtaPhi");
  h_TiSel_purityEtaPhi->SetTitle("h_TiSel_purityEtaPhi");
  h_TiSel_purityEtaPhi->Divide(h_TiSel_hltEtaPhi);

  TH1D *h_TiSel_purityTightPt                = (TH1D*) h_TiSel_recoTightPt->Clone("h_TiSel_purityTightPt");
  h_TiSel_purityTightPt->SetTitle("h_TiSel_purityTightPt");
  h_TiSel_purityTightPt->Divide(h_TiSel_hltPt);
  TH1D *h_TiSel_purityTightEta                = (TH1D*) h_TiSel_recoTightEta->Clone("h_TiSel_purityTightEta");
  h_TiSel_purityTightEta->SetTitle("h_TiSel_purityTightEta");
  h_TiSel_purityTightEta->Divide(h_TiSel_hltEta);
  TH1D *h_TiSel_purityTightEta_bin                = (TH1D*) h_TiSel_recoTightEta_bin->Clone("h_TiSel_purityTightEta_bin");
  h_TiSel_purityTightEta_bin->SetTitle("h_TiSel_purityTightEta_bin");
  h_TiSel_purityTightEta_bin->Divide(h_TiSel_hltEta_bin);
  TH1D *h_TiSel_purityTightPhi                = (TH1D*) h_TiSel_recoTightPhi->Clone("h_TiSel_purityTightPhi");
  h_TiSel_purityTightPhi->SetTitle("h_TiSel_purityTightPhi");
  h_TiSel_purityTightPhi->Divide(h_TiSel_hltPhi);
  TH2D *h_TiSel_purityTightEtaPhi                = (TH2D*) h_TiSel_recoTightEtaPhi->Clone("h_TiSel_purityTightEtaPhi");
  h_TiSel_purityTightEtaPhi->SetTitle("h_TiSel_purityTightEtaPhi");
  h_TiSel_purityTightEtaPhi->Divide(h_TiSel_hltEtaPhi);

  TH1D *h_TiSel_purityMediumPt                = (TH1D*) h_TiSel_recoMediumPt->Clone("h_TiSel_purityMediumPt");
  h_TiSel_purityMediumPt->SetTitle("h_TiSel_purityMediumPt");
  h_TiSel_purityMediumPt->Divide(h_TiSel_hltPt);
  TH1D *h_TiSel_purityMediumEta                = (TH1D*) h_TiSel_recoMediumEta->Clone("h_TiSel_purityMediumEta");
  h_TiSel_purityMediumEta->SetTitle("h_TiSel_purityMediumEta");
  h_TiSel_purityMediumEta->Divide(h_TiSel_hltEta);
  TH1D *h_TiSel_purityMediumEta_bin                = (TH1D*) h_TiSel_recoMediumEta_bin->Clone("h_TiSel_purityMediumEta_bin");
  h_TiSel_purityMediumEta_bin->SetTitle("h_TiSel_purityMediumEta_bin");
  h_TiSel_purityMediumEta_bin->Divide(h_TiSel_hltEta_bin);
  TH1D *h_TiSel_purityMediumPhi                = (TH1D*) h_TiSel_recoMediumPhi->Clone("h_TiSel_purityMediumPhi");
  h_TiSel_purityMediumPhi->SetTitle("h_TiSel_purityMediumPhi");
  h_TiSel_purityMediumPhi->Divide(h_TiSel_hltPhi);
  TH2D *h_TiSel_purityMediumEtaPhi                = (TH2D*) h_TiSel_recoMediumEtaPhi->Clone("h_TiSel_purityMediumEtaPhi");
  h_TiSel_purityMediumEtaPhi->SetTitle("h_TiSel_purityMediumEtaPhi");
  h_TiSel_purityMediumEtaPhi->Divide(h_TiSel_hltEtaPhi);

  TH1D *h_TiSel_purityLoosePt                = (TH1D*) h_TiSel_recoLoosePt->Clone("h_TiSel_purityLoosePt");
  h_TiSel_purityLoosePt->SetTitle("h_TiSel_purityLoosePt");
  h_TiSel_purityLoosePt->Divide(h_TiSel_hltPt);
  TH1D *h_TiSel_purityLooseEta                = (TH1D*) h_TiSel_recoLooseEta->Clone("h_TiSel_purityLooseEta");
  h_TiSel_purityLooseEta->SetTitle("h_TiSel_purityLooseEta");
  h_TiSel_purityLooseEta->Divide(h_TiSel_hltEta);
  TH1D *h_TiSel_purityLooseEta_bin                = (TH1D*) h_TiSel_recoLooseEta_bin->Clone("h_TiSel_purityLooseEta_bin");
  h_TiSel_purityLooseEta_bin->SetTitle("h_TiSel_purityLooseEta_bin");
  h_TiSel_purityLooseEta_bin->Divide(h_TiSel_hltEta_bin);
  TH1D *h_TiSel_purityLoosePhi                = (TH1D*) h_TiSel_recoLoosePhi->Clone("h_TiSel_purityLoosePhi");
  h_TiSel_purityLoosePhi->SetTitle("h_TiSel_purityLoosePhi");
  h_TiSel_purityLoosePhi->Divide(h_TiSel_hltPhi);
  TH2D *h_TiSel_purityLooseEtaPhi                = (TH2D*) h_TiSel_recoLooseEtaPhi->Clone("h_TiSel_purityLooseEtaPhi");
  h_TiSel_purityLooseEtaPhi->SetTitle("h_TiSel_purityLooseEtaPhi");
  h_TiSel_purityLooseEtaPhi->Divide(h_TiSel_hltEtaPhi);

  outfile           -> cd();

  h_LoSel_purityPt             -> Write();
  h_LoSel_purityEta            -> Write();
  h_LoSel_purityEta_bin        -> Write();
  h_LoSel_purityPhi            -> Write();
  h_LoSel_purityEtaPhi         -> Write();

  h_LoSel_purityTightPt             -> Write();
  h_LoSel_purityTightEta            -> Write();
  h_LoSel_purityTightEta_bin        -> Write();
  h_LoSel_purityTightPhi            -> Write();
  h_LoSel_purityTightEtaPhi         -> Write();

  h_LoSel_purityMediumPt             -> Write();
  h_LoSel_purityMediumEta            -> Write();
  h_LoSel_purityMediumEta_bin        -> Write();
  h_LoSel_purityMediumPhi            -> Write();
  h_LoSel_purityMediumEtaPhi         -> Write();

  h_LoSel_purityLoosePt             -> Write();
  h_LoSel_purityLooseEta            -> Write();
  h_LoSel_purityLooseEta_bin        -> Write();
  h_LoSel_purityLoosePhi            -> Write();
  h_LoSel_purityLooseEtaPhi         -> Write();

  h_TiSel_purityPt             -> Write();
  h_TiSel_purityEta            -> Write();
  h_TiSel_purityEta_bin        -> Write();
  h_TiSel_purityPhi            -> Write();
  h_TiSel_purityEtaPhi         -> Write();

  h_TiSel_purityTightPt             -> Write();
  h_TiSel_purityTightEta            -> Write();
  h_TiSel_purityTightEta_bin        -> Write();
  h_TiSel_purityTightPhi            -> Write();
  h_TiSel_purityTightEtaPhi         -> Write();

  h_TiSel_purityMediumPt             -> Write();
  h_TiSel_purityMediumEta            -> Write();
  h_TiSel_purityMediumEta_bin        -> Write();
  h_TiSel_purityMediumPhi            -> Write();
  h_TiSel_purityMediumEtaPhi         -> Write();

  h_TiSel_purityLoosePt             -> Write();
  h_TiSel_purityLooseEta            -> Write();
  h_TiSel_purityLooseEta_bin        -> Write();
  h_TiSel_purityLoosePhi            -> Write();
  h_TiSel_purityLooseEtaPhi         -> Write();

  outfile           -> Close();

  cout << "\t\t\tFinish!!\n" << std::endl;

  Double_t TotalRunTime = totaltime.CpuTime();
  cout << "Total RunTime: " << TotalRunTime << " seconds" << endl;

  TTimeStamp ts_end;
  cout << "[End Time(local time): " << ts_end.AsString("l") << "]" << endl;

  return;



} // readNtuples_Purity_v2


/*======================================================================*/

bool matchMuon(MuonCand mu, std::vector<HLTObjCand> toc, std::string tagFilterName){

  bool match = false;
  int ntoc = toc.size();

  float minDR = 0.1;
  //if (tagFilterName.find("L1fL1") != std::string::npos) minDR = 0.3;
  float theDR = 100;
  for ( std::vector<HLTObjCand>::const_iterator it = toc.begin(); it != toc.end(); ++it ) {
    if ( it->filterTag.find(tagFilterName)!=std::string::npos) {

      theDR = deltaR(it -> eta, it -> phi, mu.eta, mu.phi);
      if (theDR < minDR){
        minDR = theDR;
        match = true;
      }
    }
  }

  return match;
}


bool selectTagMuon(MuonCand mu, double OfflinePtCut){

  if (!( mu.pt         > OfflinePtCut  )) return false;
  if (!( fabs(mu.eta)  < 2.4 ))           return false;
  if (!( mu.isTight    == 1  ))           return false;

  //add isolation cut
  float offlineiso04 = mu.chargedDep_dR04 + std::max(0.,
                       mu.photonDep_dR04 + mu.neutralDep_dR04 - 0.5*mu.puPt_dR04);
  offlineiso04       = offlineiso04 / mu.pt;
  if (!(offlineiso04   < offlineIsoCut)) return false;

  return true;
}


bool selectProbeMuonLoose(MuonCand TagMu, HLTObjCand ProbeMu, double OnlinePtCut, TH1D *h_mass_TnP_All, TH1D *h_mass_TnP ){

  if ( !(ProbeMu.pt > OnlinePtCut) ) {
    cout << "WARNING : pT of probe < OnlinePtCut! --> " << ProbeMu.pt << endl;
    return false;
  }
  if (!( fabs(ProbeMu.eta)  < 2.4 ))           return false;

  double dRtag = 999;
  dRtag = deltaR(TagMu.eta, TagMu.phi, ProbeMu.eta, ProbeMu.phi);
  if (dRtag < 0.1) {
    return false;
  }

  TLorentzVector mu1, mu2;
  mu1.SetPtEtaPhiM (TagMu.pt   , TagMu.eta   , TagMu.phi   , muonmass);
  mu2.SetPtEtaPhiM (ProbeMu.pt , ProbeMu.eta , ProbeMu.phi , muonmass);
  double mumumass = (mu1 + mu2).M();

  h_mass_TnP_All -> Fill(mumumass);
  if ( !(mumumass > 60.0 && mumumass < 120.0 ) ) return false;
  h_mass_TnP -> Fill(mumumass);

  return true;
}

bool selectProbeMuonTight(MuonCand TagMu, HLTObjCand ProbeMu, double OnlinePtCut, TH1D *h_mass_TnP ){

  if ( !(ProbeMu.pt > OnlinePtCut) ) {
    cout << "WARNING : pT of probe < OnlinePtCut! --> " << ProbeMu.pt << endl;
    return false;
  }
  if (!( fabs(ProbeMu.eta)  < 2.4 ))           return false;

  double dRtag = 999;
  dRtag = deltaR(TagMu.eta, TagMu.phi, ProbeMu.eta, ProbeMu.phi);
  if (dRtag < 0.1) {
    return false;
  }

  TLorentzVector mu1, mu2;
  mu1.SetPtEtaPhiM (TagMu.pt   , TagMu.eta   , TagMu.phi   , muonmass);
  mu2.SetPtEtaPhiM (ProbeMu.pt , ProbeMu.eta , ProbeMu.phi , muonmass);
  double mumumass = (mu1 + mu2).M();

  if ( !(mumumass > 81.0 && mumumass < 101.0 ) ) return false;
  h_mass_TnP -> Fill(mumumass);

  return true;
}


bool isSameMuon( MuonCand mu1, MuonCand mu2 ) {
  bool isSame = false;
  if ( (mu1.pt  == mu2.pt) &&
       (mu1.eta == mu2.eta) &&
       (mu1.phi == mu2.phi)
      ) {
    isSame = true;
  }
  return isSame;
}


static inline void loadBar(int x, int n, int r, int w) {
  // Only update r times.
  if( x == n )
    cout << endl;

  if ( x % (n/r +1) != 0 ) return;


  // Calculuate the ratio of complete-to-incomplete.
  float ratio = x/(float)n;
  int   c     = ratio * w;

  // Show the percentage complete.
  printf("%3d%% [", (int)(ratio*100) );

  // Show the load bar.
  for (int x=0; x<c; x++) cout << "=";

  for (int x=c; x<w; x++) cout << " ";

  // ANSI Control codes to go back to the
  // previous line and clear it.
  cout << "]\r" << flush;
}
