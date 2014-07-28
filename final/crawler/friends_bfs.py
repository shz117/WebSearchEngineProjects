###
### test.py
###

from crawler import crawl_web
import datetime

def test_engine():
    print "Testing..."

    start = datetime.datetime.now();

    result = crawl_web('040914')

    print "Finished tests."
    delta = datetime.datetime.now()-start
    print delta/len(result)

test_engine()
