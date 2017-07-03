// $Id: LDAClass.cpp,v 1.3 2014/06/23 03:59:38 fn6418 Exp $

// Copyright: Alexander Kotov (kotov@wayne.edu), Wayne State University, 2014

#include "Utils.hpp"

void usage(const char* progName) {
  printf("Usage: %s arg_file\n", progName);
  printf("Arguments:\n");
  printf("  sids_path - path to the file with sample IDs\n");
  printf("  col_path - path to the file with sample collection\n");
  printf("  col_lm_path - path to the file with background topic\n");
  printf("  cls_tops_path - path to the file with class-specific topics\n");
  printf("  out_file - file with classification results\n");
  printf("  verbose - print debug information\n");
}

ClsTopics* loadClsTopsFile(const char *filePath, StrSet** clsSet) throw(Exception) {
  unsigned int topID;
  std::string line, curCls;
  std::string::size_type lpos, rpos;
  ClsTopics* clsTopics = NULL;
  IDLangModMap* curTopMap = NULL;
  LangMod* curTopic = NULL;
  std::ifstream fs(filePath);

  if(!fs.is_open()) {
    throwException("parsing file with geo-specific topic-word distributions", std::string("can't open file ") + filePath);
  }

  clsTopics = new ClsTopics();
  *clsSet = new StrSet();

  while(std::getline(fs, line)) {
    StrVec toks;
    stripChars(line);

    if(!line.length()) {
      continue;
    }

    splitToks(line, toks, "\t");

    if(toks.size() == 1) {
      if((lpos = toks[0].find('<')) != std::string::npos) {
        if((rpos = toks[0].find('>')) != std::string::npos) {
          curCls = toks[0].substr(lpos+1, rpos-lpos-1);
          (*clsSet)->insert(curCls);
          clsTopics->insert(std::make_pair(curCls, IDLangModMap()));
          curTopMap = &((*clsTopics)[curCls]);
        } else {
          fprintf(stderr, "Warning: malformed geo-location\n");
        }
      } else {
        topID = atoi(toks[0].c_str());
        curTopMap->insert(std::make_pair(topID, LangMod()));
        curTopic = &((*curTopMap)[topID]);
      }
    } else if(toks.size() == 2) {
      curTopic->insert(std::make_pair(toks[0], atof(toks[1].c_str())));
    } else {
      fprintf(stderr, "Warning: malformed line (incorrect number of tokens)\n");
    }
  }

  fs.close();
  return clsTopics;
}

StrLangModMap* clsTopsToClsLMs(const ClsTopics* clsTops) {
  cLangModIt lmIt;
  cIDLangModMapIt tsIt;
  std::string curCls;
  LangMod* curClsLM = NULL;
  StrLangModMap* clsLMs = new StrLangModMap();

  for(cClsTopicsIt ctIt = clsTops->begin(); ctIt != clsTops->end(); ctIt++) {
    curCls = ctIt->first;
    clsLMs->insert(std::make_pair(curCls, LangMod()));
    curClsLM = &((*clsLMs))[curCls];
    for(tsIt = ctIt->second.begin(); tsIt != ctIt->second.end(); tsIt++) {
      for(lmIt = tsIt->second.begin(); lmIt != tsIt->second.end(); lmIt++) {
        (*curClsLM)[lmIt->first] += lmIt->second;
      }
    }
  }

  return clsLMs;
}

int main(int argc, char* argv[]) {
  bool verbose = false;
  std::string sidsPath, colPath, colLMPath, clsTopsPath;
  FILE *outFD = stdout;
  StrSet *sampIDs = NULL, *clsIDs = NULL;
  StrFloatMap *clsStats = NULL;
  SampCol *sampCol = NULL;
  LangMod *colLM = NULL;
  ClsTopics *clsTops = NULL;
  StrLangModMap *clsLMs = NULL, *termClsProbs = NULL;

  if(argc != 2) {
    usage(argv[0]);
    exit(1);
  }

  try {
    StrStrMapIt pit;

    StrStrMap* params = readConfFile(argv[1]);

    if((pit = params->find("sids_path")) != params->end()) {
      sidsPath = pit->second;
    } else {
      fprintf(stderr, "Error: path to the file with sample IDs is not in the configuration file (parameter sids_path)\n");
      exit(1);
    }

    if((pit = params->find("col_path")) != params->end()) {
      colPath = pit->second;
    } else {
      fprintf(stderr, "Error: path to the collection file is not in the configuration file (parameter col_path)\n");
      exit(1);
    }

    if((pit = params->find("col_lm_path")) != params->end()) {
      colLMPath = pit->second;
    } else {
      fprintf(stderr, "Error: path to the file with background topic is not in the configuration file (parameter bkg_top_path)\n");
      exit(1);
    }

    if((pit = params->find("cls_tops_path")) != params->end()) {
      clsTopsPath = pit->second;
    } else {
      fprintf(stderr, "Error: path to the file with class-specific topics is not in the configuration file (parameter cls_tops_path)\n");
      exit(1);
    }

    if((pit = params->find("out_file")) != params->end()) {
      outFD = fopen(pit->second.c_str(), "w");
      if(outFD == NULL) {
        fprintf(stderr, "Error: can't open file %s for writing\n", pit->second.c_str());
        exit(1);
      }
    }

    if((pit = params->find("verbose")) != params->end()) {
      verbose = (atoi(pit->second.c_str()) > 0);
    }

    if(verbose) {
      fprintf(stderr, "Loading sample IDs...");
    }

    sampIDs = loadTrainSampIDs(sidsPath.c_str(), &clsStats);

    if(verbose) {
      fprintf(stderr, "OK\nLoading sub-collection...");
    }

    sampCol = loadSampSubCol(colPath.c_str(), *sampIDs);
    delete sampIDs;

    if(verbose) {
      fprintf(stderr, "OK\nLoading topics...");
    }

    colLM = loadLangMod(colLMPath.c_str());
    clsTops = loadClsTopsFile(clsTopsPath.c_str(), &clsIDs);

    if(verbose) {
      fprintf(stderr, "OK\n");
    }

    delete params;
  } catch (Exception& e) {
    fprintf(stderr, "Error: %s!\n", e.what());
    exit(1);
  }

  clsLMs = clsTopsToClsLMs(clsTops);
  normClsStats(clsStats);
  termClsProbs = invertClsLMs(clsLMs, *clsStats, *colLM);
  delete clsLMs;

  normStrLangModMap(termClsProbs);
  clsSampCol(outFD, sampCol, termClsProbs, clsIDs, verbose);

  if(outFD != stdout) {
    fclose(outFD);
  }

  delete clsIDs;
  delete sampCol;
  delete termClsProbs;

  return 0;
}
