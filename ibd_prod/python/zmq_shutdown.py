#!/usr/bin/env python3

import os
import time
import zmq

from prod_util import sockdir
from zmq_fan import INPUTREADER_SOCK_NAME, DONELOGGER_SOCK_NAME

def main():
    ctx = zmq.Context()

    def send_quit(name, socktype):
        sockpath = f'{sockdir()}/{name}.ipc'
        sock = ctx.socket(socktype)
        sock.connect(f'ipc://{sockpath}')
        sock.send_string('QUIT')

        while os.path.exists(sockpath):
            time.sleep(0.5)

    send_quit(INPUTREADER_SOCK_NAME, zmq.REQ)
    send_quit(DONELOGGER_SOCK_NAME, zmq.PUSH)

    print('Buffer servers told to shut down')

if __name__ == '__main__':
    main()
