//
//  InvertIndexGenerater.h
//  JIndexer
//
//  Created by Hongzheng Shi on 3/5/14.
//  Copyright (c) 2014 Jeremy H. Shi. All rights reserved.
//
//********************************************************
// This class calls other components to implement
// following functions:
// 1. reads source files into mem, parse them into postings,
//  aggregate and sort before writing to temp files to disk
// 2. merge-sorts temp generated files into one file
// 3. scan merged file, aggregate freq, compose final index
// for each word
// 4. in the mean time, generate lexicon table, url table
//********************************************************
#ifndef __JIndexer__InvertIndexGenerater__
#define __JIndexer__InvertIndexGenerater__

#include <iostream>
#include <vector>
#include "ResourceLoader.h"
#include "Config.h"
#include "UrlTable.h"


typedef struct {
    uint32_t docID;
    std::string word;
    uint32_t pos;
} RawPosting;

typedef std::vector<RawPosting *> RawPostingVector;

typedef struct {
    uint32_t docID;
    uint32_t actualDocID;
    uint32_t freq;
    uint32_t pos;
} DocTuple;

class InvertedTable
{
private:
    FILEMODE _mode;
    std::string _word;
    char _outputPath[128];
    std::vector<DocTuple *> _invertedList;
    uint32_t _lastDocID;
    uint32_t _counter;
    uint16_t _fileID;
    uint32_t _DocNumLastWord;
    FILE *_fp;
    void write();
public:
    InvertedTable();
    ~InvertedTable();
    void SetFileMode(FILEMODE mode);
    uint32_t Insert(const RawPosting *rawPosting);
    uint32_t WriteOutstanding();
    uint16_t GetFileID();
    uint32_t GetDocNumLastWord();
};


void MergeTmpIndex();
void GenerateTmpIndex();
int GetPostingFromPage(RawPostingVector *vector, char* page, const char* url, int length, uint32_t docID);
void freeRawPostingVector(RawPostingVector *vector);
void WriteRawPostingToFile(RawPostingVector *vector, const char* filePath, FILEMODE mode);
bool CompareRawPostingDocID(RawPosting *posting1, RawPosting *posting2);
bool CompareRawPostingWord(RawPosting *posting1, RawPosting *posting2);

/****************************************************
 Generate the final inverted index files from the
 temporary index files.
 ****************************************************/
void GenerateInvertedIndexFile();
#endif /* defined(__JIndexer__InvertIndexGenerater__) */
