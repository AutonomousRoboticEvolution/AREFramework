# Docker documentation

You can use the Docker environmnet to run your experiments. The Docker image includes the libraries and the ARE Framework.

There are two main ways of using the Docker images. 
* Download the image from Docker hub
* Build the image locally

You need to install Docker in your system.

## Downloading the image from Docker Hub

Pull the docker image from https://hub.docker.com/r/artificialsimon/areframework. This will download the image and will store it on your computer.
```
$ docker pull artificialsimon/areframework:melai
```

## Building with Docker

You can build your own version of the image defined in the Dockerfile available on this repo. N_CPU is the number of CPU to use to build the image, default value is 1. You can use any name for the image, here we are using the same as the one in the Docker hub.

From the root directory of the repo do:
```
$ sudo docker build --build-arg="N_CPU=20" -t artificialsimon/are_framework:melai docker/.
```

## Running the image

After downloading or building the image you should have it on your host:

```
$ docker images
REPOSITORY                      TAG                       IMAGE ID       CREATED             SIZE
artificialsimon/are_framework   melai                     bb5f9041e6cf   About an hour ago   4.61GB
```

You can run the image in interactive mode for development.
* You need to configure your host to allow the graphic interface.
* You need to mount your local files to maintain any changes and output files generated when running the container.

### Allowing the GUI

To allow graphic interface (for example, to visualise results on the V-Rep simulator), you need to allow local access to the dockers user group to xhost:
```
$ sudo xhost +local:docker

```

### Running the image as a new container

This line will mount the required directories for graphical use (tmp/.X11-uni), tmp directory to store logs of the experiments and your local version of the ARE Framework to run the experiments:

```
$ docker run --rm -it --env DISPLAY=$DISPLAY --volume /tmp/.X11-uni:/tmp/.X11-unix --volume /tmp:/tmp --volume host_path/AREFramework:/are/AREFramework --net=host artificialsimon/are_framework:melai /bin/bash
```

The command should leave you inside a bash shell.

### Test the graphic interface
You can try running xclock to test the graphic interface from inside the container.

```
$ xclock
```

### Running experiments

The Docker image installs the ARE Framework client on /usr/local that is available on your $PATH variable. Remember to change the directories on your parameters.cv file to match the Docker image installation, for example:

```
#scenePath,string,/are/AREFramework/simulation/models/scenes/exp_side_obstacles_hard.ttt
```

To run experiment MEIM with 5 threads:

```
$ cd /are/AREFramework/simulation/Cluster
$ python3 run_cluster.py 5 --vrep ../../vrep-3.6.2/are_sim.sh --client are-client --params /are/AREFramework/experiments/meim/parameters.csv --port-start 10000 --xvfb 0
```


### Checking results with Jupyter notebook

On the folder /experiments/scripts/ you can find examples of Jupyter notebooks to check results. Copy one of the .ipynb, and run Jupyter from within the container:
```
$ jupyter notebook --no-browser --allow-root
```
For the local files import, Jupyter notebook has to be run from the scripts folders /are/AREFramework/experiments/scripts/

You can open the Jupyter web client at localhost:8888