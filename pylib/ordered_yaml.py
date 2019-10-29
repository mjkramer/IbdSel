from __future__ import print_function

import yaml
from collections import OrderedDict

def ordered_load(stream, Loader=yaml.Loader, object_pairs_hook=OrderedDict):
    """
    Ordered dict YAML loader
    from: https://stackoverflow.com/questions/5121931/in-python-how-can-you-load-yaml-mappings-as-ordereddicts
    """
    class OrderedLoader(Loader):
        pass
    def construct_mapping(loader, node):
        loader.flatten_mapping(node)
        return object_pairs_hook(loader.construct_pairs(node))
    OrderedLoader.add_constructor(
        yaml.resolver.BaseResolver.DEFAULT_MAPPING_TAG,
        construct_mapping)
    return yaml.load(stream, OrderedLoader)

def ordered_dump(data, stream=None, Dumper=yaml.Dumper, **kwds):
    class OrderedDumper(Dumper):
        pass
    def _dict_representer(dumper, data):
        return dumper.represent_mapping(
            yaml.resolver.BaseResolver.DEFAULT_MAPPING_TAG,
            data.items())
    OrderedDumper.add_representer(OrderedDict, _dict_representer)
    return yaml.dump(data, stream, OrderedDumper, **kwds)

# from glob import glob
# from os.path import splitext, basename

# cuts_fmt = 'config/cuts/{name}.yaml'
# cuts_mask = cuts_fmt.format(name='*')

# def available_cutsets():
    # return [splitext(basename(filename))[0] for filename in glob(cuts_mask)]

# def load_cutset(name, verbose=False):
    # filename = cuts_fmt.format(name=name)
    # data = ordered_load(open(filename, 'r'))

    # if verbose:
        # print('Load cuts:', name)

        # if verbose:
            # print(ordered_dump(data))

    # return data
