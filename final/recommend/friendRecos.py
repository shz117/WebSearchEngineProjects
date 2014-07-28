__author__ = 'JeremyShi'
"""
    reference https://github.com/harn/friendRecommendations/blob/master/FriendRecomendationExample.py
"""
import os
import operator
import networkx as nx
import config
G=nx.Graph()

def add_nodes():
    with open(os.path.join(config.DATAPATH,config.USERFILE),'r') as f:
        next(f)
        for line in f:
            attrs=line.rstrip().split(' ')
            G.add_node(attrs[0])

def add_edges():
    with open(os.path.join(config.DATAPATH,config.FRIENDFILE),'r') as f:
        next(f)
        for line in f:
            [userid1,userid2]=line.rstrip().split(' ')
            G.add_edges_from([(userid1,userid2)])

def friends(graph, user):
    return set(graph.neighbors(user))

def friends_of_friends(graph,user,radius):
    return set(nx.ego_graph(G,user,radius,False).nodes()) - friends(G,user)

def common_friends(graph, user1, user2):
    return friends(graph,user1).intersection(friends(graph,user2))

def _number_of_common_friends_map(graph, user):
    friends = friends_of_friends(graph,user,2)
    bigList = dict()
    for friend in friends:
        bigList[friend] = len(common_friends(graph,user,friend))
    return bigList

def _number_map_to_sorted_list(map):
	return sorted(map.iteritems(), key=operator.itemgetter(1,0),reverse=True)

def recommend_by_number_of_common_friends(graph, user,top):
   return _number_map_to_sorted_list(_number_of_common_friends_map(graph,user))[:top]

def _influence_map(graph, user):
    friendsList = friends_of_friends(graph,user,2)
    bigList = dict()
    for friend in friendsList:
        commons = common_friends(graph,user,friend)
        bigList[friend] = 0
        for common in commons:
            bigList[friend] += (1.0/len(friends(graph,common)))
    return bigList

def recommend_by_influence(graph, user,top):
    return _number_map_to_sorted_list(_influence_map(graph,user))[:top]

def main():
    add_nodes()
    add_edges()
    recos=recommend_by_number_of_common_friends(G,'80429764',10)
    if not recos:
        print 'shit'
    print "recommended by number of common_friends:"
    for rec in recos:
        print rec
    recos=recommend_by_influence(G,'80429764',10)
    print 'recommended by influence'
    for rec in recos:
        print rec

main()