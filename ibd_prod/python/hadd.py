import os
from tempfile import TemporaryDirectory

from prod_util import chunk_list

DEFAULT_CHUNKSIZE = 50

def hadd(paths, outpath):
    inputs = ' '.join(paths)
    # -f forces recreation of the output file
    os.system(f'hadd -f {outpath} {inputs}')

def hadd_chunked(paths, outpath, chunksize=DEFAULT_CHUNKSIZE):
    chunks = chunk_list(paths, chunksize)

    with TemporaryDirectory() as tmpdir:
        def hadd_chunk(i, chunk):
            tmppath = os.path.join(tmpdir, f'tmp.{i}.root')
            hadd(chunk, tmppath)
            return tmppath

        tmp_outputs = [hadd_chunk(i, chunk)
                       for i, chunk in enumerate(chunks)]

        os.system('mkdir -p ' + os.path.dirname(outpath))
        hadd(tmp_outputs, outpath)
