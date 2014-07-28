//
//  ResourceLoader.h
//  JIndexer
//
//  Created by Hongzheng Shi on 3/5/14.
//  Copyright (c) 2014 Jeremy H. Shi. All rights reserved.
//
//********************************************************
// This is resource loader component: load one %d_data and
// corresponding index file into memory, return contents
//********************************************************
#ifndef __JIndexer__ResourceLoader__
#define __JIndexer__ResourceLoader_
#include <iostream>
#include <vector>
#include <dirent.h>
#include "Config.h"
#include "RawPage.h"
using namespace std;

typedef struct {
    std::string url; // max url lenth in IE
    int depth;
    int unknown;
    int length;
    char IP[18];
    int port;
    char status[8];
    
} IndexRecord;

typedef std::vector<IndexRecord *> IndexRecordVector;

class ResourceLoader{
private:
    vector<string> _files;
    int _curIndex;
    
    bool isdigit(char c);
    void clear(char*,int);
    char* readGzipFile(char*);
    char* getCharArrayFromString(string s);
    vector<RawPage> *getPagesFromIndexRecords(char* dataBuf, IndexRecordVector* indexArray);
    vector<IndexRecord*> *ParseIndex(char*);
public:
    ResourceLoader();
    ~ResourceLoader();
    /*************
    retrun: next page in a char array
    **************/
    vector<RawPage>* getPages();
};

#endif /* defined(__JIndexer__ResourceLoader__) */
