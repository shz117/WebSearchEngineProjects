#=========================#
#  project configurations #
#=========================#

#=====crawler configurations=======#
# Foursquare API Limitations:
# https://developer.foursquare.com/overview/ratelimits
# An application can make up to 5,000 userless requests per hour to venues/* endpoints.
# An application can make up to 500 userless requests to all other endpoints groups per hour.
# 3600s/5000r = 0.72 s/r

apiurl='https://api.foursquare.com/v2/%s/%s?v=040914&oauth_token=MGRUDACH5ZCGPSEA5PJIOH05UKOUT1WYMRNDIHPVX5GV0TMA'
userFile='users.dat'
