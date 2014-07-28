from urltable import URLTable
from lexicon import LexiconTable
from index import IndexTable
from singleton import Singleton
import time
from readpage import PageReader
import bm25
from operator import itemgetter, attrgetter
import cgi
import config

#================== helper class definition ========================#
class ResultItem:
    def __init__(self):
        self.bm25 = 0
        self.score = 0
        self.url = ""
        self.title = ""
        self.snippet = ""
        self.pos = []
        self.docID = 0


class PList:
    def __init__(self, invertedList):
        self.invertedList = invertedList
        self.cursor = 0
        if invertedList is None:
            self.size = 0
        else:
            self.size = len(invertedList)

#================== end helper class definition ========================#

@Singleton
class Query:
    def __init__(self):
        self.indexCache = {}
        self.snippetCache = {}

#================== helper functions ========================#
    def __getFromCache(self, word):
        if self.indexCache.has_key(word):
            return self.indexCache[word]
        return None

    def __getFromFile(self, word):
        lexiconTable = LexiconTable()
        lexicon = lexiconTable[word.lower()]
        if lexicon is None:
            return None
        indexTable = IndexTable()
        index = indexTable.GetIndex(lexicon.fileID, lexicon.pointer, lexicon.size, lexicon.occurence)
        self.indexCache[word] = index
        return index

    def parseQuery(self, query):
        querylist = list(query)
        querylist = [" " if not ch.isalnum() else ch for ch in querylist]
        tmplist = ''.join(querylist).split(" ")
        words = []
        for word in tmplist:
            if len(word) > 0:
                words.append(word)
        return list(set(words)) #remove dups

    def getSnippetFromCache(self, docID, pos):
        key = "d" + str(docID) + "p" + str(pos)
        if self.snippetCache.has_key(key):
            return self.snippetCache[key]
        return None

    def getSnippetFromPage(self, docID, page, pos):
        if pos == 0:  #means the word is in url, just return empty body
            return ""
        length = len(page)
        snippet = []
        startIndex = 0 if pos - 50 < 0 else pos - 50
        endIndex = length - 1 if pos + 100 > length - 1 else pos + 100
        snippet.append(page[startIndex:endIndex].strip().replace("\n", "").replace("\r", ""))
        snippet.append("......")
        snippet = ''.join(snippet)
        self.snippetCache["d" + str(docID) + "p" + str(pos)] = snippet
        return snippet

    def openList(self, word):
        index = self.__getFromCache(word)
        if index is None:
            index = self.__getFromFile(word)
        pList = PList(index)
        return pList

    def nextGEQ(self, pList, docID):
        start = pList.cursor
        for i in range(start, pList.size):
            pList.cursor += 1
            if pList.invertedList[i].docID >= docID:
                return pList.invertedList[i].docID
        return config.MAXDID

    def getFreq(self, pList, docID):
        if pList.invertedList[pList.cursor - 1].docID == docID:
            return pList.invertedList[pList.cursor - 1].freq
        return -1

    def getPos(self, pList, docID):
        if pList.invertedList[pList.cursor - 1].docID == docID:
            return pList.invertedList[pList.cursor - 1].pos
        return -1
#================== end helper functions ========================#

#================== daat query function ========================#
    def daatQuery(self, words):
        queryResult = []
        urlTable = URLTable()
        num = len(words)
        print "num of lists:", num
        lp = []
        openListStart = time.clock()
        for i in range(num):
            lp.append(self.openList(words[i]))
        print "time to openlist:", str(time.clock() - openListStart)
        sortStart = time.clock()
        bm25Time = 0
        lp.sort(key=attrgetter('size'))
        # for i in range(1,len(lp)):
        #     if
        did = 0
        print "time sorting list:", str(time.clock() - sortStart)
        nextGEQTime=0
        while did < config.MAXDID:
            nGEQStart=time.clock()
            did = self.nextGEQ(lp[0], did)
            nextGEQTime+=time.clock()-nGEQStart
            if did == config.MAXDID:
                break
            d = None
            for i in range(1, num):
                nGEQStart=time.clock()
                d = self.nextGEQ(lp[i], did)
                nextGEQTime+=time.clock()-nGEQStart
                if d != did:
                    break
            if d is not None and d > did:
                did = d
            else:
                resultItem = ResultItem()
                resultItem.docID = did
                resultItem.url = urlTable[did].url
                score = 0
                bm25Start = time.clock()
                for i in range(num):
                    freq = self.getFreq(lp[i], did)
                    score += bm25.getBM25(freq, lp[i].size, urlTable.N, urlTable[did].dl, urlTable.avgdl)
                    pos = self.getPos(lp[i], did)
                    resultItem.pos.append(pos)
                bm25Time += time.clock() - bm25Start
                resultItem.bm25 = score
                resultItem.score = resultItem.bm25
                queryResult.append(resultItem)
                did += 1
        print "total time nextGEQ:",nextGEQTime
        print "total time bm25: ", bm25Time
        return queryResult
#================== end daat query function ========================#

#================== Query class API ========================#
    def queryWords(self, query, start, limit):
        pageReader = PageReader()
        words = self.parseQuery(query)
        begin = time.clock()
        queryResult = self.daatQuery(words)
        print "total time to query:", str(time.clock() - begin)
        resultSize = len(queryResult)
        print "result set size:", resultSize
        if start > resultSize - 1:
            return [], 0
        begin = time.clock()
        queryResult.sort(key=attrgetter('score'), reverse=True)
        print "time to sort by BM25:", str(time.clock() - begin)
        startIndex = 0 if start < 0 else start
        endIndex = startIndex + limit
        endIndex = resultSize if endIndex > resultSize else endIndex
        print "start index & end index:", startIndex, endIndex
        queryResult = queryResult[startIndex:endIndex]
        begin = time.clock()
        for resultItem in queryResult:
            page = None
            snippet = ""
            for i in range(len(words)):
                if resultItem.pos[i] > 0:
                    tmpSnip = self.getSnippetFromCache(resultItem.docID, resultItem.pos[i])
                    if tmpSnip is None:
                        if page is None:
                            page = pageReader.readPage(resultItem.docID)
                        tmpSnip = self.getSnippetFromPage(resultItem.docID, page, resultItem.pos[i])
                    snippet += tmpSnip
            resultItem.snippet = cgi.escape(snippet)
        print "time to get snippets:", str(time.clock() - begin)
        return queryResult, resultSize
#================== end Query API ========================#

def Test():
    words = "hello world"
    q = Query()
    q.indexCache["foo"]="bar"
    q1 =Query()
    print q1.indexCache["foo"]
    # result, size = q.queryWords(words, 0, 10)
    # print size
    # for item in result:
    #     print item.url, item.score, item.snippet

# Test()
