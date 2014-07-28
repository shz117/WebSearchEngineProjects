//
//  main.cpp
//  JIndexer
//
//  Created by Hongzheng Shi on 3/5/14.
//  Copyright (c) 2014 Jeremy H. Shi. All rights reserved.
//
//********************************************************
// Driver program for indexer.
// See configurables in Config.h
//********************************************************
#include <iostream>
#include "Config.h"
#include "Parser.h"
#include "InvertIndexGenerater.h"
#include "LexiconTable.h"
#include "Urltable.h"
#include<time.h>

int main(int argc, const char * argv[])
{
    time_t start, middle;
    start = time(NULL);
    middle = time(NULL);
    
    cout<<"Start generating temp files and UrlTable."<<endl;
    GenerateTmpIndex();
    cout << "Runing time: " << difftime(time(NULL), middle)<< " Seconds." << endl;
    middle = time(NULL);
    
    cout<<"Start merging temp files."<<endl;
    MergeTmpIndex();
    cout << "Runing time: " << difftime(time(NULL), middle) << " Seconds." << endl;
    middle = time(NULL);
    
    cout<<"Start generating inverted-list files and LexiconTable."<<endl;
    GenerateInvertedIndexFile();
    cout << "Runing time: " << difftime(time(NULL), middle)<< " Seconds." << endl;
    cout<<"Finish!"<<endl;
    cout << "Total Runing time: " << difftime(time(NULL), start) << " Seconds." << endl;
    return 0;
}

