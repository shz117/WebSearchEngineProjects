import random

def forgeVenue():
  f=open('venue.dat','a')
  for id in range(1,100):
    idStr=str(id)
    items=[idStr,'venue'+idStr,str(100),str(200),str(round(random.random()*10,1)),'somewhere'+idStr]
    line=' '.join(items)
    f.write(line)
    f.write('\n')
  f.close()

# forgeVenue()

def forgeCheckin(start,offset,userid):
  f=open('checkin.dat','a')

  venues=[x for x in range(1,101)]
  for x in range(1,offset+1):
    id=start+x
    line=' '.join([str(id),str(userid),str(random.sample(venues,1)[0])])
    f.write(line)
    f.write('\n')
  f.close()

forgeCheckin(0,10,1)
forgeCheckin(11,10,2)
forgeCheckin(21,10,3)
forgeCheckin(31,3,4)
forgeCheckin(34,20,5)

