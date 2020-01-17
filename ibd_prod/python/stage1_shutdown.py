#!/usr/bin/env python3

import argparse
import zmq

from queue_buffer import INPUTREADER_SOCK_NAME, DONELOGGER_SOCK_NAME

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('sockdir')
    args = ap.parse_args()

    ctx = zmq.Context()

    def send_quit(name, socktype):
        sock = ctx.socket(socktype)
        sock.connect('ipc://%s/%s.ipc' % (args.sockdir, name))
        sock.send_string('QUIT')

    send_quit(INPUTREADER_SOCK_NAME, zmq.REQ)
    send_quit(DONELOGGER_SOCK_NAME, zmq.PUSH)

    print('Buffer servers told to shut down')

if __name__ == '__main__':
    main()
