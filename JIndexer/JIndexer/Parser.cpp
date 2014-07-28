//
//  Parser.cpp
//  JIndexer
//
//  Created by Hongzheng Shi on 3/5/14.
//  Copyright (c) 2014 Jeremy H. Shi. All rights reserved.
//

#include "Parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale>
#include <algorithm>
#include <string>
#include <cctype>
#include <iostream>

#define PTAG_B	1
#define PTAG_I	2
#define	PTAG_H	3
#define PTAG_TITLE	4
#define PTAG_SCRIPT	5

#define _TITLE_TAG	0x0001
#define _B_TAG		0x0004
#define _H_TAG		0x0008
#define _I_TAG		0x0010

#define xl_isdigit(c) (((c) >= '0') && ((c) <= '9'))
#define xl_islower(c) (((c) >= 'a') && ((c) <= 'z'))
#define xl_isupper(c) (((c) >= 'A') && ((c) <= 'Z'))
#define xl_isindexable(c) (xl_isdigit(c) || xl_islower(c) || xl_isupper(c))
#define xl_tolower(c) ((c) += 'a' - 'A')

char* parser_init(char* doc)
{
	char *p;
    
	if (strncasecmp(doc, "HTTP/", 5))
		return NULL;
	
	for (p = doc; (*p != ' ')&&(*p); p++);
	if (*p == '\0')
		return NULL;
    
	if (atoi(p) != 200)
		return NULL;
    
	p = strstr(p,  "\r\n\r\n");
	if (p == NULL)
		return NULL;
    
	return p+4;
}

int tag_parser(char* tag, int len, char* back_tag)
{
	int i = 0;
    
	if (tag[0] == '/')
	{
		*back_tag = 1;
		i++;
        
	} else
		*back_tag = 0;
    
	switch (tag[i])
	{
        case 'b':
        case 'B':
        case 'i':
        case 'I':
            if (!isspace(tag[i+1]))
                return 0;
            if ((tag[i] == 'b') || (tag[i] == 'B'))
                return PTAG_B;
            return PTAG_I;
            
        case 'e':
        case 'E':
            i++;
            if (((tag[i]=='m')||(tag[i]=='M')) && (isspace(tag[i+1])))
                return PTAG_I;
            return 0;
            
        case 'h':
        case 'H':
            i++;
            if (((tag[i]>='1')&&(tag[i]<='6')) && (isspace(tag[i+1])))
                return PTAG_H;
            return 0;
            
        case 't':
        case 'T':
            i++;
            if ((0==strncasecmp(tag+i, "itle", 4)) && (isspace(tag[i+4])))
                return PTAG_TITLE;
            return 0;
            
        case 's':
        case 'S':
            i++;
            if ((0==strncasecmp(tag+i, "trong", 5)) && (isspace(tag[i+5])))
                return PTAG_B;
            if ((0==strncasecmp(tag+i, "cript", 5)) && (isspace(tag[i+5])))
                return PTAG_SCRIPT;
            return 0;
            
        default:
            break;
	}
    
	return 0;
}

#define xlbit_set(__b1, __b2)	((__b1) |= (__b2))
#define xlbit_unset(__b1, __b2)	((__b1) &= ~(__b2))
#define xlbit_check(__b1, __b2) ((__b1)&(__b2))

int parser(const char* url, char* doc, RawPostingVector *vector, uint32_t docID)
{
	char *p, *word, *ptag;
   	char ch, back_tag, intag, inscript;
	unsigned tag_flag;
	int ret;
    uint32_t wordCount = 0;
	p = parser_init(doc);
	if (p == NULL) {
		return -1;
    }
    
    char wordBuf[MAX_WORD_LENGTH];
    
    /* parsing URL */
    size_t ulen = strlen(url);
    if (ulen > MAX_URL_LENGTH) {
        std::cout<<"URL length is incorrect:"<<ulen<<std::endl;
    } else {
        char *urlBuff = new char[ulen+1];
        char *purl = urlBuff;
        strcpy(purl, url);
        purl[ulen] = '\0';
        while (*purl != '\0')
        {
            if (!xl_isindexable(*purl))
            {
                purl++;
                continue;
            }
            
            word = purl;
            while (xl_isindexable(*purl))
            {
                if (xl_isupper(*purl))
                    xl_tolower(*purl);
                purl++;
            }
            
            ch = *purl;
            *purl = '\0';
            if(strlen(word) > MAX_WORD_LENGTH) {
                std::cout<<"word to long"<<strlen(word)<<std::endl;
                *purl = ch;
                continue;
            }
            strcpy(wordBuf, word);
            RawPosting *rawPosting = new RawPosting;
            rawPosting->docID = docID;
            rawPosting->word = wordBuf;
            rawPosting->pos = 0; // we never extract words from header, so we can use 0 to indicate the word is in URL
            vector->push_back(rawPosting);
            wordCount++;
            *purl = ch;
        }
        delete []urlBuff;
        urlBuff = NULL;
    }
    /* parsing page */
	tag_flag = 0;
	intag = 0;
	inscript = 0;
	while (*p != '\0')
	{
		if (!xl_isindexable(*p))
		{
			if (*p != '>')
			{
				if (*p == '<')
				{
					ptag = p;
					intag = 1;
				}
				p++;
				continue;
			}
            
            if(intag == 0)
            {
                p++;
                continue;
            }
            
			*p = ' ';
			ret = tag_parser(ptag+1, (int)(p-ptag), &back_tag);
			switch (ret)
			{
				case PTAG_B:
                    
					if (back_tag == 0)
						xlbit_set(tag_flag, _B_TAG);
					else
						xlbit_unset(tag_flag, _B_TAG);
					break;
                    
				case PTAG_I:
                    
					if (back_tag == 0)
						xlbit_set(tag_flag, _I_TAG);
					else
						xlbit_unset(tag_flag, _I_TAG);
					break;
                    
				case PTAG_H:
                    
                    if (back_tag == 0)
                        xlbit_set(tag_flag, _H_TAG);
                    else
                        xlbit_unset(tag_flag, _H_TAG);
                    break;
                    
				case PTAG_TITLE:
                    
					if (back_tag == 0)
						xlbit_set(tag_flag, _TITLE_TAG);
					else
						xlbit_unset(tag_flag, _TITLE_TAG);
					break;
                    
				case PTAG_SCRIPT:
                    
					if (back_tag == 0)
						inscript = 1;
					else
						inscript = 0;
                    
				default:
					break;
			}
            
			intag = 0;
			p++;
			continue;
		}
        
		if (inscript || intag)
		{
			p++;
			continue;
		}
        
		word = p;
		while (xl_isindexable(*p))
		{
			if (xl_isupper(*p))
				xl_tolower(*p);
			p++;
		}
        
		ch = *p;
		*p = '\0';
        
        if(strlen(word) > MAX_WORD_LENGTH) {
            std::cout<<"word to long"<<strlen(word)<<std::endl;
            *p = ch;
            continue;
        }
		strcpy(wordBuf, word);
        RawPosting *rawPosting = new RawPosting;
        rawPosting->docID = docID;
        rawPosting->word = wordBuf;
        rawPosting->pos = (uint32_t)(word-doc);
		vector->push_back(rawPosting);
        wordCount++;
		*p = ch;
	}
	return wordCount;
}