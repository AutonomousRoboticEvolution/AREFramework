#!/usr/bin/env python3
import argparse
import subprocess
import datetime
import sys

def run_servers(n: int):
    processes = []
    for rank in range(n):
        processes.append(run_server(rank))
    return processes


def run_server(rank: int):
    server_port = args.port_start + rank
    print(f'Starting server rank {rank} listening on port {server_port}')
    time = datetime.datetime.today()
    formated_time = time.strftime("%m_%d_%H_%M_%S_%f");
    logfilename = "./sim_" + str(rank) + "_" + formated_time + ".out";    
    logfile = open(logfilename,'w+')
    # parameters
    # [1] path to the parameter file
    # [2] server port
    if(not args.xvbf) :
        return subprocess.Popen([
            args.vrep,
            '-h',
            f'-g{args.params}',
            f'-gREMOTEAPISERVERSERVICE_{server_port}_TRUE_TRUE',
        ],stdout=logfile + ".out",stderr=logfile + ".err")
    else :
        return subprocess.Popen(['xvfb-run','--auto-servernum','--server-num=1',
            args.vrep,
            '-h',
            f'-g{args.params}',
            f'-gREMOTEAPISERVERSERVICE_{server_port}_TRUE_TRUE',
        ],stdout=logfile)


def run_client():
    print('Starting client')
    time = datetime.datetime.today()
    formated_time = time.strftime("%m_%d_%H_%M_%S_%f");
    logfilename = "./client_" + formated_time + ".out";
    logfile = open(logfilename,'w+')
    return subprocess.Popen([
        args.client,
        str(args.params),
        str(args.port_start),
        str(args.n_vrep),
    ],stdout=logfile)


def wait(servers, client, timeout=None):
    for i, server in enumerate(servers):
        ret = server.wait(timeout=timeout)
        print(f'v-rep rank {i} finished with code {ret}')
    if client is not None:
        ret = client.wait(timeout=timeout)
        print(f'Client finished with code {ret}')


def kill(servers, client):
    if client is None:
        processes = servers
    else:
        processes = servers + [client]
    for p in processes:
        p.stdout.close()
        p.terminate()
    try:
        wait(servers, client, timeout=30)
    except subprocess.TimeoutExpired:
        for p in processes:
            p.poll()
            if p.returncode is not None:
                p.kill()


def main():

    servers = []
    client = None
    try:
        servers = run_servers(args.n_vrep)
        client = run_client()

        import time
        time.sleep(1)

    except:
        import traceback
        traceback.print_exc()
        import sys
        print(f'Exception occurred: killing processes', file=sys.stderr)
        kill(servers, client)

    finally:
        try:
            wait(servers, client)
        except KeyboardInterrupt:
            kill(servers, client)
            wait(servers, client)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('n_vrep', metavar='N', type=int,
                        help='Number of VREP instances')
    
    parser.add_argument('--xvbf',type=int,default='0',help='run with xvfb')

    parser.add_argument('--params', type=str,
                        default=0,
                        help='path to parameters file')

    parser.add_argument('--client', type=str,
                        default='ERClient',
                        help='client executable')

    parser.add_argument('--port-start', type=int,
                        default=10400,
                        help='client executable')

    parser.add_argument('--vrep', type=str,
                        default='vrep.sh',
                        help='path to the vrep starting script')

    args = parser.parse_args()
    main()
