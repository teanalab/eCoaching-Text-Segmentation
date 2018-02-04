#!/bin/bash
echo "Running MGLabLDA model..."
echo "Splitting data..."
./Splitter  ../config/stem/split.cfg
echo "Training MGLabLDA model..."
./MGLabLDA ../config/stem/mg-lda-1.cfg
./MGLabLDA ../config/stem/mg-lda-2.cfg
./MGLabLDA ../config/stem/mg-lda-3.cfg
./MGLabLDA ../config/stem/mg-lda-4.cfg
./MGLabLDA ../config/stem/mg-lda-5.cfg
echo "Classifying test data with trained MGLDA model..."
./LDAClass ../config/stem/mg-lda-cls-1.cfg
./LDAClass ../config/stem/mg-lda-cls-2.cfg
./LDAClass ../config/stem/mg-lda-cls-3.cfg
./LDAClass ../config/stem/mg-lda-cls-4.cfg
./LDAClass ../config/stem/mg-lda-cls-5.cfg
echo "Evaluating MGLabLDA model..."
./Eval ../config/stem/mg-lda-eval.cfg
echo "Finished execution of MGLabLDA model"
