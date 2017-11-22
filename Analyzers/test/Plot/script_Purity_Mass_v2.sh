#!/bin/bash

#rm -rf outputs/Purity_Mass_v2/v20171120/
mkdir outputs/Purity_Mass_v2/v20171120/

rootbq 'ReadNtuples_Purity_Mass_v2.C("Period2","v1p1","HLT_IsoMu27_v",false)' >&outputs/Purity_Mass_v2/v20171120/Period2_v1p1_IsoMu27.txt&

rootbq 'ReadNtuples_Purity_Mass_v2.C("Period3","v1p2","HLT_IsoMu27_v",false)' >&outputs/Purity_Mass_v2/v20171120/Period3_v1p2_IsoMu27.txt&

rootbq 'ReadNtuples_Purity_Mass_v2.C("Period4","v2p0","HLT_IsoMu27_v",false)' >&outputs/Purity_Mass_v2/v20171120/Period4_v2p0_IsoMu27.txt&
rootbq 'ReadNtuples_Purity_Mass_v2.C("Period4","v2p1","HLT_IsoMu27_v",false)' >&outputs/Purity_Mass_v2/v20171120/Period4_v2p1_IsoMu27.txt&

rootbq 'ReadNtuples_Purity_Mass_v2.C("Period5","v2p1","HLT_IsoMu27_v",false)' >&outputs/Purity_Mass_v2/v20171120/Period5_v2p1_IsoMu27.txt&
rootbq 'ReadNtuples_Purity_Mass_v2.C("Period5","v2p2","HLT_IsoMu27_v",false)' >&outputs/Purity_Mass_v2/v20171120/Period5_v2p2_IsoMu27.txt&
rootbq 'ReadNtuples_Purity_Mass_v2.C("Period5","v3p0","HLT_IsoMu27_v",false)' >&outputs/Purity_Mass_v2/v20171120/Period5_v3p0_IsoMu27.txt&
rootbq 'ReadNtuples_Purity_Mass_v2.C("Period5","v3p1","HLT_IsoMu27_v",false)' >&outputs/Purity_Mass_v2/v20171120/Period5_v3p1_IsoMu27.txt&
rootbq 'ReadNtuples_Purity_Mass_v2.C("Period5","v3p2","HLT_IsoMu27_v",false)' >&outputs/Purity_Mass_v2/v20171120/Period5_v3p2_IsoMu27.txt&

rootbq 'ReadNtuples_Purity_Mass_v2.C("Period6","v4p0","HLT_IsoMu27_v",false)' >&outputs/Purity_Mass_v2/v20171120/Period6_v4p0_IsoMu27.txt&

echo "Finished @  `date`"
