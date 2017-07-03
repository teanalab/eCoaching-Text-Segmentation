/* -*- C++ -*- */

// Author: Alexander Kotov (kotov@wayne.edu), Wayne State University, 2014
// $Id: MGLabLDA.hpp,v 1.3 2014/06/22 02:30:48 fn6418 Exp $


#ifndef MGLABLDA_HPP_
#define MGLABLDA_HPP_

#include <ctime>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include "Utils.hpp"

#define ALPHA_DEF 0.1
#define BETA_DEF 0.01
#define BETA_BKG_DEF 0.01
#define BETA_CLS_DEF 0.01
#define GAMMA_BKG_DEF 1
#define GAMMA_CLS_DEF 1

class MGLabLDA {
public:
  MGLabLDA(unsigned int numClsTops);
  MGLabLDA(unsigned int numClsTops, float alpha, float betaBkg, float betaCls, float gammaBkg, float gammaCls);
  virtual ~MGLabLDA();
  // estimate the parameters of the model using Gibbs sampling
  void runSampler(const ClsSampCol* docCol, unsigned int numCls, unsigned int vocSize, unsigned int numIters,
                    unsigned int rndSeed=0, bool verbose=false);
  // compute word-topic-class distributions
  void computePhi();
  void storeBkgTopic(FILE* fd, IDStrMap& id2term);
  void storeClassTopics(FILE* fd, IDStrMap& id2class, IDStrMap& id2term);

protected:
  // sample word type and topic for word n of document m
  void sampleHiddenVars(unsigned int sampID, unsigned int sampCls, unsigned int termID, unsigned int termInd, bool& isClsTop, unsigned int& topic);
  // set model parameters to default values
  void setDefaults();
  // allocate memory and initialize training data
  void trainInit();
  // allocate memory and initialize topic-tag-word distributions
  void topicsInit();
  // deallocate memory for training data
  void delTrainData();
  // deallocate memory for word-tag-topic distributions
  void delModelData();
protected:
  ////////////////////////////////////////////////////////////////////////////////////
  // model parameters
  ///////////////////////////////////////////////////////////////////////////////////
  // number of classes
  unsigned int _C;
  // number of location-specific topics
  unsigned int _Kcls;
  // number of documents
  unsigned int _M;
  // number of unique words in collection vocabulary
  unsigned int _W;
  // hyper-parameters
  float _alpha, _betaBkg, _betaCls, _gammaBkg, _gammaCls;
  ////////////////////////////////////////////////////////////////////////////////////
  // training data
  ///////////////////////////////////////////////////////////////////////////////////
  // number of times a word has been assigned to each local topic in each location (size C x W x Kcls)
  unsigned int ***_cwz;
  // number of words assigned to each local topic (size C x Kcls)
  unsigned int **_cz;
  // number of words in a document that have been assigned to each class topic (size M x Kcls)
  unsigned int **_dcz;
  // number of words in a document that have been assigned to background topic (size M)
  unsigned int *_db;
  // number of words in a document that have been assigned to class topics (size M)
  unsigned int *_dc;
  // number of times a word has been assigned to background topic (size W)
  unsigned int *_wb;
  // number of words assigned to background topic
  unsigned int _b;
  // total number of words in each document (size M)
  unsigned int *_Nd;
  // probability of assigning an observed word to background or local topics (Kcls + 1)
  float *_pz;
  // topic assignments of words in documents (size M x N_m)
  unsigned int **_z;
  // whether a word is assigned to class topic or not (size M x N_m)
  bool **_ct;
  ////////////////////////////////////////////////////////////////////////////////////
  // model data
  ///////////////////////////////////////////////////////////////////////////////////
  // background topic (size W)
  float *_bphi;
  // class topic (size C x Kcls x W)
  float ***_cphi;
};

#endif /* MGLABLDA_HPP_ */
