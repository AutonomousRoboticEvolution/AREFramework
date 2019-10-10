#!/usr/bin/env python3
import argparse
import subprocess


def run_servers(n: int):
    processes = []
    for rank in range(n):
        processes.append(run_server(rank))
    return processes


def run_server(rank: int):
    server_port = args.port_start + rank
    print(f'Starting server rank {rank} listening on port {server_port}')

    # parameters
    # [1] rank
    # [2] type of run
    #       - 1 SERVER
    #       - 9 RECALL
    # [3] repository
    return subprocess.Popen([
        args.vrep,
        '-h',
        f'-g{rank}',
        '-g1',
        f'-g{args.repository}',
        f'-gREMOTEAPISERVERSERVICE_{server_port}_TRUE_TRUE',
    ])


def run_client():
    print('Starting client')
    return subprocess.Popen([
        args.client,
        args.repository,
        str(args.seed),
        str(args.n_vrep),
    ])


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('n_vrep', metavar='N', type=int,
                        help='Number of VREP instances')

    parser.add_argument('repository', metavar='REPOSITORY', type=str,
                        help='name of the repository to use')

    parser.add_argument('--seed', type=int,
                        default=0,
                        help='random seed')

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
    servers = run_servers(args.n_vrep)
    client = run_client()

    import time
    time.sleep(1)

    for i, server in enumerate(servers):
        ret = server.wait()
        print(f'v-rep rank {i} finished with code {ret}')
    ret = client.wait()
    print(f'Client finished with code {ret}')
