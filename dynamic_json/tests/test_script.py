#!/usr/bin/env python

import json

config = {
    'number' : 42,

    'animal' : "bunny",

    'mylist' : [1, 2, 3, 4],

    'mydict' : { "Elly"    : 31,
                 "Stoyan"  : 35,
                 "Malvina" : 29,
    },

    'conf' : { 'complex' : { 'list' : [6,7,8,9],
                             'dict' : { 'A' : 1,
                                        'B' : 2,
                             }
             }
    },

    'rlist' : [ [1,2], [3,4] ],
}


print json.dumps(config, indent=3, sort_keys=True)
