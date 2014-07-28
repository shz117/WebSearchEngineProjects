import config
import struct
from singleton import Singleton


class URLItem:
    def __init__(self):
        self.url = ""
        self.fileID = -1
        self.pointer = -1
        self.pagesize = -1  # -1 means read until the end of the datafile
        self.dl = -1


@Singleton
class URLTable:
    def __init__(self):
        self.array = []
        self.avgdl = 0
        self.N = 0
        self.LoadData()

    def __getitem__(self, docID):
        return self.array[docID]

    #load urltable to mem, also calculate avg word count for bm25
    def LoadData(self):
        print "loading urltable..."
        fileName = config.URLFile
        totalWordCount = 0
        with open(fileName, "rb") as fb:
            while True:
                bytes = fb.read(struct.calcsize('=H'))
                if not bytes:
                    break;
                (wordLen,) = struct.unpack("=H", bytes)
                urlItem = URLItem()
                urlItem.url = str(fb.read(wordLen))
                (urlItem.fileID, urlItem.pointer, urlItem.dl) = struct.unpack("=HII", fb.read(struct.calcsize('=HII')))
                length = len(self.array)
                if (length > 0):
                    lastItem = self.array[length - 1]
                    lastItem.pagesize = urlItem.pointer - lastItem.pointer
                    if lastItem.pagesize < 0:
                        lastItem.pagesize = -1
                totalWordCount += urlItem.dl
                self.array.append(urlItem)
        self.avgdl = totalWordCount / len(self.array)
        self.N = len(self.array)
        print "urltable size:", self.N

    def GetDoc(self, docID):
        return self.array[docID]

    def Length(self):
        return self.N


def test():
    a = URLTable()
    print a[1000].url

