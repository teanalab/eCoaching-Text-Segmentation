/* -*- C++ -*- */

// Author: Alexander Kotov (kotov@wayne.edu), Wayne State University, 2014
// $Id: LabLDA.hpp,v 1.4 2014/06/22 02:30:48 fn6418 Exp $

#ifndef LABLDA_HPP_
#define LABLDA_HPP_

#include <ctime>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include "Utils.hpp"

#define ALPHA_DEF 0.01
#define BETA_DEF 0.1

class LabLDA {
public:
  LabLDA(unsigned int numClsTops);
  LabLDA(unsigned int numClsTops, float alpha, float beta);
  virtual ~LabLDA();
  // estimate the parameters of the model using Gibbs sampling
  void runSampler(const ClsSampCol* docCol, unsigned int numCls, unsigned int vocSize, unsigned int numIters,
                    unsigned int rndSeed=0, bool verbose=false);
  // compute word-topic-class distributions
  void computePhi();
  // save global topics for each tag into a file
  void storeClassTopics(FILE* fd, const IDStrMap& id2tag, const IDStrMap& id2term, unsigned int maxTerms=0, float probThresh=0);

protected:
  void setDefaults();
  // allocate memory and initialize training data
  void trainInit();
  // allocate memory and initialize class-specific topics
  void classTopicsInit();
  // deallocate memory for training data
  void delTrainData();
  // deallocate memory for word-tag-topic distributions
  void delModelData();
  // sample a topic for word n of document m during training
  unsigned int sampleTopic(unsigned int docID, unsigned int docCls, unsigned int docLen, unsigned int termID, unsigned int termInd);

protected:
  ////////////////////////////////////////////////////////////////////////////////////
  // model parameters
  ///////////////////////////////////////////////////////////////////////////////////
  // number of distinct classes
  unsigned int _C;
  // number of topics per class
  unsigned int _K;
  // number of documents
  unsigned int _M;
  // vocabulary size
  unsigned int _W;
  // hyper-parameters
  float _alpha, _beta;
  ////////////////////////////////////////////////////////////////////////////////////
  // training data
  ///////////////////////////////////////////////////////////////////////////////////
  // number of times each word has been labeled with different topics for each class (size W x C x K)
  unsigned int ***_wcz;
  // number of labels of different topics for each class (size C x K)
  unsigned int **_cz;
  // number of tokens in each document that have been labeled with different topics (size M x K)
  unsigned int **_dz;
  // topic assignments to tokens in documents (size M x N_m)
  unsigned int **_z;
  // probabilities of topics
  float *_pz;
  ////////////////////////////////////////////////////////////////////////////////////
  // model data
  ///////////////////////////////////////////////////////////////////////////////////
  // word-tag-topic distribution (size C x W x K)
  float ***_phi;
  // document-tag-topic distribution (size M x K)
  float **_theta;
};

#endif /* TAGLDA_HPP_ */
