// $Id: Eval.cpp,v 1.4 2014/06/23 20:44:00 fn6418 Exp $

// Author: Alexander Kotov (kotov@wayne.edu), Wayne State University, 2014

#include "Utils.hpp"
#include "DirReader.hpp"

typedef struct ClsStats_ {
  ClsStats_() : truePos(0), falsePos(0), falseNeg(0), totSamp(0)
  { }

  ClsStats_(unsigned int tp, unsigned int fp, unsigned int fn, unsigned int total) : truePos(tp), falsePos(fp), falseNeg(fn), totSamp(total)
  { }

  unsigned int truePos;
  unsigned int falsePos;
  unsigned int falseNeg;
  // total number of samples of a given class
  unsigned int totSamp;
} ClsStats;

typedef std::map<std::string, ClsStats> ClsStatsMap;
typedef ClsStatsMap::iterator ClsStatsMapIt;
typedef ClsStatsMap::const_iterator cClsStatsMapIt;

typedef struct PerfStats_ {
  PerfStats_() : goodSamp(0), totSamp(0), recall(0), precision(0), f1(0)
  { }

  unsigned int goodSamp;
  unsigned int totSamp;
  float recall;
  float precision;
  float f1;
} PerfStats;

typedef std::map<std::string, PerfStats> PerfStatsMap;
typedef PerfStatsMap::iterator PerfStatsMapIt;
typedef PerfStatsMap::const_iterator cPerfStatsMapIt;

typedef std::map<std::string, StrCntMap> ConfMat;
typedef ConfMat::iterator ConfMatIt;
typedef ConfMat::const_iterator cConfMatIt;

void usage(const char* progName) {
  printf("Usage: %s arg_file\n", progName);
  printf("Arguments:\n");
  printf("  test_splits_dir - path to the directory with test splits\n");
  printf("  cls_res_dir - path to the directory with classification results\n");
  printf("  perf_file - path to the file with the classification performance\n");
}

void loadGoldStanFile(const std::string filePath, StrStrMap& splitClsMap, ClsStatsMap& clsStats) {
  ClsStatsMapIt csmIt;
  std::string curLine, curSampID, curCls;

  std::ifstream fs(filePath.c_str());

  if(!fs.is_open()) {
    throw Exception(std::string("Eval::parseGoldStanFile()"), std::string("can't open ") + filePath + " for reading");
  }

  while(getline(fs, curLine)) {
    StrVec lineToks;
    stripChars(curLine, SPACE_CHARS);

    if(!curLine.length()) {
      continue;
    }

    splitToks(curLine, lineToks, "\t");

    if(lineToks.size() == 2) {
      curSampID = lineToks[0];
      curCls = lineToks[1];
      if((csmIt = clsStats.find(curCls)) == clsStats.end()) {
        clsStats.insert(std::make_pair(curCls, ClsStats()));
      }
      splitClsMap[curSampID] = curCls;
    }
  }

  fs.close();
}

void loadGoldStanDir(const std::string& gsDirPath, SplitClsMap& goldStan, ClsStatsMap& clsStats) throw(Exception) {
  DirReader gsDir;
  std::string filePath, fileName;

  gsDir.open(gsDirPath);
  while(gsDir.goToNext()) {
    StrStrMap splitClsMap;
    filePath = gsDir.getFullFilePath();
    parseFileName(filePath, fileName);
    loadGoldStanFile(filePath, splitClsMap, clsStats);
    goldStan.insert(std::make_pair(fileName, splitClsMap));
  }
}

void parseResFile(const std::string& resFilePath, const StrStrMap& splitGoldStan, ConfMat& confMat, ClsStatsMap& clsStats) {
  ClsStatsMapIt csIt;
  cStrStrMapIt gsIt;
  // true class -> actual class
  StrStrMap splitCls;
  std::string curLine, sampID, actCls, gsCls;

  std::ifstream fs(resFilePath.c_str());
  if(!fs.is_open()) {
    throw Exception(std::string("Eval::parseResFile()"), std::string("can't open ") + resFilePath + " for reading");
  }

  while(getline(fs, curLine)) {
    StrVec lineToks;
    stripChars(curLine, SPACE_CHARS);

    if(!curLine.length()) {
      continue;
    }

    splitToks(curLine, lineToks, "\t");

    if(lineToks.size() == 2) {
      sampID = lineToks[0];
      actCls = lineToks[1];
      if((csIt = clsStats.find(actCls)) == clsStats.end()) {
        fprintf(stderr, "Warning: ignoring unknown class '%s'\n", actCls.c_str());
        continue;
      }
      splitCls[sampID] = actCls;

      gsCls = splitGoldStan.at(sampID);
      clsStats[gsCls].totSamp++;

      if(!gsCls.compare(actCls)) {
        clsStats[gsCls].truePos++;
      } else {
        clsStats[gsCls].falseNeg++;
        clsStats[actCls].falsePos++;
      }
    }
  }

  for(gsIt = splitGoldStan.begin(); gsIt != splitGoldStan.end(); gsIt++) {
    sampID = gsIt->first;
    gsCls = gsIt->second;
    actCls = splitCls[sampID];
    confMat[gsCls][actCls]++;
  }

  fs.close();
}

void procResDir(const std::string& resDirPath, const SplitClsMap& goldStan, ConfMat& confMat, ClsStatsMap& clsStats) {
  DirReader resDir;
  cSplitClsMapIt gsIt;
  std::string filePath, fileName;

  resDir.open(resDirPath);
  while(resDir.goToNext()) {
    filePath = resDir.getFullFilePath();
    parseFileName(filePath, fileName);
    if((gsIt = goldStan.find(fileName)) != goldStan.end()) {
      parseResFile(filePath, gsIt->second, confMat, clsStats);
    } else {
      fprintf(stderr, "Warning: can't find golden standard for split '%s'\n", fileName.c_str());
    }
  }
}

void calcPerfStats(const ClsStatsMap& clsStats, PerfStatsMap& clsPerfStats, PerfStats& avgPerfStats) {
  std::string curCls;
  cClsStatsMapIt clsIt;

  for(clsIt = clsStats.begin(); clsIt != clsStats.end(); clsIt++) {
    PerfStats perfStats;
    avgPerfStats.goodSamp += clsIt->second.truePos;
    avgPerfStats.totSamp += clsIt->second.totSamp;
    perfStats.recall = ((float) clsIt->second.truePos) / (clsIt->second.truePos + clsIt->second.falseNeg);
    //perfStats.precision = ((float) clsIt->second.truePos) / (clsIt->second.truePos + clsIt->second.falsePos);
    if((clsIt->second.truePos + clsIt->second.falsePos) == 0){
        perfStats.precision = 0;
    }
    else{
        perfStats.precision = ((float) clsIt->second.truePos) / (clsIt->second.truePos + clsIt->second.falsePos);
    }
    //perfStats.f1 = perfStats.recall == 0 && perfStats.precision == 0 ? 0 : 2 * perfStats.recall * perfStats.precision / (perfStats.recall + perfStats.precision);
    if((clsIt->second.truePos + clsIt->second.falsePos) == 0){
        perfStats.f1 = 0;
    }
    else{
        perfStats.f1 = perfStats.recall == 0 && perfStats.precision == 0 ? 0 : 2 * perfStats.recall * perfStats.precision / (perfStats.recall + perfStats.precision);
    }
    clsPerfStats[clsIt->first] = perfStats;
  }

  for(PerfStatsMapIt prfIt = clsPerfStats.begin(); prfIt != clsPerfStats.end(); prfIt++) {
    curCls = prfIt->first;
    if((clsIt = clsStats.find(curCls)) != clsStats.end()) {
      avgPerfStats.recall += prfIt->second.recall * clsIt->second.totSamp / avgPerfStats.totSamp;
      avgPerfStats.precision += prfIt->second.precision * clsIt->second.totSamp / avgPerfStats.totSamp;
      avgPerfStats.f1 += prfIt->second.f1 * clsIt->second.totSamp / avgPerfStats.totSamp;
    }
  }
}

void storeConfMat(FILE* outFD, const ConfMat& confMat) {
  cStrCntMapIt scIt;

  fprintf(outFD, "Gold standard vs. actual class confusion matrix:\n");
  fprintf(outFD, "    ");
  for(cConfMatIt cmIt = confMat.begin(); cmIt != confMat.end(); cmIt++) {
    fprintf(outFD, " %3s", cmIt->first.c_str());
  }
  fprintf(outFD, "\n");

  for(cConfMatIt cmIt = confMat.begin(); cmIt != confMat.end(); cmIt++) {
    fprintf(outFD, " %3s", cmIt->first.c_str());
    for(scIt = cmIt->second.begin(); scIt != cmIt->second.end(); scIt++) {
      fprintf(outFD, " %3u", scIt->second);
    }
    fprintf(outFD, "\n");
  }
}

void storePerfStats(FILE* outFD, const PerfStatsMap& perfStats, PerfStats& avgStats) {
  unsigned int badSamp = avgStats.totSamp - avgStats.goodSamp;

  fprintf(outFD, "Total number of samples:                  \t%u\n", avgStats.totSamp);
  fprintf(outFD, "Number of correctly classified samples:   \t%u\t%.2f%%\n", avgStats.goodSamp, ((float) avgStats.goodSamp) / avgStats.totSamp * 100);
  fprintf(outFD, "Number of incorrectly classified samples: \t%u\t%.2f%%\n\n", badSamp, ((float) badSamp) / avgStats.totSamp * 100);

  fprintf(outFD, "Class\tPrecision\tRecall\tF1-Measure\n");
  for(cPerfStatsMapIt prfIt = perfStats.begin(); prfIt != perfStats.end(); prfIt++) {
    fprintf(outFD, "%s   \t%.4f  \t%.4f\t%.4f\n", prfIt->first.c_str(), prfIt->second.precision, prfIt->second.recall, prfIt->second.f1);
  }
  fprintf(outFD, "\nAvg.\t%.4f  \t%.4f\t%.4f\n", avgStats.precision, avgStats.recall, avgStats.f1);
}

int main(int argc, char* argv[]) {
  std::string testDirPath, resDirPath;
  SplitClsMap goldStan;
  ConfMat confMat;
  ClsStatsMap clsStats;
  PerfStatsMap perfStats;
  PerfStats avgStats;

  FILE* outFD = stdout;

  if(argc != 2) {
    usage(argv[0]);
    exit(1);
  }

  try {
    StrStrMapIt ssIt;
    StrStrMap* params = readConfFile(argv[1]);

    if((ssIt = params->find("test_splits_dir")) != params->end()) {
      testDirPath = ssIt->second;
    } else {
      fprintf(stderr, "Error: no parameter 'test_splits_dir' in the configuration file!\n");
      exit(1);
    }

    if((ssIt = params->find("cls_res_dir")) != params->end()) {
      resDirPath = ssIt->second;
    } else {
      fprintf(stderr, "Error: no parameter 'cls_res_dir' in the configuration file!\n");
      exit(1);
    }

    if((ssIt = params->find("perf_file")) != params->end()) {
      outFD = fopen(ssIt->second.c_str(), "w");
      if(outFD == NULL) {
        fprintf(stderr, "Error: can't open file %s for writing\n", ssIt->second.c_str());
        exit(1);
      }
    }

    loadGoldStanDir(testDirPath, goldStan, clsStats);
    for(ClsStatsMapIt csmIt1 = clsStats.begin(); csmIt1 != clsStats.end(); csmIt1++) {
      for(ClsStatsMapIt csmIt2 = clsStats.begin(); csmIt2 != clsStats.end(); csmIt2++) {
        confMat[csmIt1->first][csmIt2->first] = 0;
      }
    }
    procResDir(resDirPath, goldStan, confMat, clsStats);

    delete params;
  } catch(Exception& e) {
    fprintf(stderr, "Error parsing the configuration file: %s!\n", e.what());
    exit(1);
  }

  calcPerfStats(clsStats, perfStats, avgStats);
  storeConfMat(outFD, confMat);
  fprintf(outFD, "\n");
  storePerfStats(outFD, perfStats, avgStats);

  if(outFD != stdout) {
    fclose(outFD);
  }

  return 0;
}
