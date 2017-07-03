/* -*- C++ -*- */

// Author: Alexander Kotov (kotov@wayne.edu), Wayne State University, 2014
// $Id: ColMGLabLDA.cpp,v 1.3 2014/06/23 03:59:38 fn6418 Exp $

#include <cstdio>
#include <cstdlib>
#include "Utils.hpp"
#include "MGLabLDA.hpp"

void usage(const char* progName) {
  printf("Usage: %s arg_file\n", progName);
  printf("Arguments:\n");
  printf("  sids_path - path to the file with sample IDs\n");
  printf("  col_path - path to the file with classified sample collection\n");
  printf("  alpha - concentration hyper-parameter of Dirichlet prior for document-topic distributions\n");
  printf("  beta - concentration hyper-parameter of Dirichlet prior for language models\n");
  printf("  gamma_bkg - Beta prior for the background component in document background-class LMs mixture\n");
  printf("  gamma_cls - Beta prior for the class-specific component in document background-class LMs mixture\n");
  printf("  num_cls_tops - number of topics per class\n");
  printf("  num_iters - number of iterations\n");
  printf("  rnd_seed - seed for random number generator (initial state of the Markov chain)\n");
  printf("  bkg_top_file - file with background topic\n");
  printf("  cls_tops_file - file with class-specific topics\n");
  printf("  verbose - print debug information\n");
}

int main(int argc, char* argv[]) {
  std::string sidsPath, colPath;
  IDStrMap id2class, id2term;
  bool verbose = false;
  unsigned int numClsTops = 0, numIters = 100, rndSeed = 0;
  float alpha = ALPHA_DEF, beta = BETA_DEF, gammaBkg = GAMMA_BKG_DEF, gammaCls = GAMMA_CLS_DEF;
  FILE *bwtFD = stdout, *cwtFD = stdout;
  StrSet *sampIDs = NULL;
  ClsSampCol *sampCol = NULL;
  MGLabLDA *model = NULL;

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

    if((pit = params->find("alpha")) != params->end()) {
      alpha = atof(pit->second.c_str());
    }

    if((pit = params->find("beta")) != params->end()) {
      beta = atof(pit->second.c_str());
    }

    if((pit = params->find("gamma_bkg")) != params->end()) {
      gammaBkg = atof(pit->second.c_str());
    }

    if((pit = params->find("gamma_cls")) != params->end()) {
      gammaCls = atof(pit->second.c_str());
    }

    if((pit = params->find("num_cls_tops")) != params->end()) {
      numClsTops = atoi(pit->second.c_str());
    }

    if((pit = params->find("num_iters")) != params->end()) {
      numIters = atoi(pit->second.c_str());
    }

    if((pit = params->find("rnd_seed")) != params->end()) {
      rndSeed = atoi(pit->second.c_str());
    }

    if((pit = params->find("bkg_top_file")) != params->end()) {
      bwtFD = fopen(pit->second.c_str(), "w");
      if(bwtFD == NULL) {
        fprintf(stderr, "Error: can't open file %s for writing\n", pit->second.c_str());
        exit(1);
      }
    }

    if((pit = params->find("cls_tops_file")) != params->end()) {
      cwtFD = fopen(pit->second.c_str(), "w");
      if(cwtFD == NULL) {
        fprintf(stderr, "Error: can't open topics file %s for writing\n", pit->second.c_str());
        exit(1);
      }
    }

    if((pit = params->find("verbose")) != params->end()) {
      verbose = atoi(pit->second.c_str()) > 0 ? true : false;
    }

    if(verbose) {
      fprintf(stderr, "Loading sample IDs...");
    }

    sampIDs = loadOneColFile(sidsPath.c_str());

    if(verbose) {
      fprintf(stderr, "OK\nLoading sub-collection...");
    }

    sampCol = loadClsSampSubCol(colPath.c_str(), *sampIDs, id2class, id2term);
    delete sampIDs;

    // DEBUG
    /* printf("Tags:\n");
    printIdStrMap(id2tag);
    printf("Terms:\n");
    printIdStrMap(id2term);*/

    delete params;
  } catch (Exception& e) {
    fprintf(stderr, "Error: %s!\n", e.what());
    exit(1);
  }

  model = new MGLabLDA(numClsTops, alpha, beta, beta, gammaBkg, gammaCls);

  model->runSampler(sampCol, id2class.size(), id2term.size(), numIters, rndSeed, verbose);

  if(verbose) {
    fprintf(stderr, "Calculating model...");
  }

  model->computePhi();

  if(verbose) {
    fprintf(stderr, "OK\nStoring results...");
  }

  model->storeBkgTopic(bwtFD, id2term);
  model->storeClassTopics(cwtFD, id2class, id2term);

  if(verbose) {
    fprintf(stderr, "OK\n");
  }

  if(bwtFD != stdout) {
    fclose(bwtFD);
  }

  if(cwtFD != stdout) {
    fclose(cwtFD);
  }

  delete sampCol;
  delete model;

  return 0;
}
