import os
import subprocess
import argparse

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--log_repository', type=str, required=True, help='Path to the logs repository folder')
    parser.add_argument('--exp_name', type=str, default='me2im', help='Experiment name filter')
    args = parser.parse_args()

    exec_path = os.path.abspath(os.path.join(os.path.dirname(__file__), '../..', 'build', 'experiments', 'me2im', 'generate_grid_archive'))

    for folder in os.listdir(args.log_repository):
        if not folder.startswith(args.exp_name):
            continue
        print(folder)
        log_folder = os.path.join(args.log_repository, folder)
        
        cmd = [exec_path, log_folder]
        subprocess.run(cmd, check=True) 
