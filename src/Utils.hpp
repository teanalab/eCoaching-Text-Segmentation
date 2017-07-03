// $Id: Utils.hpp,v 1.4 2014/06/22 02:30:48 fn6418 Exp $

#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <fstream>
#include <stdexcept>
#include <utility>
#include <math.h>
#include <stdlib.h>
#include "Types.hpp"
#include "Exception.hpp"
#include "PorterStemmer.hpp"

#define SPACE_CHARS " \t\r\n"
#define PUNCT_CHARS ",.;:!?()-'[]`{}\"$"

void toLower(std::string& token);
void removeNonAlphaSpace(std::string& newToken, const std::string& oldToken);
void stripChars(std::string& token, const char *chars=SPACE_CHARS);
void rightStrip(std::string& token, const char *chars=SPACE_CHARS);
void leftStrip(std::string& token, const char *chars=SPACE_CHARS);
bool isAllAlpha(const std::string& token);
void stemWord(std::string& token, lemur::parse::PorterStemmer *stemmer);
void splitToks(const std::string& str, StrVec& toks, const char *seps=" ", const std::string::size_type lpos = 0,
               const std::string::size_type rpos = std::string::npos);
void sortByWeight(IDVec& ord, const MultDist& topic);
void parseFileName(const std::string& filePath, std::string& fileName);
StrSet* loadOneColFile(const char *filePath) throw(Exception);
StrSet* loadColFromFile(const char *filePath, unsigned int colNum) throw(Exception);
StrStrMap* loadTwoColMapFile(const char *filePath) throw(Exception);
StrStrMap* readConfFile(const char *filePath) throw(Exception);
ClsSampCol* loadClsSampSubCol(const char *filePath, const StrSet& sampIDs, IDStrMap& id2class, IDStrMap& id2term) throw(Exception);
SampCol* loadSampSubCol(const char *filePath, const StrSet& sampIDs) throw(Exception);
LangMod* loadLangMod(const char *filePath) throw(Exception);
StrLangModMap* loadClsLangMods(const char *filePath, StrSet** clsSet) throw(Exception);
StrSet* loadTrainSampIDs(const char *filePath, StrFloatMap** clsStats) throw(Exception);
void normClsStats(StrFloatMap* clsStats);
void normStrLangModMap(StrLangModMap* strLangMods);
StrLangModMap* invertClsLMs(const StrLangModMap* clsLMs, const StrFloatMap& clsStats, const LangMod& bkgLM);
void clsSampCol(FILE* outFD, const SampCol* sampCol, const StrLangModMap* termClsProbs, const StrSet* clsIDs, bool verbose);

#endif /* UTILS_HPP_ */
