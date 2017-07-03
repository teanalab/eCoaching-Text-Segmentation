// $Id: LTA.hpp,v 1.2 2014/06/18 01:16:15 fn6418 Exp $

/*
 ******************************************************************************
 *      Latent Term Allocation Model with Background and Class-specific terms
 ******************************************************************************
 *  Author: Alexander Kotov (kotov@wayne.edu), Wayne State University, 2014
 */

#ifndef LTA_HPP_
#define LTA_HPP_

#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "Utils.hpp"

#define BETA_DEF 0.01
#define BETA_BKG_DEF 0.01
#define BETA_CLS_DEF 0.01
#define GAMMA_BKG_DEF 1
#define GAMMA_CLS_DEF 1

class LTA {
public:
  LTA();
  LTA(float betaBkg, float betaCls, float gammaBkg, float gammaCls);
  virtual ~LTA();
  void runSampler(const ClsSampCol* sampCol, unsigned int numCls, unsigned int vocSize, unsigned int numIters, unsigned int rndSeed, bool verbose);
  void computePhi();
  void storeBkgLM(FILE* fd, IDStrMap& id2term);
  void storeClassLMs(FILE* fd, IDStrMap& id2class, IDStrMap& id2term);

protected:
  // set default values
  void setDefaults();
  // allocate memory and initialize training data
  void trainInit();
  // allocate memory and initialize language models
  void langModInit();
  // deallocate memory for model training data
  void delTrainData();
  // deallocate memory for language model data
  void delLangModData();
  // sample a type for a word in a document
  void sampleTermType(unsigned int sampID, unsigned int sampCls, unsigned int termInd, unsigned int termID, bool& termType);
protected:
  ////////////////////////////////////////////////////////////////////////////////////
  // model parameters
  ///////////////////////////////////////////////////////////////////////////////////
  // number of classes
  unsigned int _C;
  // number of samples
  unsigned int _S;
  // number of unique words in collection vocabulary
  unsigned int _W;
  // hyper-parameters
  float _betaBkg, _betaCls, _gammaBkg, _gammaCls;
  ////////////////////////////////////////////////////////////////////////////////////
  // training data
  ///////////////////////////////////////////////////////////////////////////////////
  // number of times a word has been assigned to each class (size W x C)
  unsigned int **_wc;
  // number of words assigned to each class (size C)
  unsigned int *_c;
  // number of background words in a sample (size S)
  unsigned int *_sb;
  // number of class words in a sample (size S)
  unsigned int *_sc;
  // number of times a word has been labeled as background (size W)
  unsigned int *_wb;
  // number of words assigned to background topic
  unsigned int _b;
  // total number of words in each sample (size S)
  unsigned int *_Nd;
  // probability of labeling a given word as background or class-specific
  float _pt[2];
  // labels of words in documents to background or class-specific: false=background true=class-specific (size S x N_m)
  bool **_wt;
  ////////////////////////////////////////////////////////////////////////////////////
  // model data
  ///////////////////////////////////////////////////////////////////////////////////
  // background language model (size W)
  float *_bphi;
  // class-specific language models (size C x W)
  float **_cphi;
};

#endif /* LTA_HPP_ */
