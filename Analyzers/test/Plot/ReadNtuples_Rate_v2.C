#include "TROOT.h"
#include "TStyle.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TH1D.h"
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

double muonmass = 0.10565837;

double pt_bins[14]  = { 20 ,  24 ,  27 ,   30,   35,   40,   45,   50,  60, 70 ,  90, 150, 250, 500 };
double eta_bins[16] = {-2.4, -2.1, -1.6, -1.2, -1.04, -0.9, -0.3, -0.2,  0.2, 0.3, 0.9, 1.04, 1.2, 1.6, 2.1, 2.4};

static inline void loadBar(int x, int n, int r, int w);

void ReadNtuples_Rate_v2(TString Menu, std::string hltname, bool verbose) {
  TTimeStamp ts_start;
  cout << "[Start Time(local time): " << ts_start.AsString("l") << "]" << endl;
  cout << "Menu :    " << Menu << endl;
  cout << "hltname : " << hltname << endl;

  TStopwatch totaltime;
  totaltime.Start();

  time_t rawtime;
  struct tm * timeinfo;
  char buffer[80];

  time (&rawtime);
  timeinfo = localtime(&rawtime);

  strftime(buffer,sizeof(buffer),"%Y%m%d",timeinfo);
  TString theDate(buffer);  //std::string

  TString outputDir = "outputs/Rate_v2";
  if (gSystem->mkdir(outputDir,kTRUE) != -1)
    gSystem->mkdir(outputDir,kTRUE);

  //TString inputPath = "";
  //TString outputPath = "";

  //inputPath = "inputs/muonNtuple_SingleMuon_"+Menu+"_GJ.root";
  TString outputPath = outputDir+"/Rate_v2_"+Menu+"_"+hltname+"_"+theDate+".root";

  TFile* outfile = new TFile(outputPath,"RECREATE");

  std::string filter;
  if (hltname.find("HLT_IsoMu27_v") != std::string::npos ) {    //( hltname == "HLT_IsoMu27_v9" || hltname == "HLT_IsoMu27_v10" || hltname == "HLT_IsoMu27_v11") {
    filter = "hltL3crIsoL1sMu22Or25L1f0L2f10QL3f27QL3trkIsoFiltered0p07";
  }
  else if (hltname.find("HLT_Mu50_v") != std::string::npos ) {    //( hltname == "HLT_Mu50_v7" || hltname == "HLT_Mu50_v8" || hltname == "HLT_Mu50_v9") {
    filter = "hltL3fL1sMu22Or25L1f0L2f10QL3Filtered50Q";
  }

  //cout << filter << endl;

  TGraph* nvtx_event = new TGraph();
  nvtx_event->SetName("nvtx_event");
  TGraph* lumi_event = new TGraph();
  lumi_event->SetName("lumi_event");
  TGraph* lumi_nvtx = new TGraph();
  lumi_nvtx->SetName("lumi_nvtx");
  TGraph2D* lumi_run = new TGraph2D();
  lumi_run->SetName("lumi_run");
  TGraph2D* nvtx_run = new TGraph2D();
  nvtx_run->SetName("nvtx_run");

  TH1F* muonPt  = new TH1F( "muonPt" , "muonPt" , 13,  pt_bins );
  TH1F* muonEta = new TH1F( "muonEta", "muonEta", 24, -2.4, 2.4 );
  TH1F* muonEta_bin = new TH1F( "muonEta_bin", "muonEta_bin", 15, eta_bins );
  TH1F* muonPhi = new TH1F( "muonPhi", "muonPhi", 40, -3.2, 3.2);
  TH2D* muonEtaPhi = new TH2D( "muonEtaPhi", "muonEtaPhi", 24, -2.4, 2.4, 40, -3.2, 3.2);
  TH2D* muonEtaPhi_bin = new TH2D( "muonEtaPhi_bin", "muonEtaPhi_bin", 15, eta_bins, 40, -3.2, 3.2);

  /*TFile* inputfile = TFile::Open(inputPath, "READ");
  TTree *tree = (TTree*) inputfile -> Get("muonNtuples/muonTree");
  if (!tree) {
    std::cout << " *** tree not found *** " << std::endl;
    return;
  }

  MuonEvent* ev      = new MuonEvent();
  TBranch*  evBranch = tree->GetBranch("event");
  evBranch -> SetAddress(&ev);

  int nentries = tree->GetEntries();  //GetEntriesFast();
  std::cout << "Number of entries = " << nentries << std::endl;*/

  TString Runs[9] = {"Run2016Hv2", "Run2017Bv1", "Run2017Bv2", "Run2017Cv1", "Run2017Cv2", "Run2017Cv3", "Run2017Dv1", "Run2017Ev1", "Run2017Fv1"};
  TChain* inputchain = new TChain("muonNtuples/muonTree");
  for(Int_t iRun=0; iRun<9; ++iRun) {
    TString NtuplePath = "/u/user/msoh/SE_UserHome/RateStudy2017_v3/SingleMuon/crab_RateStudy2017_v3_SingleMuon_"+Runs[iRun]+"_"+Menu+"_20171102";
    if( gSystem->cd(NtuplePath) ) {
      cout << "Adding...." << NtuplePath << endl;
      inputchain -> Add(NtuplePath+"/muonNtuple_*.root");
    }
  }
  MuonEvent* ev      = new MuonEvent();
  TBranch*  evBranch = inputchain->GetBranch("event");
  inputchain -> SetBranchAddress("event",&ev);

  int nentries = inputchain->GetEntries(); //Fast();
  std::cout << "Number of entries = " << nentries << std::endl;


  int currentLS = -1;
  double currentInstLumi = -1;
  int nLS = 0;
  int nMuons = 0;
  bool newLS = false;
  int count = 0;
  int total = 0;
  double avePU = 0;
  double pt = 0;
  double eta = 0;
  double phi = 0;

  //nentries = 10000;
  for (Int_t eventNo=0; eventNo < nentries; eventNo++) {
    if(!verbose) loadBar(eventNo+1, nentries, 100, 100);
    //Int_t IgetEvent   = tree   -> GetEvent(eventNo);
    Int_t IgetEvent   = inputchain   -> GetEvent(eventNo);
    //if (verbose) std::cout<<"\n\tinstLumi = "<<ev->instLumi<<std::endl;

    Double_t run = (Double_t)(ev->runNumber);

    if ( currentLS != ev->luminosityBlockNumber ) {
      currentLS = ev->luminosityBlockNumber;
      ++nLS;
      newLS = true;
      if( eventNo!=0 ) {
        avePU = avePU/(double)count;
        if (verbose) cout <<"\neventNo = "<<eventNo<<endl;
        if (verbose) cout <<"nLS = "<<nLS-1<<endl;
        if (verbose) cout <<"count = "<<count<<endl;
        if (verbose) cout<<"Average PU = "<<avePU<<endl;
        if (verbose) cout <<"rate@LS = "<<(double)count/23.31<<endl;
        nvtx_event->SetPoint(nLS-1,avePU,(double)count/23.31);
        lumi_event->SetPoint(nLS-1,currentInstLumi,(double)count/23.31);
        lumi_nvtx ->SetPoint(nLS-1,currentInstLumi,avePU);
        lumi_run  ->SetPoint(nLS-1,currentInstLumi,run,(double)count/23.31);
        nvtx_run  ->SetPoint(nLS-1,avePU,run,(double)count/23.31);
        if (verbose) cout <<"SetPoint!"<<endl;
        avePU = 0;
        count = 0;
      }
      currentInstLumi = ev->instLumi;
    }

    nMuons = 0;   // # of muons in each event
    if (ev-> hlt.find(hltname)) {
      //if (verbose) std::cout<< hltname << " is fired!" << endl;;
      //cout<<ev->nVtx<<endl;
      avePU += ev->nVtx;
      pt=0;
      for (std::vector<HLTObjCand>::const_iterator imu = ev->hlt.objects.begin(); imu < ev->hlt.objects.end(); imu++){
        if(imu->filterTag.find(filter)!=std::string::npos) {
          //if (verbose) std::cout<< "\tFinal filter : " <<imu->filterTag<<endl;
          if(imu->pt > pt) {
            pt = imu->pt;
            eta = imu->eta;
            phi = imu->phi;
          }
          ++nMuons;
        }
      }
      ++total;
      ++count;
      muonPt       -> Fill( pt );
      muonEta      -> Fill( eta);
      muonEta_bin  -> Fill( eta);
      muonPhi      -> Fill( phi);
      muonEtaPhi   -> Fill( eta, phi);

      /*if (nMuons != 0) {
        if (verbose) cout<<"\t\tnMuons : "<<nMuons<<endl;
      }
      else if (nMuons == 0) {
        if (verbose) cout<<"\t\tnMuons = "<<nMuons<<"  ---> fired path but no final filter object"<<endl;
      }*/
    }

    if (eventNo == nentries-1) {         //need to modi for muti runs
      avePU = avePU/(double)count;
      if (verbose) cout <<"\neventNo = "<<eventNo<<endl;
      if (verbose) cout <<"nLS = "<<nLS<<endl;                                    //nLS not nLS-1
      if (verbose) cout <<"count = "<<count<<endl;
      if (verbose) cout<<"Average PU = "<<avePU<<endl;
      if (verbose) cout <<"rate@LS = "<<(double)count/23.31<<endl;
      nvtx_event->SetPoint(nLS,avePU,(double)count/23.31);                        //nLS not nLS-1
      lumi_event->SetPoint(nLS,currentInstLumi,(double)count/23.31);              //nLS not nLS-1
      lumi_nvtx ->SetPoint(nLS,currentInstLumi,avePU);
      lumi_run  ->SetPoint(nLS,currentInstLumi,run,(double)count/23.31);
      nvtx_run  ->SetPoint(nLS,avePU,run,(double)count/23.31);
      if (verbose) cout <<"SetPoint!"<<endl;
    }

  }

  nvtx_event->RemovePoint(0);
  lumi_event->RemovePoint(0);
  lumi_nvtx->RemovePoint(0);
  lumi_run->RemovePoint(0);
  nvtx_run->RemovePoint(0);


  double SF = 1.0 / (nLS * 23.31);
  cout << "\n||||||||||||||||||||||||||||||" << endl;
  cout<<"Total count = " << total << endl;
  cout<<"Rate = "<<total*SF<<endl;
  cout << "||||||||||||||||||||||||||||||" << endl;

  outfile           -> cd();

  nvtx_event        -> Write();
  lumi_event        -> Write();
  lumi_nvtx         -> Write();
  lumi_run         -> Write();
  nvtx_run         -> Write();
  muonPt            -> Write();
  muonEta           -> Write();
  muonEta_bin       -> Write();
  muonPhi           -> Write();
  muonEtaPhi        -> Write();

  outfile           -> Close();

  std::cout << "\t\t\tFinish!!\n" << std::endl;

  Double_t TotalRunTime = totaltime.CpuTime();
  cout << "Total RunTime: " << TotalRunTime << " seconds" << endl;

  TTimeStamp ts_end;
  cout << "[End Time(local time): " << ts_end.AsString("l") << "]" << endl;

  return;
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
