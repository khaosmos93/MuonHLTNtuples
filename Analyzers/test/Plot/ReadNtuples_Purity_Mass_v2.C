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

bool matchMuon              (MuonCand, std::vector<HLTObjCand>, std::string);
bool selectTagMuon          (MuonCand, double );
bool selectProbeMuonLoose   (MuonCand, HLTObjCand, double, TH1D*, TH1D* );
bool selectProbeMuonTight   (MuonCand, HLTObjCand, double, TH1D* );
bool isSameMuon             (MuonCand, MuonCand);
static inline void loadBar(int x, int n, int r, int w);

/*======================================================================*/

void ReadNtuples_Purity_Mass_v2(TString Period, TString Menu, std::string hltname, bool verbose) {

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
  const double PtBin[14]  = { 20, 24, 27, 30, 35, 40, 45, 50, 60, 70, 90, 150, 250, 1e99 };
  const Int_t nEtaBin = 15;
  const double EtaBin[16] = {-2.4, -2.1, -1.6, -1.2, -1.04, -0.9, -0.3, -0.2,  0.2, 0.3, 0.9, 1.04, 1.2, 1.6, 2.1, 2.4};

  TH1D *h_Mass = new TH1D("h_Mass","",10000,0,10000);
  TH1D *h_Mass_Pt[nPtBin];
  TH1D *h_Mass_Eta[nEtaBin];
  //TH2D *h_Mass_PtEta[nPtBin][nEtaBin];

  TH1D *h_Mass_OS = new TH1D("h_Mass_OS","",10000,0,10000);
  TH1D *h_Mass_OS_Pt[nPtBin];
  TH1D *h_Mass_OS_Eta[nEtaBin];
  //TH2D *h_Mass_OS_PtEta[nPtBin][nEtaBin];

  TH1D *h_Mass_SS = new TH1D("h_Mass_SS","",10000,0,10000);
  TH1D *h_Mass_SS_Pt[nPtBin];
  TH1D *h_Mass_SS_Eta[nEtaBin];
  //TH2D *h_Mass_SS_PtEta[nPtBin][nEtaBin];

  TString PtBinName = ""; TString EtaBinName = "";
  for(Int_t iPt=0; iPt<nPtBin; ++iPt) {
    PtBinName.Form("%.0f-%.0f",PtBin[iPt],PtBin[iPt+1]);
    h_Mass_Pt[iPt] = new TH1D("h_Mass_Pt_"+PtBinName,"",10000,0,10000);
    h_Mass_OS_Pt[iPt] = new TH1D("h_Mass_OS_Pt_"+PtBinName,"",10000,0,10000);
    h_Mass_SS_Pt[iPt] = new TH1D("h_Mass_SS_Pt_"+PtBinName,"",10000,0,10000);
  }
  for(Int_t iEta=0; iEta<nEtaBin; ++iEta) {
    EtaBinName.Form("%.1f-%.1f",EtaBin[iEta],EtaBin[iEta+1]);
    h_Mass_Eta[iEta] = new TH1D("h_Mass_Eta_"+EtaBinName,"",10000,0,10000);
    h_Mass_OS_Eta[iEta] = new TH1D("h_Mass_OS_Eta_"+EtaBinName,"",10000,0,10000);
    h_Mass_SS_Eta[iEta] = new TH1D("h_Mass_SS_Eta_"+EtaBinName,"",10000,0,10000);
  }

  // -- Input & Output files -- //
    TString outputDir = "outputs/Purity_Mass_v2/v"+theDate;
    if (gSystem->mkdir(outputDir,kTRUE) != -1)
      gSystem->mkdir(outputDir,kTRUE);

    TString outputPath = outputDir+"/Purity_Mass_v2_"+Period+"_"+Menu+"_"+hltname+"_"+theDate+".root";
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
  int nentries = inputchain->GetEntries(); //Fast();
  std::cout << "Number of entries = " << nentries << std::endl;

  Int_t nL3MoreThan2 = 0;
  Int_t nTnPPair = 0;

  //nentries = 1000;
  for (Int_t eventNo=0; eventNo < nentries; eventNo++) {
    if(!verbose) loadBar(eventNo+1, nentries, 100, 100);
    Int_t IgetEvent   = inputchain   -> GetEvent(eventNo);

    if( !( (ev->runNumber >= RunRanges[iPeriod]) && (ev->runNumber < RunRanges[iPeriod+1]) ) ) continue;

    unsigned int nOffMuons = ev->muons.size();
    if (verbose) cout<< "\n\t# Offline muons : " << nOffMuons << endl;
    if (nOffMuons < 1) continue;

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
    if (nL3Muons < 2) continue;

    nL3MoreThan2 += 1;

    TLorentzVector vec4_i, vec4_j;
    Double_t theMass = -1;   //pair<HLTObjCand, HLTObjCand> thePair;
    HLTObjCand theTag;
    HLTObjCand theProbe;
    MuonCand theOffMu;
    for(Int_t i=0; i<nL3Muons; ++i) {
      HLTObjCand iMu = vec_L3s[i];  //Tag
      vec4_i.SetPtEtaPhiM(vec_L3s[i].pt, vec_L3s[i].eta, vec_L3s[i].phi, muonmass);

      for(Int_t j=0; j<nL3Muons; ++j) {
        if(i == j) continue;

        HLTObjCand jMu = vec_L3s[j];  //Probe
        vec4_j.SetPtEtaPhiM(vec_L3s[j].pt, vec_L3s[j].eta, vec_L3s[j].phi, muonmass);

        // Probe(j), Off match
        bool match = false;
        minDR = 0.1;
        for(Int_t iOff=0; iOff<nOffMuons; ++iOff){
          MuonCand OffMu = ev->muons.at(iOff);
          Double_t dR_temp = deltaR(jMu.eta, jMu.phi, OffMu.eta, OffMu.phi);
          if (verbose) cout << "\t\t\t\tdR_temp = " << dR_temp << endl;
          if (dR_temp < minDR){
            theOffMu = OffMu;
            minDR = dR_temp;
            match = true;
          }
        }
        if(match) {
          Double_t Mass_temp = (vec4_i+vec4_j).M();
          if (verbose) cout << "\t\t\tMass_temp = " << Mass_temp << endl;
          if(Mass_temp > theMass) {
            theMass  = Mass_temp;
            theTag   = vec_L3s[i];
            theProbe = vec_L3s[j];
          }
        }

      }  //for(Int_t j=0
    }  //for(Int_t i=0
    if(theMass > 0) {
      if (verbose) cout<< "\t\tTag         : " << theTag.pt << "\t" << theTag.eta << "\t" << theTag.phi << endl;
      if (verbose) cout<< "\t\tProve       : " << theProbe.pt << "\t" << theProbe.eta << "\t" << theProbe.phi << endl;
      if (verbose) cout<< "\t\tMatched Off : " << theOffMu.pt << "\t" << theOffMu.eta << "\t" << theOffMu.phi << endl;
      if (verbose) cout<< "\t\tTnP Mass    = " << theMass << endl;

      nTnPPair += 1;

      // Fill Histo
      h_Mass->Fill(theMass);
      for(Int_t iPt=0; iPt<nPtBin; ++iPt) {
        if( (theProbe.pt > PtBin[iPt]) && (theProbe.pt < PtBin[iPt+1]) ) {
          h_Mass_Pt[iPt]->Fill(theMass);
        }
      }
      for(Int_t iEta=0; iEta<nEtaBin; ++iEta) {
        if( (theProbe.eta > EtaBin[iEta]) && (theProbe.eta < EtaBin[iEta+1]) ) {
          h_Mass_Eta[iEta]->Fill(theMass);
        }
      }

      if( theTag.id*theProbe.id < 0 ) {
        h_Mass_OS->Fill(theMass);
        for(Int_t iPt=0; iPt<nPtBin; ++iPt) {
          if( (theProbe.pt > PtBin[iPt]) && (theProbe.pt < PtBin[iPt+1]) ) {
            h_Mass_OS_Pt[iPt]->Fill(theMass);
          }
        }
        for(Int_t iEta=0; iEta<nEtaBin; ++iEta) {
          if( (theProbe.eta > EtaBin[iEta]) && (theProbe.eta < EtaBin[iEta+1]) ) {
            h_Mass_OS_Eta[iEta]->Fill(theMass);
          }
        }
      }

      else if( theTag.id*theProbe.id > 0 ) {
        h_Mass_SS->Fill(theMass);
        for(Int_t iPt=0; iPt<nPtBin; ++iPt) {
          if( (theProbe.pt > PtBin[iPt]) && (theProbe.pt < PtBin[iPt+1]) ) {
            h_Mass_SS_Pt[iPt]->Fill(theMass);
          }
        }
        for(Int_t iEta=0; iEta<nEtaBin; ++iEta) {
          if( (theProbe.eta > EtaBin[iEta]) && (theProbe.eta < EtaBin[iEta+1]) ) {
            h_Mass_SS_Eta[iEta]->Fill(theMass);
          }
        }
      }

    }
    else
      if (verbose) cout<< "\t\tNo TnP Pair    " << theMass << endl;

  }  //for (Int_t eventNo

  outfile           -> cd();
  h_Mass   ->Sumw2();
  h_Mass_OS->Sumw2();
  h_Mass_SS->Sumw2();
  h_Mass   ->Write();
  h_Mass_OS->Write();
  h_Mass_SS->Write();
  for(Int_t iPt=0; iPt<nPtBin; ++iPt) {
    h_Mass_Pt[iPt]   ->Sumw2();
    h_Mass_OS_Pt[iPt]->Sumw2();
    h_Mass_SS_Pt[iPt]->Sumw2();
    h_Mass_Pt[iPt]   ->Write();
    h_Mass_OS_Pt[iPt]->Write();
    h_Mass_SS_Pt[iPt]->Write();
  }
  for(Int_t iEta=0; iEta<nEtaBin; ++iEta) {
    h_Mass_Eta[iEta]   ->Sumw2();
    h_Mass_OS_Eta[iEta]->Sumw2();
    h_Mass_SS_Eta[iEta]->Sumw2();
    h_Mass_Eta[iEta]   ->Write();
    h_Mass_OS_Eta[iEta]->Write();
    h_Mass_SS_Eta[iEta]->Write();

  }
  outfile           -> Close();

  cout<< "\nnL3MoreThan2 =    " << nL3MoreThan2 << endl;
  cout<<   "nTnPPair     =    " << nTnPPair << endl;

  cout << "\n\n\t\t\tFinish!!\n" << std::endl;

  Double_t TotalRunTime = totaltime.CpuTime();
  cout << "Total RunTime: " << TotalRunTime << " seconds" << endl;

  TTimeStamp ts_end;
  cout << "[End Time(local time): " << ts_end.AsString("l") << "]" << endl;

  return;
}

/*======================================================================*/

bool matchMuon(MuonCand mu, std::vector<HLTObjCand> toc, std::string tagFilterName){

  bool match = false;
  int ntoc = toc.size();

  float minDR = 0.1;
  //if (tagFilterName.find("L1fL1") != std::string::npos) minDR = 0.3;
  float theDR = 100;
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

