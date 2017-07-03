/* -*- C++ -*- */

// Author: Alexander Kotov (alex.s.kotov@gmail.com), Emory University, 2013

#include "LabLDA.hpp"

LabLDA::LabLDA(unsigned int numClsTops) {
  setDefaults();
  _K = numClsTops; _alpha = ALPHA_DEF; _beta = BETA_DEF;
}

LabLDA::LabLDA(unsigned int numClsTops, float alpha, float beta) {
  setDefaults();
  _K = numClsTops; _alpha = alpha; _beta = beta;
}

LabLDA::~LabLDA() {
  delTrainData();
  delModelData();
}

void LabLDA::runSampler(const ClsSampCol* sampCol, unsigned int numCls, unsigned int vocSize, unsigned int numIters,
                            unsigned int rndSeed, bool verbose) {
  unsigned int *ind2tid = NULL;
  unsigned int sampInd, termInd, termID, curIter, topic;
  ClsSampCol& C = *((ClsSampCol* ) sampCol);

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
    _z[sampInd] = new unsigned int[C[sampInd].termIDs.size()];

    for(termInd = 0; termInd < C[sampInd].termIDs.size(); termInd++) {
      termID = C[sampInd].termIDs[termInd];
      topic = (int)(random() / (RAND_MAX + 1.0) * _K);
      _z[sampInd][termInd] = topic;
      _wcz[termID][C[sampInd].sampCls][topic]++;
      _cz[C[sampInd].sampCls][topic]++;
      _dz[sampInd][topic]++;
    }

    delete ind2tid;
  }

  if(verbose) {
    fprintf(stderr, "OK\n");
    fprintf(stderr, "Number of document tags: %u\n", _C);
    fprintf(stderr, "Number of documents: %u\n", _M);
    fprintf(stderr, "Number of unique terms in documents: %u\n", _W);
  }

  for(curIter = 0; curIter < numIters; curIter++) {
    if(verbose) {
      fprintf(stderr, "Training sampler iteration %d of %d\n", curIter+1, numIters);
    }
    for(sampInd = 0; sampInd < C.size(); sampInd++) {
      for(termInd = 0; termInd < C[sampInd].termIDs.size(); termInd++) {
        termID = C[sampInd].termIDs[termInd];
        topic = sampleTopic(sampInd, C[sampInd].sampCls, C[sampInd].termIDs.size(), termID, termInd);
        _z[sampInd][termInd] = topic;
      }
    }
  }
}

void LabLDA::computePhi() {
  unsigned int w, c, z;

  classTopicsInit();

  for(w = 0; w < _W; w++) {
    for(c = 0; c < _C; c++) {
      for(z = 0; z < _K; z++) {
        if(_wcz[w][c][z] > 0) {
          _phi[w][c][z] = (_wcz[w][c][z]+_beta)/(_cz[c][z]+_beta*_W);
        }
      }
    }
  }
}

void LabLDA::delTrainData() {
  unsigned int d, w, t;

  if(_wcz != NULL) {
    for(w = 0; w < _W; w++) {
      if(_wcz[w] != NULL) {
        for(t = 0; t < _C; t++) {
          if(_wcz[w][t] != NULL) {
            delete[] _wcz[w][t];
            _wcz[w][t] = NULL;
          }
        }
        delete[] _wcz[w];
        _wcz[w] = NULL;
      }
    }
    delete[] _wcz;
    _wcz = NULL;
  }

  if(_cz != NULL) {
    for(t = 0; t < _C; t++) {
      if(_cz[t] != NULL) {
        delete[] _cz[t];
        _cz[t] = NULL;
      }
    }
    delete[] _cz;
    _cz = NULL;
  }

  if(_dz != NULL) {
    for(d = 0; d < _M; d++) {
      if(_dz[d] != NULL) {
        delete[] _dz[d];
        _dz[d] = NULL;
      }
    }
    delete[] _dz;
    _dz = NULL;
  }

  if(_z != NULL) {
    for(d = 0; d < _M; d++) {
      if(_z[d] != NULL) {
        delete[] _z[d];
        _z[d] = NULL;
      }
    }
    delete[] _z;
    _z = NULL;
  }

  if(_pz != NULL) {
    delete[] _pz;
    _pz = NULL;
  }
}

void LabLDA::delModelData() {
  unsigned int d, w, t;

  if(_phi != NULL) {
    for(w = 0; w < _W; w++) {
      if(_phi[w] != NULL) {
        for(t = 0; t < _C; t++) {
          if(_phi[w][t] != NULL) {
            delete[] _phi[w][t];
            _phi[w][t] = NULL;
          }
        }
        delete[] _phi[w];
        _phi[w] = NULL;
      }
    }
    delete[] _phi;
    _phi = NULL;
  }

  if(_theta != NULL) {
    for(d = 0; d < _M; d++) {
      if(_theta[d] != NULL) {
        delete[] _theta[d];
        _theta[d] = NULL;
      }
    }

    delete[] _theta;
    _theta = NULL;
  }
}

void LabLDA::storeClassTopics(FILE* fd, const IDStrMap& id2tag, const IDStrMap& id2term, unsigned int maxTerms, float probThresh) {
  TermWgtSetIt twIt;
  unsigned int clsID, termID, termCnt, topic;

  for(clsID = 0; clsID < _C; clsID++) {
    fprintf(fd, "<%s>\n", id2tag.at(clsID).c_str());
    for(topic = 0; topic < _K; topic++) {
      TermWgtSet sortTermWgts;

      for(termID = 0; termID < _W; termID++) {
        if(_phi[termID][clsID][topic] >= probThresh) {
          sortTermWgts.insert(TermWeight(id2term.at(termID), _phi[termID][clsID][topic]));
        }
      }

      if(sortTermWgts.size() > 0) {
        fprintf(fd, "%u\n", topic);

        for(twIt = sortTermWgts.begin(), termCnt = 0; twIt != sortTermWgts.end(); twIt++, termCnt++) {
          if(maxTerms != 0 && termCnt >= maxTerms) {
            break;
          }
          fprintf(fd, "%s\t%.8f\n", twIt->term.c_str(), twIt->weight);
        }
      }
    }
  }
}

void LabLDA::setDefaults() {
  _C = 0; _K = 0; _M = 0; _W = 0;
  _alpha = 0; _beta = 0;
  _wcz = NULL; _cz = NULL; _dz = NULL; _z = NULL;
  _phi = NULL; _theta = NULL;
}

void LabLDA::trainInit() {
  unsigned int d, w, t, z;

  _wcz = new unsigned int**[_W];
  for(w = 0; w < _W; w++) {
    _wcz[w] = new unsigned int*[_C];
    for(t = 0; t < _C; t++) {
      _wcz[w][t] = new unsigned int[_K];
      for(z = 0; z < _K; z++) {
        _wcz[w][t][z] = 0;
      }
    }
  }

  _cz = new unsigned int*[_C];
  for(t = 0; t < _C; t++) {
    _cz[t] = new unsigned int[_K];
    for(z = 0; z < _K; z++) {
      _cz[t][z] = 0;
    }
  }

  _dz = new unsigned int*[_M];
  for(d = 0; d < _M; d++) {
    _dz[d] = new unsigned int[_K];
    for(z = 0; z < _K; z++) {
      _dz[d][z] = 0;
    }
  }

  _z = new unsigned int*[_M];
  _pz = new float[_K];
}

void LabLDA::classTopicsInit() {
  unsigned int w, t, z;

  _phi = new float**[_W];
  for(w = 0; w < _W; w++) {
    _phi[w] = new float*[_C];
    for(t = 0; t < _C; t++) {
      _phi[w][t] = new float[_K];
      for(z = 0; z < _K; z++) {
        _phi[w][t][z] = 0;
      }
    }
  }
}

unsigned int LabLDA::sampleTopic(unsigned int sampInd, unsigned int sampCls, unsigned int sampLen, unsigned int termID, unsigned int termInd) {
  float samProb;
  unsigned int z, topic;

  topic = _z[sampInd][termInd];

  _wcz[termID][sampCls][topic]--;
  _cz[sampCls][topic]--;
  _dz[sampInd][topic]--;

  for(z = 0; z < _K; z++) {
    _pz[z] = (_wcz[termID][sampCls][z]+_beta)/(_cz[sampCls][z]+_beta*_W)*(_dz[sampInd][z]+_alpha)/(sampLen+_K*_alpha-1);
  }

  for(z = 1; z < _K; z++) {
    _pz[z] += _pz[z-1];
  }

  samProb = random() / (RAND_MAX+1.0) * _pz[_K-1];

  for(z = 0; z < _K; z++) {
    if(_pz[z] > samProb) {
      topic = z;
      break;
    }
  }

  _wcz[termID][sampCls][topic]++;
  _cz[sampCls][topic]++;
  _dz[sampInd][topic]++;

  return topic;
}
