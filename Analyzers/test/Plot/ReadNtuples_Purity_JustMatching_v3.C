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
//#include "MuonTree.h"
#include "TLorentzVector.h"

#include <ctime>
#include <TStopwatch.h>
#include <TTimeStamp.h>

using namespace std;

double muonmass = 0.10565837;

double offlineIsoCut = 0.15;

double minDR = 0.1;

bool MatchTightIdIso(HLTObjCand , MuonCand , double );
bool MatchTightIdNoIso(HLTObjCand , MuonCand , double );
bool MatchLooseIdIso(HLTObjCand , MuonCand , double );
bool MatchLooseIdNoIso(HLTObjCand , MuonCand , double );
bool MatchNoIdIso(HLTObjCand , MuonCand , double );
bool MatchNoIdNoIso(HLTObjCand , MuonCand , double );

bool matchMuon              (MuonCand, std::vector<HLTObjCand>, std::string);
bool selectTagMuon          (MuonCand, double );
bool selectProbeMuonLoose   (MuonCand, HLTObjCand, double, TH1D*, TH1D* );
bool selectProbeMuonTight   (MuonCand, HLTObjCand, double, TH1D* );
bool isSameMuon             (MuonCand, MuonCand);
static inline void loadBar(int x, int n, int r, int w);

/*======================================================================*/

void ReadNtuples_Purity_JustMatching_v3(TString Period, TString Menu, std::string hltname, bool verbose) {

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
  const Int_t nPtBin = 13;
  const double PtBin[14]  = { 20, 24, 27, 30, 35, 40, 45, 50, 60, 70, 90, 150, 250, 500 };
  const Int_t nEtaBin = 15;
  const double EtaBin[16] = {-2.4, -2.1, -1.6, -1.2, -1.04, -0.9, -0.3, -0.2,  0.2, 0.3, 0.9, 1.04, 1.2, 1.6, 2.1, 2.4};

  TH1D* h_L3_Pt                  = new TH1D( "h_L3_Pt" , "" , nPtBin,  PtBin );
  TH1D* h_L3_Pt_LowVTX           = new TH1D( "h_L3_Pt_LowVTX" , "" , nPtBin,  PtBin );
  TH1D* h_L3_Pt_HighVTX          = new TH1D( "h_L3_Pt_HighVTX" , "" , nPtBin,  PtBin );
  TH1D* h_L3_Eta                 = new TH1D( "h_L3_Eta" , "" , nEtaBin,  EtaBin );
  TH1D* h_L3_Eta_LowVTX          = new TH1D( "h_L3_Eta_LowVTX" , "" , nEtaBin,  EtaBin );
  TH1D* h_L3_Eta_HighVTX         = new TH1D( "h_L3_Eta_HighVTX" , "" , nEtaBin,  EtaBin );

  TH1D* h_TightIdIso_Pt          = new TH1D( "h_TightIdIso_Pt" , "" , nPtBin,  PtBin );
  TH1D* h_TightIdIso_Pt_LowVTX   = new TH1D( "h_TightIdIso_Pt_LowVTX" , "" , nPtBin,  PtBin );
  TH1D* h_TightIdIso_Pt_HighVTX  = new TH1D( "h_TightIdIso_Pt_HighVTX" , "" , nPtBin,  PtBin );
  TH1D* h_TightIdIso_Eta         = new TH1D( "h_TightIdIso_Eta", "", nEtaBin, EtaBin );
  TH1D* h_TightIdIso_Eta_LowVTX  = new TH1D( "h_TightIdIso_Eta_LowVTX", "", nEtaBin, EtaBin );
  TH1D* h_TightIdIso_Eta_HighVTX = new TH1D( "h_TightIdIso_Eta_HighVTX", "", nEtaBin, EtaBin );

  TH1D* h_TightIdNoIso_Pt          = new TH1D( "h_TightIdNoIso_Pt" , "" , nPtBin,  PtBin );
  TH1D* h_TightIdNoIso_Pt_LowVTX   = new TH1D( "h_TightIdNoIso_Pt_LowVTX" , "" , nPtBin,  PtBin );
  TH1D* h_TightIdNoIso_Pt_HighVTX  = new TH1D( "h_TightIdNoIso_Pt_HighVTX" , "" , nPtBin,  PtBin );
  TH1D* h_TightIdNoIso_Eta         = new TH1D( "h_TightIdNoIso_Eta", "", nEtaBin, EtaBin );
  TH1D* h_TightIdNoIso_Eta_LowVTX  = new TH1D( "h_TightIdNoIso_Eta_LowVTX", "", nEtaBin, EtaBin );
  TH1D* h_TightIdNoIso_Eta_HighVTX = new TH1D( "h_TightIdNoIso_Eta_HighVTX", "", nEtaBin, EtaBin );

  TH1D* h_LooseIdIso_Pt          = new TH1D( "h_LooseIdIso_Pt" , "" , nPtBin,  PtBin );
  TH1D* h_LooseIdIso_Pt_LowVTX   = new TH1D( "h_LooseIdIso_Pt_LowVTX" , "" , nPtBin,  PtBin );
  TH1D* h_LooseIdIso_Pt_HighVTX  = new TH1D( "h_LooseIdIso_Pt_HighVTX" , "" , nPtBin,  PtBin );
  TH1D* h_LooseIdIso_Eta         = new TH1D( "h_LooseIdIso_Eta", "", nEtaBin, EtaBin );
  TH1D* h_LooseIdIso_Eta_LowVTX  = new TH1D( "h_LooseIdIso_Eta_LowVTX", "", nEtaBin, EtaBin );
  TH1D* h_LooseIdIso_Eta_HighVTX = new TH1D( "h_LooseIdIso_Eta_HighVTX", "", nEtaBin, EtaBin );

  TH1D* h_LooseIdNoIso_Pt          = new TH1D( "h_LooseIdNoIso_Pt" , "" , nPtBin,  PtBin );
  TH1D* h_LooseIdNoIso_Pt_LowVTX   = new TH1D( "h_LooseIdNoIso_Pt_LowVTX" , "" , nPtBin,  PtBin );
  TH1D* h_LooseIdNoIso_Pt_HighVTX  = new TH1D( "h_LooseIdNoIso_Pt_HighVTX" , "" , nPtBin,  PtBin );
  TH1D* h_LooseIdNoIso_Eta         = new TH1D( "h_LooseIdNoIso_Eta", "", nEtaBin, EtaBin );
  TH1D* h_LooseIdNoIso_Eta_LowVTX  = new TH1D( "h_LooseIdNoIso_Eta_LowVTX", "", nEtaBin, EtaBin );
  TH1D* h_LooseIdNoIso_Eta_HighVTX = new TH1D( "h_LooseIdNoIso_Eta_HighVTX", "", nEtaBin, EtaBin );

  TH1D* h_NoIdIso_Pt          = new TH1D( "h_NoIdIso_Pt" , "" , nPtBin,  PtBin );
  TH1D* h_NoIdIso_Pt_LowVTX   = new TH1D( "h_NoIdIso_Pt_LowVTX" , "" , nPtBin,  PtBin );
  TH1D* h_NoIdIso_Pt_HighVTX  = new TH1D( "h_NoIdIso_Pt_HighVTX" , "" , nPtBin,  PtBin );
  TH1D* h_NoIdIso_Eta         = new TH1D( "h_NoIdIso_Eta", "", nEtaBin, EtaBin );
  TH1D* h_NoIdIso_Eta_LowVTX  = new TH1D( "h_NoIdIso_Eta_LowVTX", "", nEtaBin, EtaBin );
  TH1D* h_NoIdIso_Eta_HighVTX = new TH1D( "h_NoIdIso_Eta_HighVTX", "", nEtaBin, EtaBin );

  TH1D* h_NoIdNoIso_Pt          = new TH1D( "h_NoIdNoIso_Pt" , "" , nPtBin,  PtBin );
  TH1D* h_NoIdNoIso_Pt_LowVTX   = new TH1D( "h_NoIdNoIso_Pt_LowVTX" , "" , nPtBin,  PtBin );
  TH1D* h_NoIdNoIso_Pt_HighVTX  = new TH1D( "h_NoIdNoIso_Pt_HighVTX" , "" , nPtBin,  PtBin );
  TH1D* h_NoIdNoIso_Eta         = new TH1D( "h_NoIdNoIso_Eta", "", nEtaBin, EtaBin );
  TH1D* h_NoIdNoIso_Eta_LowVTX  = new TH1D( "h_NoIdNoIso_Eta_LowVTX", "", nEtaBin, EtaBin );
  TH1D* h_NoIdNoIso_Eta_HighVTX = new TH1D( "h_NoIdNoIso_Eta_HighVTX", "", nEtaBin, EtaBin );

  TH1D* h_L3_NVTX                 = new TH1D( "h_L3_NVTX" , "" , 100, 0, 100);
  TH1D* h_TightIdIso_NVTX         = new TH1D( "h_TightIdIso_NVTX" , "" , 100, 0, 100);
  TH1D* h_TightIdNoIso_NVTX       = new TH1D( "h_TightIdNoIso_NVTX" , "" , 100, 0, 100);
  TH1D* h_LooseIdIso_NVTX         = new TH1D( "h_LooseIdIso_NVTX" , "" , 100, 0, 100);
  TH1D* h_LooseIdNoIso_NVTX       = new TH1D( "h_LooseIdNoIso_NVTX" , "" , 100, 0, 100);
  TH1D* h_NoIdIso_NVTX            = new TH1D( "h_NoIdIso_NVTX" , "" , 100, 0, 100);
  TH1D* h_NoIdNoIso_NVTX          = new TH1D( "h_NoIdNoIso_NVTX" , "" , 100, 0, 100);


  // -- Input & Output files -- //
    TString outputDir = "outputs/Purity_JustMatching_v3/v"+theDate;
    if (gSystem->mkdir(outputDir,kTRUE) != -1)
      gSystem->mkdir(outputDir,kTRUE);

    TString outputPath = outputDir+"/Purity_JustMatching_v3_"+Period+"_"+Menu+"_"+hltname+"_"+theDate+".root";
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
    if(Menu.Contains("v3p1")) {
      if(Menu == "v3p1_D") {
        TString NtuplePath = "/u/user/msoh/SE_UserHome/RateStudy2017_v3/v20171117/SingleMuon/crab_RateStudy2017_v3_SingleMuon_Run2017Dv1_v3p1_20171117";
        cout << "Adding...." << NtuplePath << endl;
        inputchain -> Add(NtuplePath+"/muonNtuple_*.root");
      }
      else if(Menu == "v3p1_E") {
        TString NtuplePath = "/u/user/msoh/SE_UserHome/RateStudy2017_v3/v20171117/SingleMuon/crab_RateStudy2017_v3_SingleMuon_Run2017Ev1_v3p1_20171117";
        cout << "Adding...." << NtuplePath << endl;
        inputchain -> Add(NtuplePath+"/muonNtuple_*.root");
      }
      else if(Menu == "v3p1_F") {
        TString NtuplePath = "/u/user/msoh/SE_UserHome/RateStudy2017_v3/v20171117/SingleMuon/crab_RateStudy2017_v3_SingleMuon_Run2017Fv1_v3p1_20171117";
        cout << "Adding...." << NtuplePath << endl;
        inputchain -> Add(NtuplePath+"/muonNtuple_*.root");
      }
    }
    else {
      for(Int_t iRun=0; iRun<nRuns; ++iRun) {
        TString NtuplePath = "/u/user/msoh/SE_UserHome/RateStudy2017_v3/v20171117/SingleMuon/crab_RateStudy2017_v3_SingleMuon_"+Runs[iRun]+"_"+Menu+"_20171117";
        if( gSystem->cd(NtuplePath) ) {
          cout << "Adding...." << NtuplePath << endl;
          inputchain -> Add(NtuplePath+"/muonNtuple_*.root");
        }
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
  int nentries = inputchain->GetEntries(); //Fast();
  std::cout << "Number of entries = " << nentries << std::endl;

  Int_t nL3MoreThan1  = 0;
  Int_t nNoIdIso      = 0;
  Int_t nNoIdNoIso    = 0;
  Int_t nLooseIdIso   = 0;
  Int_t nLooseIdNoIso = 0;
  Int_t nTightIdIso   = 0;
  Int_t nTightIdNoIso = 0;

  //nentries = 1000;
  for (Int_t eventNo=0; eventNo < nentries; eventNo++) {
    if(!verbose) loadBar(eventNo+1, nentries, 100, 100);
    Int_t IgetEvent   = inputchain   -> GetEvent(eventNo);

    if( !( (ev->runNumber >= RunRanges[iPeriod]) && (ev->runNumber < RunRanges[iPeriod+1]) ) ) continue;

    Int_t nVTX = ev->nVtx;

    unsigned int nOffMuons = ev->muons.size();
    if (verbose) cout<< "\n\t# Offline muons : " << nOffMuons << endl;
    //if (nOffMuons < 1) continue;

    std::vector< HLTObjCand > vec_L3s;

    if ( ev-> hlt.find(hltname) ) {
      if (verbose) std::cout<< "\t" << hltname << " is fired!" << endl;
      for (std::vector<HLTObjCand>::const_iterator it_mu = ev->hlt.objects.begin(); it_mu != ev->hlt.objects.end(); ++it_mu) {
        if(it_mu->filterTag.find(filter)!=std::string::npos) {
          if (verbose) cout<< "\t\tfilter : " <<it_mu->filterTag << "\t" << it_mu->id << endl;
          vec_L3s.push_back(*it_mu);
        }
      }  //for (std::vector<HLTObjCand>::const_iterator
    }  //if ( ev-> hlt.find(hltname) )

    Int_t nL3Muons = (Int_t)vec_L3s.size();
    if (verbose) cout<< "\t# L3 muons : " << nL3Muons << endl;
    if (nL3Muons < 1) continue;

    nL3MoreThan1 += 1;

    HLTObjCand theL3;
    Double_t maxPt = OfflinePtCut;
    for(Int_t i=0; i<nL3Muons; ++i) {
      if( vec_L3s[i].pt > maxPt) {
        maxPt = vec_L3s[i].pt;
        theL3 = vec_L3s[i];
      }
    }

    bool isMatchTightIdIso   = false;
    bool isMatchTightIdNoIso = false;
    bool isMatchLooseIdIso   = false;
    bool isMatchLooseIdNoIso = false;
    bool isMatchNoIdIso      = false;
    bool isMatchNoIdNoIso    = false;
    MuonCand   theMu;
    for(Int_t iOff=0; iOff<nOffMuons; ++iOff){
      MuonCand OffMu = ev->muons.at(iOff);
      if ( MatchTightIdIso(theL3, OffMu, OfflinePtCut) )    isMatchTightIdIso   = true;
      if ( MatchTightIdNoIso(theL3, OffMu, OfflinePtCut) )  isMatchTightIdNoIso = true;
      if ( MatchLooseIdIso(theL3, OffMu, OfflinePtCut) )    isMatchLooseIdIso   = true;
      if ( MatchLooseIdNoIso(theL3, OffMu, OfflinePtCut) )  isMatchLooseIdNoIso = true;
      if ( MatchNoIdIso(theL3, OffMu, OfflinePtCut) )       isMatchNoIdIso      = true;
      if ( MatchNoIdNoIso(theL3, OffMu, OfflinePtCut) )     isMatchNoIdNoIso    = true;
    }

    h_L3_Pt ->Fill(theL3.pt);
    h_L3_Eta->Fill(theL3.eta);
    if(nVTX<27) {
      h_L3_Pt_LowVTX ->Fill(theL3.pt);
      h_L3_Eta_LowVTX->Fill(theL3.eta);
    }
    else if(nVTX>=27) {
      h_L3_Pt_HighVTX ->Fill(theL3.pt);
      h_L3_Eta_HighVTX->Fill(theL3.eta);
    }
    h_L3_NVTX->Fill(nVTX);

    if(isMatchTightIdIso) {
      nTightIdIso += 1;
      h_TightIdIso_Pt ->Fill(theL3.pt);
      h_TightIdIso_Eta->Fill(theL3.eta);
      if(nVTX<27) {
        h_TightIdIso_Pt_LowVTX ->Fill(theL3.pt);
        h_TightIdIso_Eta_LowVTX->Fill(theL3.eta);
      }
      else if(nVTX>=27) {
        h_TightIdIso_Pt_HighVTX ->Fill(theL3.pt);
        h_TightIdIso_Eta_HighVTX->Fill(theL3.eta);
      }
      h_TightIdIso_NVTX->Fill(nVTX);
    }
    if(isMatchTightIdNoIso) {
      nTightIdNoIso += 1;
      h_TightIdNoIso_Pt ->Fill(theL3.pt);
      h_TightIdNoIso_Eta->Fill(theL3.eta);
      if(nVTX<27) {
        h_TightIdNoIso_Pt_LowVTX ->Fill(theL3.pt);
        h_TightIdNoIso_Eta_LowVTX->Fill(theL3.eta);
      }
      else if(nVTX>=27) {
        h_TightIdNoIso_Pt_HighVTX ->Fill(theL3.pt);
        h_TightIdNoIso_Eta_HighVTX->Fill(theL3.eta);
      }
      h_TightIdNoIso_NVTX->Fill(nVTX);
    }
    if(isMatchLooseIdIso) {
      nLooseIdIso += 1;
      h_LooseIdIso_Pt ->Fill(theL3.pt);
      h_LooseIdIso_Eta->Fill(theL3.eta);
      if(nVTX<27) {
        h_LooseIdIso_Pt_LowVTX ->Fill(theL3.pt);
        h_LooseIdIso_Eta_LowVTX->Fill(theL3.eta);
      }
      else if(nVTX>=27) {
        h_LooseIdIso_Pt_HighVTX ->Fill(theL3.pt);
        h_LooseIdIso_Eta_HighVTX->Fill(theL3.eta);
      }
      h_LooseIdIso_NVTX->Fill(nVTX);
    }
    if(isMatchLooseIdNoIso) {
      nLooseIdNoIso += 1;
      h_LooseIdNoIso_Pt ->Fill(theL3.pt);
      h_LooseIdNoIso_Eta->Fill(theL3.eta);
      if(nVTX<27) {
        h_LooseIdNoIso_Pt_LowVTX ->Fill(theL3.pt);
        h_LooseIdNoIso_Eta_LowVTX->Fill(theL3.eta);
      }
      else if(nVTX>=27) {
        h_LooseIdNoIso_Pt_HighVTX ->Fill(theL3.pt);
        h_LooseIdNoIso_Eta_HighVTX->Fill(theL3.eta);
      }
      h_LooseIdNoIso_NVTX->Fill(nVTX);
    }
    if(isMatchNoIdIso) {
      nNoIdIso += 1;
      h_NoIdIso_Pt ->Fill(theL3.pt);
      h_NoIdIso_Eta->Fill(theL3.eta);
      if(nVTX<27) {
        h_NoIdIso_Pt_LowVTX ->Fill(theL3.pt);
        h_NoIdIso_Eta_LowVTX->Fill(theL3.eta);
      }
      else if(nVTX>=27) {
        h_NoIdIso_Pt_HighVTX ->Fill(theL3.pt);
        h_NoIdIso_Eta_HighVTX->Fill(theL3.eta);
      }
      h_NoIdIso_NVTX->Fill(nVTX);
    }
    if(isMatchNoIdNoIso) {
      nNoIdNoIso += 1;
      h_NoIdNoIso_Pt ->Fill(theL3.pt);
      h_NoIdNoIso_Eta->Fill(theL3.eta);
      if(nVTX<27) {
        h_NoIdNoIso_Pt_LowVTX ->Fill(theL3.pt);
        h_NoIdNoIso_Eta_LowVTX->Fill(theL3.eta);
      }
      else if(nVTX>=27) {
        h_NoIdNoIso_Pt_HighVTX ->Fill(theL3.pt);
        h_NoIdNoIso_Eta_HighVTX->Fill(theL3.eta);
      }
      h_NoIdNoIso_NVTX->Fill(nVTX);
    }

  }  //for (Int_t eventNo

  outfile           -> cd();

  h_L3_Pt              ->Sumw2();
  h_L3_Pt_LowVTX       ->Sumw2();
  h_L3_Pt_HighVTX      ->Sumw2();
  h_L3_Eta             ->Sumw2();
  h_L3_Eta_LowVTX      ->Sumw2();
  h_L3_Eta_HighVTX     ->Sumw2();

  h_TightIdIso_Pt              ->Sumw2();
  h_TightIdIso_Pt_LowVTX       ->Sumw2();
  h_TightIdIso_Pt_HighVTX      ->Sumw2();
  h_TightIdIso_Eta             ->Sumw2();
  h_TightIdIso_Eta_LowVTX      ->Sumw2();
  h_TightIdIso_Eta_HighVTX     ->Sumw2();
  h_TightIdNoIso_Pt            ->Sumw2();
  h_TightIdNoIso_Pt_LowVTX     ->Sumw2();
  h_TightIdNoIso_Pt_HighVTX    ->Sumw2();
  h_TightIdNoIso_Eta           ->Sumw2();
  h_TightIdNoIso_Eta_LowVTX    ->Sumw2();
  h_TightIdNoIso_Eta_HighVTX   ->Sumw2();

  h_LooseIdIso_Pt              ->Sumw2();
  h_LooseIdIso_Pt_LowVTX       ->Sumw2();
  h_LooseIdIso_Pt_HighVTX      ->Sumw2();
  h_LooseIdIso_Eta             ->Sumw2();
  h_LooseIdIso_Eta_LowVTX      ->Sumw2();
  h_LooseIdIso_Eta_HighVTX     ->Sumw2();
  h_LooseIdNoIso_Pt            ->Sumw2();
  h_LooseIdNoIso_Pt_LowVTX     ->Sumw2();
  h_LooseIdNoIso_Pt_HighVTX    ->Sumw2();
  h_LooseIdNoIso_Eta           ->Sumw2();
  h_LooseIdNoIso_Eta_LowVTX    ->Sumw2();
  h_LooseIdNoIso_Eta_HighVTX   ->Sumw2();

  h_NoIdIso_Pt              ->Sumw2();
  h_NoIdIso_Pt_LowVTX       ->Sumw2();
  h_NoIdIso_Pt_HighVTX      ->Sumw2();
  h_NoIdIso_Eta             ->Sumw2();
  h_NoIdIso_Eta_LowVTX      ->Sumw2();
  h_NoIdIso_Eta_HighVTX     ->Sumw2();
  h_NoIdNoIso_Pt            ->Sumw2();
  h_NoIdNoIso_Pt_LowVTX     ->Sumw2();
  h_NoIdNoIso_Pt_HighVTX    ->Sumw2();
  h_NoIdNoIso_Eta           ->Sumw2();
  h_NoIdNoIso_Eta_LowVTX    ->Sumw2();
  h_NoIdNoIso_Eta_HighVTX   ->Sumw2();

  h_L3_NVTX           ->Sumw2();
  h_TightIdIso_NVTX   ->Sumw2();
  h_TightIdNoIso_NVTX ->Sumw2();
  h_LooseIdIso_NVTX   ->Sumw2();
  h_LooseIdNoIso_NVTX ->Sumw2();
  h_NoIdIso_NVTX      ->Sumw2();
  h_NoIdNoIso_NVTX    ->Sumw2();

  h_L3_Pt              ->Write();
  h_L3_Pt_LowVTX       ->Write();
  h_L3_Pt_HighVTX      ->Write();
  h_L3_Eta             ->Write();
  h_L3_Eta_LowVTX      ->Write();
  h_L3_Eta_HighVTX     ->Write();

  h_TightIdIso_Pt              ->Write();
  h_TightIdIso_Pt_LowVTX       ->Write();
  h_TightIdIso_Pt_HighVTX      ->Write();
  h_TightIdIso_Eta             ->Write();
  h_TightIdIso_Eta_LowVTX      ->Write();
  h_TightIdIso_Eta_HighVTX     ->Write();
  h_TightIdNoIso_Pt            ->Write();
  h_TightIdNoIso_Pt_LowVTX     ->Write();
  h_TightIdNoIso_Pt_HighVTX    ->Write();
  h_TightIdNoIso_Eta           ->Write();
  h_TightIdNoIso_Eta_LowVTX    ->Write();
  h_TightIdNoIso_Eta_HighVTX   ->Write();

  h_LooseIdIso_Pt              ->Write();
  h_LooseIdIso_Pt_LowVTX       ->Write();
  h_LooseIdIso_Pt_HighVTX      ->Write();
  h_LooseIdIso_Eta             ->Write();
  h_LooseIdIso_Eta_LowVTX      ->Write();
  h_LooseIdIso_Eta_HighVTX     ->Write();
  h_LooseIdNoIso_Pt            ->Write();
  h_LooseIdNoIso_Pt_LowVTX     ->Write();
  h_LooseIdNoIso_Pt_HighVTX    ->Write();
  h_LooseIdNoIso_Eta           ->Write();
  h_LooseIdNoIso_Eta_LowVTX    ->Write();
  h_LooseIdNoIso_Eta_HighVTX   ->Write();

  h_NoIdIso_Pt              ->Write();
  h_NoIdIso_Pt_LowVTX       ->Write();
  h_NoIdIso_Pt_HighVTX      ->Write();
  h_NoIdIso_Eta             ->Write();
  h_NoIdIso_Eta_LowVTX      ->Write();
  h_NoIdIso_Eta_HighVTX     ->Write();
  h_NoIdNoIso_Pt            ->Write();
  h_NoIdNoIso_Pt_LowVTX     ->Write();
  h_NoIdNoIso_Pt_HighVTX    ->Write();
  h_NoIdNoIso_Eta           ->Write();
  h_NoIdNoIso_Eta_LowVTX    ->Write();
  h_NoIdNoIso_Eta_HighVTX   ->Write();

  h_L3_NVTX           ->Write();
  h_TightIdIso_NVTX   ->Write();
  h_TightIdNoIso_NVTX ->Write();
  h_LooseIdIso_NVTX   ->Write();
  h_LooseIdNoIso_NVTX ->Write();
  h_NoIdIso_NVTX      ->Write();
  h_NoIdNoIso_NVTX    ->Write();

  outfile           -> Close();

  cout<< "\n\tnL3MoreThan1   =    " << nL3MoreThan1 << endl;
  cout<< "\t    nNoIdIso      =    " << nNoIdIso << endl;
  cout<< "\t    nNoIdNoIso    =    " << nNoIdNoIso << endl;
  cout<< "\t    nLooseIdIso   =    " << nLooseIdIso << endl;
  cout<< "\t    nLooseIdNoIso =    " << nLooseIdNoIso << endl;
  cout<< "\t    nTightIdIso   =    " << nTightIdIso << endl;
  cout<< "\t    nTightIdNoIso =    " << nTightIdNoIso << endl;

  cout << "\n\n\t\t\tFinish!!\n" << std::endl;

  Double_t TotalRunTime = totaltime.CpuTime();
  cout << "Total RunTime: " << TotalRunTime << " seconds" << endl;

  TTimeStamp ts_end;
  cout << "[End Time(local time): " << ts_end.AsString("l") << "]" << endl;

  return;
}

/*======================================================================*/

bool MatchTightIdIso(HLTObjCand L3Mu, MuonCand OffMu, double OfflinePtCut){

  bool match = false;

  Double_t minDR = 0.1;

  if (!( OffMu.pt         > OfflinePtCut  )) return false;
  if (!( fabs(OffMu.eta)  < 2.4 ))           return false;
  if (!( OffMu.isTight    == 1  ))           return false;

  //add isolation cut
  Double_t offlineiso04 = OffMu.chargedDep_dR04 + std::max(0.,
                       OffMu.photonDep_dR04 + OffMu.neutralDep_dR04 - 0.5*OffMu.puPt_dR04);
  offlineiso04       = offlineiso04 / OffMu.pt;
  if (!(offlineiso04   < offlineIsoCut)) return false;

  Double_t theDR = deltaR(L3Mu.eta, L3Mu.phi, OffMu.eta, OffMu.phi);
  if(theDR < minDR)  match = true;

  return match;
}

bool MatchTightIdNoIso(HLTObjCand L3Mu, MuonCand OffMu, double OfflinePtCut){

  bool match = false;

  Double_t minDR = 0.1;

  if (!( OffMu.pt         > OfflinePtCut  )) return false;
  if (!( fabs(OffMu.eta)  < 2.4 ))           return false;
  if (!( OffMu.isTight    == 1  ))           return false;

  //add isolation cut
  //Double_t offlineiso04 = OffMu.chargedDep_dR04 + std::max(0.,
  //                     OffMu.photonDep_dR04 + OffMu.neutralDep_dR04 - 0.5*OffMu.puPt_dR04);
  //offlineiso04       = offlineiso04 / OffMu.pt;
  //if (!(offlineiso04   < offlineIsoCut)) return false;

  Double_t theDR = deltaR(L3Mu.eta, L3Mu.phi, OffMu.eta, OffMu.phi);
  if(theDR < minDR)  match = true;

  return match;
}

bool MatchLooseIdIso(HLTObjCand L3Mu, MuonCand OffMu, double OfflinePtCut){

  bool match = false;

  Double_t minDR = 0.1;

  if (!( OffMu.pt         > OfflinePtCut  )) return false;
  if (!( fabs(OffMu.eta)  < 2.4 ))           return false;
  if (!( OffMu.isLoose    == 1  ))           return false;

  //add isolation cut
  Double_t offlineiso04 = OffMu.chargedDep_dR04 + std::max(0.,
                       OffMu.photonDep_dR04 + OffMu.neutralDep_dR04 - 0.5*OffMu.puPt_dR04);
  offlineiso04       = offlineiso04 / OffMu.pt;
  if (!(offlineiso04   < offlineIsoCut)) return false;

  Double_t theDR = deltaR(L3Mu.eta, L3Mu.phi, OffMu.eta, OffMu.phi);
  if(theDR < minDR)  match = true;

  return match;
}

bool MatchLooseIdNoIso(HLTObjCand L3Mu, MuonCand OffMu, double OfflinePtCut){

  bool match = false;

  Double_t minDR = 0.1;

  if (!( OffMu.pt         > OfflinePtCut  )) return false;
  if (!( fabs(OffMu.eta)  < 2.4 ))           return false;
  if (!( OffMu.isLoose    == 1  ))           return false;

  //add isolation cut
  //Double_t offlineiso04 = OffMu.chargedDep_dR04 + std::max(0.,
  //                     OffMu.photonDep_dR04 + OffMu.neutralDep_dR04 - 0.5*OffMu.puPt_dR04);
  //offlineiso04       = offlineiso04 / OffMu.pt;
  //if (!(offlineiso04   < offlineIsoCut)) return false;

  Double_t theDR = deltaR(L3Mu.eta, L3Mu.phi, OffMu.eta, OffMu.phi);
  if(theDR < minDR)  match = true;

  return match;
}

bool MatchNoIdIso(HLTObjCand L3Mu, MuonCand OffMu, double OfflinePtCut){

  bool match = false;

  Double_t minDR = 0.1;

  if (!( OffMu.pt         > OfflinePtCut  )) return false;
  if (!( fabs(OffMu.eta)  < 2.4 ))           return false;
  //if (!( OffMu.isLoose    == 1  ))           return false;

  //add isolation cut
  Double_t offlineiso04 = OffMu.chargedDep_dR04 + std::max(0.,
                       OffMu.photonDep_dR04 + OffMu.neutralDep_dR04 - 0.5*OffMu.puPt_dR04);
  offlineiso04       = offlineiso04 / OffMu.pt;
  if (!(offlineiso04   < offlineIsoCut)) return false;

  Double_t theDR = deltaR(L3Mu.eta, L3Mu.phi, OffMu.eta, OffMu.phi);
  if(theDR < minDR)  match = true;

  return match;
}

bool MatchNoIdNoIso(HLTObjCand L3Mu, MuonCand OffMu, double OfflinePtCut){

  bool match = false;

  Double_t minDR = 0.1;

  if (!( OffMu.pt         > OfflinePtCut  )) return false;
  if (!( fabs(OffMu.eta)  < 2.4 ))           return false;
  //if (!( OffMu.isLoose    == 1  ))           return false;

  //add isolation cut
  //Double_t offlineiso04 = OffMu.chargedDep_dR04 + std::max(0.,
  //                     OffMu.photonDep_dR04 + OffMu.neutralDep_dR04 - 0.5*OffMu.puPt_dR04);
  //offlineiso04       = offlineiso04 / OffMu.pt;
  //if (!(offlineiso04   < offlineIsoCut)) return false;

  Double_t theDR = deltaR(L3Mu.eta, L3Mu.phi, OffMu.eta, OffMu.phi);
  if(theDR < minDR)  match = true;

  return match;
}





bool matchMuon(MuonCand mu, std::vector<HLTObjCand> toc, std::string tagFilterName){

  bool match = false;
  int ntoc = toc.size();

  Double_t minDR = 0.1;
  //if (tagFilterName.find("L1fL1") != std::string::npos) minDR = 0.3;
  Double_t theDR = 100;
  for ( std::vector<HLTObjCand>::const_iterator it = toc.begin(); it != toc.end(); ++it ) {
    if ( it->filterTag.compare(tagFilterName) == 0) {

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
  Double_t offlineiso04 = mu.chargedDep_dR04 + std::max(0.,
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
  Double_t ratio = x/(Double_t)n;
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

