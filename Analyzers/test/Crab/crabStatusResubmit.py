import sys, os
if __name__ == '__main__':

  dirs = [
            ('Run2017Bv1_v1p1','crab_RateStudy2017_v3_SingleMuon_20171113/crab_RateStudy2017_v3_SingleMuon_Run2017Bv1_v1p1_20171113'),

            ('Run2017Bv1_v1p2','crab_RateStudy2017_v3_SingleMuon_20171113/crab_RateStudy2017_v3_SingleMuon_Run2017Bv1_v1p2_20171113'),
            ('Run2017Bv2_v1p2','crab_RateStudy2017_v3_SingleMuon_20171113/crab_RateStudy2017_v3_SingleMuon_Run2017Bv2_v1p2_20171113'),

            ('Run2017Cv1_v2p0','crab_RateStudy2017_v3_SingleMuon_20171113/crab_RateStudy2017_v3_SingleMuon_Run2017Cv1_v2p0_20171113'),
            #('Run2017Cv2_v2p0','crab_RateStudy2017_v3_SingleMuon_20171113/crab_RateStudy2017_v3_SingleMuon_Run2017Cv2_v2p0_20171113'),

            ('Run2017Cv2_v2p1','crab_RateStudy2017_v3_SingleMuon_20171113/crab_RateStudy2017_v3_SingleMuon_Run2017Cv2_v2p1_20171113'),
            ('Run2017Cv3_v2p1','crab_RateStudy2017_v3_SingleMuon_20171113/crab_RateStudy2017_v3_SingleMuon_Run2017Cv3_v2p1_20171113'),

            ('Run2017Cv3_v2p2','crab_RateStudy2017_v3_SingleMuon_20171113/crab_RateStudy2017_v3_SingleMuon_Run2017Cv3_v2p2_20171113'),

            ('Run2017Dv1_v3p0','crab_RateStudy2017_v3_SingleMuon_20171113/crab_RateStudy2017_v3_SingleMuon_Run2017Dv1_v3p0_20171113'),

            ('Run2017Dv1_v3p1','crab_RateStudy2017_v3_SingleMuon_20171113/crab_RateStudy2017_v3_SingleMuon_Run2017Dv1_v3p1_20171113'),
            ('Run2017Ev1_v3p1','crab_RateStudy2017_v3_SingleMuon_20171113/crab_RateStudy2017_v3_SingleMuon_Run2017Ev1_v3p1_20171113'),
            ('Run2017Fv1_v3p1','crab_RateStudy2017_v3_SingleMuon_20171113/crab_RateStudy2017_v3_SingleMuon_Run2017Fv1_v3p1_20171113'),

            ('Run2017Fv1_v3p2','crab_RateStudy2017_v3_SingleMuon_20171113/crab_RateStudy2017_v3_SingleMuon_Run2017Fv1_v3p2_20171113'),

            ('Run2017Fv1_v4p0','crab_RateStudy2017_v3_SingleMuon_20171113/crab_RateStudy2017_v3_SingleMuon_Run2017Fv1_v4p0_20171113'),

            ('Run2016Hv2','crab_RateStudy2017_v3_SingleMuon_20171113/crab_RateStudy2017_v3_SingleMuon_Run2016Hv2_20171113'),

          ]


  for name, di in dirs:

    name = '|| -- '+name+' -- ||'
    if 'status' in sys.argv:
      command = 'crab status -d ' + di #+ ' --long'
    elif 'resubmit' in sys.argv:
      command = 'crab resubmit -d ' + di
    elif 'getoutput' in sys.argv:
      command = 'crab getoutput -d ' + di + ' --quantity=all'
    elif 'kill' in sys.argv:
      command = 'crab kill -d ' + di

    print '\n',name
    os.system(command)
