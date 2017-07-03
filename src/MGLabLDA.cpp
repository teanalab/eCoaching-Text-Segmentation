/* -*- C++ -*- */

// Author: Alexander Kotov (kotov@wayne.edu), Wayne State University, 2014
// $Id: MGLabLDA.cpp,v 1.4 2014/06/23 20:44:00 fn6418 Exp $

#include "MGLabLDA.hpp"

MGLabLDA::MGLabLDA(unsigned int numClsTops) {
  setDefaults();
  _Kcls = numClsTops; _alpha = ALPHA_DEF; _betaBkg = BETA_BKG_DEF; _betaCls = BETA_CLS_DEF; _gammaBkg = GAMMA_BKG_DEF; _gammaCls = GAMMA_CLS_DEF;
}

MGLabLDA::MGLabLDA(unsigned int numClsTops, float alpha, float betaBkg, float betaCls, float gammaBkg, float gammaCls) {
  setDefaults();
  _Kcls = numClsTops; _alpha = alpha; _betaBkg = betaBkg; _betaCls = betaCls; _gammaBkg = gammaBkg; _gammaCls = gammaCls;
}

void MGLabLDA::setDefaults() {
  _C = 0; _Kcls = 0; _M = 0; _W = 0;
  _alpha = 0; _betaBkg = 0; _betaCls = 0; _gammaBkg = 0; _gammaCls = 0;
  _cwz = NULL; _cz = NULL; _dcz = NULL; _db = NULL; _dc = NULL;
  _wb = NULL; _b = 0; _Nd = NULL; _pz = NULL; _z = NULL; _ct = NULL;
  _bphi = NULL; _cphi = NULL;
}

MGLabLDA::~MGLabLDA() {
  delTrainData();
  delModelData();
}

void MGLabLDA::trainInit() {
  unsigned int c, d, w, z;

  _cwz = new unsigned int**[_C];
  _cz = new unsigned int*[_C];

  for(c = 0; c < _C; c++) {
    _cz[c] = new unsigned int[_Kcls];
    for(z = 0; z < _Kcls; z++) {
      _cz[c][z] = 0;
    }
    _cwz[c] = new unsigned int*[_W];
    for(w = 0; w < _W; w++) {
      _cwz[c][w] = new unsigned int[_Kcls];
      for(z = 0; z < _Kcls; z++) {
        _cwz[c][w][z] = 0;
      }
    }
  }

  _dcz = new unsigned int*[_M];

  for(d = 0; d < _M; d++) {
    _dcz[d] = new unsigned int[_Kcls];
    for(z = 0; z < _Kcls; z++) {
      _dcz[d][z] = 0;
    }
  }

  _db = new unsigned int[_M];
  _dc = new unsigned int[_M];
  for(d = 0; d < _M; d++) {
    _db[d] = 0;
    _dc[d] = 0;
  }

  _wb = new unsigned int[_W];
  for(w = 1; w < _W; w++) {
    _wb[w] = 0;
  }

  _Nd = new unsigned int[_M];
  _z = new unsigned int*[_M];

  _ct = new bool*[_M];

  _pz = new float[_Kcls + 1];
}

void MGLabLDA::topicsInit() {
  unsigned int c, w, z;

  _bphi = new float[_W];
  for(w = 0; w < _W; w++) {
    _bphi[w] = 0;
  }

  _cphi = new float**[_C];
  for(c = 0; c < _C; c++) {
    _cphi[c] = new float*[_Kcls];
    for(z = 0; z < _Kcls; z++) {
      _cphi[c][z] = new float[_W];
      for(w = 0; w < _W; w++) {
        _cphi[c][z][w] = 0;
      }
    }
  }
}

void MGLabLDA::delTrainData() {
  unsigned int c, d, w;

  if(_cwz != NULL) {
    for(c = 0; c < _C; c++) {
      if(_cwz[c] != NULL) {
        for(w = 0; w < _W; w++) {
          if(_cwz[c][w] != NULL) {
            delete[] _cwz[c][w];
          }
        }
        delete[] _cwz[c];
      }
    }
    delete[] _cwz;
    _cwz = NULL;
  }

  if(_cz != NULL) {
    for(c = 0; c < _C; c++) {
      if(_cz[c] != NULL) {
        delete[] _cz[c];
      }
    }
    delete[] _cz;
    _cz = NULL;
  }

  if(_dcz != NULL) {
    for(d = 0; d < _M; d++) {
      if(_dcz[d] != NULL) {
        delete[] _dcz[d];
      }
    }
    delete[] _dcz;
    _dcz = NULL;
  }

  if(_db != NULL) {
    delete[] _db;
    _db = NULL;
  }

  if(_dc != NULL) {
    delete[] _dc;
    _dc = NULL;
  }

  if(_wb != NULL) {
    delete[] _wb;
    _wb = NULL;
  }

  if(_Nd != NULL) {
    delete[] _Nd;
    _Nd = NULL;
  }

  if(_pz != NULL) {
    delete[] _pz;
    _pz = NULL;
  }

  if(_z != NULL) {
    for(d = 0; d < _M; d++) {
      if(_z[d] != NULL) {
        delete[] _z[d];
      }
    }
    delete[] _z;
    _z = NULL;
  }

  if(_ct != NULL) {
    for(d = 0; d < _M; d++) {
      if(_ct[d] != NULL) {
        delete[] _ct[d];
      }
    }
    delete[] _ct;
    _ct = NULL;
  }
}

void MGLabLDA::delModelData() {
  unsigned int c, z;

  if(_bphi != NULL) {
    delete[] _bphi;
    _bphi = NULL;
  }

  if(_cphi != NULL) {
    for(c = 0; c < _C; c++) {
      if(_cphi[c] != NULL) {
        for(z = 0; z < _Kcls; z++) {
          if(_cphi[c][z] != NULL) {
            delete[] _cphi[c][z];
          }
        }
        delete[] _cphi[c];
      }
    }
    delete[] _cphi;
    _cphi = NULL;
  }
}

void MGLabLDA::runSampler(const ClsSampCol* docCol, unsigned int numCls, unsigned int vocSize, unsigned int numIters, unsigned int rndSeed, bool verbose) {
  bool isClsTop;
  unsigned int sampCls, sampInd, numTerms, termInd, termID, curIter, topTypeInd, topic;
  ClsSampCol& C = *((ClsSampCol* ) docCol);

  _C = numCls;
  _W = vocSize;
  _M = C.size();

  if(verbose) {
    fprintf(stderr, "Initializing training data...");
  }

  trainInit();

  if(rndSeed == 0) {
    srandom(time(NULL));
  } else {
    srandom(rndSeed);
  }

  for(sampInd = 0; sampInd < C.size(); sampInd++) {
    sampCls = C[sampInd].sampCls;
    _Nd[sampInd] = C[sampInd].termIDs.size();
    _z[sampInd] = new unsigned int[C[sampInd].termIDs.size()];
    _ct[sampInd] = new bool[C[sampInd].termIDs.size()];
    for(termInd = 0; termInd < C[sampInd].termIDs.size(); termInd++) {
      termID = C[sampInd].termIDs[termInd];
      topTypeInd = (int)(random() / (RAND_MAX + 1.0) * 2);
      if(topTypeInd == 0) {
        _ct[sampInd][termInd] = false;
        _wb[termID]++;
        _db[sampInd]++;
        _b++;
      } else {
        _ct[sampInd][termInd] = true;
        topic = (int)(random() / (RAND_MAX + 1.0) * _Kcls);
        _z[sampInd][termInd] = topic;
        _cwz[sampCls][termID][topic]++;
        _cz[sampCls][topic]++;
        _dcz[sampInd][topic]++;
        _dc[sampInd]++;
      }
    }
  }

  if(verbose) {
    fprintf(stderr, "OK\n");

    numTerms = 0;
    for(sampInd = 0; sampInd < C.size(); sampInd++) {
      numTerms += C[sampInd].termIDs.size();
    }

    fprintf(stderr, "Number of classes: %u\n", _C);
    fprintf(stderr, "Number of documents: %u\n", _M);
    fprintf(stderr, "Number of terms in all samples: %u\n", numTerms);
    fprintf(stderr, "Number of unique terms in documents: %u\n", _W);
  }

  for(curIter = 0; curIter < numIters; curIter++) {
    if(verbose) {
      fprintf(stderr, "Training sampler iteration %d of %d\n", curIter+1, numIters);
      // DEBUG
      fprintf(stderr, "Background terms: %u Total terms: %u\n", _b, numTerms);
    }
    for(sampInd = 0; sampInd < C.size(); sampInd++) {
      sampCls = C[sampInd].sampCls;
      for(termInd = 0; termInd < C[sampInd].termIDs.size(); termInd++) {
        termID = C[sampInd].termIDs[termInd];
        sampleHiddenVars(sampInd, sampCls, termID, termInd, isClsTop, topic);
        _ct[sampInd][termInd] = isClsTop;
        _z[sampInd][termInd] = topic;
      }
    }
  }
}

void MGLabLDA::computePhi() {
  unsigned int c, w, z;

  topicsInit();

  for(w = 0; w < _W; w++) {
    _bphi[w] = (_wb[w]+_betaBkg)/(_b+_W*_betaBkg);
  }

  for(c = 0; c < _C; c++) {
    for(z = 0; z < _Kcls; z++) {
      for(w = 0; w < _W; w++) {
        _cphi[c][z][w] = (_cwz[c][w][z]+_betaCls)/(_cz[c][z]+_W*_betaCls);
      }
    }
  }
}

void MGLabLDA::storeBkgTopic(FILE* fd, IDStrMap& id2term) {
  TermWgtSetIt twIt;
  TermWgtSet sortTermProbs;

  for(unsigned int w = 1; w < _W; w++) {
    sortTermProbs.insert(TermWeight(id2term.at(w), _bphi[w]));
  }

  for(twIt = sortTermProbs.begin(); twIt != sortTermProbs.end(); twIt++) {
    fprintf(fd, "%s\t%.8f\n", twIt->term.c_str(), twIt->weight);
  }
}

void MGLabLDA::storeClassTopics(FILE* fd, IDStrMap& id2class, IDStrMap& id2term) {
  TermWgtSetIt twIt;
  unsigned int c, w, z, termCnt;

  for(c = 0; c < _C; c++) {
    fprintf(fd, "<%s>\n", id2class.at(c).c_str());
    for(z = 0; z < _Kcls; z++) {
      TermWgtSet sortTermProbs;

      for(w = 0; w < _W; w++) {
        sortTermProbs.insert(TermWeight(id2term.at(w), _cphi[c][z][w]));
      }

      if(sortTermProbs.size() > 0) {
        fprintf(fd, "%u\n", z);

        for(twIt = sortTermProbs.begin(), termCnt = 0; twIt != sortTermProbs.end(); twIt++, termCnt++) {
          fprintf(fd, "%s\t%.8f\n", twIt->term.c_str(), twIt->weight);
        }
      }
    }
  }
}

void MGLabLDA::sampleHiddenVars(unsigned int sampInd, unsigned int sampCls, unsigned int termID, unsigned int termInd, bool& isClsTop, unsigned int& topic) {
  float p;
  bool ct;
  unsigned int z;

  ct = _ct[sampInd][termInd];
  z = _z[sampInd][termInd];

  if(!ct) {
    _wb[termID]--;
    _db[sampInd]--;
    _b--;
  } else {
    _cwz[sampCls][termID][z]--;
    _cz[sampCls][z]--;
    _dcz[sampInd][z]--;
    _dc[sampInd]--;
  }

  _pz[0] = (_db[sampInd]+_gammaBkg)/(_Nd[sampInd]+_gammaBkg+_gammaCls-1)*(_wb[termID]+_betaBkg)/(_b+_W*_betaBkg);

  for(z = 0; z < _Kcls; z++) {
    _pz[z+1] = (_dc[sampInd]+_gammaCls)/(_Nd[sampInd]+_gammaBkg+_gammaCls-1)*(_dcz[sampInd][z]+_alpha)/(_dc[sampInd]+_Kcls*_alpha)*(_cwz[sampCls][termID][z]+_betaCls)/(_cz[sampCls][z]+_W*_betaCls);
  }

  for(z = 1; z < _Kcls + 1; z++) {
    _pz[z] += _pz[z - 1];
  }

  p = random() / (RAND_MAX + 1.0) * _pz[_Kcls];

  for(z = 0; z < _Kcls + 1; z++) {
    if(_pz[z] > p) {
      if(z == 0) {
        isClsTop = false;
        topic = 0;
        _wb[termID]++;
        _db[sampInd]++;
        _b++;
      } else {
        isClsTop = true;
        topic = z - 1;
        _cwz[sampCls][termID][topic]++;
        _cz[sampCls][topic]++;
        _dcz[sampInd][topic]++;
        _dc[sampInd]++;
      }
      break;
    }
  }
}
