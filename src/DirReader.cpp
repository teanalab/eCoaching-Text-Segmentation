/* -*- C++ -*- */

// Author: Alexander Kotov (Emory University) alex.s.kotov@gmail.com, 2011
// $Rev$

#include "DirReader.hpp"

DirReader::~DirReader() {
  while(!_dirs.empty()) {
    while((closedir(_dirs.top()) == -1) && (errno = EINTR));
    _dirs.pop();
  }
}

bool DirReader::pathExists(const char *path) {
  struct stat statbuf;
  return stat(path, &statbuf) != -1;
}

bool DirReader::fileExists(const char *file, const char *dir) {
  DIR* dirp = NULL;
  struct dirent *dirent;

  if(dir == NULL) {
    dirp = opendir(".");
  } else {
    if(!DirReader::isDir(dir)) {
      return false;
    } else {
      dirp = opendir(dir);
    }
  }

  while((dirent = readdir(dirp)) != NULL) {
    if(strcmp(dirent->d_name, file) == 0) {
      return true;
    }
  }

  return false;
}

bool DirReader::isDir(const char *path) {
  struct stat statbuf;
  if(stat(path, &statbuf) == -1) {
    return false;
  } else {
    return S_ISDIR(statbuf.st_mode);
  }
}

bool DirReader::isFile(const char *path) {
  struct stat statbuf;
  if(stat(path, &statbuf) == -1) {
    return false;
  } else {
    return S_ISREG(statbuf.st_mode);
  }
}

void DirReader::open() throw(Exception) {
  struct stat statbuf;

  if(stat(_path.c_str(), &statbuf) == -1) {
    throwException(std::string("DirReader::open()"), _path + std::string(" does not exist"));
  } else {
    if(!S_ISDIR(statbuf.st_mode)) {
      throwException(std::string("DirReader::open()"), _path + std::string(" is not a directory"));
    }
  }

  _rel_path = "";
  addDir(_path.c_str());
}

void DirReader::open(const std::string& path) throw(Exception) {
  _path = path;
  open();
}

void DirReader::addDir(const char* path) throw(Exception) {
  DIR* dir = opendir(path);
  if(dir == NULL) {
    if(errno == EACCES) {
      throwException(std::string("DirReader::open()"), "Access denied");
    } else {
      throwException(std::string("DirReader::open()"), "Can't open directory");
    }
  }
  else {
    _dirs.push(dir);
    if(chdir(path) == -1) {
      throwException(std::string("DirReader::addDir()"), "Can't change directory");
    } else {
      getcwd(_cwd, PATH_MAX);
    }
  }
}

bool DirReader::goToNext() {
  DIR* dir;
  std::string::size_type pos;
  while(!_dirs.empty()) {
    dir = _dirs.top();
    if((_dire = readdir(dir)) == NULL) {
      while((closedir(_dirs.top()) == -1) && (errno == EINTR));
      _dirs.pop();
      pos = _rel_path.rfind('/');
      if(pos != std::string::npos) {
        _rel_path = _rel_path.substr(0, pos);
      } else {
        _rel_path = "";
      }
      if(chdir("..") == -1) {
        return false;
      } else {
        getcwd(_cwd, PATH_MAX);
      }
    }
    else {
      if(_dire->d_type == DT_DIR) {
        if(strcmp(_dire->d_name, "..") != 0 &&
           strcmp(_dire->d_name, ".") != 0) {
          try {
            addDir(_dire->d_name);
            if(_rel_path.length() != 0)
              _rel_path = _rel_path + "/";
            _rel_path += std::string(_dire->d_name);
          } catch(std::exception &e) {
          }
        }
      }
      else if(_dire->d_type == DT_REG) {
        break;
      }
    }
  }
  return _dire != NULL;
}

std::string DirReader::getFileName() {
  return _dire->d_name;
}

std::string DirReader::getFullFilePath() {
  return std::string(_cwd) + "/" + _dire->d_name;
}

std::string DirReader::getRelFileName() {
  return (_rel_path.length() ? _rel_path + "/" + _dire->d_name : _dire->d_name);
}
