#!/bin/bash


#######################################################################################################
#			MP-CTA-F
#######################################################################################################
cd gem5
mv src src_original_files
mv src_CTA_GPU src
echo "Building Gem-5 for CTA Scope for GPU system"
scons -sQ -j4 build/GCN3_X86/gem5.opt
cd ..
echo "Executing MP-cta-F"
gem5/build/GCN3_X86/gem5.opt --stats-file=stats_MP-cta-F_GPU_only_2048runs gem5/configs/example/apu_se.py --cpu-type=DerivO3CPU -n 3 -c gem5-resources/gpu/GPU_Litmus_test/MP/MP-cta-F/bin/MP_RelAcq_Wg_Scope >> temp
cd m5out
grep -inr "L1cache.passTest " stats_MP-cta-F_GPU_only_2048runs | awk '{print $2}' | awk '{ if ($1 != 0 ) print "Pass Test: "$1 }'
grep -inr "L1cache.failTest " stats_MP-cta-F_GPU_only_2048runs | awk '{print $2}' | awk '{ if ($1 != 0 ) print "Fail Test: "$1 }'
passTest=$(grep -inr "L1cache.passTest " stats_MP-cta-F_GPU_only_2048runs | awk '{print $2}' | awk '{ if ($1 != 0 ) print $1 }')
failTest=$(grep -inr "L1cache.failTest " stats_MP-cta-F_GPU_only_2048runs | awk '{print $2}' | awk '{ if ($1 != 0 ) print $1 }')
if [[ $failTest -gt 0 ]]; then
	echo "====================================================================="
	echo "		GPU-Only MP-cta-F Allowed				     "
	echo "====================================================================="
else 
	echo "====================================================================="
	echo "		GPU-Only MP-cta-F Disallowed				     "
	echo "====================================================================="
fi
cd ..
cd gem5
mv src src_CTA_GPU
mv src_original_files src
echo "Re-Build Gem-5 with Original Files"
scons -sQ -j4 build/GCN3_X86/gem5.opt
cd ..


















#########################################################################################################
#			MP1-CTA-F
#########################################################################################################
cd gem5
mv src src_original_files
mv src_CTA_Hetero src
cd configs/ruby/
sed -i "s/'8kB'/'16kB'/g" GPU_VIPER.py
cd ../../
echo "Building Gem-5 for CTA Scope for heterogeneous"
scons -sQ -j4 build/GCN3_X86/gem5.opt
cd ..
echo "Executing MP1-cta-F"
gem5/build/GCN3_X86/gem5.opt --stats-file=stats_MP1-cta-F_2048runs gem5/configs/example/apu_se.py --cpu-type=DerivO3CPU -n 16 -c gem5-resources/gpu/Heterogenous_Litmus_Test/MP/MP1-CTA-F/bin/MP_Ver1_Rel_Acq_GPU_Scope -o 4 | grep 'Disallowed\|Allowed'
	echo "================================================================================================================"
cd gem5
mv src src_CTA_Hetero
mv src_original_files src
cd configs/ruby/
sed -i "s/'16kB'/'8kB'/g" GPU_VIPER.py
cd ../../
echo "Re-Build Gem-5 with Original Files"
scons -sQ -j4 build/GCN3_X86/gem5.opt
cd ..
