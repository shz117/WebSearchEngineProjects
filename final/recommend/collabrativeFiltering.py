from __future__ import division
__author__ = 'JeremyShi'


import config
import os
import sys
import math
import random

VENUES=set()

def printRecos(recos):
    global VENUES
    for rec in recos:
        print VENUES[rec]

def sample(all, fraction):
    return random.sample(all, int(math.ceil(fraction * len(all))))

def addCheckin(CHECKINS_PER_USER,userid,venueid):
    """
        CHECKINS_PER_USER {user1: set of venueids for user1
                            ...
                        }
        ignore duplicated checkins
    """
    checkins=CHECKINS_PER_USER.get(userid)
    if not checkins:
        checkins=set()
        checkins.add(venueid)
    elif venueid in checkins:
        pass
    else:
        checkins.add(venueid)
    CHECKINS_PER_USER[userid]=checkins

def loadVenues():
    global VENUES
    VENUES=dict()
    with open(os.path.join(config.DATAPATH,config.VENUEFILE),'r') as f:
        next(f)
        for line in f:
            [venueid,name,lat,longt,rating,address]=line.rstrip().split(' ')
            VENUES[venueid]=name

def loadCheckins():
    CHECKINS_PER_USER=dict()
    with open(os.path.join(config.DATAPATH,config.CHECKINFILE),'r') as f:
        next(f)
        for line in f:
            [userid,venueid]=line.rstrip().split(' ')
            addCheckin(CHECKINS_PER_USER,userid,venueid)

    return CHECKINS_PER_USER

def cosine(CHEKINS_PER_USER, u1, u2):
    common = CHEKINS_PER_USER[u1].intersection(CHEKINS_PER_USER[u2])
    try:
        return len(common) / ( math.sqrt(len(CHEKINS_PER_USER[u1])) * math.sqrt(len(CHEKINS_PER_USER[u2])) )
    except ZeroDivisionError as zerr:
        print CHEKINS_PER_USER[u1], CHEKINS_PER_USER[u2]
        sys.exit(-1)

def neighborhood(CHEKINS_PER_USER, train_set, u):
    return dict({(n, cosine(CHEKINS_PER_USER, u, n)) for n in train_set})

def knn(CHEKINS_PER_USER, tv_watchers, test_set, user):
    neighbors = neighborhood(CHEKINS_PER_USER, tv_watchers.difference(test_set), user)
    return sorted([(k, v) for (k, v) in neighbors.iteritems() if v > 0.0], key=lambda tup: -tup[1]) # '-' sign to do reverse sort

def recommendations(CHEKINS_PER_USER, neighborhood, max_neighbors=10):
    recos = set([])
    for (neighbor, score) in neighborhood[:max_neighbors]:
        recos = recos.union(CHEKINS_PER_USER[neighbor])
    return recos

def evaluate(CHEKINS_PER_USER, active_users, test_set, user):
    """
    For the given test user, temporarily hold back half the Checkins he had;
    then train (compute neighborhood) using the rest of the data. Next, generate
    recommendations for the test user and see how they compared to the held out set.
    """
    sample_size = int(math.ceil(0.5 * len(CHEKINS_PER_USER[user])))
    held_out = random.sample( CHEKINS_PER_USER[user], sample_size )
    CHEKINS_PER_USER[user] = CHEKINS_PER_USER[user].difference(held_out)
    neighborhood = knn(CHEKINS_PER_USER, active_users, test_set, user)
    recos = recommendations(CHEKINS_PER_USER, neighborhood)
    printRecos(recos)
    if len(recos) == 0: return (0, 0)
    # precision = fraction of recos that are purchased
    precision = len(recos.intersection(CHEKINS_PER_USER[user].union(held_out))) / len(recos)
    # recall = fraction of purchased items returned by the recommender
    recall = len(recos.intersection(held_out)) / len(held_out)
    # restore user's shows
    CHEKINS_PER_USER[user] = CHEKINS_PER_USER[user].union(held_out)
    return precision, recall

def main():
    # load venues data
    global VENUES
    loadVenues()
    #get checkins per user
    CHECKINS_PER_USER=loadCheckins()

    total_precision = 0.0
    total_recall = 0.0
    for i in xrange(10):
        active_users = set([u for u in CHECKINS_PER_USER.keys() if len(CHECKINS_PER_USER[u])>1])
        test_set = set(sample(active_users,0.3))
        aggr_precision = 0.0
        aggr_recall = 0.0
        for u in test_set:
            (precision, recall) = evaluate(CHECKINS_PER_USER, active_users, test_set, u)
            aggr_precision += precision
            aggr_recall += recall
        total_precision += aggr_precision/len(test_set)
        total_recall += aggr_recall/len(test_set)
    P = total_precision/10
    R = total_recall/10
    print P, R#, (2 * P * R)/(P + R)




if __name__ == '__main__':
    main()