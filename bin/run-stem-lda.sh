#!/bin/bash
echo "Running LDA model..."
echo "Splitting data..."
./Splitter  ../config/stem/split.cfg
echo "Training LDA model..."
./LabLDA ../config/stem/lda-1.cfg
./LabLDA ../config/stem/lda-2.cfg
./LabLDA ../config/stem/lda-3.cfg
./LabLDA ../config/stem/lda-4.cfg
./LabLDA ../config/stem/lda-5.cfg
echo "Classifying test data with trained LDA model..."
./LDAClass ../config/stem/lda-cls-1.cfg
./LDAClass ../config/stem/lda-cls-2.cfg
./LDAClass ../config/stem/lda-cls-3.cfg
./LDAClass ../config/stem/lda-cls-4.cfg
./LDAClass ../config/stem/lda-cls-5.cfg
echo "Evaluating LDA model..."
./Eval ../config/stem/lda-eval.cfg
echo "Finished execution of LDA model"
