import config
import struct
from singleton import Singleton


class Lexicon:
    def __init__(self):
        self.occurence = 0
        self.fileID = 0
        self.pointer = 0
        self.size = 0


@Singleton
class LexiconTable:
    def __init__(self):
        self.dict = {}
        self.LoadData()

    def __getitem__(self, word):
        if (self.dict.has_key(word)):
            return self.dict[word]
        else:
            return None

    def LoadData(self):
        print "loading lexicon..."
        fileName = config.LexiconFile
        with open(fileName, "rb") as fb:
            while True:
                bytes = fb.read(struct.calcsize('=H'))
                if not bytes:
                    break
                (wordLen,) = struct.unpack("=H", bytes)
                word = str(fb.read(wordLen))
                lexicon = Lexicon()
                (lexicon.occurence, lexicon.fileID, lexicon.pointer, lexicon.size) = struct.unpack("=IHII", fb.read(
                    struct.calcsize('=IHII')))
                self.dict[word] = lexicon
        print "lexicon size:", len(self.dict)

    def GetLexicon(self, word):
        return self.dict[word]

    def Length(self):
        return len(self.dict)


def test():
    a = LexiconTable()
    print a.Length()
    #litem = a["new"]
    #print litem.fileID, litem.pointer, litem.occurence