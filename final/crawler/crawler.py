###
### crawler.py
###
###

import json
from webcorpus import WebCorpus
from getpage import get_page, get_4sq_page
from bs4 import BeautifulSoup
import tweepy
import time
 
consumer_key = 'SXnbJLl01FUwbDdHhfLa1hEKL'
consumer_secret = 'aw98KXosbl4SCQoMXmVpHR0cEOsxnZriQZyNWPKdFtN3mgcQLN'
access_token = '35389991-nzPLU5KJxgWGMmFK9cqM1nT6iex2vpGRG2JGNIF7C'
access_token_secret = 'higyds4naaCTzWJz3oauzZ5CfzWC8is93FfIqKmFOq5Hy'

auth = tweepy.OAuthHandler(consumer_key, consumer_secret)
auth.set_access_token(access_token, access_token_secret)

api = tweepy.API(auth)

def write_dat(filename, content):
    try:
        with open('../data/' + filename + '.dat', 'a') as fout:
            fout.write(' '.join([str(word) for word in content]) + '\n')
            fout.close()
    except IOError, e:
        print "Cannot write out: " + str(e)

user = api.me()
def put_venue(venueid):
    content = get_4sq_page('venues', venueid)
    venue = json.loads(content)['response']['venue']
    #print '_'.join(venue['name'].split())
    #print content
    lat = ''
    lng = ''
    address = ''
    if 'location' in venue:
        location = venue['location']
        lat = location['lat'] if 'lat' in location else 0
        lng = location['lng'] if 'lng' in location else 0
        #address = '_'.join([location['address'], location['crossStreet'], location['postalCode'], location['cc'], location['city'], location['state'], location['country']]).replace(' ', '_')
        address = location['address'] if 'address' in location else 'address'
    #print '_'.join(location.values())
    #print '_'.join([location['address'], location['crossStreet'], location['cc']])
    #write_dat('venue', [venueid, venue['name'].replace (' ', '_'), location['lat'], location['lng']])
    rating = venue['rating'] if 'rating' in venue else 0
    #print 'venue', [venueid, venue['name'].replace(' ', '_'), lat, lng, rating, address]
    name = venue['name'] if 'name' in venue else 'name'
    write_dat('venue', [venueid, name.replace(' ', '_'), lat, lng, rating, address.replace(' ','_')])

def get_venue(userid, page):
    soup = BeautifulSoup(page)
    try:
        for link in soup.find_all('meta'):
            href = link.get('content')
            if href.startswith('https://foursquare.com/v/'):
                venueid = href.split('/')[-1]

                write_dat('checkin', [userid, venueid])

                put_venue(venueid)
                return
    except:
        pass
    return

def get_4sq(friend_twitter):
    #print friend_twitter
    return [page.entities['urls'] for page in api.user_timeline(id=friend_twitter) if page.source == 'foursquare']

def get_all_friends(user1):
    friends = []
    page = get_4sq_page('users', user1)
    user = json.loads(page)['response']['user']
    count = 0
    if 'checkins' in user:
        checkins = user['checkins']
        if 'count' in checkins:
            count = checkins['count']
    #print user, count
    firstName = user['firstName'] if 'firstName' in user else 'firstName'
    lastName = user['lastName'] if 'lastName' in user else 'lastName'
    homeCity = user['homeCity'] if 'homeCity' in user else 'homeCity'
    write_dat('user', [user1, firstName, lastName, homeCity.replace(' ','_'), count])
    for group in user['friends']['groups']:
        for item in group['items']:
            user2 = item['id'] if 'id' in item else 'user2'
            friends.append(item['id'])

            write_dat('friendship', [user1, user2])

            if 'contact' in item:
                contact = item['contact']
                if 'twitter' in contact:
                    try:
                        for t_urls in get_4sq(contact['twitter']):
                            for t_url in t_urls:
                                #print t_url['expanded_url']
                                get_venue(user2, get_page(t_url['expanded_url']))
                    except tweepy.TweepError:
                        time.sleep(60 * 2)
                        continue
                    except StopIteration:
                        break
                    except:
                        pass
    return friends

def crawl_web(seed):
    tocrawl = set([seed])
    crawled = []
    corpus = WebCorpus()
    while tocrawl:
        user1 = tocrawl.pop()
        if user1 not in crawled:
            friends = get_all_friends(user1)
            #corpus.add_friend(id, friends)
            tocrawl.update(friends)
            crawled.append(user1)
    return crawled

# def crawl_web(seed): # returns index, graph of inlinks
#     tocrawl = set([seed])
#     crawled = []
#     corpus = WebCorpus()
#     while tocrawl and len(crawled)<1000:
#         id = tocrawl.pop() # changed page to url - clearer name
#         if id not in crawled:
#             content = get_page(id)
#             friends = get_all_friends(content)
#             corpus.add_friend(id, friends)
#             tocrawl.update(friends)
#             crawled.append(id)
#     return crawled
