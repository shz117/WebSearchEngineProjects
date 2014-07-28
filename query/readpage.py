import gzip
import os
import config
from urltable import URLTable
from singleton import Singleton


@Singleton
class PageReader:
    def __init__(self):
        self.cache = {}

    def findFile(self, fileID):
        if self.cache.has_key(fileID):
            return self.cache[fileID]
        i = 0;
        for root, dirnames, filenames in os.walk(config.RawDataFile):
            for filename in filenames:
                if '_data' in filename:
                    if i == fileID:
                        self.cache[fileID] = os.path.join(root, filename)
                        return os.path.join(root, filename)
                    i += 1
        return None

    def readGZFile(self, filepath, pointer, pagesize):
        try:
            pages = gzip.open(filepath, 'r')
        except Exception, e:
            print e
            return None
        pages.seek(pointer)
        page = pages.read(pagesize)
        pages.close()
        return page

    def readPage(self, docID):
        urlTable = URLTable()
        urlItem = urlTable[docID]
        fileID = urlItem.fileID
        pointer = urlItem.pointer
        pagesize = urlItem.pagesize  # the pagesize of the last docID is -1, means to read until the end
        filepath = self.findFile(fileID)
        if not filepath:
            print "No such file."
            return None
        return self.readGZFile(filepath, pointer, pagesize)
