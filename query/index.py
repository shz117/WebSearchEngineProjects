import config
from lexicon import LexiconTable
from singleton import Singleton
from vbyte import decode


class Posting:
    def __init__(self):
        self.docID = -1
        self.freq = -1
        self.pos = -1

    def __cmp__(self, other):
        if self.docID < other:
            return -1
        elif self.docID == other:
            return 0
        elif self.docID > other:
            return 1


@Singleton
class IndexTable:
    def __init__(self):
        self.files = {}

    def GetIndex(self, fileID, pointer, length, occurence):
        fileName = config.IndexFile
        if self.files.has_key(fileID) is False:
            fb = open(fileName % (fileID), "rb")
            self.files[fileID] = fb
        fb = self.files[fileID]
        fb.seek(pointer)
        invertedList = []
        lastDocID = 0
        bytes = fb.read(length)
        numbers = decode(bytes)
        # print "len, occurence, len of num:", length, occurence, len(numbers)
        if len(numbers) != occurence * 3:
            print "error decoding"
            return []
        for i in range(occurence):
            posting = Posting()
            posting.docID = numbers[i * 3 + 0]
            posting.freq = numbers[i * 3 + 1]
            posting.pos = numbers[i * 3 + 2]
            if i == 0:
                lastDocID = posting.docID
            else:
                posting.docID = posting.docID + lastDocID
                lastDocID = posting.docID
            invertedList.append(posting)
        return invertedList


def test():
    lexiconTable = LexiconTable()
    lexicon = lexiconTable["hello"]
    indexTable = IndexTable()
    l = indexTable.GetIndex(lexicon.fileID, lexicon.pointer, lexicon.occurence)
    print len(l)
    print l[0].docID, l[0].freq, l[0].pos

    #test()