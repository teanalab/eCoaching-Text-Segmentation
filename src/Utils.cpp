// $Id: Utils.cpp,v 1.5 2014/06/22 02:30:48 fn6418 Exp $

#include "Utils.hpp"

void toLower(std::string& token) {
  for(std::string::size_type pos = 0; pos < token.size(); pos++) {
    if(isupper(token[pos])) {
      token[pos] = tolower(token[pos]);
    }
  }
}

void removeNonAlphaSpace(std::string& newToken, const std::string& oldToken) {
  newToken = "";
  for(std::string::const_iterator sit = oldToken.begin(); sit != oldToken.end(); sit++) {
    if(isalpha(*sit) || isspace(*sit)) {
      newToken += *sit;
    } else {
      newToken += " ";
    }
  }
}

void stripChars(std::string& token, const char* chars) {
  rightStrip(token, chars);
  leftStrip(token, chars);
}

void rightStrip(std::string& token, const char* chars) {
  std::string::size_type pos = token.find_first_not_of(chars);
  token.erase(0, pos);
}

void leftStrip(std::string& token, const char* chars) {
  std::string::size_type pos = token.find_last_not_of(chars);
  token.erase(pos + 1);
}

bool isAllAlpha(const std::string& token) {
  for(std::string::const_iterator sit = token.begin(); sit != token.end(); sit++) {
    if(!isalpha(*sit)) {
      return false;
    }
  }

  return true;
}

void stemWord(std::string& token, lemur::parse::PorterStemmer *stemmer) {
  std::string::size_type pos;
  char *buf = new char[token.size()+1];

  if((pos = token.find("'")) != std::string::npos) {
    token = token.substr(0, pos);
  }

  memset((void *) buf, 0, sizeof(char)*(token.size()+1));
  token.copy(buf, token.size()+1);
  token = stemmer->stemWord(buf);
  delete[] buf;
}

void splitToks(const std::string& str, StrVec& toks, const char *seps, const std::string::size_type lpos,
               const std::string::size_type rpos) {
  std::string::size_type prev = lpos; // previous delimiter position
  std::string::size_type cur = lpos; // current position of the delimiter
  std::string::size_type rbound = rpos == std::string::npos ? str.length()-1 : rpos;

  if (lpos >= rpos || lpos >= str.length() || rbound >= str.length())
    return;

  while (1) {
    cur = str.find_first_of(seps, prev);
    if (cur == std::string::npos || cur > rbound) {
      toks.push_back(str.substr(prev, rbound-prev+1));
      break;
    } else {
      if (cur != prev) {
        toks.push_back(str.substr(prev, cur-prev));
      }
      prev = cur + 1;
    }
  }
}

void sortByWeight(IDVec& ord, const MultDist& topic) {
  TermIDWgtSet st;

  for(cMultDistIt tit = topic.begin(); tit != topic.end(); tit++) {
    st.insert(TermIDWeight(tit->first, tit->second));
  }

  for(TermIDWgtSetIt sit = st.begin(); sit != st.end(); sit++) {
    ord.push_back(sit->id);
  }
}

void parseFileName(const std::string& filePath, std::string& fileName) {
  std::string::size_type lpos, rpos;

  if((lpos = filePath.rfind("/")) == std::string::npos) {
    lpos = -1;
  }

  if((rpos = filePath.find(".", lpos+1)) != std::string::npos) {
    fileName = filePath.substr(lpos+1, rpos-lpos-1);
  } else {
    fileName = filePath;
  }
}

StrSet* loadOneColFile(const char *filePath) throw(Exception) {
  std::string line;
  StrSet* lines = NULL;
  std::ifstream fs(filePath);

  if(!fs.is_open()) {
    throw Exception(std::string("Utils::readOneColFile()"), std::string("can't open ") + filePath + " for reading");
  }

  lines = new StrSet();

  while(getline(fs, line)) {
    stripChars(line, SPACE_CHARS);

    if(!line.length()) {
      continue;
    }

    lines->insert(line);
  }

  fs.close();
  return lines;
}

StrSet* loadColFromFile(const char *filePath, unsigned int colNum) throw(Exception) {
  std::string curLine;
  StrSet* col = NULL;
  std::ifstream fs(filePath);

  if(!fs.is_open()) {
    throw Exception(std::string("Utils::readColFromFile()"), std::string("can't open ") + filePath + " for reading");
  }

  col = new StrSet();

  while(getline(fs, curLine)) {
    StrVec lineToks;
    stripChars(curLine, SPACE_CHARS);

    if(!curLine.length()) {
      continue;
    }

    splitToks(curLine, lineToks, "\t");

    if(colNum <= lineToks.size()) {
      col->insert(lineToks[colNum-1]);
    }
  }

  fs.close();
  return col;
}

StrStrMap* loadTwoColMapFile(const char *filePath) throw(Exception) {
  std::string curLine;
  StrStrMap* colMap = NULL;
  std::ifstream fs(filePath);

  if(!fs.is_open()) {
    throw Exception(std::string("Utils::loadTwoColMapFile"), std::string("can't open ") + filePath + " for reading");
  }

  colMap = new StrStrMap();

  while(getline(fs, curLine)) {
    StrVec lineToks;
    stripChars(curLine, SPACE_CHARS);

    if(!curLine.length()) {
      continue;
    }

    splitToks(curLine, lineToks, " \t");

    if(lineToks.size() == 2) {
      (*colMap)[lineToks[0]] = lineToks[1];
    } else {
      throw Exception(std::string("Utils::loadTwoColMapFile"), std::string("line '") + curLine + "' does not have two tokens");
    }
  }

  fs.close();
  return colMap;
}

StrStrMap* readConfFile(const char* filePath) throw(Exception) {
  std::string::size_type lpos, rpos;
  std::string line, param;
  bool is_val = false;
  StrStrMap *pm = NULL;
  std::ifstream fs(filePath);

  if(!fs.is_open()) {
    throw Exception(std::string("Utils::readConFile()"), std::string("can't open ") + filePath);
  }

  pm = new StrStrMap();

  while(getline(fs, line)) {
    stripChars(line, SPACE_CHARS);

    if(!line.length()) {
      continue;
    }

    if(!is_val) {
      // parameter name
      if((lpos = line.find('[')) != std::string::npos) {
        if((rpos = line.find(']', lpos+1)) != std::string::npos) {
          is_val = true;
          param = line.substr(lpos+1, rpos-lpos-1);
        } else {
          throw Exception(std::string("Utils::readConfFile()"), "missing closing ]");
        }
      }
    } else {
      // parameter value
      pm->insert(std::make_pair(param, line));
      param = "";
      is_val = false;
    }
  }

  fs.close();
  return pm;
}

ClsSampCol* loadClsSampSubCol(const char *filePath, const StrSet& sampIDs, IDStrMap& id2class, IDStrMap& id2term) throw(Exception) {
  bool inText = false, isGoodSamp = false;
  cStrSetIt ssIt;
  StrIDMapIt sidIt;
  StrIDMap class2id, term2id;
  std::string curLine, curClass, curSampID;
  std::string::size_type lpos, rpos;
  unsigned int termID, curClassID, curTermID;
  SampClsPair* curSamp = NULL;
  ClsSampCol* sampCol = NULL;

  std::ifstream fs(filePath);

  if(!fs.is_open()) {
    throwException(std::string("parsing sample file ") + filePath, "can't open file for reading");
  } else {
    sampCol = new ClsSampCol();
  }

  curClassID = 0; curTermID = 0;
  while(getline(fs, curLine)) {
    stripChars(curLine, SPACE_CHARS);

    if(!curLine.length()) {
      continue;
    }

    if(inText) {
      if(curLine.find("</TEXT>") != std::string::npos) {
        inText = false;
      } else {
        if(isGoodSamp) {
          StrVec toks;
          splitToks(curLine, toks);
          for(unsigned int i = 0; i < toks.size(); i++) {
            if((sidIt = term2id.find(toks[i])) != term2id.end()) {
              termID = sidIt->second;
            } else {
              termID = curTermID++;
              term2id[toks[i]] = termID;
              id2term[termID] = toks[i];
            }
            curSamp->termIDs.push_back(termID);
          }
        }
      }
    } else {
      if((lpos = curLine.find("<ID>")) != std::string::npos) {
        lpos += 4;
        if((rpos = curLine.find("</ID>", lpos)) != std::string::npos) {
          curSampID = curLine.substr(lpos, rpos-lpos);
          curSamp->sampID = atoi(curSampID.c_str());
          isGoodSamp = ((ssIt = sampIDs.find(curSampID)) != sampIDs.end());
        } else {
          throwException(std::string("parsing sample file ") + filePath, "missing matching </ID>");
        }
      } else if(isGoodSamp && ((lpos = curLine.find("<CODE1>")) != std::string::npos)) {
        lpos += 7;
        if((rpos = curLine.find("</CODE1>", lpos)) != std::string::npos) {
          curClass = curLine.substr(lpos, rpos-lpos);
          if((sidIt = class2id.find(curClass)) != class2id.end()) {
            curSamp->sampCls = sidIt->second;
          } else {
            curSamp->sampCls = curClassID++;
            class2id[curClass] = curSamp->sampCls;
            id2class[curSamp->sampCls] = curClass;
          }
        } else {
          throwException(std::string("parsing sample file ") + filePath, "missing matching </CODE1>");
        }
      } else if(curLine.find("<TEXT>") != std::string::npos) {
        inText = true;
      } else if(curLine.find("<SAMPLE>") != std::string::npos) {
        curSamp = new SampClsPair();
      } else if(curLine.find("</SAMPLE>") != std::string::npos) {
        if(isGoodSamp) {
          sampCol->push_back(*curSamp);
        }
        delete curSamp;
        curSamp = NULL;
      }
    }
  }

  fs.close();
  return sampCol;
}

SampCol* loadSampSubCol(const char *filePath, const StrSet& sampIDs) throw(Exception) {
  cStrSetIt ssIt;
  bool inText = false, isGoodSamp = false;
  std::string curLine, curSampID;
  std::string::size_type lpos, rpos;
  StrVec* curSamp = NULL;
  SampCol* sampCol = NULL;

  std::ifstream fs(filePath);

  if(!fs.is_open()) {
    throwException(std::string("parsing sample file ") + filePath, "can't open file for reading");
  } else {
    sampCol = new SampCol();
  }

  while(getline(fs, curLine)) {
    stripChars(curLine, SPACE_CHARS);

    if(!curLine.length()) {
      continue;
    }

    if(inText) {
      if(curLine.find("</TEXT>") != std::string::npos) {
        inText = false;
      } else {
        if(isGoodSamp) {
          StrVec toks;
          splitToks(curLine, toks);
          for(unsigned int i = 0; i < toks.size(); i++) {
            curSamp->push_back(toks[i]);
          }
        }
      }
    } else {
      if((lpos = curLine.find("<ID>")) != std::string::npos) {
        lpos += 4;
        if((rpos = curLine.find("</ID>", lpos)) != std::string::npos) {
          curSampID = curLine.substr(lpos, rpos-lpos);
          isGoodSamp = ((ssIt = sampIDs.find(curSampID)) != sampIDs.end());
        } else {
          throwException(std::string("parsing sample file ") + filePath, "missing matching </ID>");
        }
      } else if(curLine.find("<TEXT>") != std::string::npos) {
        inText = true;
      } else if(curLine.find("<SAMPLE>") != std::string::npos) {
        curSamp = new StrVec();
      } else if(curLine.find("</SAMPLE>") != std::string::npos) {
        if(isGoodSamp) {
          sampCol->insert(std::make_pair(curSampID, *curSamp));
        }
        delete curSamp;
        curSamp = NULL;
      }
    }
  }

  fs.close();

  return sampCol;
}

LangMod* loadLangMod(const char *filePath) throw(Exception) {
  float termProb;
  std::string curLine, curTerm;
  LangMod *langMod = NULL;

  std::ifstream fs(filePath);

  if(!fs.is_open()) {
    throwException("loadLangMod", std::string("can't open file ") + filePath);
  }

  langMod = new LangMod();

  while(std::getline(fs, curLine)) {
    StrVec toks;
    stripChars(curLine, SPACE_CHARS);

    if(!curLine.length()) {
      continue;
    }

    splitToks(curLine, toks, "\t");

    if(toks.size() == 2) {
      termProb = atof(toks[1].c_str());
      curTerm = toks[0];
      langMod->insert(std::make_pair(curTerm, termProb));
    } else {
      fprintf(stderr, "Warning: malformed line '%s' (incorrect number of tokens)\n", curLine.c_str());
    }
  }

  fs.close();
  return langMod;
}

StrLangModMap* loadClsLangMods(const char *filePath, StrSet** clsSet) throw(Exception) {
  float termProb;
  std::string curLine, curCls, curTerm;
  LangMod *curLM = NULL;
  StrLangModMap *clsLMs = NULL;

  std::ifstream fs(filePath);

  if(!fs.is_open()) {
    throwException("loadClsLangMods", std::string("can't open file ") + filePath);
  }

  clsLMs = new StrLangModMap();
  *clsSet = new StrSet();

  while(std::getline(fs, curLine)) {
    StrVec toks;
    stripChars(curLine, SPACE_CHARS);

    if(!curLine.length()) {
      continue;
    }

    splitToks(curLine, toks, "\t");

    if(toks.size() == 1) {
      curCls = toks[0];
      (*clsSet)->insert(curCls);
      clsLMs->insert(std::make_pair(curCls, LangMod()));
      curLM = &((*clsLMs)[curCls]);
    } else if(toks.size() == 2) {
      termProb = atof(toks[1].c_str());
      curTerm = toks[0];
      curLM->insert(std::make_pair(curTerm, termProb));
    } else {
      fprintf(stderr, "Warning: malformed line '%s' (incorrect number of tokens)\n", curLine.c_str());
    }
  }

  fs.close();
  return clsLMs;
}

StrSet* loadTrainSampIDs(const char *filePath, StrFloatMap** clsStats) throw(Exception) {
  std::string curLine, curSampID, curCls;
  StrSet* sampIDs = NULL;
  std::ifstream fs(filePath);

  if(!fs.is_open()) {
    throw Exception(std::string("Utils::readColFromFile()"), std::string("can't open ") + filePath + " for reading");
  }

  sampIDs = new StrSet();
  *clsStats = new StrFloatMap();

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
      sampIDs->insert(curSampID);
      (**clsStats)[curCls] += 1;
    }
  }

  fs.close();
  return sampIDs;
}

void normClsStats(StrFloatMap* clsStats) {
  StrFloatMapIt csIt;
  float sumCnt = 0;

  for(csIt = clsStats->begin(); csIt != clsStats->end(); csIt++) {
    sumCnt += csIt->second;
  }

  for(csIt = clsStats->begin(); csIt != clsStats->end(); csIt++) {
    csIt->second /= sumCnt;
  }
}

void normStrLangModMap(StrLangModMap* strLangMods) {
  float sumProb;
  LangModIt lmIt;

  for(StrLangModMapIt slmIt = strLangMods->begin(); slmIt != strLangMods->end(); slmIt++) {
    // DEBUG
    // printf("%s\n", tcpIt->first.c_str());
    sumProb = 0;
    for(lmIt = slmIt->second.begin(); lmIt != slmIt->second.end(); lmIt++) {
      sumProb += lmIt->second;
    }

    for(lmIt = slmIt->second.begin(); lmIt != slmIt->second.end(); lmIt++) {
      // DEBUG
      // printf("   %s %f -> ", lmIt->first.c_str(), lmIt->second);
      lmIt->second /= sumProb;
      // DEBUG
      // printf("%s %f\n", lmIt->first.c_str(), lmIt->second);
    }
  }
}

StrLangModMap* invertClsLMs(const StrLangModMap* clsLMs, const StrFloatMap& clsStats, const LangMod& bkgLM) {
  cLangModIt lmIt;
  StrLangModMapIt tcpIt;
  cStrLangModMapIt clmIt;
  float termProb, clsProb;
  std::string curCls, curTerm;
  StrLangModMap* termClsProbs = new StrLangModMap();

  for(clmIt = clsLMs->begin(); clmIt != clsLMs->end(); clmIt++) {
    curCls = clmIt->first;
    for(lmIt = clmIt->second.begin(); lmIt != clmIt->second.end(); lmIt++) {
      curTerm = lmIt->first;
      termProb = lmIt->second;
      //clsProb = termProb * clsStats.at(curCls) / bkgLM.at(curTerm);
      // Added for avoiding divided by zero error
      if(clsStats.find(curCls) == clsStats.end()){
        clsProb = 0;
        //cout << curCls << " class not found" << endl;
      }
      else{
        clsProb = termProb * clsStats.at(curCls) / bkgLM.at(curTerm);
      }

      // DEBUG
      // printf("w=%s c=%s p(w|c)=%.4f p(c)=%.4f p(w)= %.4f p(c|w)=%.4f\n", curTerm.c_str(), curCls.c_str(), termProb, clsStats.at(curCls), bkgLM.at(curTerm), clsProb);
      if((tcpIt = termClsProbs->find(curTerm)) != termClsProbs->end()) {
        tcpIt->second[curCls] += clsProb;
      } else {
        LangMod clsProbs;
        clsProbs[curCls] = clsProb;
        termClsProbs->insert(std::make_pair(curTerm, clsProbs));
      }
    }
  }

  return termClsProbs;
}

void clsSampCol(FILE* outFD, const SampCol* sampCol, const StrLangModMap* termClsProbs, const StrSet* clsIDs, bool verbose) {
  cStrSetIt ssIt;
  cLangModIt lmIt;
  TermWgtMapIt twmIt;
  TermWgtSetIt twsIt;
  cStrLangModMapIt tcpIt;

  unsigned int termInd;
  std::string curClsID, curSampID, sampTerm;

  for(cSampColIt scIt = sampCol->begin(); scIt != sampCol->end(); scIt++) {
    TermWgtMap sampClsProbs;
    TermWgtSet sortClsProbs;
    curSampID = scIt->first;

    if(verbose) {
      std::string sampText = "";
      for(termInd = 0; termInd < scIt->second.size(); termInd++) {
        sampText += scIt->second.at(termInd) + " ";
      }
      printf("Classifying sample %s: %s\n", curSampID.c_str(), sampText.c_str());
    }

    for(ssIt = clsIDs->begin(); ssIt != clsIDs->end(); ssIt++) {
      sampClsProbs[*ssIt] = 0;
    }

    for(termInd = 0; termInd < scIt->second.size(); termInd++) {
      sampTerm = scIt->second.at(termInd);
      // DEBUG
      // printf("%s\n", sampTerm.c_str());
      if((tcpIt = termClsProbs->find(sampTerm)) != termClsProbs->end()) {
        for(lmIt = tcpIt->second.begin(); lmIt != tcpIt->second.end(); lmIt++) {
          sampClsProbs[lmIt->first] += log(lmIt->second);
          // DEBUG
          // printf("%s %.4f\n", lmIt->first.c_str(), lmIt->second);
        }
      }
    }

    for(twmIt = sampClsProbs.begin(); twmIt != sampClsProbs.end(); twmIt++) {
      sortClsProbs.insert(TermWeight(twmIt->first, twmIt->second));
    }

    if(verbose) {

      for(twsIt = sortClsProbs.begin(); twsIt != sortClsProbs.end(); twsIt++) {
        printf("%s  %f\n", twsIt->term.c_str(), twsIt->weight);
      }
    }

    twsIt = sortClsProbs.begin();
    if(twsIt->weight == 0) {
      fprintf(stderr, "Warning: sample %s does not have any class-specific terms\n", curSampID.c_str());
    }
    fprintf(outFD, "%s\t%s\n", curSampID.c_str(), twsIt->term.c_str());
  }
}

