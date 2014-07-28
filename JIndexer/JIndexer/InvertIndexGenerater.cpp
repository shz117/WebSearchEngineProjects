//
//  InvertIndexGenerater.cpp
//  JIndexer
//
//  Created by Hongzheng Shi on 3/5/14.
//  Copyright (c) 2014 Jeremy H. Shi. All rights reserved.
//

#include "InvertIndexGenerater.h"
#include "Parser.h"
#include "stdio.h"
#include "LexiconTable.h"
#include "vbyte.h"

using namespace std;



void GenerateTmpIndex()
{
    ResourceLoader* loader=new ResourceLoader();
    uint32_t docID = 0;
    uint32_t fileID = 0;
    RawPostingVector *postingVector = new RawPostingVector;
    URLTable urlTable = URLTable();
    vector<RawPage>* pages;
    while ((pages=loader->getPages())) {
        uint32_t urlPointer = 0;
        for(int j=0;j<pages->size();j++) {
            RawPage pageObj=(*pages)[j];
            char *page = pageObj.getContent();
            int pageLength = pageObj.pageLenth;
            const char *url = pageObj._url.c_str();
            uint32_t wordCount = GetPostingFromPage(postingVector, page,url, pageLength, docID);
                
            URLItem *urlItem = new URLItem;
            urlItem->docID = docID;
            urlItem->fileID= fileID;
            urlItem->startIndex=urlPointer;
            urlItem->url= url;
            urlItem->dl = wordCount;
            urlPointer+=pageObj.pageLenth;
            urlTable.Add(urlItem);
            docID++;
            char tmpIndexFileName[MAX_PATH_LENGTH];
            if(postingVector->size() > MAX_POSTING_PER_TMP_INDEX) {
                sprintf(tmpIndexFileName, TMPINDEX_FILE, docID);
                WriteRawPostingToFile(postingVector, tmpIndexFileName, FILEMODE_ASCII);
                freeRawPostingVector(postingVector);
                }
            delete []page;
            page = NULL;
        }
        pages->clear();
        delete pages;
        pages = NULL;
        fileID++;
    }

    char tmpIndexFileName[MAX_PATH_LENGTH];
    if(postingVector->size() > 0) {
        sprintf(tmpIndexFileName, TMPINDEX_FILE, docID);
        WriteRawPostingToFile(postingVector, tmpIndexFileName, FILEMODE_ASCII);
        freeRawPostingVector(postingVector);
    }
    delete postingVector;
    urlTable.Write(CURRENT_FILEMODE);
}

int GetPostingFromPage(RawPostingVector *vector, char* page, const char* url, int length, uint32_t docID)
{
    // call parser
    int ret = parser(url, page, vector, docID);
    if(ret < 0) {
        // page has errors
        return 0;
    }
    
    return ret; //it's also the word count
}

void MergeTmpIndex()
{
    //merge sort the index
    system(MERGE_SORT_COMMAND);
    system(CLEAR_TMP_COMMAND);
}

void freeRawPostingVector(RawPostingVector *vector)
{
    for(int i=0;i<vector->size();i++) {
        RawPosting *posting = (*vector)[i];
        delete posting;
        (*vector)[i] = NULL;
    }
    
    vector->clear();
}

void WriteRawPostingToFile(RawPostingVector *vector, const char* filePath, FILEMODE mode)
{
    FILE *fp = fopen(filePath, "w+");
    if(fp != NULL) {
        std::stable_sort(vector->begin(), vector->end(), CompareRawPostingDocID);
        // must use stable sort in the second round
        std::stable_sort(vector->begin(), vector->end(), CompareRawPostingWord);
        for(int i=0; i<vector->size();i++) {
            RawPosting *posting = (*vector)[i];
            fprintf(fp, "%d %s %d\n", posting->docID, posting->word.c_str(), posting->pos);
        }
        fclose(fp);
    } else{
        cout<<"Failed to open temp file!"<<endl;
    }
}

bool CompareRawPostingDocID(RawPosting *posting1, RawPosting *posting2)
{
    return posting1->docID < posting2->docID;
}

bool CompareRawPostingWord(RawPosting *posting1, RawPosting *posting2)
{
    return posting1->word < posting2->word;
}



InvertedTable::InvertedTable()
{
    _mode = CURRENT_FILEMODE;
    _fileID=0;
    sprintf(_outputPath, INVERTEDINDEX_FILE, _fileID);
    _counter = 0;
    _fp = fopen(_outputPath, "a");
}
InvertedTable::~InvertedTable()
{
    fclose(_fp);
}

void InvertedTable::SetFileMode(FILEMODE mode)
{
    _mode = mode;
}

uint16_t InvertedTable::GetFileID()
{
    return _fileID;
}

uint32_t InvertedTable::GetDocNumLastWord()
{
    return _DocNumLastWord;
}

void InvertedTable::write()
{
    if(_mode == FILEMODE_ASCII) {
        if(_counter > MAX_FILE_SIZE) {
            // close current file and open a new file
            fclose(_fp);
            _fileID++;
            sprintf(_outputPath, INVERTEDINDEX_FILE, _fileID);
            _fp = fopen(_outputPath, "a");
            _counter = 0;
        }
        _DocNumLastWord = (uint32_t)_invertedList.size();
        for(int j=0;j<_invertedList.size();j++) {
            DocTuple *docTuple =_invertedList[j];
            _counter += fprintf(_fp,"%u %u %u ", docTuple->docID, docTuple->freq, docTuple->pos);
            delete docTuple;
            _invertedList[j] = NULL;
        }
        _counter += fprintf(_fp, "\n");
        _invertedList.clear();
        vector<DocTuple *> tmp;
        _invertedList.swap(tmp);
    }
    
    if(_mode == FILEMODE_BIN) {
        if(_counter > MAX_FILE_SIZE) {
            fclose(_fp);
            _fileID++;
            sprintf(_outputPath, INVERTEDINDEX_FILE, _fileID);
            _fp = fopen(_outputPath, "a");
            _counter = 0;
        }
        _DocNumLastWord = (uint32_t)_invertedList.size();
        for(int j=0;j<_invertedList.size();j++) {
            DocTuple *docTuple =_invertedList[j];
            uint32_t n;
            uint32_t *np;
            char buf[5];
            
            char *p = buf;
            n = (uint32_t)docTuple->docID;
            np = &n;
            ZVBCODE(np, p);
            int size = (int)(p-buf);
            _counter += sizeof(uint8_t) * fwrite((uint8_t*)buf, sizeof(uint8_t), size, _fp);
            
            p = buf;
            n = (uint32_t)docTuple->freq;
            np = &n;
            ZVBCODE(np, p);
            size = (int)(p-buf);
            _counter += sizeof(uint8_t) * fwrite((uint8_t*)buf, sizeof(uint8_t),size,_fp);
            
            p = buf;
            n = (uint32_t)docTuple->pos;
            np = &n;
            ZVBCODE(np, p);
            size = (int)(p-buf);
            _counter += sizeof(uint8_t) * fwrite((uint8_t*)buf, sizeof(uint8_t),size,_fp);
            delete docTuple;
            _invertedList[j] = NULL;
        }
        _invertedList.clear();
        vector<DocTuple *> tmp;
        _invertedList.swap(tmp);
    }
}

uint32_t InvertedTable::WriteOutstanding()
{
    write();
    _invertedList.clear();
    return _counter;
}

uint32_t InvertedTable::Insert(const RawPosting *rawPosting)
{
    // first insert, new word;
    if(_invertedList.size() == 0) {
        _word = rawPosting->word;
        DocTuple * docTuple = new DocTuple;
        _lastDocID = rawPosting->docID;
        docTuple->actualDocID = rawPosting->docID;
        docTuple->docID = rawPosting->docID;
        docTuple->freq =1;
        docTuple->pos = rawPosting->pos;
        _invertedList.push_back(docTuple);
    } else if (_word != rawPosting->word){
        //write and free the memory
        write();
        _word = rawPosting->word;
        DocTuple * docTuple = new DocTuple;
        _lastDocID = rawPosting->docID;
        docTuple->actualDocID = rawPosting->docID;
        docTuple->docID = rawPosting->docID;
        docTuple->freq=1;
        docTuple->pos = rawPosting->pos;
        _invertedList.push_back(docTuple);
        return _counter;
    } else {//insert same word
        // The same doc, compress position
        if(rawPosting->docID == _lastDocID) {
            DocTuple * docTuple = _invertedList.back();
            docTuple->freq++;
        } else {
            // Different doc, compress the docID
            DocTuple * docTuple = new DocTuple;
            _lastDocID = rawPosting->docID;
            docTuple->actualDocID = rawPosting->docID;
            // compress docID
            docTuple->docID = (rawPosting->docID-_invertedList.back()->actualDocID);
            docTuple->freq =1;
            docTuple->pos = rawPosting->pos;
            _invertedList.push_back(docTuple);
        }
        
    }
    // 0 means no data written
    return 0;
}


void GenerateInvertedIndexFile()
{
    //convert it into inverted index
    FILE *mergedIndex = fopen(TMP_INDEX_PATH, "r");
    if (mergedIndex==NULL) {
        cout<<"Failed to open merged index file!!!"<<endl;
    }
    RawPosting posting;
    char invertedWord[MAX_WORD_LENGTH];
    uint32_t invertedDocID;
    uint32_t invertedPost;
    InvertedTable invertedTable;
    LexiconTable lexiconTable;
    string lastWord = "";
    uint32_t lastCounter = 0;
    uint32_t count = 0;
    while(fscanf(mergedIndex, "%d %s %d\n", &invertedDocID, invertedWord, &invertedPost) != EOF) {
        if(strlen(invertedWord) > MAX_WORD_LENGTH) {
            cout<<invertedWord<<endl;
            cout<<strlen(invertedWord)<<endl;
            exit(1);
        }
        posting.docID = invertedDocID;
        posting.word=invertedWord;
        //bzero(invertedWord, MAX_WORD_LENGTH);
        posting.pos = invertedPost;
        
        // first time
        if(lastWord == "") {
            lastWord = posting.word;
            LexiconItem *lexiconItem = new LexiconItem;
            lexiconItem->word = posting.word;
            lexiconItem->invertedPointer = 0;
            lexiconTable.push_back(lexiconItem);
        }
        
        count = invertedTable.Insert(&posting);
        
        // count > 0 means meet new word and there is data written
        if(count > 0) {
            LexiconItem *lexiconItem = lexiconTable.back();
            lexiconItem->fileID = invertedTable.GetFileID();
            lexiconItem->num = invertedTable.GetDocNumLastWord();
            if(count < lastCounter) {
                lexiconItem->invertedPointer = 0;
            }
            lexiconItem->size = count - lexiconItem->invertedPointer;
            lexiconItem = new LexiconItem;
            lexiconItem->word = posting.word;
            lastWord = posting.word;
            lexiconItem->invertedPointer = count;
            lexiconTable.push_back(lexiconItem);
            lastCounter = count;
        }
        
    }
    
    count = invertedTable.WriteOutstanding();
    lexiconTable.back()->fileID = invertedTable.GetFileID();
    lexiconTable.back()->num = invertedTable.GetDocNumLastWord();
    if(count > 0 && count < lastCounter) {
        lexiconTable.back()->invertedPointer = 0;
    }
    lexiconTable.back()->size = count - lexiconTable.back()->invertedPointer;
    
    WriteLexiconTable(&lexiconTable, CURRENT_FILEMODE);
}
