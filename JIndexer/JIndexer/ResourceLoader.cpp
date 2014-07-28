//
//  ResourceLoader.cpp
//  JIndexer
//
//  Created by Hongzheng Shi on 3/5/14.
//  Copyright (c) 2014 Jeremy H. Shi. All rights reserved.
//

#include "ResourceLoader.h"
#include <queue>
#include <zlib.h>
using namespace std;
ResourceLoader::ResourceLoader(){
    //init _files vector and fill with file names
    _files = *new vector<string>();
    queue<char*> folderQueue = queue<char*>();
    char *fullPath = new char[MAX_PATH_LENGTH];
    sprintf(fullPath, "%s", DATA_SOURCE_PATH);
    folderQueue.push(fullPath);
    struct dirent *de;
    // Loop while not NULL
    while(folderQueue.size() > 0) {
        char* path = folderQueue.front();
        DIR *dir = opendir(path);
        if (dir == NULL)
            continue;
        while((de = readdir(dir))) {
            if(de == NULL || de->d_name == NULL) {
                break;
            }
            if (de->d_type == DT_REG && strlen(de->d_name) > 0 && de->d_name[0] != '.') {
                char *fullPath = new char[MAX_PATH_LENGTH];
                sprintf(fullPath, "%s/%s", path, de->d_name);
                _files.push_back(fullPath);
                delete []fullPath;
                fullPath = NULL;
            }
            else if(de->d_type == DT_DIR && strlen(de->d_name) > 0 && de->d_name[0] != '.') {
                char *fullPath = new char[MAX_PATH_LENGTH];
                sprintf(fullPath, "%s/%s", path, de->d_name);
                folderQueue.push(fullPath);
            }
        }
        
        // release the queue
        delete []path;
        folderQueue.front() = NULL;
        folderQueue.pop();
        closedir(dir);
    }
    //remove files other than index files
    size_t oSize=_files.size();
    for(vector<string>::size_type i = 0;i<oSize; ) {
        string s=_files[i];
        string lastFive = s.size()>=7?s.substr(s.size()-5,5): "";
        if (lastFive!="index") {
            _files.erase(_files.begin()+i);
            oSize--;
        }else{
            i++;
        }
    }
    //end init _files
    
    //set curIndex to 0
    _curIndex=0;
    
}

ResourceLoader::~ResourceLoader(){
}

vector<RawPage>* ResourceLoader::getPages(){
    //check if more pages available
    if (_curIndex>=_files.size()) {
        return NULL;
    }
    //start getting pages for current index
    string fileName=_files.at(_curIndex);
    char* indexPath=getCharArrayFromString(fileName);
    char* indexBuffer=readGzipFile(indexPath);
    //get index records array
    IndexRecordVector *indexArray=ParseIndex(indexBuffer);
    size_t i=fileName.find("index");
    fileName.replace(i,5,"data");
//    cout<<"Data file path:"<<fileName<<endl;
    char* dataPath=getCharArrayFromString(fileName);
    char* dataBuffer=readGzipFile(dataPath);
    //get next pages
    vector<RawPage>* pages=getPagesFromIndexRecords(dataBuffer,indexArray);
    if(pages==NULL){
        vector<RawPage>* emptyPages=new vector<RawPage>();
        _curIndex++;
        return emptyPages;
    }
    //increase pointer
    _curIndex++;
    return pages;
}

vector<IndexRecord *> *ResourceLoader::ParseIndex(char *indexBuf)
{
    IndexRecordVector *indexArray = new IndexRecordVector;
    char* indexPointer = indexBuf;
    char urlBuff[MAX_URL_LENGTH];
    while((*indexPointer) != 0) {
        IndexRecord *indexRecord = new IndexRecord;
        int i = 0;
        while((*indexPointer)!=' ') {
            urlBuff[i] = (*indexPointer);
            i++;
            indexPointer++;
        }
        urlBuff[i] = 0;
        indexRecord->url = urlBuff;
        
        while((*indexPointer) == ' ') {
            indexPointer++;
        }
        
        indexRecord->depth = atoi(indexPointer);
        while((*indexPointer) != ' ') {
            indexPointer++;
        }
        while((*indexPointer) == ' ') {
            indexPointer++;
        }
        
        indexRecord->unknown = atoi(indexPointer);
        while((*indexPointer) != ' ') {
            indexPointer++;
        }
        while((*indexPointer) == ' ') {
            indexPointer++;
        }
        
        indexRecord->length = atoi(indexPointer);
        while((*indexPointer) != ' ') {
            indexPointer++;
        }
        while((*indexPointer) == ' ') {
            indexPointer++;
        }
        
        i=0;
        while((*indexPointer)!=' ') {
            indexRecord->IP[i] = (*indexPointer);
            i++;
            indexPointer++;
        }
        indexRecord->IP[i] = 0;
        
        while((*indexPointer) == ' ') {
            indexPointer++;
        }
        
        indexRecord->port = atoi(indexPointer);
        while((*indexPointer) != ' ') {
            indexPointer++;
        }
        while((*indexPointer) == ' ') {
            indexPointer++;
        }
        
        i=0;
        while((*indexPointer)!=' ' && (*indexPointer) != '\n'&& (*indexPointer) != 0) {
            indexRecord->status[i] = (*indexPointer);
            i++;
            indexPointer++;
        }
        indexRecord->status[i] = '\0';
        indexArray->push_back(indexRecord);
        while((*indexPointer) != '\n' && (*indexPointer) != 0) {
            indexPointer++;
        }
        if((*indexPointer) == 0) {
            break;
        } else {
            indexPointer++;
        }
    }
    
    return indexArray;
}

vector<RawPage> *ResourceLoader::getPagesFromIndexRecords(char* dataBuf, IndexRecordVector* indexArray)
{
    if (dataBuf==NULL) {
        return NULL;
    }
    vector<RawPage> *pages = new vector<RawPage>();
    char *dataPointer = dataBuf;
    for(int i=0;i<indexArray->size();i++) {
        IndexRecord *indexRecord = (*indexArray)[i];
        int pageLength= indexRecord->length;
        if (pageLength < 0 || pageLength > MAX_PAGE_LENGTH) {
            cout<<"Page length is incorrect, drop the whole file:"<<pageLength<<endl;
            cout<<indexRecord->url<<endl;
            return pages;
        }
        if(strcmp(indexRecord->status, "ok") != 0) {
            cout<<"stat is not ok"<<endl;
        }else {
            int pageLength= indexRecord->length;
            char *content = new char[pageLength+1];
            strncpy(content, dataPointer, pageLength);
            content[pageLength] = '\0';
            RawPage page=*new RawPage::RawPage(indexRecord->url,content);
            page.pageLenth=indexRecord->length;
            pages->push_back(page);
        }
        
        dataPointer += pageLength;
    }
    return pages;
}

char* ResourceLoader::readGzipFile(char* filePath){
    gzFile gzfile = gzopen(filePath, "r");
    int size = INDEX_CHUNK;
    char *buffer=(char *)malloc(size);
    int oldSize=size;
    
    int count=0;             //The number of bytes that already read
    while (!gzeof(gzfile))
    {
        count+=gzread(gzfile,buffer+count,oldSize);
        if (count==size)                    // Reallocate when buffer is full
        {
            oldSize=size;
            size*=2;
            if (size > MAX_GZFILE_LENGTH) {
                std::cout<<"gzip file incorrect:"<<size<<std::endl;
                gzclose(gzfile);
                free(buffer);
                return NULL;
            }
            
            buffer=(char *)realloc(buffer,size);
        }
    }
    buffer[count] = '\0';
    gzclose(gzfile);
    //    cout<<buffer<<endl;
    gzfile=NULL;
    return buffer;
}



//helper functions
bool ResourceLoader::isdigit(char c){
    return c>='0'&&c<='9';
}
void ResourceLoader::clear(char* array,int length){
    for (int i=0; i<length; i++) {
        array[i]='\0';
    }
}
char* ResourceLoader::getCharArrayFromString(string s){
    char* ret=new char[s.size()+1];
    ret[s.size()]=0;
    memcpy(ret,s.c_str(),s.size());
    return ret;
}