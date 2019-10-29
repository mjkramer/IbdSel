#!/usr/bin/env python
# -*- coding: utf-8 -*-

from load import ROOT as R

numpy_typemap = {
        'int32':   'int',
        'float64': 'double',
        'float32': 'float',
        # 'uint64':  'size_t',
        }
python_typemap = {
        int: 'int',
        float: 'double',
        str: 'std::string',
        'int': 'int',
        'float': 'double',
        'str': 'std::string',
        'variable<double>': 'variable<double>',
        'variable<float>': 'variable<float>',
        }

def get_cpp_type(array):
    """Guess appropriate C++ type to store data based on array.dtype or type"""
    try:
        if hasattr( array, 'dtype' ):
            atype = array.dtype.name
            typemap = numpy_typemap
        else:
            atype = type(next(iter(array))).__name__
            typemap = python_typemap
    except Exception as e:
        raise Exception('Do not know how to convert type of '+str(array))


    ret = typemap.get(atype, None)
    if not ret:
        raise Exception('Do not know how to convert '+str(ret))

    return ret

def stdvector(lst, dtype='auto'):
    """Convert a list to the std::vector<dtype>"""
    if dtype=='auto':
        dtype = get_cpp_type( lst )

    ret = R.vector(dtype)()
    ret.reserve(len(lst))
    for v in lst:
        ret.push_back(v)

    return ret
