import tornado.ioloop
import tornado.web
import tornado.options
import tornado.httpserver
import tornado.autoreload
from tornado.options import define, options
import queryhandler

define("port", default=8080, help="run on the given port", type=int)

def main():
    tornado.options.parse_command_line()
    application = tornado.web.Application([
        (r"/", queryhandler.QueryHandler),
        ])
    http_server = tornado.httpserver.HTTPServer(application)
    http_server.listen(options.port)
    print "server started."
    loop=tornado.ioloop.IOLoop.instance() 
    tornado.autoreload.start(loop)
    loop.start()

if __name__ == "__main__":
    main()