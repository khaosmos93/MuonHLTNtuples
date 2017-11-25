from CRABClient.UserUtilities import config, getUsernameFromSiteDB
import sys, os
import datetime

now = datetime.datetime.now()
date = now.strftime('%Y%m%d')

submitVersion = 'PurityStudy2017_v1' ### modified
mainOutputDir = '/store/user/%s/%s/v%s' % (getUsernameFromSiteDB(),submitVersion,date) ### modified
PrimaryDataset = ['SingleMuon']

#Golden_JSON_2017 = 'https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions17/13TeV/PromptReco/Cert_294927-305364_13TeV_PromptReco_Collisions17_JSON.txt'
Golden_JSON_2017 = 'https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions17/13TeV/PromptReco/Cert_294927-306462_13TeV_PromptReco_Collisions17_JSON.txt'

DSCOnly_2017 = 'https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions17/13TeV/DCSOnly/json_DCSONLY.txt'

Golden_JSON_2016 = 'https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions16/13TeV/Final/Cert_271036-284044_13TeV_PromptReco_Collisions16_JSON.txt'



DataSample = [
                #('Run20176Hv2_vOld', '281207-284035', '/Run2016H-PromptReco-v2/AOD', '80X_dataRun2_Prompt_v14', Golden_JSON_2016),

                #('Run2017Bv1_v1p1', '297050-297556', '/Run2017B-PromptReco-v1/AOD', '92X_dataRun2_Prompt_v4', Golden_JSON_2017),

                #('Run2017Bv1_v1p2', '297557-298677', '/Run2017B-PromptReco-v1/AOD', '92X_dataRun2_Prompt_v4', Golden_JSON_2017),
                #('Run2017Bv2_v1p2', '298678-299367', '/Run2017B-PromptReco-v2/AOD', '92X_dataRun2_Prompt_v5', Golden_JSON_2017),

                #('Run2017Cv1_v2p0', '299368-299649', '/Run2017C-PromptReco-v1/AOD', '92X_dataRun2_Prompt_v6', Golden_JSON_2017),

                #('Run2017Cv2_v2p1', '300079-300676', '/Run2017C-PromptReco-v2/AOD', '92X_dataRun2_Prompt_v7', Golden_JSON_2017),
                #('Run2017Cv3_v2p1', '300742-301045', '/Run2017C-PromptReco-v3/AOD', '92X_dataRun2_Prompt_v8', Golden_JSON_2017),

                #('Run2017Cv3_v2p2', '301046-302025', '/Run2017C-PromptReco-v3/AOD', '92X_dataRun2_Prompt_v8', Golden_JSON_2017),

                #('Run2017Dv1_v3p0', '302026-302508', '/Run2017D-PromptReco-v1/AOD', '92X_dataRun2_Prompt_v8', Golden_JSON_2017),

                #('Run2017Dv1_v3p1', '302509-302663', '/Run2017D-PromptReco-v1/AOD', '92X_dataRun2_Prompt_v8', Golden_JSON_2017),
                #('Run2017Ev1_v3p1', '303572-304797', '/Run2017E-PromptReco-v1/AOD', '92X_dataRun2_Prompt_v9', Golden_JSON_2017),
                #('Run2017Fv1_v3p1', '305040-305112', '/Run2017F-PromptReco-v1/AOD', '92X_dataRun2_Prompt_v10', Golden_JSON_2017),

                #('Run2017Fv1_v3p2', '305113-305387', '/Run2017F-PromptReco-v1/AOD', '92X_dataRun2_Prompt_v10', Golden_JSON_2017),
                #('Run2017Fv1_v4p0', '305388-306028', '/Run2017F-PromptReco-v1/AOD', '92X_dataRun2_Prompt_v10', Golden_JSON_2017),
                #('Run2017Fv1_v4p1', '306029-306415', '/Run2017F-PromptReco-v1/AOD', '92X_dataRun2_Prompt_v10', Golden_JSON_2017),

              ]

MCSample = [
              ('W_P2_v1p1', '/WJetsToLNu_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer17DRStdmix-NZSFlatPU28to62_SUS01_92X_upgrade2017_realistic_v10-v1/AODSIM', 'HLT_P2_v1p1', '92X_upgrade2017_realistic_v12'),
              ('W_P3_v1p2', '/WJetsToLNu_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer17DRStdmix-NZSFlatPU28to62_SUS01_92X_upgrade2017_realistic_v10-v1/AODSIM', 'HLT_P3_v1p2', '92X_upgrade2017_realistic_v12'),
              ('W_P4_v2p1', '/WJetsToLNu_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer17DRStdmix-NZSFlatPU28to62_SUS01_92X_upgrade2017_realistic_v10-v1/AODSIM', 'HLT_P4_v2p1', '92X_upgrade2017_realistic_v12'),
              ('W_P5_v3p2', '/WJetsToLNu_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer17DRStdmix-NZSFlatPU28to62_SUS01_92X_upgrade2017_realistic_v10-v1/AODSIM', 'HLT_P5_v3p2', '92X_upgrade2017_realistic_v12'),
              ('W_P6_v4p0', '/WJetsToLNu_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer17DRStdmix-NZSFlatPU28to62_SUS01_92X_upgrade2017_realistic_v10-v1/AODSIM', 'HLT_P6_v4p0', '92X_upgrade2017_realistic_v12'),

              ('Z_P2_v1p1', '/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer17DRStdmix-NZSFlatPU28to62_92X_upgrade2017_realistic_v10_ext1-v1/AODSIM', 'HLT_P2_v1p1', '92X_upgrade2017_realistic_v12'),
              ('Z_P3_v1p2', '/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer17DRStdmix-NZSFlatPU28to62_92X_upgrade2017_realistic_v10_ext1-v1/AODSIM', 'HLT_P3_v1p2', '92X_upgrade2017_realistic_v12'),
              ('Z_P4_v2p1', '/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer17DRStdmix-NZSFlatPU28to62_92X_upgrade2017_realistic_v10_ext1-v1/AODSIM', 'HLT_P4_v2p1', '92X_upgrade2017_realistic_v12'),
              ('Z_P5_v3p2', '/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer17DRStdmix-NZSFlatPU28to62_92X_upgrade2017_realistic_v10_ext1-v1/AODSIM', 'HLT_P5_v3p2', '92X_upgrade2017_realistic_v12'),
              ('Z_P6_v4p0', '/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer17DRStdmix-NZSFlatPU28to62_92X_upgrade2017_realistic_v10_ext1-v1/AODSIM', 'HLT_P6_v4p0', '92X_upgrade2017_realistic_v12'),

              ('ttbar_P2_v1p1', '/TT_TuneCUETP8M2T4_13TeV-powheg-pythia8/RunIISummer17DRStdmix-NZSFlatPU28to62_92X_upgrade2017_realistic_v10-v2/AODSIM', 'HLT_P2_v1p1', '92X_upgrade2017_realistic_v12'),
              ('ttbar_P3_v1p2', '/TT_TuneCUETP8M2T4_13TeV-powheg-pythia8/RunIISummer17DRStdmix-NZSFlatPU28to62_92X_upgrade2017_realistic_v10-v2/AODSIM', 'HLT_P3_v1p2', '92X_upgrade2017_realistic_v12'),
              ('ttbar_P4_v2p1', '/TT_TuneCUETP8M2T4_13TeV-powheg-pythia8/RunIISummer17DRStdmix-NZSFlatPU28to62_92X_upgrade2017_realistic_v10-v2/AODSIM', 'HLT_P4_v2p1', '92X_upgrade2017_realistic_v12'),
              ('ttbar_P5_v3p2', '/TT_TuneCUETP8M2T4_13TeV-powheg-pythia8/RunIISummer17DRStdmix-NZSFlatPU28to62_92X_upgrade2017_realistic_v10-v2/AODSIM', 'HLT_P5_v3p2', '92X_upgrade2017_realistic_v12'),
              ('ttbar_P6_v4p0', '/TT_TuneCUETP8M2T4_13TeV-powheg-pythia8/RunIISummer17DRStdmix-NZSFlatPU28to62_92X_upgrade2017_realistic_v10-v2/AODSIM', 'HLT_P6_v4p0', '92X_upgrade2017_realistic_v12'),


            ]


if 'Data' in sys.argv:
  Sample = DataSample
if 'MC' in sys.argv:
  Sample = MCSample

if __name__ == '__main__':  # and 'submit' in sys.argv:
  print "Date : ", now

  crab_cfg = '''
from WMCore.Configuration import Configuration
config = Configuration()

config.section_('General')
config.General.transferOutputs = True
config.General.transferLogs = False
config.General.workArea = 'crab_%(submitVersion)s_%(date)s'
config.General.requestName = '%(submitVersion)s_%(FullName)s_%(date)s'

config.section_('JobType')
config.JobType.psetName = 'run_crab.py'
config.JobType.pluginName = 'Analysis'
#config.JobType.disableAutomaticOutputCollection = True
config.JobType.numCores = 8
config.JobType.maxMemoryMB = 2500
config.JobType.maxJobRuntimeMin = 2500

config.section_('Data')
config.Data.outLFNDirBase = '%(mainOutputDir)s/'
config.Data.inputDataset = '%(datasetPath)s'
config.Data.publication = False
job_control
config.section_('User')

config.section_('Site')
config.Site.blacklist = ['T3_UK_London_RHUL','T3_UK_SGrid_Oxford','T3_US_Rice']
config.Site.storageSite = 'T2_KR_KNU'
#config.Site.storageSite = 'T3_KR_KISTI'
'''

  if 'Data' in sys.argv:
    crab_cfg = crab_cfg.replace('job_control','''
config.Data.splitting = 'LumiBased'
config.Data.unitsPerJob = 70 #100
config.Data.totalUnits = -1
config.Data.inputDBS = 'global'
config.Data.lumiMask = '%(json)s'
config.Data.runRange = '%(runRange)s'
''')

  if 'MC' in sys.argv:
    crab_cfg = crab_cfg.replace('job_control','''
config.Data.useParent = True
config.Data.splitting = 'EventAwareLumiBased'
#config.Data.splitting = 'FileBased'
config.Data.totalUnits = 500000
config.Data.unitsPerJob  = 5000
config.Data.inputDBS = 'global'
config.Data.allowNonValidInputDataset = True
''')


  justtesting = False
  if 'testing' in sys.argv:
    justtesting = True
  if 'submit' in sys.argv:
    if 'Data' in sys.argv:
      for PDataset in PrimaryDataset:
        print "\n\n", PDataset
        for name, runRange, dataset, GT, json in Sample:
          FullName = PDataset + '_' + name
          datasetPath = '/' + PDataset + dataset
          outputName = "muonNtuple_" + name + ".root"
          print "\n", datasetPath

          f = open('hltNtuples_cfg.py', 'r')
          fData = f.read()
          f.close()
          newData = fData.replace('GlobalTagReplace',GT)
          newData = newData.replace("OutputReplace",outputName)
          fc = open('run_crab.py','w')
          fc.write(newData)
          fc.close()

          open('crabConfig.py', 'wt').write(crab_cfg % locals())
          if not justtesting:
            os.system('crab submit -c crabConfig.py')

    if 'MC' in sys.argv:
      for name, dataset, configName, GT in Sample:
        FullName = name
        datasetPath = dataset
        outputName = "muonNtuple_" + name + ".root"
        print "\n", datasetPath

        f = open(configName+'.py', 'r')
        fData = f.read()
        f.close()
        newData = fData.replace('GlobalTagReplace',GT)
        newData = newData.replace("OutputReplace",outputName)
        fc = open('run_crab.py','w')
        fc.write(newData)
        fc.close()

        open('crabConfig.py', 'wt').write(crab_cfg % locals())
        if not justtesting:
          os.system('crab submit -c crabConfig.py')
