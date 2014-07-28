//
//  Config.h
//  JIndexer
//
//  Created by Hongzheng Shi on 3/5/14.
//  Copyright (c) 2014 Jeremy H. Shi. All rights reserved.
//
//********************************************************
// This is the global settings file. To be imported in
// classes where needed.
//********************************************************
#ifndef JIndexer_Config_h
#define JIndexer_Config_h

#define DATA_SOURCE_PATH "/Users/JeremyShi/Developer/Projects/JIndexer/JIndexer/data/source"
#define MAX_PATH_LENGTH 256 //Max length of file path
#define MAX_URL_LENGTH  4096//mas lenth of url
#define MAX_PAGE_LENGTH 33554432
#define MAX_WORD_LENGTH 524288
#define FILEMODE_ASCII 0
#define FILEMODE_BIN 1

#define CURRENT_FILEMODE FILEMODE_BIN
#define LEXICON_FILE "/Users/JeremyShi/Developer/Projects/JIndexer/JIndexer/data/results/lexicon.index"
#define URL_FILE "/Users/JeremyShi/Developer/Projects/JIndexer/JIndexer/data/results/url.index"
#define INVERTEDINDEX_FILE "/Users/JeremyShi/Developer/Projects/JIndexer/JIndexer/data/results/inverted_%d.index"
#define TMPINDEX_FILE "/Users/JeremyShi/Developer/Projects/JIndexer/JIndexer/data/tmp/%d.index.tmp"
#define POST_PER_CHUNK 128
#define BLOCK_SIZE 65536   //64k bytes
#define MAX_DOCID 268435456 //use almost 28 bits representing 0-268435455
//, which means we can only have 268.43 million pages
#define INDEX_CHUNK 409600 //50KB
#define DATA_CHUNK 20971520 //2.5MB
#define MAX_GZFILE_LENGTH 536870912 //512M Max gzip file size
#define TMP_INDEX_PATH "/Users/JeremyShi/Developer/Projects/JIndexer/JIndexer/data/tmp/merge.index"
#define MERGE_SORT_COMMAND "sort -S 1G -m -k 2f,2f -k 1n,1n /Users/JeremyShi/Developer/Projects/JIndexer/JIndexer/data/tmp/*.tmp -o  /Users/JeremyShi/Developer/Projects/JIndexer/JIndexer/data/tmp/merge.index"
#define CLEAR_TMP_COMMAND "rm -rf /Users/JeremyShi/Developer/Projects/JIndexer/JIndexer/data/tmp/*.tmp"
#define MAX_POSTING_PER_TMP_INDEX 10000000 // 10 million
#define MAX_FILE_SIZE 4*1000*1000*1000 // Max size of final inverted index file
#endif
