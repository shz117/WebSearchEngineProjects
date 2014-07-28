//
//  UrlTable.cpp
//  JIndexer
//
//  Created by Hongzheng Shi on 3/5/14.
//  Copyright (c) 2014 Jeremy H. Shi. All rights reserved.
//

#include "UrlTable.h"
URLTable::URLTable()
{
    _urlTable = new URLVector;
}

URLTable::~URLTable()
{
    for(int i=0;i<_urlTable->size();i++) {
        delete (*_urlTable)[i];
        (*_urlTable)[i] = NULL;
    }
    _urlTable->clear();
    delete _urlTable;
    _urlTable = NULL;
}

void URLTable::Add(URLItem* urlitem)
{
    _urlTable->push_back(urlitem);
}

void URLTable::Write(FILEMODE mode)
{
    if(mode == FILEMODE_ASCII) {
        FILE *fp = fopen(URL_FILE, "w+");
        for(int i=0;i<_urlTable->size();i++) {
            URLItem *urlitem =(*_urlTable)[i];
            fprintf(fp,"%s %d %d %d\n", urlitem->url.data(), urlitem->fileID, urlitem->startIndex, urlitem->dl);
        }
        
        fclose(fp);
    }
    
    if(mode == FILEMODE_BIN) {
        FILE *fp = fopen(URL_FILE, "wb+");
        for(int i=0;i<_urlTable->size();i++) {
            URLItem *urlitem =(*_urlTable)[i];
            uint16_t urlLength = urlitem->url.length();
            fwrite(&urlLength, sizeof(uint16_t),1, fp);
            fwrite(urlitem->url.data(), sizeof(char), urlLength, fp);
            fwrite(&urlitem->fileID, sizeof(uint16_t), 1, fp);
            fwrite(&urlitem->startIndex, sizeof(uint32_t), 1, fp);
            fwrite(&urlitem->dl, sizeof(uint32_t), 1, fp);
        }
        fclose(fp);
    }
}