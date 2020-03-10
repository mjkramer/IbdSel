#!/usr/bin/env python3
"""
Provides a buffer between this KNL node's workers and the
lockfile-controlled resources (input/done list). See bash/stage1_job.sh for
invocation.

"""

import argparse
from multiprocessing import Process
import zmq

from prod_io import ListReaderBase, ListWriterBase, LockfileListReader, LockfileListWriter
from prod_util import job_chunksize, buffer_timeout_mins, sockdir, unbuf_stdout

INPUTREADER_SOCK_NAME = 'InputReader'
DONELOGGER_SOCK_NAME = 'DoneLogger'

# ------------------------------ Client code ------------------------------

class ZmqListReader(ListReaderBase):
    def __init__(self, sockname=INPUTREADER_SOCK_NAME, **kwargs):
        super().__init__(**kwargs)

        ctx = zmq.Context()
        self.sock = ctx.socket(zmq.REQ)
        self.sock.connect('ipc://%s/%s.ipc' % (sockdir(), sockname))

    def __next__(self):
        self._check_timeout()
        self.sock.send_string('')
        item = self.sock.recv_string()
        if item == '':
            raise StopIteration
        return item

class ZmqListWriter(ListWriterBase):
    def __init__(self, sockname=DONELOGGER_SOCK_NAME, **kwargs):
        super().__init__(**kwargs)

        ctx = zmq.Context()
        self.sock = ctx.socket(zmq.PUSH)
        self.sock.connect('ipc://%s/%s.ipc' % (sockdir(), sockname))

    def _do_put(self, line):
        self.sock.send_string(line)

# ------------------------------ Server code ------------------------------

class InputBuffer:
    def __init__(self, infile, chunksize, timeout_mins=None):
        ctx = zmq.Context()
        self.sock = ctx.socket(zmq.REP)
        self.sock.bind('ipc://%s/%s.ipc' % (sockdir(), INPUTREADER_SOCK_NAME))

        self.reader = LockfileListReader(infile, chunksize=chunksize, timeout_mins=timeout_mins)
        self.done = False       # to avoid grabbing the lock when we know it's all over

    def serve(self):
        while True:
            msg = self.sock.recv_string()

            if msg == 'QUIT':
                return

            item = ''

            if not self.done:
                try:
                    item = next(self.reader)
                except StopIteration:  # timed out or drained input
                    self.done = True

            self.sock.send_string(item)

class OutputBuffer:
    def __init__(self, outfile, chunksize):
        ctx = zmq.Context()
        self.sock = ctx.socket(zmq.PULL)
        self.sock.bind('ipc://%s/%s.ipc' % (sockdir(), DONELOGGER_SOCK_NAME))

        self.writer = LockfileListWriter(outfile, chunksize=chunksize)

    def serve(self):
        with self.writer:  # ensure cache is flushed
            while True:
                item = self.sock.recv_string()

                if item == 'QUIT':
                    return

                self.writer.put(item)

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('infile')
    args = ap.parse_args()

    def serve_inbuf():
        ib = InputBuffer(args.infile, job_chunksize(),
                         timeout_mins=buffer_timeout_mins())
        ib.serve()

    def serve_outbuf():
        donefile = args.infile + '.done'
        ob = OutputBuffer(donefile, job_chunksize())
        ob.serve()

    Process(target=serve_inbuf).start()
    Process(target=serve_outbuf).start()

if __name__ == '__main__':
    unbuf_stdout()
    main()
