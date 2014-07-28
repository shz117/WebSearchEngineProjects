//
//  LexiconTable.h
//  JIndexer
//
//  Created by Hongzheng Shi on 3/5/14.
//  Copyright (c) 2014 Jeremy H. Shi. All rights reserved.
//
//********************************************************
// This class defines LexiconTable structrue and its
//functions
//********************************************************

#ifndef __JIndexer__LexiconTable__
#define __JIndexer__LexiconTable__

#include <iostream>
#include <string>
#include <vector>
#include "Config.h"

typedef int FILEMODE;

typedef struct {
    std::string word;
    uint32_t num;   // number of docs containing the word
    uint16_t fileID;    // the ID of the file containing the invertd index
    uint32_t invertedPointer;   //pointer to the inverted index
    uint32_t size;  //size of this list in bytes
} LexiconItem;

typedef std::vector<LexiconItem *> LexiconTable;

void WriteLexiconTable(LexiconTable *lexiconTable, FILEMODE mode);

#endif /* defined(__JIndexer__LexiconTable__) */
