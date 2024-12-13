#!/usr/bin/env python3
import argparse
import subprocess
import psutil
import datetime
import sys
import time

def run_servers(args,n: int):
    processes = []
    for rank in range(n):
        processes.append(run_server(args,rank))
    return processes


def run_server(args,rank: int):
    server_port = args.port_start + rank
    print(f'Starting server rank {rank} listening on port {server_port}')
    time = datetime.datetime.today()
    formated_time = time.strftime("%m_%d_%H_%M_%S_%f");
    logfilename = args.log_folder + "/sim_" + str(rank) + "_" + formated_time + ".out";    
    logfile = open(logfilename,'w+')
    # parameters
    # [1] path to the parameter file
    # [2] server port
    if(not args.xvfb) :
        return [subprocess.Popen([#"gdb","--batch","--ex=r","--ex=bt","--args",
            args.vrep,
            'simulation','-h',
            f'-g{args.params}',
            f'-gREMOTEAPISERVERSERVICE_{server_port}_TRUE_TRUE',
        ],stdout=logfile,stderr=logfile),logfile]
    else :
        print("run with xvfb")
        return [subprocess.Popen(['xvfb-run','--auto-servernum','--server-num=1',
         # "gdb","--ex=r","--args",
            args.vrep,
            'simulation','-h',
            f'-g{args.params}',
            f'-gREMOTEAPISERVERSERVICE_{server_port}_TRUE_TRUE',
        ],stdout=logfile,stderr=logfile),logfile]


   
def run_client(args):
    print('Starting client')
    time = datetime.datetime.today()
    formated_time = time.strftime("%m_%d_%H_%M_%S_%f");
    logfilename = args.log_folder + "/client_" + formated_time + ".out";
    logfile = open(logfilename,'w+')
    return subprocess.Popen([#"gdb","--args",
        args.client,
        str(args.params),
        str(args.port_start),
        str(args.n_vrep),
    ],stdout=logfile,stderr=logfile)


def wait(servers, client, timeout=None):
    print("WAIT")
    for i, server in enumerate(servers):
        ret = server[0].wait(timeout=timeout)
        print(f'v-rep rank {i} finished with code {ret}')
    if client is not None:
        ret = client.wait(timeout=timeout)
        print(f'Client finished with code {ret}')

def poll(servers):
    for i in range(len(servers)):
        ret = servers[i][0].poll()
        if(ret is not None):
            print(f'Restarting server rank {i}') 
            servers[i][0] = subprocess.Popen(servers[i][0].args,stdout=servers[i][1],stderr=servers[i][1])


def kill(servers, client):
    print("KILL")
    processes = []
    for s in servers:
        processes.append(s[0])
    if client is not None:
        processes += [client]
    for p in processes:
        if p.stdout is not None:
            p.stdout.close()
        p.terminate()
        p.wait()
       
    #try:
    #    wait(servers, client, timeout=30)
    #except subprocess.TimeoutExpired:
    #    for p in processes:
    #        p.poll()
    #        if p.returncode is not None:
    #            parent = psutil.Process(p.pid)
    #            for child in parent.children(recursive=True):
    #                child.kill()
    #            p.kill()


def main():
    servers = []
    client = None
    try:
        import time
        servers = run_servers(args,args.n_vrep)
        client = run_client(args)
        time.sleep(1)

    except:
        import traceback
        traceback.print_exc()
        import sys
        print(f'Exception occurred: killing processes', file=sys.stderr)
        kill(servers, client)

    finally:
        try:
            while(client.poll() is None):
                poll(servers)
                time.sleep(0.1)
            kill(servers, client)
            wait(servers, client)
        except KeyboardInterrupt:
            kill(servers, client)
            wait(servers, client)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('n_vrep', metavar='N', type=int,
                        help='Number of VREP instances')
    
    parser.add_argument('--xvfb',type=int,default=0,help='run with xvfb')

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


    parser.add_argument('--log-folder', type=str,
                        default='.',
                        help='path where to store the output files')

    args = parser.parse_args()
    main()
    if(args.xvfb):
        subprocess.call(["killall","-9","Xvfb"])
