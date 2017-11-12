import FWCore.ParameterSet.Config as cms

process = cms.Process("NTUPLE")

process.source = cms.Source("PoolSource",
                    fileNames = cms.untracked.vstring(
			#'file:F61F985E-476D-E711-BF13-02163E011926.root',
			#'/store/data/Run2015D/SingleMuon/RAW-RECO/ZMu-PromptReco-v3/000/258/158/00000/FA0B284A-856B-E511-9378-02163E0133E8.root',
                    ),
                    secondaryFileNames = cms.untracked.vstring(),
#                     lumisToProcess = cms.untracked.VLuminosityBlockRange('258158:1-258158:1786'),

)

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff")
GT = 'GlobalTagReplace'
process.GlobalTag.globaltag = GT     ##'92X_dataRun2_Prompt_v5'   #'74X_dataRun2_Prompt_v2'

process.load("Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff")
process.load('Configuration.Geometry.GeometryRecoDB_cff')

#import FWCore.PythonUtilities.LumiList as LumiList
#process.source.lumisToProcess = LumiList.LumiList(filename = '/u/user/msoh/Samples/Data/Cert_271036-284044_13TeV_23Sep2016ReReco_Collisions16_JSON.txt').getVLuminosityBlockRange()

process.muonNtuples =cms.EDAnalyzer("MuonNtuples",
                       offlineVtx               = cms.InputTag("offlinePrimaryVertices"),
                       offlineMuons             = cms.InputTag("muons"),

                       triggerResult            = cms.untracked.InputTag("TriggerResults::HLT"),
                       triggerSummary           = cms.untracked.InputTag("hltTriggerSummaryAOD::HLT"),
                       tagTriggerResult         = cms.untracked.InputTag("TriggerResults::HLT"),
                       tagTriggerSummary        = cms.untracked.InputTag("hltTriggerSummaryAOD::HLT"),

                       L3Candidates             = cms.untracked.InputTag("hltL3MuonCandidates"),
                       L2Candidates             = cms.untracked.InputTag("hltL2MuonCandidates"),
                       L1Candidates             = cms.untracked.InputTag("gmtStage2Digis", "Muon", "RECO"), #if HLT non re-run
                       #L1Candidates             = cms.untracked.InputTag("hltGmtStage2Digis", "Muon"), # if re-run HLT and L1 emulator
                       TkMuCandidates           = cms.untracked.InputTag("hltHighPtTkMuonCands"),
                       NeutralDeposit           = cms.untracked.InputTag("hltMuonHcalPFClusterIsoForMuons"),
                       PhotonsDeposit           = cms.untracked.InputTag("hltMuonEcalPFClusterIsoForMuons"),
                       NeutralDeposit05         = cms.untracked.InputTag("hltMuonHcalPFClusterIsoForMuonsNoEffAreaVeto0p05"),
                       PhotonsDeposit05         = cms.untracked.InputTag("hltMuonEcalPFClusterIsoForMuonsNoEffAreaVeto0p05"),
                       NeutralDeposit1          = cms.untracked.InputTag("hltMuonHcalPFClusterIsoForMuonsNoEffAreaVeto0p1"),
                       PhotonsDeposit1          = cms.untracked.InputTag("hltMuonEcalPFClusterIsoForMuonsNoEffAreaVeto0p1"),
                       ChargedDeposit           = cms.untracked.InputTag("hltMuonTkRelIsolationCut0p09Map", "trkIsoDeposits", "HLT"),

                       RhoCorrectionOnline      = cms.untracked.InputTag("hltFixedGridRhoFastjetAllCaloForMuons"), # for now, same for tag and probe muons
                       RhoCorrectionOffline     = cms.untracked.InputTag("fixedGridRhoFastjetAllCalo"),

                       offlineECalPFIso03       = cms.untracked.InputTag("muonEcalPFClusterIsolationProducer03"),
                       offlineHCalPFIso03       = cms.untracked.InputTag("muonHcalPFClusterIsolationProducer03"),
                       offlineECalPFIso04       = cms.untracked.InputTag("muonEcalPFClusterIsolationProducer04"),
                       offlineHCalPFIso04       = cms.untracked.InputTag("muonHcalPFClusterIsolationProducer04"),

                       lumiScalerTag            = cms.untracked.InputTag("scalersRawToDigi"),
                       puInfoTag                = cms.untracked.InputTag("addPileupInfo"),

                       genParticlesTag          = cms.untracked.InputTag("genParticles"),
                       doOffline                = cms.untracked.bool(True)
                       )

process.mypath  = cms.Path(process.muonNtuples)

outputName = "OutputReplace"
process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string(outputName),
                                   closeFileFast = cms.untracked.bool(False)
                                   )


process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))


process.MessageLogger = cms.Service("MessageLogger",
   destinations   = cms.untracked.vstring('cerr'),
   cerr           = cms.untracked.PSet(
       threshold      = cms.untracked.string('ERROR'),
   ),
#    debugModules  = cms.untracked.vstring('*')
)

process.options = cms.untracked.PSet(
    SkipEvent = cms.untracked.vstring('ProductNotFound'),
    numberOfThreads = cms.untracked.uint32(8)
)

