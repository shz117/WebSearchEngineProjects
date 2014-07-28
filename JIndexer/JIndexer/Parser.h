//
//  Parser.h
//  JIndexer
//
//  Created by Hongzheng Shi on 3/5/14.
//  Copyright (c) 2014 Jeremy H. Shi. All rights reserved.
//
//********************************************************
// This class will parse page file, write postings to vector
// and return word count
//********************************************************
#ifndef __JIndexer__Parser__
#define __JIndexer__Parser__

#include <iostream>
#include "InvertIndexGenerater.h"
/***************************************************************
 
 function to parse urls and pages into words
 
 return value:
 a) >= 0, if success;
 b) -1, if buffer (for words) too small
 
 arguments:
 1) url of page;
 2) page to be parsed;
 3) buffer to store parsed out words and their contexts;
 format: "<WORD> <CONTEXT>\n"
 <CONTEXT>: 'B', bold; 'H', head (h1-h6); 'I', italic;
 'P', plain; 'T', title; 'U', URL.
 4) length of buffer
 
 ***************************************************************/
int parser(const char* url, char* doc, RawPostingVector *vector, uint32_t docID);
#endif /* defined(__JIndexer__Parser__) */
