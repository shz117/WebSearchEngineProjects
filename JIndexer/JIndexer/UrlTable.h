//
//  UrlTable.h
//  JIndexer
//
//  Created by Hongzheng Shi on 3/5/14.
//  Copyright (c) 2014 Jeremy H. Shi. All rights reserved.
//
//********************************************************
// This class defines URLtable structrue and its functions
//********************************************************

#ifndef __JIndexer__UrlTable__
#define __JIndexer__UrlTable__

#include <iostream>
#include <string>
#include <vector>
#include "Config.h"

// Header is docID(4 bytes) + fileID(2 bytes) + startIndex(4 bytes) + pageRank(2 bytes) + urlLen(2 bytes)
static const int uHeaderSize = 4 + 2 + 4 + 2 + 2;
static const int uBlockSize = 32768;
typedef int FILEMODE;
typedef struct{
    uint32_t docID; // doc id
    uint32_t fileID; // the number of data file containing this page
    uint32_t startIndex;   // pointer to the start point in the data file
    uint32_t dl;    //number of words
    std::string url; // url
} URLItem;

typedef std::vector<URLItem*> URLVector;

class URLTable
{
private:
    uint32_t _totalURL;
    URLVector *_urlTable;
public:
    URLTable();
    ~URLTable();
    void Add(URLItem* urlitem);
    void Write(FILEMODE mode);
};

#endif /* defined(__JIndexer__UrlTable__) */
