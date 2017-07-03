// $Id: LTA.cpp,v 1.3 2014/06/18 01:16:15 fn6418 Exp $

// Author: Alexander Kotov (kotov@wayne.edu), Emory University, 2014

#include "LTA.hpp"

LTA::LTA() {
  setDefaults();
  _betaBkg = BETA_BKG_DEF; _betaCls = BETA_CLS_DEF; _gammaBkg = GAMMA_BKG_DEF; _gammaCls = GAMMA_CLS_DEF;
}

LTA::LTA(float betaBkg, float betaCls, float gammaBkg, float gammaCls) {
  setDefaults();
  _betaBkg = betaBkg; _betaCls = betaCls; _gammaBkg = gammaBkg; _gammaCls = gammaCls;
}

void LTA::setDefaults() {
  _C = 0; _S = 0; _W = 0;
  _betaBkg = 0; _betaCls = 0; _gammaBkg = 0; _gammaCls = 0;
  _wc = NULL; _c = NULL; _sb = NULL; _sc = NULL; _wb = NULL; _b = 0; _Nd = NULL; _wt = NULL;
  _bphi = NULL; _cphi = NULL;
}

LTA::~LTA() {
  delTrainData();
  delLangModData();
}

void LTA::trainInit() {
  unsigned int w, c, d;

  _wc = new unsigned int*[_W];
  for(w = 0; w < _W; w++) {
    _wc[w] = new unsigned int[_C];
    for(c = 0; c < _C; c++) {
      _wc[w][c] = 0;
    }
  }

  _c = new unsigned int[_C];
  for(c = 0; c < _C; c++) {
    _c[c] = 0;
  }

  _sb = new unsigned int[_S];
  _sc = new unsigned int[_S];
  for(d = 0; d < _S; d++) {
    _sb[d] = 0;
    _sc[d] = 0;
  }

  _wb = new unsigned int[_W];
  for(w = 0; w < _W; w++) {
    _wb[w] = 0;
  }

  _Nd = new unsigned int[_S];
  _wt = new bool*[_S];
}

void LTA::langModInit() {
  unsigned int w, c;

  _bphi = new float[_W];
  for(w = 0; w < _W; w++) {
    _bphi[w] = 0;
  }

  _cphi = new float*[_C];
  for(c = 0; c < _C; c++) {
    _cphi[c] = new float[_W];
    for(w = 0; w < _W; w++) {
      _cphi[c][w] = 0;
    }
  }
}

void LTA::delTrainData() {
  if(_wc != NULL) {
    for(unsigned int w = 0; w < _W; w++) {
      delete[] _wc[w];
      _wc[w] = NULL;
    }
    delete[] _wc;
  }

  if(_c != NULL) {
    delete[] _c;
    _c = NULL;
  }

  if(_sb != NULL) {
    delete[] _sb;
    _sb = NULL;
  }

  if(_sc != NULL) {
    delete[] _sc;
    _sc = NULL;
  }

  if(_wb != NULL) {
    delete[] _wb;
    _wb = NULL;
  }

  if(_Nd != NULL) {
    delete[] _Nd;
    _Nd = NULL;
  }

  if(_wt != NULL) {
    for(unsigned int d = 0; d < _S; d++) {
      if(_wt[d] != NULL) {
        delete[] _wt[d];
        _wt[d] = NULL;
      }
    }
    delete[] _wt;
    _wt = NULL;
  }
}

void LTA::delLangModData() {
  if(_bphi != NULL) {
    delete[] _bphi;
    _bphi = NULL;
  }

  if(_cphi != NULL) {
    for(unsigned int c = 0; c < _C; c++) {
      if(_cphi[c] != NULL) {
        delete[] _cphi[c];
        _cphi[c] = NULL;
      }
    }
    delete[] _cphi;
    _cphi = NULL;
  }
}

void LTA::runSampler(const ClsSampCol* sampCol, unsigned int numCls, unsigned int vocSize, unsigned int numIters, unsigned int rndSeed, bool verbose) {
  bool termType;
  unsigned int curIter, numTerms, sampInd, sampCls, termInd, termID, termTypeInd;

  ClsSampCol& C = *((ClsSampCol* ) sampCol);

  _C = numCls;
  _W = vocSize;
  _S = C.size();

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
    _wt[sampInd] = new bool[C[sampInd].termIDs.size()];
    for(termInd = 0; termInd < C[sampInd].termIDs.size(); termInd++) {
      termID = C[sampInd].termIDs[termInd];
      termTypeInd = (int)(random() / (RAND_MAX + 1.0) * 2);
      if(termTypeInd == 0) {
        _wt[sampInd][termInd] = false;
        _wb[termID]++;
        _sb[sampInd]++;
        _b++;
      } else {
        _wt[sampInd][termInd] = true;
        _wc[termID][sampCls]++;
        _sc[sampInd]++;
        _c[sampCls]++;
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
    fprintf(stderr, "Number of samples: %u\n", _S);
    fprintf(stderr, "Number of terms in all samples: %u\n", numTerms);
    fprintf(stderr, "Number of unique terms in samples: %u\n", _W);
  }


  for(curIter = 0; curIter < numIters; curIter++) {
    if(verbose) {
      fprintf(stderr, "Gibbs sampler iteration %d of %d\n", curIter+1, numIters);
      // DEBUG
      unsigned int clsTerms = 0;
      for(unsigned int c = 0; c < _C; c++) {
        clsTerms += _c[c];
      }
      fprintf(stderr, "Background terms: %u Class-specific terms: %u Total terms: %u\n", _b, clsTerms, numTerms);
    }
    for(sampInd = 0; sampInd < C.size(); sampInd++) {
      sampCls = C[sampInd].sampCls;
      for(termInd = 0; termInd < C[sampInd].termIDs.size(); termInd++) {
        termID = C[sampInd].termIDs[termInd];
        sampleTermType(sampInd, sampCls, termInd, termID, termType);
        _wt[sampInd][termInd] = termType;
      }
    }
  }
}

void LTA::sampleTermType(unsigned int sampInd, unsigned int sampCls, unsigned int termInd, unsigned int termID, bool& termType) {
  float sp;
  bool prevTermType = _wt[sampInd][termInd];

  if(!prevTermType) {
    // background
    _wb[termID]--;
    _sb[sampInd]--;
    _b--;
  } else {
    // class-specific
    _wc[termID][sampCls]--;
    _sc[sampInd]--;
    _c[sampCls]--;
  }

  _pt[0] = (_sb[sampInd]+_gammaBkg)/(_Nd[sampInd]+_gammaBkg+_gammaCls-1)*(_wb[termID]+_betaBkg)/(_b+_W*_betaBkg);
  _pt[1] = (_sc[sampInd]+_gammaCls)/(_Nd[sampInd]+_gammaBkg+_gammaCls-1)*(_wc[termID][sampCls]+_betaCls)/(_c[sampCls]+_W*_betaCls);

  _pt[1] += _pt[0];

  sp = random() / (RAND_MAX + 1.0) * _pt[1];

  if(_pt[0] > sp) {
    termType = false;
    _wb[termID]++;
    _sb[sampInd]++;
    _b++;
  } else {
    termType = true;
    _wc[termID][sampCls]++;
    _sc[sampInd]++;
    _c[sampCls]++;
  }
}

void LTA::computePhi() {
  unsigned int w, c;

  langModInit();

  for(w = 0; w < _W; w++) {
    _bphi[w] = (_wb[w]+_betaBkg)/(_b+_W*_betaBkg);
  }

  for(c = 0; c < _C; c++) {
    for(w = 0; w < _W; w++) {
      _cphi[c][w] = (_wc[w][c]+_betaCls)/(_c[c]+_W*_betaCls);
      if(_cphi[c][w]-_bphi[w] < 0) {
        _cphi[c][w] = _betaCls/(_c[c]+_W*_betaCls);
      } else {
        _cphi[c][w]-=_bphi[w];
      }
    }
  }
}

void LTA::storeBkgLM(FILE* fd, IDStrMap& id2term) {
  TermWgtSetIt twIt;
  TermWgtSet sortTermWgts;

  for(unsigned int w = 0; w < _W; w++) {
    sortTermWgts.insert(TermWeight(id2term.at(w), _bphi[w]));
  }

  for(twIt = sortTermWgts.begin(); twIt != sortTermWgts.end(); twIt++) {
    fprintf(fd, "%s\t%.8f\n", twIt->term.c_str(), twIt->weight);
  }
}

void LTA::storeClassLMs(FILE* fd, IDStrMap& id2class, IDStrMap& id2term) {
  TermWgtSetIt twIt;
  unsigned int c, w;

  for(c = 0; c < _C; c++) {
    TermWgtSet sortTermWgts;

    fprintf(fd, "%s\n", id2class[c].c_str());
    for(w = 0; w < _W; w++) {
      sortTermWgts.insert(TermWeight(id2term.at(w), _cphi[c][w]));
    }

    for(twIt = sortTermWgts.begin(); twIt != sortTermWgts.end(); twIt++) {
      fprintf(fd, "%s\t%.8f\n", twIt->term.c_str(), twIt->weight);
    }
  }
}
