# INTT parameters
root -q -b -l 'macro/ELPH/trend_over_runs.cc( "adc", 8, 0, 8, "")' &
root -q -b -l 'macro/ELPH/trend_over_runs.cc( "chip_id", 26, 1, 27, "")' &
root -q -b -l 'macro/ELPH/trend_over_runs.cc( "chip_id%13", 13, 1, 14, "")' &
root -q -b -l 'macro/ELPH/trend_over_runs.cc( "chan_id", 128, 0, 128, "")' &
root -q -b -l 'macro/ELPH/trend_over_runs.cc( "bco", 128, 0, 128, "")' &
root -q -b -l 'macro/ELPH/trend_over_runs.cc( "bco_full", 655, 0, 65536, "")' &
wait

# camac parameters
root -q -b -l 'macro/ELPH/trend_over_runs.cc( "camac_adc[0]", 400, 0, 400 )' &
root -q -b -l 'macro/ELPH/trend_over_runs.cc( "camac_adc[1]", 600, 0, 600 )' &
root -q -b -l 'macro/ELPH/trend_over_runs.cc( "camac_adc[2]", 600, 0, 600 )' &
root -q -b -l 'macro/ELPH/trend_over_runs.cc( "camac_adc[3]", 250, 0, 1000 )' &
#root -q -b -l 'macro/ELPH/trend_over_runs.cc( "camac_adc[4]", 2048, 0, 2048 )' &
root -q -b -l 'macro/ELPH/trend_over_runs.cc( "camac_tdc[0]", 200, 900, 1100 )' &
root -q -b -l 'macro/ELPH/trend_over_runs.cc( "camac_tdc[1]", 150, 200, 350 )' &
root -q -b -l 'macro/ELPH/trend_over_runs.cc( "camac_tdc[2]", 200, 300, 500 )' &
root -q -b -l 'macro/ELPH/trend_over_runs.cc( "camac_tdc[3]", 225, 400, 1300 )' &
root -q -b -l 'macro/ELPH/trend_over_runs.cc( "camac_tdc[4]", 200, 200, 400 )' &
#root -q -b -l 'macro/ELPH/trend_over_runs.cc( "camac_tdc[5]", 2048, 0, 2048 )' &

wait
