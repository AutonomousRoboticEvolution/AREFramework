#! /usr/bin/python3

import zmq
from argparse import ArgumentParser
import subprocess
import time

def wait_for_order(socket):

    message = ""
    while message == "":
        #  Wait for next request from client
        message = socket.recv()
        print("Received request: %s" % message)

        #  Do some 'work'
        time.sleep(1)

    #  Send reply back to client
    socket.send(b"Request received launching the script")
    return True

def launch_command(com,args=[]):
    assert(len(script)!=0)
    subprocess.Popen([com]+args)

if __name__ == '__main__':
    parser = ArgumentParser()
    parser.add_argument("command",type=str,help="a command")
    parser.add_argument("-a","--args",type=str,nargs="+",dest="args",help="the arguments for the command")
    args = parser.parse_args()


    context = zmq.Context()
    socket = context.socket(zmq.REP)
    socket.bind("tcp://*:5555")

    if(wait_for_order(socket)):
        if(args.args == None):
            launch_command(args.script)
        else: 
            launch_command(args.script,args.args)