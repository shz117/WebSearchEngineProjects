###
### webcorpus.py
###

import graph

class WebCorpus:
    """
    Represents the result of a web crawl.
    """

    def __init__(self):
        self.graph = graph.Graph()

    def add_friend(self, id, friends):
        """Add the url, content, and outlinks to the index."""
        self.graph.add_node(id)
        for friend in friends:
            self.graph.add_node(friend)
            self.graph.add_edge(id, friend)
