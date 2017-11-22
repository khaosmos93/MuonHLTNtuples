
from WMCore.Configuration import Configuration
config = Configuration()

config.section_('General')
config.General.transferOutputs = True
config.General.transferLogs = False
config.General.workArea = 'crab_RateStudy2017_v3_SingleMuon_20171117'
config.General.requestName = 'RateStudy2017_v3_SingleMuon_Run2017Bv1_v1p2_20171117'

config.section_('JobType')
config.JobType.psetName = 'run_crab.py'
config.JobType.pluginName = 'Analysis'
#config.JobType.disableAutomaticOutputCollection = True
config.JobType.numCores = 8
config.JobType.maxMemoryMB = 2500
config.JobType.maxJobRuntimeMin = 2500

config.section_('Data')
config.Data.outLFNDirBase = '/store/user/moh/RateStudy2017_v3/v20171117/'
config.Data.inputDataset = '/SingleMuon/Run2017B-PromptReco-v1/AOD'
config.Data.publication = False

config.Data.splitting = 'LumiBased'
config.Data.unitsPerJob = 70 #100
config.Data.totalUnits = -1
config.Data.inputDBS = 'global'
config.Data.lumiMask = 'https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions17/13TeV/PromptReco/Cert_294927-305364_13TeV_PromptReco_Collisions17_JSON.txt'
config.Data.runRange = '297557-298677'

config.section_('User')

config.section_('Site')
config.Site.blacklist = ['T3_UK_London_RHUL','T3_UK_SGrid_Oxford','T3_US_Rice']
config.Site.storageSite = 'T2_KR_KNU'
#config.Site.storageSite = 'T3_KR_KISTI'
