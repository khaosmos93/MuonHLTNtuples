
from WMCore.Configuration import Configuration
config = Configuration()

config.section_('General')
config.General.transferOutputs = True
config.General.transferLogs = False
config.General.workArea = 'crab_RateStudy2017_v3_SingleMuon_20171113'
config.General.requestName = 'RateStudy2017_v3_SingleMuon_Run2016Hv2_20171113'

config.section_('JobType')
config.JobType.psetName = 'run_crab.py'
config.JobType.pluginName = 'Analysis'
#config.JobType.disableAutomaticOutputCollection = True
config.JobType.numCores = 8
config.JobType.maxMemoryMB = 2500
config.JobType.maxJobRuntimeMin = 2500

config.section_('Data')
config.Data.outLFNDirBase = '/store/user/moh/RateStudy2017_v3/v20171113/'
config.Data.inputDataset = '/SingleMuon/Run2016H-PromptReco-v2/AOD'
config.Data.publication = False

config.Data.splitting = 'LumiBased'
config.Data.unitsPerJob = 80 #100
config.Data.totalUnits = -1
config.Data.inputDBS = 'global'
config.Data.lumiMask = 'https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions16/13TeV/Final/Cert_271036-284044_13TeV_PromptReco_Collisions16_JSON.txt'
config.Data.runRange = '281207-284035'

config.section_('User')

config.section_('Site')
config.Site.blacklist = ['T3_UK_London_RHUL','T3_UK_SGrid_Oxford','T3_US_Rice']
config.Site.storageSite = 'T2_KR_KNU'
#config.Site.storageSite = 'T3_KR_KISTI'
