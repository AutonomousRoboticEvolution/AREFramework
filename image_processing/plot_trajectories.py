import os
import matplotlib.pyplot as plt

def plot_all_learning_paths():
    folder_path = "/home/robofab/are-logs/hardware_test/"
    target_position = [0.75,0.75]

    # for each robot_infos_ file in the given folder
    for filename in os.listdir(folder_path):
        if filename.startswith("robot_infos_"):

            robot_ID = filename.split("_")[-1]
            print("Making plot for file {} (robot {})".format(filename,robot_ID))

            ## extract trajectories from files
            xs = [ ]
            ys = [ ]
            fitnesses=[]
            evaluation_numbers=[]
            start_of_trajectory = False
            with open ( folder_path + '/' + filename ) as file:
                lines = file.readlines ()
                evaluation_number=0
                for line in lines:
                    str_pos = line.split ( ';' ) [ 0 ].split ( ',' )
                    # we expect the file to take this format:
                    # evaluation_1
                    # fitnesses;[value];
                    # trajectory;
                    # [x],[y],0;0,0,0
                    # [x],[y],0;0,0,0
                    # ...
                    # evaluation_2
                    # fitnesses;[value];
                    # 0.67448;
                    # trajectory;
                    # [x],[y],0;0,0,0
                    # [x],[y],0;0,0,0
                    # ...
                    if line.startswith("evaluation_"): # new evaluation of this robot
                        new_evaluation_number = int( line.split("_")[-1].replace("\n","") )# the "replace" removes newline character
                        if new_evaluation_number != evaluation_number+1:
                            print("WARNING evaluation numbers not sequential - {} follows {} in robot {}".format(new_evaluation_number,evaluation_number,robot_ID))
                        evaluation_number = new_evaluation_number
                        evaluation_numbers.append(evaluation_number)
                        start_of_trajectory=False
                        #print("evaluation {}".format(evaluation_number))

                    elif line.startswith("fitness"):
                        fitnesses.append( float( line.split(";")[-2] ))
                        #print("fitness: {}".format(fitnesses[-1]))

                    elif line == "trajectory;\n":
                        start_of_trajectory = True
                        xs.append( [] ) # add new trajectory
                        ys.append( [] ) # add new trajectory

                    else:
                        ## should be a trajectory value
                        assert(start_of_trajectory) # check things have happened in the order we expect
                        xs[-1 ].append( float( line.split( "," )[0 ] ) )
                        ys[-1 ].append( float( line.split( "," )[1 ] ) )


            ## set up figure
            plt.figure()
            plt.xlim(-1.1,1.1)
            plt.ylim(-1.1,1.1)
            plt.gca().axis('equal')
            plt.plot( [-1,1,1,-1,-1],[-1,-1,1,1,-1],'k-')
            plt.plot(target_position[0],target_position[1],'kx',markersize=10,label="Target")

            ## plot
            for i in range( len(evaluation_numbers)):
                proportion = i/(len(evaluation_numbers)-1)
                if i==0 or i == len(evaluation_numbers)-1: ## first or last, add label
                    plt.plot( xs[i], ys[i ] , color=[proportion,0,1-proportion], label = "Evaluation {}".format(i))
                else: # same but no label
                    plt.plot( xs[i], ys[i ] , color=[proportion,0,1-proportion])
                # plt.text(xs[i][-1],ys[i][-1],"{}".format(fitnesses[i]),va="centre",ha="centre")

            plt.legend(loc="upper right")
            plt.title("Robot {}".format(robot_ID))
            plt.show()


def compare_sim_to_sim2real():

    robot_ID = "22"
    folder_path = "/home/robofab/robotlibrary/morph_"+robot_ID

    x_sim = []; y_sim = []
    x_sim2real = []; y_sim2real = []

    with open(folder_path + '/sim_traj_' + robot_ID) as file:
        lines = file.readlines()
        for line in lines:
            str_pos = line.split(';')[0].split(',')
            x_sim.append(float(str_pos[0]))
            y_sim.append(float(str_pos[1]))

    with open(folder_path + '/sim2real_traj_' + robot_ID) as file:
        lines = file.readlines()
        for line in lines:
            str_pos = line.split(';')[0].split(',')
            print(str_pos)
            x_sim2real.append(float(str_pos[0]))
            y_sim2real.append(float(str_pos[1]))

    # set up figure
    plt.figure()
    plt.xlim(-1.1, 1.1)
    plt.ylim(-1.1, 1.1)
    plt.gca().axis('equal')
    plt.plot([-1, 1, 1, -1, -1], [-1, -1, 1, 1, -1], 'k-')

    ## plot
    plt.plot(x_sim, y_sim, label="simulation")
    plt.plot(x_sim2real, y_sim2real, label="sim to real")

    # plt.text(xs[i][-1],ys[i][-1],"{}".format(fitnesses[i]),va="centre",ha="centre")

    plt.legend(loc="upper right")
    plt.title("Robot {}".format(robot_ID))
    plt.show()


compare_sim_to_sim2real()