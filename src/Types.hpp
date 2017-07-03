// $Id: Types.hpp,v 1.5 2014/06/22 02:30:48 fn6418 Exp $

#ifndef TYPES_HPP_
#define TYPES_HPP_

#include <set>
#include <string>
#include <vector>
#include <map>

typedef std::vector<std::string> StrVec;
typedef std::vector<StrVec> StrVecs;
typedef std::set<std::string> StrSet;
typedef StrSet::iterator StrSetIt;
typedef StrSet::const_iterator cStrSetIt;

typedef std::set<unsigned int> IDSet;
typedef IDSet::iterator IDSetIt;
typedef IDSet::const_iterator cIDSetIt;

typedef std::vector<unsigned int> IDVec;

typedef std::map<unsigned int, std::string> IDStrMap;
typedef IDStrMap::iterator IDStrMapIt;
typedef IDStrMap::const_iterator cIDStrMapIt;

typedef std::map<std::string, unsigned int> StrIDMap, StrCntMap;
typedef StrIDMap::iterator StrIDMapIt;
typedef StrCntMap::iterator StrCntMapIt;
typedef StrIDMap::const_iterator cStrIDMapIt;
typedef StrCntMap::const_iterator cStrCntMapIt;

typedef std::map<std::string, float> StrFloatMap;
typedef StrFloatMap::iterator StrFloatMapIt;
typedef StrFloatMap::iterator StrFloatMapIt;

typedef std::map<unsigned int, float> MultDist;
typedef MultDist::iterator MultDistIt;
typedef MultDist::const_iterator cMultDistIt;

typedef std::map<unsigned int, unsigned int> IDIDMap, CntMap;
typedef IDIDMap::iterator IDIDMapIt;
typedef IDIDMap::const_iterator cIDIDMapIt;
typedef CntMap::iterator CntMapIt;
typedef CntMap::const_iterator cCntMapIt;

typedef struct TermIDWeight_ {
  TermIDWeight_() : id(0), weight(0)
  { }

  TermIDWeight_(unsigned int id, float weight) : id(id), weight(weight)
  { }

  bool operator<(const TermIDWeight_& tidWgt) const {
    return tidWgt.weight <= weight;
  }

  unsigned int id;
  float weight;
} TermIDWeight;

typedef std::set<TermIDWeight> TermIDWgtSet;
typedef TermIDWgtSet::iterator TermIDWgtSetIt;
typedef TermIDWgtSet::const_iterator cTermIDWgtSetIt;

typedef std::map<std::string, std::string> StrStrMap;
typedef StrStrMap::iterator StrStrMapIt;
typedef StrStrMap::const_iterator cStrStrMapIt;

typedef struct SampClsPair_ {
  SampClsPair_() : sampID(0), sampCls(0)
  { }

  SampClsPair_(unsigned int sampID, unsigned int sampCls) : sampID(sampID), sampCls(sampCls)
  { }

  unsigned int sampID;
  unsigned int sampCls;
  IDVec termIDs;
} SampClsPair;

typedef std::vector<SampClsPair> ClsSampCol;
typedef ClsSampCol::iterator ClsSampColIt;
typedef ClsSampCol::const_iterator cClsSampColIt;

typedef std::map<std::string, StrVec> SampCol;
typedef SampCol::iterator SampColIt;
typedef SampCol::const_iterator cSampColIt;

typedef struct TermWeight_ {
  TermWeight_() : term(""), weight(0)
  { }

  TermWeight_(const std::string& str, float weight) : term(str), weight(weight)
  { }

  bool operator<(const TermWeight_& termWeight) const {
    return termWeight.weight <= weight;
  }

  std::string term;
  float weight;
} TermWeight;

typedef std::set<TermWeight> TermWgtSet;
typedef TermWgtSet::iterator TermWgtSetIt;
typedef TermWgtSet::const_iterator cTermWgtSetIt;

typedef std::map<std::string, float> LangMod, TermWgtMap;
typedef LangMod::iterator LangModIt;
typedef LangMod::const_iterator cLangModIt;
typedef TermWgtMap::iterator TermWgtMapIt;
typedef TermWgtMap::const_iterator cTermWgtMapIt;

typedef std::map<std::string, LangMod> StrLangModMap;
typedef StrLangModMap::iterator StrLangModMapIt;
typedef StrLangModMap::const_iterator cStrLangModMapIt;

typedef std::map<unsigned int, LangMod> IDLangModMap;
typedef IDLangModMap::iterator IDLangModMapIt;
typedef IDLangModMap::const_iterator cIDLangModMapIt;

typedef std::map<std::string, IDLangModMap> ClsTopics;
typedef ClsTopics::iterator ClsTopicsIt;
typedef ClsTopics::const_iterator cClsTopicsIt;

typedef std::map<std::string, StrStrMap> SplitClsMap;
typedef SplitClsMap::iterator SplitClsMapIt;
typedef SplitClsMap::const_iterator cSplitClsMapIt;

typedef std::map<unsigned int, unsigned int*> SemiSparseIntMat;
typedef SemiSparseIntMat::iterator SemiSparseIntMatIt;
typedef SemiSparseIntMat::const_iterator cSemiSparseIntMatIt;

typedef std::map<unsigned int, float*> SemiSparseFloatMat;
typedef SemiSparseFloatMat::iterator SemiSparseFloatMatIt;
typedef SemiSparseFloatMat::const_iterator cSemiSparseFloatMatIt;

#endif /* TYPES_HPP_ */
