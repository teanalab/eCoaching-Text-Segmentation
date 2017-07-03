// $Id: LTAClass.cpp,v 1.5 2014/06/23 03:59:38 fn6418 Exp $

// Author: Alexander Kotov (kotov@wayne.edu), Wayne State University, 2014

#include "Utils.hpp"

void usage(const char* progName) {
  printf("Usage: %s arg_file\n", progName);
  printf("Arguments:\n");
  printf("  sids_path - path to the file with sample IDs\n");
  printf("  col_path - path to the file with sample collection\n");
  printf("  col_lm_path - path to the file with background LM\n");
  printf("  cls_lms_path - path to the file with class-specific LMs\n");
  printf("  out_file - file with classification results\n");
  printf("  verbose - print debug information\n");
}

int main(int argc, char* argv[]) {
  bool verbose = false;
  std::string sidsPath, colPath, colLMPath, clsLMsPath;
  FILE *outFD = stdout;
  StrSet *sampIDs = NULL, *clsIDs = NULL;
  StrFloatMap *clsStats = NULL;
  SampCol *sampCol = NULL;
  LangMod *colLM = NULL;
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
      fprintf(stderr, "Error: path to the file with background LM is not in the configuration file (parameter col_lm_path)\n");
      exit(1);
    }

    if((pit = params->find("cls_lms_path")) != params->end()) {
      clsLMsPath = pit->second;
    } else {
      fprintf(stderr, "Error: path to the file with class-specific LMs is not in the configuration file (parameter cls_lms_path)\n");
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
      fprintf(stderr, "OK\nLoading LMs...");
    }

    colLM = loadLangMod(colLMPath.c_str());
    clsLMs = loadClsLangMods(clsLMsPath.c_str(), &clsIDs);

    if(verbose) {
      fprintf(stderr, "OK\n");
    }

    delete params;
  } catch (Exception& e) {
    fprintf(stderr, "Error: %s!\n", e.what());
    exit(1);
  }

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
