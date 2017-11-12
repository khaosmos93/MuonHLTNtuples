#!/bin/bash

rootbq 'ReadNtuples_Rate_v3.C("v3p2","HLT_IsoMu27_v",false)' >&outputs/Rate_v3/v3p2_IsoMu27.txt&
rootbq 'ReadNtuples_Rate_v3.C("v3p2","HLT_Mu50_v",false)' >&outputs/Rate_v3/v3p2_Mu50.txt&
rootbq 'ReadNtuples_Rate_v3.C("v3p2","HLT_OldMu100_v",false)' >&outputs/Rate_v3/v3p2_OldMu100.txt&
rootbq 'ReadNtuples_Rate_v3.C("v3p2","HLT_TkMu100_v",false)' >&outputs/Rate_v3/v3p2_TkMu100.txt&
rootbq 'ReadNtuples_Rate_v3.C("v3p2","HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8_v",false)' >&outputs/Rate_v3/v3p2_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8.txt&

rootbq 'ReadNtuples_Rate_v3.C("v4p0","HLT_IsoMu27_v",false)' >&outputs/Rate_v3/v4p0_IsoMu27.txt&
rootbq 'ReadNtuples_Rate_v3.C("v4p0","HLT_Mu50_v",false)' >&outputs/Rate_v3/v4p0_Mu50.txt&
rootbq 'ReadNtuples_Rate_v3.C("v4p0","HLT_OldMu100_v",false)' >&outputs/Rate_v3/v4p0_OldMu100.txt&
rootbq 'ReadNtuples_Rate_v3.C("v4p0","HLT_TkMu100_v",false)' >&outputs/Rate_v3/v4p0_TkMu100.txt&
rootbq 'ReadNtuples_Rate_v3.C("v4p0","HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8_v",false)' >&outputs/Rate_v3/v4p0_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8.txt&
