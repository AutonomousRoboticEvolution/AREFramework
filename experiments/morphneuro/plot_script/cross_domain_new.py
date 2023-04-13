import csv
import numpy as np
import matplotlib.pyplot as plt
from scipy.interpolate import make_interp_spline
import scipy.stats as spstat
import numpy.linalg as nplinalg
from scipy import stats
import os
import seaborn as sns
import pandas as pd
import math

###############################################################################################
# Path and Settings
###############################################################################################
scenario = 'steps_arena'
root_path_ds = '/home/miller/Desktop/test/Feb23/' + scenario
root_path_as = '/home/miller/Desktop/test/Feb26/morph_bootstrap/' + scenario
root_path_sss = '/home/miller/Desktop/test/Feb26/1-many/' + scenario
exp_trial_ds = ['01', '02', '03', '04', '05', '06', '07', '08', '09', '010']
exp_trial_ss = ['01', '02', '03', '04', '05', '06', '07', '08', '09', '010']
exp_trial_sss = ['01', '02', '03', '04', '05', '06', '07', '08', '09', '010']
morph_loc = 'morphdesc.csv'
fitness_loc = 'fitnesses.csv'
total_generations = 200
ind_per_gen = 100
bin_num = 5


###############################################################################################
# Define Functions
###############################################################################################
def read_data(root_path_ds, exp_trial_ds, fitness_loc, morph_loc, total_generations, bin_num):
    cc_run_best_fitness = []
    cc_avg_fitness = []
    cc_diversity = []
    cc_uni = []
    for run_num in exp_trial_ds:
        fitness_abs_path = root_path_ds + '/' + run_num + '/' + fitness_loc
        last_generation = 'generation' + ' ' + str(total_generations) + ':' + ' '
        if os.path.exists(fitness_abs_path):
            # Log Data
            with open(fitness_abs_path) as fitness_file:
                fitness_reader = csv.reader(fitness_file)
                average_fitness = []
                best_fitness = []
                best_fitness_ind = []
                calculated_average_fitness = []
                for line in fitness_reader:
                    average_fitness.append(revert_fitness(float(line[102])))
                    best_fitness.append(revert_fitness(float(line[106])))
                    best_fitness_ind.append(float(line[104]))
                    current_fitness = []
                    for i in range(100):
                        current_fitness.append(float(line[i+1].split(':')[1]) if float(line[i+1].split(':')[1]) > 0 else 0)
                    calculated_average_fitness.append(np.mean(current_fitness))
                # Best Fitness Data
                cc_run_best_fitness.append(best_fitness[0:total_generations+1:1])
                cc_avg_fitness.append(calculated_average_fitness[0:total_generations+1:1])

        morph_abs_path = root_path_ds + '/' + run_num + '/' + morph_loc
        if os.path.exists(morph_abs_path):
            with open(morph_abs_path) as morphdesc_file:
                morphdesc_reader = csv.reader(morphdesc_file)
                coverage_run = []
                uni_run = []
                combinations_gen = []
                _gen_count_desc = 0
                for line in morphdesc_reader:
                    desc = []
                    for i in range(8):
                        desc.append(float(line[i + 1]))
                    combinations_gen.append(desc)
                    _gen_count_desc += 1
                    if _gen_count_desc == 100:
                        hist, grid_size = compute_desc_hist(np.array(combinations_gen),bin_num)
                        coverage_run.append(float(len(hist))/float(grid_size))
                        uni_run.append(float(uniformity_scores(hist, grid_size)))
                        _gen_count_desc = 0
                        combinations_gen = []
                cc_diversity.append(coverage_run[0:total_generations+1:1])
                cc_uni.append(uni_run[0:total_generations+1:1])
    return cc_avg_fitness, cc_run_best_fitness, cc_diversity, cc_uni

def compute_desc_hist(descriptors,bin):
    '''
    compute an histogram based on a list of descriptors
    '''
    grid_size = bin**len(descriptors[0])
    hist = dict()
    for desc in descriptors :
        coord = 0
        for val,i in zip(desc,range(len(desc))):
            coord = coord + int(val*bin**i)
        if coord in hist :
            hist[coord] = hist[coord] + 1
        else :
            hist[coord] = 1
    return hist, grid_size

def JSD(P,Q) :
    '''
    Return the Jensen-Shanon divergence between the distributions P and Q
    '''
    P = P / nplinalg.norm(P,ord=1)
    Q = Q / nplinalg.norm(Q,ord=1)
    M = (P + Q)*.5
    return (spstat.entropy(P,M) + spstat.entropy(Q,M))*.5

def uniformity_scores(hist,grid_size) :
    '''
    data
    '''

    max_key = max(hist.keys())
    min_key = min(hist.keys())
    pop_size = 0
    pop_dist = []
    for i in range(min_key,max_key) :
        if i in hist :
            pop_dist.append(hist[i])
            pop_size += hist[i]
        else :
            pop_dist.append(0)
    uni_ref_val = float(pop_size)/float(len(hist))
    uni_dist = np.full(len(pop_dist),uni_ref_val)
    # print(len(hist),grid_size)
    return  1 - JSD(uni_dist,pop_dist)

def plot_std(list):
    mean_gen = []
    std_gen = []
    for i in range(len(list)):
        mean_gen.append(np.mean(list[i]))
        std_gen.append(np.std(list[i]))
    max_line = np.array(mean_gen) + np.array(std_gen)
    min_line = np.array(mean_gen) - np.array(std_gen)
    return mean_gen, std_gen, max_line, min_line

def data_processing(boot_run_best_fitness, total_generations, ind_per_gen):
    inv_boot_avg_fitness = [[r[col] for r in boot_run_best_fitness] for col in range(len(boot_run_best_fitness[0]))]
    evaluation_list_all = []
    boot_avg_fitness_mean = []
    for i in range(total_generations+1):
        evaluation_list_all.append(i * ind_per_gen)
        boot_avg_fitness_mean.append(np.mean(inv_boot_avg_fitness[i]))
    x_smooth = np.linspace(np.array(evaluation_list_all).min(), np.array(evaluation_list_all).max(), 20)
    fitness_boot_smooth = make_interp_spline(np.array(evaluation_list_all), np.array(boot_avg_fitness_mean))(x_smooth)
    fitness_boot_mean, fitness_boot_std, fitness_bootmax_line, fitness_bootmin_line= plot_std(inv_boot_avg_fitness)
    return fitness_boot_smooth, fitness_boot_mean, fitness_bootmax_line, fitness_bootmin_line

def revert_fitness(fitness):
    plain_sqr = ((1 - fitness)*2.83)*((1 - fitness)*2.83) - 0.5
    new_fitness = 1 - math.sqrt(plain_sqr) / 2.83
    return new_fitness

def box_plot(list1, label1, list2, label2, ylabel, scene, skip, y_lim):
    list1 = [[r[col] for r in list1] for col in range(len(list1[0]))]
    list2 = [[r[col] for r in list2] for col in range(len(list2[0]))]
    list3 = []
    gen_id = []
    setup = []
    for i in range(len(list1)):
        list3.extend(list1[i])
        list3.extend(list2[i])
        for j in range(len(list1[i])):
            gen_id.append(i)
            setup.append(label1)
        for k in range(len(list2[i])):
            gen_id.append(i)
            setup.append(label2)
    list3_plot = []
    gen_id_plot = []
    setup_plot = []
    for j in range(len(gen_id)):
        if gen_id[j] % skip == 0:
            list3_plot.append(list3[j])
            gen_id_plot.append(gen_id[j])
            setup_plot.append(setup[j])
    data = {
        ylabel: list3_plot,
        'Generations': gen_id_plot,
        'Setup': setup_plot
    }
    df = pd.DataFrame(data)
    fig = plt.figure()
    ax = fig.add_subplot(111)
    sns.boxplot(x="Generations", y=ylabel, hue="Setup", data=df)
    plt.title(scene, fontsize=35, weight='black')
    plt.tick_params(labelsize=25)
    plt.legend(fontsize=25)
    ax.set_xlabel('Generations', fontsize=35)
    ax.set_ylabel(ylabel, fontsize=35)
    ax.set(ylim=y_lim)
    plt.show()

def compute_p_value(array1, array2):
    array1 = [[r[col] for r in array1] for col in range(len(array1[0]))]
    array2 = [[r[col] for r in array2] for col in range(len(array2[0]))]
    last_gen_1 = array1[-1]
    last_gen_2 = array2[-1]
    t, p = stats.mannwhitneyu(last_gen_1,last_gen_2)
    return p

def box_plot_2(list1, label1, list2, label2, list3, label3, ylabel, scene, skip, y_lim):
    list1 = [[r[col] for r in list1] for col in range(len(list1[0]))]
    list2 = [[r[col] for r in list2] for col in range(len(list2[0]))]
    list3 = [[r[col] for r in list3] for col in range(len(list3[0]))]
    list = []
    gen_id = []
    setup = []
    for i in range(len(list1)):
        list.extend(list1[i])
        list.extend(list2[i])
        list.extend(list3[i])
        for j in range(len(list1[i])):
            gen_id.append(i)
            setup.append(label1)
        for k in range(len(list2[i])):
            gen_id.append(i)
            setup.append(label2)
        for l in range(len(list3[i])):
            gen_id.append(i)
            setup.append(label3)
    list_plot = []
    gen_id_plot = []
    setup_plot = []
    for j in range(len(gen_id)):
        if gen_id[j] % skip == 0:
            list_plot.append(list[j])
            gen_id_plot.append(gen_id[j])
            setup_plot.append(setup[j])
    data = {
        ylabel: list_plot,
        'Generations': gen_id_plot,
        'Setup': setup_plot
    }
    df = pd.DataFrame(data)
    fig = plt.figure()
    ax = fig.add_subplot(111)
    sns.boxplot(x="Generations", y=ylabel, hue="Setup", data=df)
    plt.title(scene, fontsize=35, weight='black')
    plt.tick_params(labelsize=25)
    plt.legend(fontsize=25)
    ax.set_xlabel('Generations', fontsize=35)
    ax.set_ylabel(ylabel, fontsize=35)
    ax.set(ylim=y_lim)
    plt.show()

#####################################################################################################
# Data Processing
####################################################################################################
# evaluation_list_all = []
# for i in range(total_generations+1):
#     evaluation_list_all.append(i * ind_per_gen)
# x_smooth = np.linspace(np.array(evaluation_list_all).min(), np.array(evaluation_list_all).max(), 20)

af1, bf1, div1, uni1 = read_data(root_path_ds, exp_trial_ds, fitness_loc, morph_loc, total_generations, bin_num)
af2, bf2, div2, uni2 = read_data(root_path_as, exp_trial_ss, fitness_loc, morph_loc, total_generations, bin_num)
af3, bf3, div3, uni3 = [], [], [], []
for run_num in exp_trial_sss:
    fitness_abs_path = root_path_sss + '/' + run_num + '/' + fitness_loc
    last_generation = 'generation' + ' ' + str(total_generations) + ':' + ' '
    if os.path.exists(fitness_abs_path):
        # Log Data
        with open(fitness_abs_path) as fitness_file:
            fitness_reader = csv.reader(fitness_file)
            average_fitness = []
            best_fitness = []
            best_fitness_ind = []
            calculated_average_fitness = []
            count = 0
            for line in fitness_reader:
                if count == 0:
                    average_fitness.append(revert_fitness(float(line[102])))
                    best_fitness.append(revert_fitness(float(line[106])))
                    best_fitness_ind.append(float(line[104]))
                    count += 1
                else:
                    average_fitness.append(revert_fitness(float(line[5])))
                    best_fitness.append(revert_fitness(float(line[9])))
                    best_fitness_ind.append(float(line[7]))
            # Best Fitness Data
            bf3.append(best_fitness[0:total_generations + 1:1])
            af3.append(average_fitness[0:total_generations + 1:1])

    morph_abs_path = root_path_ds + '/' + run_num + '/' + morph_loc
    if os.path.exists(morph_abs_path):
        with open(morph_abs_path) as morphdesc_file:
            morphdesc_reader = csv.reader(morphdesc_file)
            coverage_run = []
            uni_run = []
            combinations_gen = []
            _gen_count_desc = 0
            for line in morphdesc_reader:
                desc = []
                for i in range(8):
                    desc.append(float(line[i + 1]))
                combinations_gen.append(desc)
                _gen_count_desc += 1
                if _gen_count_desc == 3:
                    hist, grid_size = compute_desc_hist(np.array(combinations_gen), bin_num)
                    coverage_run.append(float(len(hist)) / float(grid_size))
                    uni_run.append(float(uniformity_scores(hist, grid_size)))
                    _gen_count_desc = 0
                    combinations_gen = []
            div3.append(coverage_run[0:total_generations + 1:1])
            uni3.append(uni_run[0:total_generations + 1:1])
af3_c, bf3_c, div3_c, uni3_c = [], [], [], []
for i in range(len(af3)):
    print('Length', i+1, ':', len(af3[i]))
    if len(af3[i]) == 201:
        af3_c.append(af3[i])
        bf3_c.append(bf3[i])
        div3_c.append(div3[i])
        uni3_c.append(uni3[i])


af1_c = []
bf1_c = []
div1_c = []
uni1_c = []
for i in range(len(af1)):
    print('Length', i+1, ':', len(af1[i]))
    if len(af1[i]) == 201:
        af1_c.append(af1[i])
        bf1_c.append(bf1[i])
        div1_c.append(div1[i])
        uni1_c.append(uni1[i])

af2_c = []
bf2_c = []
div2_c = []
uni2_c = []
for i in range(len(af2)):
    print('Length', i+1, ':', len(af2[i]))
    if len(af2[i]) == 201:
        af2_c.append(af2[i])
        bf2_c.append(bf2[i])
        div2_c.append(div2[i])
        uni2_c.append(uni2[i])

# af1_c, bf1_c, div1_c, uni1_c = length_check(af1, bf1, div1, uni1)
# af2_c, bf2_c, div2_c, uni2_c = length_check(af2, bf2, div2, uni2)


box_plot_2(af1_c,  'Indirect Encoding 1-1 Method', af2_c, 'Indirect Encoding 1-1 Method (Morph Bootstrap)', af3_c, 'Indirect Encoding 1-many Method', 'Average Fitness', scenario, 10, (0, 1))
box_plot_2(bf1_c, 'Indirect Encoding 1-1 Method', bf2_c, 'Indirect Encoding 1-1 Method (Morph Bootstrap)', bf3_c, 'Indirect Encoding 1-many Method', 'Best Fitness', scenario, 10, (0, 1))
box_plot_2(div1_c, 'Indirect Encoding 1-1 Method', div2_c, 'Indirect Encoding 1-1 Method (Morph Bootstrap)', div3_c, 'Indirect Encoding 1-many Method', 'Coverage', scenario, 10, (0, 0.0003))
box_plot_2(uni1_c, 'Indirect Encoding 1-1 Method', uni2_c, 'Indirect Encoding 1-1 Method (Morph Bootstrap)', uni3_c, 'Indirect Encoding 1-many Method', 'Uniformity', scenario, 10, (0.3, 0.33))


print('average fitness P-Value:', compute_p_value(af1_c, af2_c))
print('best fitness P-Value:', compute_p_value(bf1_c, bf2_c))
print('coverage P-Value:', compute_p_value(div1_c, div2_c))
print('uniformity P-Value:', compute_p_value(uni1_c, uni2_c))