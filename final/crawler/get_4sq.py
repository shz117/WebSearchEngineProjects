import tweepy
 
# Consumer keys and access tokens, used for OAuth
consumer_key = 'SXnbJLl01FUwbDdHhfLa1hEKL'
consumer_secret = 'aw98KXosbl4SCQoMXmVpHR0cEOsxnZriQZyNWPKdFtN3mgcQLN'
access_token = '35389991-nzPLU5KJxgWGMmFK9cqM1nT6iex2vpGRG2JGNIF7C'
access_token_secret = 'higyds4naaCTzWJz3oauzZ5CfzWC8is93FfIqKmFOq5Hy'

# OAuth process, using the keys and tokens
auth = tweepy.OAuthHandler(consumer_key, consumer_secret)
auth.set_access_token(access_token, access_token_secret)

# Creation of the actual interface, using authentication
api = tweepy.API(auth)

# Sample method, used to update a status
#api.update_status('Hello Python Central!')
user = api.me()
 
#print('Name: ' + user.name)
#print('Location: ' + user.location)
#print('Friends: ' + str(user.friends_count))

#results = api.search(q='foursquare', since_id=150908960, max_id=150908960)

#for result in results:
#    print result.text

def get_4sq(friend_twitter):
    return [page.entities['urls'] for page in api.user_timeline(id=friend_twitter) if page.source == 'foursquare']
#for page in api.user_timeline(id='MusaTariq'):
#       # page is a list of statuses
#       if page.source == 'foursquare':
#           print page.entities['urls']
