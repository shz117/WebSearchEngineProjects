import config
import urllib2

def get_4sq_page(var_type, id):
    return get_page(config.apiurl%(var_type, id))

def get_page(url):
    try:
        response =  urllib2.urlopen(url,timeout=3)
    except:
        print "Failed to open API response!"
        return ""
    return response.read()
