#!/usr/bin/env python3
"""
Provides a buffer between this KNL node's workers and the
lockfile-controlled resources (input/done list). See bashlib/stage1_job.sh for
invocation.

"""

import argparse
import time
from multiprocessing import Process
import zmq

from prod_util import ParallelListReader, ParallelListWriter

CHUNKSIZE = 600

DONELOGGER_SOCK_NAME = 'DoneLogger'
INPUTREADER_SOCK_NAME = 'InputReader'

class BufferedParallelListReader:
    def __init__(self, sockdir):
        ctx = zmq.Context()
        self.sock = ctx.socket(zmq.REQ)
        self.sock.connect('ipc://%s/%s.ipc' % (sockdir, INPUTREADER_SOCK_NAME))

    def __iter__(self):
        return self

    def __next__(self):
        self.sock.send_string('')
        item = self.sock.recv_string()
        if item == '':
            raise StopIteration
        return item

class BufferedParallelListWriter:
    def __init__(self, sockdir):
        ctx = zmq.Context()
        self.sock = ctx.socket(zmq.PUSH)
        self.sock.connect('ipc://%s/%s.ipc' % (sockdir, DONELOGGER_SOCK_NAME))

    def __enter__(self):
        return self

    def __exit__(self, _exc_type, _exc_value, _traceback):
        pass

    def put(self, line):
        self.sock.send_string(line)

class BufferedDoneLogger(BufferedParallelListWriter):
    def log(self, path):
        tstamp = time.strftime('%Y-%m-%dT%H:%M:%S')
        self.put(tstamp + ' ' + path)

class InputBuffer:
    def __init__(self, sockdir, infile, chunksize, timeout_secs=None):
        ctx = zmq.Context()
        self.sock = ctx.socket(zmq.REP)
        self.sock.bind('ipc://%s/%s.ipc' % (sockdir, INPUTREADER_SOCK_NAME))

        self.plr = ParallelListReader(infile, chunksize=chunksize, timeout_secs=timeout_secs)
        self.done = False       # to avoid grabbing the lock when we know it's all over

    def serve(self):
        while True:
            msg = self.sock.recv_string()

            if msg == 'QUIT':
                return

            item = ''

            if not self.done:
                try:
                    item = next(self.plr)
                except StopIteration:  # timed out or drained input
                    self.done = True

            self.sock.send_string(item)

class OutputBuffer:
    def __init__(self, sockdir, outfile, chunksize):
        ctx = zmq.Context()
        self.sock = ctx.socket(zmq.PULL)
        self.sock.bind('ipc://%s/%s.ipc' % (sockdir, DONELOGGER_SOCK_NAME))

        self.plw = ParallelListWriter(outfile, chunksize=chunksize)

    def serve(self):
        while True:
            item = self.sock.recv_string()

            if item == 'QUIT':
                if self.plw._cache:
                    self.plw._flush()
                return

            self.plw.put(item)

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('sockdir')
    ap.add_argument('infile')
    ap.add_argument('-c', '--chunksize', type=int, default=CHUNKSIZE,
                    help='How many items to pop off or log at once')
    ap.add_argument('-t', '--timeout', type=float, default=18, help='hours')
    args = ap.parse_args()

    def serve_inbuf():
        timeout_secs = None if args.timeout == -1 else 3600 * args.timeout
        ib = InputBuffer(args.sockdir, args.infile, args.chunksize,
                         timeout_secs=timeout_secs)
        ib.serve()

    def serve_outbuf():
        donefile = args.infile[:-4] + '.done.txt'
        ob = OutputBuffer(args.sockdir, donefile, args.chunksize)
        ob.serve()

    Process(target=serve_inbuf).start()
    Process(target=serve_outbuf).start()

if __name__ == '__main__':
    main()
