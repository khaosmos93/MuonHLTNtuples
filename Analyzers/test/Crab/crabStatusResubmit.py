import sys, os
if __name__ == '__main__':

  dirs = [
            #('W_P2_v1p1','crab_PurityStudy2017_v1_20171124/crab_PurityStudy2017_v1_W_P2_v1p1_20171124'),
            #('W_P3_v1p2','crab_PurityStudy2017_v1_20171124/crab_PurityStudy2017_v1_W_P3_v1p2_20171124'),
            ('W_P4_v2p1','crab_PurityStudy2017_v1_20171124/crab_PurityStudy2017_v1_W_P4_v2p1_20171124'),
            ('W_P5_v3p2','crab_PurityStudy2017_v1_20171124/crab_PurityStudy2017_v1_W_P5_v3p2_20171124'),
            ('W_P6_v4p0','crab_PurityStudy2017_v1_20171124/crab_PurityStudy2017_v1_W_P6_v4p0_20171124'),

            #('Z_P2_v1p1','crab_PurityStudy2017_v1_20171124/crab_PurityStudy2017_v1_Z_P2_v1p1_20171124'),
            #('Z_P3_v1p2','crab_PurityStudy2017_v1_20171124/crab_PurityStudy2017_v1_Z_P3_v1p2_20171124'),
            ('Z_P4_v2p1','crab_PurityStudy2017_v1_20171124/crab_PurityStudy2017_v1_Z_P4_v2p1_20171124'),
            ('Z_P5_v3p2','crab_PurityStudy2017_v1_20171124/crab_PurityStudy2017_v1_Z_P5_v3p2_20171124'),
            ('Z_P6_v4p0','crab_PurityStudy2017_v1_20171124/crab_PurityStudy2017_v1_Z_P6_v4p0_20171124'),

            #('ttbar_P2_v1p1','crab_PurityStudy2017_v1_20171124/crab_PurityStudy2017_v1_ttbar_P2_v1p1_20171124'),
            #('ttbar_P3_v1p2','crab_PurityStudy2017_v1_20171124/crab_PurityStudy2017_v1_ttbar_P3_v1p2_20171124'),
            ('ttbar_P4_v2p1','crab_PurityStudy2017_v1_20171124/crab_PurityStudy2017_v1_ttbar_P4_v2p1_20171124'),
            ('ttbar_P5_v3p2','crab_PurityStudy2017_v1_20171124/crab_PurityStudy2017_v1_ttbar_P5_v3p2_20171124'),
            ('ttbar_P6_v4p0','crab_PurityStudy2017_v1_20171124/crab_PurityStudy2017_v1_ttbar_P6_v4p0_20171124'),


            ('Run2017Bv1_v1p1','crab_PurityStudy2017_v1_20171124/crab_PurityStudy2017_v1_SingleMuon_Run2017Bv1_v1p1_20171124'),

            ('Run2017Bv1_v1p2','crab_PurityStudy2017_v1_20171124/crab_PurityStudy2017_v1_SingleMuon_Run2017Bv1_v1p2_20171124'),
            ('Run2017Bv2_v1p2','crab_PurityStudy2017_v1_20171124/crab_PurityStudy2017_v1_SingleMuon_Run2017Bv2_v1p2_20171124'),

            ('Run2017Cv1_v2p0','crab_PurityStudy2017_v1_20171124/crab_PurityStudy2017_v1_SingleMuon_Run2017Cv1_v2p0_20171124'),

            ('Run2017Cv2_v2p1','crab_PurityStudy2017_v1_20171124/crab_PurityStudy2017_v1_SingleMuon_Run2017Cv2_v2p1_20171124'),
            ('Run2017Cv3_v2p1','crab_PurityStudy2017_v1_20171124/crab_PurityStudy2017_v1_SingleMuon_Run2017Cv3_v2p1_20171124'),

            ('Run2017Cv3_v2p2','crab_PurityStudy2017_v1_20171124/crab_PurityStudy2017_v1_SingleMuon_Run2017Cv3_v2p2_20171124'),

            ('Run2017Dv1_v3p0','crab_PurityStudy2017_v1_20171124/crab_PurityStudy2017_v1_SingleMuon_Run2017Dv1_v3p0_20171124'),

            ('Run2017Dv1_v3p1','crab_PurityStudy2017_v1_20171124/crab_PurityStudy2017_v1_SingleMuon_Run2017Dv1_v3p1_20171124'),
            ('Run2017Ev1_v3p1','crab_PurityStudy2017_v1_20171124/crab_PurityStudy2017_v1_SingleMuon_Run2017Ev1_v3p1_20171124'),
            ('Run2017Fv1_v3p1','crab_PurityStudy2017_v1_20171124/crab_PurityStudy2017_v1_SingleMuon_Run2017Fv1_v3p1_20171124'),

            ('Run2017Fv1_v3p2','crab_PurityStudy2017_v1_20171124/crab_PurityStudy2017_v1_SingleMuon_Run2017Fv1_v3p2_20171124'),

            ('Run2017Fv1_v4p0','crab_PurityStudy2017_v1_20171124/crab_PurityStudy2017_v1_SingleMuon_Run2017Fv1_v4p0_20171124'),

            ('Run2017Fv1_v4p1','crab_PurityStudy2017_v1_20171124/crab_PurityStudy2017_v1_SingleMuon_Run2017Fv1_v4p1_20171124'),

          ]


  for name, di in dirs:

    name = '|| -- '+name+' -- ||'
    if 'status' in sys.argv:
      command = 'crab status -d ' + di #+ ' --long'
    elif 'resubmit' in sys.argv:
      command = 'crab resubmit -d ' + di + ' --siteblacklist=T3_UK_London_RHUL,T3_UK_SGrid_Oxford,T3_US_Rice'
    elif 'getoutput' in sys.argv:
      command = 'crab getoutput -d ' + di + ' --quantity=all'
    elif 'kill' in sys.argv:
      command = 'crab kill -d ' + di

    print '\n',name
    os.system(command)
