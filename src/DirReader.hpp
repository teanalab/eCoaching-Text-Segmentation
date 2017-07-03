/* -*- C++ -*- */

// Author: Alexander Kotov (Emory University) alex.s.kotov@gmail.com, 2011
// $Rev$

#ifndef DIRREADER_HPP_
#define DIRREADER_HPP_

#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <exception>
#include <string>
#include <stack>
#include "Exception.hpp"

#ifndef PATH_MAX
#define PATH_MAX 512
#endif

/* Class provides sequential access to the contents of a directory, including its sub-directories */

class DirReader {
public:
  DirReader()
  { }
  DirReader(const std::string& dirPath) : _path(dirPath)
  { }
  DirReader(const char* dirPath) : _path(dirPath)
  { }
  ~DirReader();
  // check if path is a directory
  static bool isDir(const char *path);
  // check if path is a regular file
  static bool isFile(const char *path);
  // check if path exists
  static bool pathExists(const char *path);
  // check if file exists in directory
  static bool fileExists(const char *file, const char *dir = NULL);
  // open directory for reading
  void open() throw(Exception);
  // open directory for reading
  void open(const std::string& path) throw(Exception);
  // get the name of the next file in the directory
  std::string getFileName();
  // get the name of the next file in the directory with the full path to it
  std::string getFullFilePath();
  // get the name of the next file with the path relative to the top directory
  std::string getRelFileName();
  // go to the next file in the directory
  bool goToNext();
  // get path to the directory
  inline std::string getPath() const {
    return _path;
  }
private:
  void addDir(const char* path) throw(Exception);
private:
  std::stack<DIR* > _dirs;
  std::string _rel_path;
  std::string _path;
  char _cwd[PATH_MAX];
  dirent *_dire;
};

#endif // DIRREADER_HPP_
