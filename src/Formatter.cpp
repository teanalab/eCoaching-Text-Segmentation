// $Id: Formatter.cpp,v 1.6 2014/06/23 20:44:00 fn6418 Exp $

// Author: Alexander Kotov (kotov@wayne.edu), Wayne State University, 2013

#include "Utils.hpp"
#include "DirReader.hpp"


typedef struct SampInfo_ {
  SampInfo_() : shortCode(""), longCode("")
  { }

  SampInfo_(const std::string& shortCode, const std::string& longCode, const StrVec& sampTerms) : shortCode(shortCode), longCode(longCode), sampTerms(sampTerms)
  { }

  std::string shortCode;
  std::string longCode;
  StrVec sampTerms;
} SampInfo;

typedef std::vector<SampInfo> SampVec;
typedef SampVec::const_iterator cSampVecIt;

typedef struct TermStats_ {
  TermStats_() : term(""), sampFrac(0)
  { }

  TermStats_(const std::string& term, float sampFrac) : term(term), sampFrac(sampFrac)
  { }

  bool operator<(const TermStats_& termStats) const {
    return termStats.sampFrac <= sampFrac;
  }

  std::string term;
  float sampFrac;
} TermStats;

typedef std::set<TermStats> TermStatsSet;
typedef TermStatsSet::iterator TermStatsSetIt;

void usage(const char* progName) {
  printf("Usage: %s arg_file\n", progName);
  printf("Arguments:\n");
  printf("  in_dir - path to the input directory\n");
  printf("  code_map_file - file with mapping of classification codes\n");
  printf("  code_stats_file - file with statistics of classification codes\n");
  printf("  out_xml_file - path to the output file in XML format\n");
  printf("  out_arff_file - path to the output file in XML format\n");
  printf("  term_stats_file - path to the file with term statistics\n");
  printf("  max_samp_frac - maximum fraction of samples a term can occur\n");
  printf("  col_lm_file - path to the file with collection LM\n");
  printf("  remove_stopwords - remove stopwords\n");
  printf("  stop_words - path to the file with a list of stopwords\n");
  printf("  do_stemming - stem words with Porter stemmer\n");
}

void filePathToRelation(const std::string& codeMapFilePath, std::string& relation) {
  relation = "";
  for(std::string::const_iterator sit = codeMapFilePath.begin(); sit != codeMapFilePath.end(); sit++) {
    if(*sit == '/') {
      relation += "_";
    } else if(*sit == '.') {
      break;
    } else {
      relation += *sit;
    }
  }
}

void printARFFHeader(FILE* arffFD, const std::string& codeMapFilePath, const StrStrMap& codeMap) {
  unsigned int codeCnt;
  std::string relation;
  StrSetIt scIt;
  StrSet shortCodes;

  filePathToRelation(codeMapFilePath, relation);

  for(cStrStrMapIt smIt = codeMap.begin(); smIt != codeMap.end(); smIt++) {
    shortCodes.insert(smIt->second);
  }

  fprintf(arffFD, "@relation %s\n\n", relation.c_str());
  fprintf(arffFD, "@attribute text string\n");
  fprintf(arffFD, "@attribute @@class@@ {");

  for(scIt = shortCodes.begin(), codeCnt = 0; scIt != shortCodes.end(); scIt++, codeCnt++) {
    if(codeCnt == 0) {
      fprintf(arffFD, "%s", (*scIt).c_str());
    } else {
      fprintf(arffFD, ",%s", (*scIt).c_str());
    }
  }

  fprintf(arffFD, "}\n\n@data\n");
}

void storeCodeStats(const std::string& statsFilePath, const StrCntMap& codeCnts) throw(Exception) {
  cStrCntMapIt ccIt;
  unsigned int totSamp;
  FILE* fd = fopen(statsFilePath.c_str(), "w");

  if(fd == NULL) {
    throwException("storeCodeStats", std::string("can't open ") + statsFilePath + " for writing");
  }

  totSamp = 0;
  for(ccIt = codeCnts.begin(); ccIt != codeCnts.end(); ccIt++) {
    totSamp += ccIt->second;
  }

  fprintf(fd, "Total number of samples: %u\n", totSamp);
  for(ccIt = codeCnts.begin(); ccIt != codeCnts.end(); ccIt++) {
    fprintf(fd, "%s: %u (%.2f%%)\n", ccIt->first.c_str(), ccIt->second, ((float) ccIt->second)/totSamp * 100);
  }

  fclose(fd);
}

void storeColLangMod(const std::string& colLMFilePath, const StrCntMap& termSampCnts) throw(Exception) {
  cStrCntMapIt ccIt;
  unsigned int totTerms;
  FILE* fd = fopen(colLMFilePath.c_str(), "w");

  if(fd == NULL) {
    throwException("storeCodeStats", std::string("can't open ") + colLMFilePath + " for writing");
  }

  totTerms = 0;
  for(ccIt = termSampCnts.begin(); ccIt != termSampCnts.end(); ccIt++) {
    totTerms += ccIt->second;
  }

  for(ccIt = termSampCnts.begin(); ccIt != termSampCnts.end(); ccIt++) {
    fprintf(fd, "%s\t%.8f\n", ccIt->first.c_str(), ((float) ccIt->second)/totTerms);
  }

  fclose(fd);
}

void storeTermStats(const std::string& termStatsPath, const StrCntMap& termSampCnts, unsigned int numSamp) {
  float termSampFrac;
  TermStatsSet termStats;
  FILE* fd = fopen(termStatsPath.c_str(), "w");

  if(fd == NULL) {
    throwException("storeTermStats", std::string("can't open ") + termStatsPath + " for writing");
  }

  for(cStrCntMapIt tcIt = termSampCnts.begin(); tcIt != termSampCnts.end(); tcIt++) {
    termSampFrac = ((float) tcIt->second) / numSamp;
    termStats.insert(TermStats(tcIt->first, termSampFrac));
  }

  for(TermStatsSetIt tsIt = termStats.begin(); tsIt != termStats.end(); tsIt++) {
    fprintf(fd, "%s %.4f\n", tsIt->term.c_str(), tsIt->sampFrac);
  }

  fclose(fd);
}

bool isGoodTok(const std::string& token) {
  if(!token.compare("s") || !token.compare("ve") || !token.compare("m") || !token.compare("t") || !token.compare("re")) {
    return false;
  } else {
    return true;
  }
}

void findGoodTerms(const StrCntMap& termSampCnts, StrSet& goodTerms, unsigned int numSamp, float maxTermSampFrac) {
  float termSampFrac;

  for(cStrCntMapIt tcIt = termSampCnts.begin(); tcIt != termSampCnts.end(); tcIt++) {
    termSampFrac = ((float) tcIt->second) / numSamp;
    if(termSampFrac < maxTermSampFrac) {
      goodTerms.insert(tcIt->first);
    }
  }
}

void storeSampCol(FILE *xmlFD, FILE* arffFD, const SampVec& sampVec, const StrSet& goodTerms) {
  cStrSetIt ssIt;
  unsigned int termInd;
  std::string curSamp;
  unsigned int goodTermsCnt, curSampID = 1;

  for(cSampVecIt svIt = sampVec.begin(); svIt != sampVec.end(); svIt++) {
    goodTermsCnt = 0;
    for(termInd = 0; termInd < svIt->sampTerms.size(); termInd++) {
      if((ssIt = goodTerms.find(svIt->sampTerms.at(termInd))) != goodTerms.end()) {
        if(goodTermsCnt == 0) {
          curSamp = svIt->sampTerms.at(termInd);
        } else {
          curSamp += " " + svIt->sampTerms.at(termInd);
        }
        goodTermsCnt++;
      }
    }

    if(goodTermsCnt > 1) {
      fprintf(xmlFD, "<SAMPLE>\n<ID>%u</ID>\n<CODE1>%s</CODE1>\n<CODE2>%s</CODE2>\n<TEXT>\n%s\n</TEXT>\n</SAMPLE>\n", curSampID++, svIt->shortCode.c_str(), svIt->longCode.c_str(), curSamp.c_str());
      fprintf(arffFD, "'%s',%s\n", curSamp.c_str(), svIt->shortCode.c_str());
    }
  }
}

void procFile(const std::string& filePath, SampVec& sampVec, const std::string& shortCode, const std::string& longCode, lemur::parse::PorterStemmer *stemmer, const StrSet *stopWords, unsigned int& numSamps, StrCntMap& termSampCnts, StrCntMap& codeCnts) throw(Exception) {
  cStrSetIt ssIt;
  cStrStrMapIt cmIt;
  std::string curLine;
  unsigned int tokInd;
  std::ifstream inFS(filePath.c_str());

  if(!inFS.is_open()) {
    throwException("procFile", std::string("can't open ") + filePath + " for reading");
  }

  while(std::getline(inFS, curLine)) {
    StrVec lineToks;
    std::string normLine;

    removeNonAlphaSpace(normLine, curLine);
    stripChars(normLine, SPACE_CHARS);

    if(curLine.length() != 0) {
      StrVec sampTerms;
      StrSet sampUniqueTerms;

      splitToks(normLine, lineToks, " '");

      for(tokInd = 0; tokInd < lineToks.size(); tokInd++) {
        stripChars(lineToks[tokInd], SPACE_CHARS);

        if(lineToks[tokInd].length() != 0) {
          toLower(lineToks[tokInd]);

          if(isGoodTok(lineToks[tokInd])) {
            if(stopWords != NULL && (ssIt = stopWords->find(lineToks[tokInd])) != stopWords->end()) {
              continue;
            }

            if(stemmer != NULL) {
              stemWord(lineToks[tokInd], stemmer);
            }

            sampTerms.push_back(lineToks[tokInd]);
            sampUniqueTerms.insert(lineToks[tokInd]);
          }
        }
      }

      if(sampTerms.size() > 0) {
        numSamps++;
        codeCnts[shortCode]++;
        sampVec.push_back(SampInfo(shortCode, longCode, sampTerms));

        for(ssIt = sampUniqueTerms.begin(); ssIt != sampUniqueTerms.end(); ssIt++) {
          termSampCnts[*ssIt]++;
        }
      }
    }
  }

  inFS.close();
}

int main(int argc, char* argv[]) {
  unsigned int numSamps;
  float maxTermSampFrac = 1;
  std::string curCode, curFileName, colLMFilePath, arffFilePath, termStatsFilePath, clsStatsFilePath;
  DirReader inDir;
  SampVec sampVec;
  StrStrMapIt ssIt;
  StrSet goodTerms;
  StrCntMap termSampCnts, codeCnts;
  StrStrMap* codeMap = NULL;
  FILE *xmlFD = NULL, *arffFD = NULL;
  StrSet *stopWords = NULL;
  lemur::parse::PorterStemmer *stemmer = NULL;

  if(argc != 2) {
    usage(argv[0]);
    exit(1);
  }

  try {
    StrStrMap* params = readConfFile(argv[1]);

    if((ssIt = params->find("in_dir")) != params->end()) {
      inDir.open(ssIt->second);
    } else {
      fprintf(stderr, "Error: no parameter 'in_dir' in the configuration file!\n");
      exit(1);
    }

    if((ssIt = params->find("code_map_file")) != params->end()) {
      codeMap = loadTwoColMapFile(ssIt->second.c_str());
    } else {
      fprintf(stderr, "Error: no parameter 'code_map_file' in the configuration file!\n");
      exit(1);
    }

    if((ssIt = params->find("code_stats_file")) != params->end()) {
      clsStatsFilePath = ssIt->second;
    } else {
      fprintf(stderr, "Error: no parameter 'code_stats_file' in the configuration file!\n");
      exit(1);
    }

    if((ssIt = params->find("out_xml_file")) != params->end()) {
      xmlFD = fopen(ssIt->second.c_str(), "w");
      if(xmlFD == NULL) {
        fprintf(stderr, "Error: can't open file %s for writing\n", ssIt->second.c_str());
        exit(1);
      }
    } else {
      fprintf(stderr, "Error: no parameter 'out_xml_file' in the configuration file!\n");
      exit(1);
    }

    if((ssIt = params->find("out_arff_file")) != params->end()) {
      arffFilePath = ssIt->second;
      arffFD = fopen(arffFilePath.c_str(), "w");
      if(arffFD == NULL) {
        fprintf(stderr, "Error: can't open file %s for writing\n", ssIt->second.c_str());
        exit(1);
      }
    } else {
      fprintf(stderr, "Error: no parameter 'out_arff_file' in the configuration file!\n");
      exit(1);
    }

    if((ssIt = params->find("max_samp_frac")) != params->end()) {
      maxTermSampFrac = atof(ssIt->second.c_str());
    }

    if((ssIt = params->find("col_lm_file")) != params->end()) {
      colLMFilePath = ssIt->second;
    } else {
      fprintf(stderr, "Error: no parameter 'col_lm_file' in the configuration file!\n");
      exit(1);
    }

    if((ssIt = params->find("term_stats_file")) != params->end()) {
      termStatsFilePath = ssIt->second;
    } else {
      fprintf(stderr, "Error: no parameter 'term_stats_file' in the configuration file!\n");
      exit(1);
    }

    if((ssIt = params->find("remove_stopwords")) != params->end()) {
      if(atoi(ssIt->second.c_str()) > 0) {
        if((ssIt = params->find("stop_words")) != params->end()) {
          stopWords = loadOneColFile(ssIt->second.c_str());
        } else {
          fprintf(stderr, "Error: no parameter 'stop_words' in the configuration file!!\n");
          exit(1);
        }
      }
    }

    if((ssIt = params->find("do_stemming")) != params->end()) {
      if(atoi(ssIt->second.c_str()) > 0) {
        stemmer = new lemur::parse::PorterStemmer();
      }
    }

    delete params;
  } catch(Exception& e) {
    fprintf(stderr, "Error parsing the configuration file: %s!\n", e.what());
    exit(1);
  }

  numSamps = 0;
  try {
    while(inDir.goToNext()) {
      try {
        curFileName = inDir.getFileName();
        parseFileName(curFileName, curCode);
        if((ssIt = codeMap->find(curCode)) != codeMap->end()) {
          printf("Processing file for code %s\n", curCode.c_str());
          fflush(stdout);
          procFile(inDir.getFileName(), sampVec, ssIt->second, curCode, stemmer, stopWords, numSamps, termSampCnts, codeCnts);
        } else {
          printf("Error: unknown code %s!\n", curCode.c_str());
        }
      } catch(Exception& e) {
        fprintf(stderr, "Error: %s!\n", e.what());
      }
    }

    printARFFHeader(arffFD, arffFilePath, *codeMap);
    findGoodTerms(termSampCnts, goodTerms, numSamps, maxTermSampFrac);
    storeSampCol(xmlFD, arffFD, sampVec, goodTerms);
    storeTermStats(termStatsFilePath, termSampCnts, numSamps);
    storeColLangMod(colLMFilePath, termSampCnts);
    storeCodeStats(clsStatsFilePath, codeCnts);
  } catch(Exception& e) {
    fprintf(stderr, "Error: %s!\n", e.what());
    exit(1);
  }

  fclose(xmlFD);
  fclose(arffFD);

  if(stemmer != NULL) {
    delete stemmer;
  }

  if(stopWords != NULL) {
    delete stopWords;
  }

  return 0;
}
