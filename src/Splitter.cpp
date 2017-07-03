// $Id: Splitter.cpp,v 1.2 2014/02/04 08:47:14 fn6418 Exp $

#include "Utils.hpp"

#define NUM_SPLITS_DEF 10

typedef struct SampCodes_ {
  SampCodes_() : codeOne(""), codeTwo("")
  { }

  SampCodes_(std::string& codeOne, std::string& codeTwo) : codeOne(codeOne), codeTwo(codeTwo)
  { }

  std::string codeOne;
  std::string codeTwo;
} SampCodes;

typedef std::map<std::string, SampCodes> SampCodeMap;
typedef SampCodeMap::iterator SampCodeMapIt;
typedef SampCodeMap::const_iterator cSampCodeMapIt;

void usage(const char* progName) {
  printf("Usage: %s arg_file\n", progName);
  printf("Arguments:\n");
  printf("  samp_file - input file with sample IDs\n");
  printf("  train_dir - path to the directory with training splits\n");
  printf("  test_dir - path to the directory with test splits\n");
  printf("  num_splits - number of splits\n");
  printf("  stats_file - file with statistics per split\n");
  printf("  rnd_seed - seed for random shuffling of sample IDs\n");
}

SampCodeMap* loadSampCodeMap(const char *filePath) throw(Exception) {
  bool inSample = false;
  std::string curLine, curSampID;
  std::string::size_type lpos, rpos;
  SampCodes* sampCodes = NULL;
  SampCodeMap* sampMap = NULL;

  std::ifstream fs(filePath);

  if(!fs.is_open()) {
    throwException(std::string("parsing sample file ") + filePath, "can't open file for reading");
  } else {
    sampMap = new SampCodeMap();
  }

  while(getline(fs, curLine)) {
    stripChars(curLine, SPACE_CHARS);

    if(!curLine.length()) {
      continue;
    }

    if(inSample) {
      if((lpos = curLine.find("<ID>")) != std::string::npos) {
        lpos += 4;
        if((rpos = curLine.find("</ID>", lpos)) != std::string::npos) {
          curSampID = curLine.substr(lpos, rpos-lpos);
        } else {
          throwException(std::string("parsing sample file ") + filePath, "missing matching </ID>");
        }
      } else if((lpos = curLine.find("<CODE1>")) != std::string::npos) {
        lpos += 7;
        if((rpos = curLine.find("</CODE1>", lpos)) != std::string::npos) {
          sampCodes->codeOne = curLine.substr(lpos, rpos-lpos);
        } else {
          throwException(std::string("parsing sample file ") + filePath, "missing matching </CODE1>");
        }
      } else if((lpos = curLine.find("<CODE2>")) != std::string::npos) {
        lpos += 7;
        if((rpos = curLine.find("</CODE2>", lpos)) != std::string::npos) {
          sampCodes->codeTwo = curLine.substr(lpos, rpos-lpos);
        } else {
          throwException(std::string("parsing sample file ") + filePath, "missing matching </CODE2>");
        }
      } else if(curLine.find("</SAMPLE>") != std::string::npos) {
        inSample = false;
        sampMap->insert(std::make_pair(curSampID, *sampCodes));
        delete sampCodes;
        sampCodes = NULL;
      }
    } else {
      if(curLine.find("<SAMPLE>") != std::string::npos) {
        inSample = true;
        sampCodes = new SampCodes();
      }
    }
  }

  fs.close();
  return sampMap;
}

StrVec* getSampVec(const SampCodeMap* codeMap) {
  StrVec* sampVec = new StrVec();

  for(cSampCodeMapIt cmIt = codeMap->begin(); cmIt != codeMap->end(); cmIt++) {
    sampVec->push_back(cmIt->first);
  }

  return sampVec;
}

StrVec* randShuffleStrVec(const StrVec& origVec, unsigned int rndSeed) {
  unsigned int curInd, rndInd;
  IDSetIt riIt;
  IDSet remInds;
  StrVec *rndVec = new StrVec();

  for(unsigned int rndInd = 0; rndInd < origVec.size(); rndInd++) {
    remInds.insert(rndInd);
  }

  if(rndSeed == 0) {
    srandom(time(NULL));
  } else {
    srandom(rndSeed);
  }

  while(remInds.size()) {
    rndInd = (unsigned int) (random() / (RAND_MAX + 1.0) * remInds.size());
    for(curInd = 0, riIt = remInds.begin(); riIt != remInds.end(); curInd++, riIt++) {
      if(curInd == rndInd) {
        rndVec->push_back(origVec.at(*riIt));
        remInds.erase(riIt);
        break;
      }
    }
  }

  return rndVec;
}

void storeSplitCodeStats(FILE* fd, unsigned int splitNum, const StrCntMap& trainCodeCnts, const StrCntMap& testCodeCnts) throw(Exception) {
  cStrCntMapIt ccIt;
  unsigned int totSamp;

  fprintf(fd, "---------------- Split %u ----------------\n", splitNum);

  totSamp = 0;
  for(ccIt = trainCodeCnts.begin(); ccIt != trainCodeCnts.end(); ccIt++) {
    totSamp += ccIt->second;
  }

  fprintf(fd, "Total number of training samples: %u\n", totSamp);
  for(ccIt = trainCodeCnts.begin(); ccIt != trainCodeCnts.end(); ccIt++) {
    fprintf(fd, "%s: %u (%.2f%%)\n", ccIt->first.c_str(), ccIt->second, ((float) ccIt->second)/totSamp * 100);
  }

  totSamp = 0;
  for(ccIt = testCodeCnts.begin(); ccIt != testCodeCnts.end(); ccIt++) {
    totSamp += ccIt->second;
  }

  fprintf(fd, "Total number of testing samples: %u\n", totSamp);
  for(ccIt = testCodeCnts.begin(); ccIt != testCodeCnts.end(); ccIt++) {
    fprintf(fd, "%s: %u (%.2f%%)\n", ccIt->first.c_str(), ccIt->second, ((float) ccIt->second)/totSamp * 100);
  }
}

void createSplits(const std::string& trainDir, const std::string& testDir, FILE* statFD, const StrVec& sampVec, const SampCodeMap& codeMap, unsigned int numSplits) {
  char chSplitNum[10];
  std::string fileName, shortCode;
  unsigned int splitBounds[numSplits];
  unsigned int sampInd, splitInd, curSplit, splitSize;
  FILE *trainFD = NULL, *testFD = NULL;

  splitSize = sampVec.size() / numSplits;
  for(curSplit = 1; curSplit <= numSplits; curSplit++) {
    if(curSplit != numSplits) {
      splitBounds[curSplit-1] = curSplit * splitSize;
    } else {
      splitBounds[numSplits-1] = sampVec.size();
    }
  }

  for(curSplit = 0; curSplit < numSplits; curSplit++) {
    StrCntMap trainCodeCnts, testCodeCnts;
    snprintf(chSplitNum, 10, "%u", curSplit+1);
    fileName = trainDir + "/" + "train-" + chSplitNum + ".txt";
    trainFD = fopen(fileName.c_str(), "w");
    if(trainFD == NULL) {
      fprintf(stderr, "Error: can't open file %s for writing\n", fileName.c_str());
      exit(1);
    }
    fileName = testDir + "/" + "test-" + chSplitNum + ".txt";
    testFD = fopen(fileName.c_str(), "w");
    if(testFD == NULL) {
      fprintf(stderr, "Error: can't open file %s for writing\n", fileName.c_str());
      exit(1);
    }
    splitInd = 0;
    for(sampInd = 0; sampInd < sampVec.size(); sampInd++) {
      if(sampInd == splitBounds[splitInd]) {
        splitInd++;
      }

      shortCode = codeMap.at(sampVec.at(sampInd)).codeOne;
      if(splitInd == curSplit) {
        testCodeCnts[shortCode]++;
        fprintf(testFD, "%s\t%s\n", sampVec.at(sampInd).c_str(), shortCode.c_str());
      } else {
        trainCodeCnts[shortCode]++;
        fprintf(trainFD, "%s\n", sampVec.at(sampInd).c_str());
      }
    }

    storeSplitCodeStats(statFD, curSplit+1, trainCodeCnts, testCodeCnts);

    fclose(trainFD);
    fclose(testFD);
  }
}

int main(int argc, char* argv[]) {
  FILE* statFD = NULL;
  std::string trainDir = "", testDir = "";
  unsigned int rndSeed = 0, numSplits = NUM_SPLITS_DEF;
  SampCodeMap *scodeMap = NULL;
  StrVec *sampVec, *rndSampVec = NULL;

  if(argc != 2) {
    usage(argv[0]);
    exit(1);
  }

  try {
    StrStrMapIt ssIt;
    StrStrMap* params = readConfFile(argv[1]);

    if((ssIt = params->find("train_dir")) != params->end()) {
      trainDir = ssIt->second;
    } else {
      fprintf(stderr, "Error: no parameter 'train_dir' in the configuration file!\n");
      exit(1);
    }

    if((ssIt = params->find("test_dir")) != params->end()) {
      testDir = ssIt->second;
    } else {
      fprintf(stderr, "Error: no parameter 'test_dir' in the configuration file!\n");
      exit(1);
    }

    if((ssIt = params->find("samp_file")) != params->end()) {
      scodeMap = loadSampCodeMap(ssIt->second.c_str());
    } else {
      fprintf(stderr, "Error: no parameter 'samp_file' in the configuration file!\n");
      exit(1);
    }

    if((ssIt = params->find("num_splits")) != params->end()) {
      numSplits = atoi(ssIt->second.c_str());
    }

    if((ssIt = params->find("stats_file")) != params->end()) {
      statFD = fopen(ssIt->second.c_str(), "w");
      if(statFD == NULL) {
        fprintf(stderr, "Error: can't open file %s for writing\n", ssIt->second.c_str());
        exit(1);
      }
    } else {
      fprintf(stderr, "Error: no parameter 'stats_file' in the configuration file!\n");
      exit(1);
    }

    if((ssIt = params->find("rnd_seed")) != params->end()) {
      rndSeed = atoi(ssIt->second.c_str());
    }

    delete params;
  } catch(Exception& e) {
    fprintf(stderr, "Error: %s!\n", e.what());
    exit(1);
  }

  sampVec = getSampVec(scodeMap);
  rndSampVec = randShuffleStrVec(*sampVec, rndSeed);
  delete sampVec;

  createSplits(trainDir, testDir, statFD, *rndSampVec, *scodeMap, numSplits);

  fclose(statFD);

  delete rndSampVec;
  delete scodeMap;
  return 0;
}



