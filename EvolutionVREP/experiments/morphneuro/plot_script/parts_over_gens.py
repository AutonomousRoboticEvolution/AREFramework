import csv
import numpy as np
import matplotlib.pyplot as plt

root_path = '/home/miller/Desktop/ARE_Plot/data/elite/ctrl_boot_multi_eval'
exp_method = 'Controller Bootstrap + Multi Evaluation'
exp_trial = ['01', '02', '03', '04', '05', '06', '07', '08', '09', '010']
# exp_trial = ['01', '02', '03', '04', '05', '06', '07', '09', '010'] #middle wall
morph_loc = 'morphdesc.csv'
total_generations = 200
ind_per_gen = 100
total_eval_count = (total_generations + 1) * ind_per_gen
replicate = len(exp_trial)

def count_parts(desc_list, ind_gen, num_runs):
    list = [[r[col] for r in desc_list] for col in range(len(desc_list[0]))]
    cat_list = []
    eval_count = 0
    cat = {0:0, 1:0, 2:0, 3:0, 4:0, 5:0}
    for evals in list:
        for part in evals:
            cat[part] += 1
        if eval_count == ind_gen - 1:
            for i in range(len(cat)):
                cat[i] = cat[i] / num_runs
            cat_list.append(cat)
            eval_count = 0
            cat = {0:0, 1:0, 2:0, 3:0, 4:0, 5:0}
        else:
            eval_count += 1
    return cat_list

def number_of_parts_all_gen(part_dictionarys):
    y0 = [part_dictionarys[i][0] for i in range(len(part_dictionarys))]
    y1 = [part_dictionarys[i][1] for i in range(len(part_dictionarys))]
    y2 = [part_dictionarys[i][2] for i in range(len(part_dictionarys))]
    y3 = [part_dictionarys[i][3] for i in range(len(part_dictionarys))]
    y4 = [part_dictionarys[i][4] for i in range(len(part_dictionarys))]
    y5 = [part_dictionarys[i][5] for i in range(len(part_dictionarys))]
    return y0, y1, y2, y3, y4, y5

def multi_bar_plot(x, y0, y1, y2, y3, y4, y5, name, tick_list):
    #堆叠状图图
    #https://www.cnblogs.com/czz0508/p/10457366.html
    fig = plt.figure()
    ax = fig.add_subplot(111)
    plt.title(name, fontsize=35)
    plt.xlabel('Evaluations', fontsize=35)
    plt.ylabel('Number of Robots', fontsize=35)
    ax.bar(x, y0, align='center', tick_label = tick_list, label = '0')
    ax.bar(x, y1, bottom=y0, align='center', label = '1')
    bottom_step = np.array(y0) + np.array(y1)
    ax.bar(x, y2, bottom=bottom_step, align='center', label = '2')
    bottom_step = bottom_step + np.array(y2)
    ax.bar(x, y3, bottom=bottom_step, align='center', label = '3')
    bottom_step = bottom_step + np.array(y3)
    ax.bar(x, y4, bottom=bottom_step, align='center', label = '4')
    bottom_step = bottom_step + np.array(y4)
    ax.bar(x, y5, bottom=bottom_step, align='center', label = '5')
    plt.legend(loc='upper right')
    plt.tick_params(labelsize=15)
    plt.legend(fontsize=15)
    label_counter = 0
    for label in ax.xaxis.get_ticklabels():
        if label_counter % 10 !=0:
            label.set_visible(False)
        label_counter += 1
    plt.show()
    # plt.savefig('/home/miller/Desktop/parts_plot/every_100_eval_escape_room/' + name+'.png')
    # plt.close()


# Data Collection
wheels_all_b = []
joints_all_b = []
sensors_all_b = []
casters_all_b = []
wheels_all_nb = []
joints_all_nb = []
sensors_all_nb = []
casters_all_nb = []

for run_num in exp_trial:
    morph_abs_path = root_path + '/' + run_num + '/' + morph_loc
    with open(morph_abs_path) as morphdesc_file:
        morphdesc_reader = csv.reader(morphdesc_file)
        wheels = []
        joints = []
        sensors = []
        casters = []
        counter = 0
        for line in morphdesc_reader:
            if counter < total_eval_count:
                wheels.append(float(line[5])*5)
                sensors.append(float(line[6])*5)
                joints.append(float(line[7])*5)
                casters.append(float(line[8])*5)
            counter += 1
    wheels_all_b.append(wheels)
    sensors_all_b.append(sensors)
    joints_all_b.append(joints)
    casters_all_b.append(casters)


wheels_all_b_dict = count_parts(wheels_all_b, ind_per_gen, replicate)
sensors_all_b_dict = count_parts(sensors_all_b, ind_per_gen, replicate)
joints_all_b_dict = count_parts(joints_all_b, ind_per_gen, replicate)
casters_all_b_dict = count_parts(casters_all_b, ind_per_gen, replicate)


wb_0, wb_1, wb_2, wb_3, wb_4, wb_5 = number_of_parts_all_gen(wheels_all_b_dict)
sb_0, sb_1, sb_2, sb_3, sb_4, sb_5 = number_of_parts_all_gen(sensors_all_b_dict)
jb_0, jb_1, jb_2, jb_3, jb_4, jb_5 = number_of_parts_all_gen(joints_all_b_dict)
cb_0, cb_1, cb_2, cb_3, cb_4, cb_5 = number_of_parts_all_gen(casters_all_b_dict)



x_range = [i for i in range(total_generations+1)]
x_range_100 = [i * 100 for i in range(total_generations+1)]
multi_bar_plot(x_range, wb_0, wb_1, wb_2, wb_3, wb_4, wb_5, exp_method + ' Wheels', x_range_100)
multi_bar_plot(x_range, sb_0, sb_1, sb_2, sb_3, sb_4, sb_5, exp_method + ' Sensors', x_range_100)
multi_bar_plot(x_range, jb_0, jb_1, jb_2, jb_3, jb_4, jb_5, exp_method + ' Joints', x_range_100)
multi_bar_plot(x_range, cb_0, cb_1, cb_2, cb_3, cb_4, cb_5, exp_method + ' Casters', x_range_100)
