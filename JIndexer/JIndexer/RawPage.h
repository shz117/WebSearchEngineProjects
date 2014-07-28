//
//  RawPage.h
//  JIndexer
//
//  Created by Hongzheng Shi on 3/5/14.
//  Copyright (c) 2014 Jeremy H. Shi. All rights reserved.
//
//********************************************************
// This class encapsulates Raw Page data uncompressed from
// gziped files
//********************************************************


#ifndef __JIndexer__RawPage__
#define __JIndexer__RawPage__

#include <iostream>
#include <string>
using namespace std;
class RawPage{
private:
    char* _content;
public:
    
    //total number of tokens
    int d;
    int docId;
    int pageLenth;
    //url associated with this page
    string _url;
    
    RawPage(string url,char* content);
    ~RawPage();
    char* getContent();
};
#endif /* defined(__JIndexer__RawPage__) */
