"""
Singleton for status messages
"""

class StatusObject(object):
    """
    print status messages

    """


    def __init__(self):
        pass

    def __call__(self, message):
        print "message:", message

Message = StatusObject()

    
        
        
