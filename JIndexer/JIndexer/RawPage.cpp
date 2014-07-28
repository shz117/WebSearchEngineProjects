//
//  RawPage.cpp
//  JIndexer
//
//  Created by Hongzheng Shi on 3/5/14.
//  Copyright (c) 2014 Jeremy H. Shi. All rights reserved.
//

#include "RawPage.h"

RawPage::RawPage(string url,char* content){
    _content=content;
    _url=url;
}

RawPage::~RawPage(){
    
}

char* RawPage::getContent(){
    return _content;
}