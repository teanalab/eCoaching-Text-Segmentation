#!/bin/bash
echo "Running LTA model..."
echo "Splitting data..."
./Splitter  ../config/stem/split.cfg
echo "Training LTA model..."
./LTA ../config/stem/lta-1.cfg
./LTA ../config/stem/lta-2.cfg
./LTA ../config/stem/lta-3.cfg
./LTA ../config/stem/lta-4.cfg
./LTA ../config/stem/lta-5.cfg
echo "Classifying test data with trained LTA model..."
./LTAClass ../config/stem/lta-cls-1.cfg
./LTAClass ../config/stem/lta-cls-2.cfg
./LTAClass ../config/stem/lta-cls-3.cfg
./LTAClass ../config/stem/lta-cls-4.cfg
./LTAClass ../config/stem/lta-cls-5.cfg
echo "Evaluating LTA model..."
./Eval ../config/stem/lta-eval.cfg
echo "Finished execution of LTA model"
