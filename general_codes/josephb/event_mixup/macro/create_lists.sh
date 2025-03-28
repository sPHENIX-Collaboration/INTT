#! /bin/bash

for i in {0..7}; do
	ls -1 /sphenix/lustre01/sphnxpro/production/run2auau/physics/ana464_nocdbtag_v001/DST_STREAMING_EVENT_INTT${i}/run_00054200_00054300/dst/*54280* > intt${i}.list
done
