#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""Define converters to convert between Python (numpy) and C++ (std::vecotr, Eigen, etc) types
Implements individual converters along with generic 'convert' function."""

from __future__ import print_function
from load import ROOT as R

# List all converters in dict: converters['from']['to']

def save_converter( from_type, to_type ):
    """Make a decorator to store converter in a converters dictionary based on from/to types"""
    def decorator( converter ):
        fts, tts = [from_type], [to_type]
        if from_type in nicknames:
            fts.append( nicknames[from_type] )
        if to_type in nicknames:
            tts.append( nicknames[to_type] )
        for ft in fts:
            for tt in tts:
                converters[ft][tt] = converter
        return converter
    return decorator

def get_cpp_type( array ):
    """Guess appropriate C++ type to store data based on array.dtype or type"""
    if hasattr( array, 'dtype' ):
        typemap = {
                'int32':   'int',
                'float64': 'double',
                'float32': 'float',
                # 'uint64':  'size_t',
                }
        atype = array.dtype.name
    else:
        typemap = {
                int: 'int',
                float: 'double',
                str: 'std::string',
                'int': 'int',
                'float': 'double',
                'str': 'std::string',
                'variable<double>': 'variable<double>',
                'variable<float>': 'variable<float>',
                }
        atype = type( array[0] ).__name__
    ret = typemap.get( atype )
    if not ret:
        raise Exception( 'Do not know how to convert type '+str(atype) )
    return ret

@save_converter( list, R.vector )
def list_to_stdvector( lst, dtype='auto' ):
    """Convert a list to the std::vector<dtype>"""
    if dtype=='auto':
        dtype = get_cpp_type( lst )
    ret = R.vector(dtype)()
    ret.reserve(len(lst))
    for v in lst:
        ret.push_back(v)
    return ret
