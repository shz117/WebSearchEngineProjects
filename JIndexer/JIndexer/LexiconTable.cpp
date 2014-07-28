//
//  LexiconTable.cpp
//  JIndexer
//
//  Created by Hongzheng Shi on 3/5/14.
//  Copyright (c) 2014 Jeremy H. Shi. All rights reserved.
//

#include "LexiconTable.h"
void WriteLexiconTable(LexiconTable *lexiconTable, FILEMODE mode)
{
    if(mode == FILEMODE_ASCII) {
        FILE *fp = fopen(LEXICON_FILE, "w+");
        for(int i=0;i<lexiconTable->size();i++) {
            LexiconItem *lexiconItem =(*lexiconTable)[i];
            fprintf(fp,"%s %d %d %d %d\n", lexiconItem->word.data(), lexiconItem->num, lexiconItem->fileID, lexiconItem->invertedPointer, lexiconItem->size);
        }
        
        fclose(fp);
    }
    
    if(mode == FILEMODE_BIN) {
        FILE *fp = fopen(LEXICON_FILE, "wb+");
        for(int i=0;i<lexiconTable->size();i++) {
            LexiconItem *lexiconItem =(*lexiconTable)[i];
            //in binary file, the format is:
            //Lengthof word, word, num, fileID, pointer
            uint16_t wordLength = lexiconItem->word.length();
            fwrite(&wordLength, sizeof(uint16_t),1, fp);    // write the word length
            fwrite(lexiconItem->word.data(), sizeof(char), wordLength, fp);
            fwrite(&lexiconItem->num, sizeof(uint32_t), 1, fp);
            fwrite(&lexiconItem->fileID, sizeof(uint16_t), 1, fp);
            fwrite(&lexiconItem->invertedPointer, sizeof(uint32_t), 1, fp);
            fwrite(&lexiconItem->size, sizeof(uint32_t), 1, fp);
        }
        
        fclose(fp);
    }
}