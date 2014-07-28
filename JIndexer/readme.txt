========================================================================
What this program can do?

This program will generate:
1. inverted-list files
{docID,freq,pos,cont}
2. lexicon file
{word,num,invertedListFileID,pointerInIVListFile,sizeOfList}
3. URL table file
{url, invertedListFileID, pointerInIVListFile, numWords} docId is array index
from compressed web page files and corresponding index files.

========================================================================
How to run?

Follow these steps:
1. Open JIndexer.xcodeproj with Xcode
2. Copy source data folder into path specified by "Config.h", or change "Config.h" according to source data path
3. Run from XCode
4. Check output files in path specified by "Config.h"
========================================================================
Main conponents/modules?

1. ResourceLoader:
load one %d_data and corresponding %d_index file into memory, return contents as vectors
2. Parser:
This class will parse page file, write postings to vector and return word count for given page
3. RawPage
This class defines RawPage data structure
4. UrlTable
This class defines URLtable structrue and its functions
5. LexiconTable
This class defines LexiconTable structrue and its functions
6. InvertedIndexGenerater
This class calls other components to implement following functions:
  1) reads source files into mem, parse them into postings,
    aggregate and sort before writing to temp files to disk
  2) merge-sorts temp generated files into one file
  3) scan merged file, aggregate freq, compose final index
    for each word
  4) in the mean time, generate lexicon table, url table
7. vbyte.h
header file containing macros for vbyte compression
8. Config.h
define global configurations
9. main.cpp
driver class
========================================================================
How it works internally?

1. ResourceLoader provides an API vector<RawPage>* getPages(), encapsulates details about how the program get data from source, several
  problems addressed:
  a. maintain a list of files in data source folder, and a pointer recording current file number
  b. on each call to getPages(), ResourceLoader reads next index file to mem, uncompress it, get a list of pointers to corresponding data file
  c. load corresponding data file into mem, uncompress it, using index records to read individual pages, return them as a vector
2. How does the program generate temp index files?
  a. ask ResourceLoader for next bunch of pages
  b. for each page, parse it to get raw postings, append postings to buffer
  c. if buffer size is large enough, call WriteRawPostingToFile() to sort by (word,docID), then write to a new temp file
  d. repeat above steps until no more page to load
3. How does the program merge temp index files?
  Let unix sort do the job, this step will merge all temp files into one intermediate file
4. When to generate UrlTable?
  While generating temp files, keep it in mem, write to file only at last.
5. How final invertedList is generated?
  Scan merged file from former steps, now postings on same word appear consequetively. Iterate through the whole file
  and merge postings, compress and write into file
6. What happens also while generating inv-list?
  generate lexicon file.
========================================================================
Features:

Compression
  Use difference of the docID for the same word to compress the docID
  Use difference of the position for the same word and same docID
File format:
  can select ASCII or BINARY format
========================================================================
Result and Performance
nz2.tar:
Size of inverted index: 275MB
Time used: 180 seconds

about 1GB data from NZ_ALL:
Size of inverted index: 1.37GB
Time used: about 50 minutes

NZ_ALL:
size of inverted index: 2.75GB
Time used: 9190 seconds = 153 minutes = 2.5 hours
========================================================================
Limitation
Didn't compress intermidiate files, take up too large space (27.6GB for 5GB data)
========================================================================