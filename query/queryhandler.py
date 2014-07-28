# -*- coding: utf-8 -*-  

import tornado.web
import query
import time

inputTemplate = open("template.html").read()
resultTemplate = open("resulttemplate.html").read()

class QueryHandler(tornado.web.RequestHandler):
    def get(self):
        starttime = time.clock()
        queryWords = self.get_argument("query",default=None, strip=False)
        if queryWords is None:
            self.write(inputTemplate)
            return
        else:
            queryWords = queryWords.encode('utf-8')
            page = self.get_argument("page",default=None, strip=False)
            page = "0" if page is None else page
            try:
                page = int(page, 10)
            except:
                page = 0
            q = query.Query()
            result, totalResult = q.queryWords(queryWords, page*10, 10)
            delta = time.clock() - starttime
            output = "results: "+str(totalResult)+"&nbsp;&nbsp;&nbsp;&nbsp;time: "+str(delta)+" seconds <br><br>"
            for item in result:
                output += "<a href=http://"+item.url+">"+item.url+"</a>"+"<br>"+str(item.score)+"<br>"+item.snippet+"<br><br>"
            self.write(resultTemplate%(queryWords, str(page-1), queryWords, str(page+1), output))
            return