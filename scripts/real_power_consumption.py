#!/usr/bin/env python3

import pandas as pd
import errno    
import os
import numpy as np

'''
Author: Enrico Ceccolini
    TODO write the description
'''

datadir = "/datasets/eurora_data/db/"
infile_jobs_to_nodes = datadir + "job_nodes.csv"
infile_jobs = datadir + "jobs.csv"

# settings
interval_comment = "WholeData"
suffix = "_1min_"

start_time = pd.to_datetime('2014-03-31')
end_time = pd.to_datetime('2015-08-11')


## notice that 43 doesn't exists in the db
# cpus=['01', '02', '03', '04', '05', '06', '07', '08', '09', '10', '11', '12', '13', '14', '15', '16', '17', '18', '19', '20', '21', '22', '23', '24', '25', '26', '27', '28', '29', '30', '31', '32', '33', '34', '35', '36', '37', '38', '39', '40', '41', '42', '44', '45', '46', '47', '48', '49', '50', '51', '52', '53', '54', '55', '56', '57', '58', '59', '60', '61', '62', '63', '64']
cpus=['01'] # test


def main():
    ### read the data
    jobs_to_nodes_whole_data = pd.read_csv(infile_jobs_to_nodes)
    print("jobs_to_nodes_whole_data contains {} records".format(jobs_to_nodes_whole_data.shape[0]))
    jobs_whole_data = pd.read_csv(infile_jobs)
    print("jobs_whole_data contains {} records".format(jobs_whole_data.shape[0]))

    ### clean the data
    # remove jobs runned on the inexistent node 129
    jobs_to_nodes_whole_data = jobs_to_nodes_whole_data[jobs_to_nodes_whole_data['node_id'] != 129] 
    # remove jobs with the same id that runned on the same node
    jobs_to_nodes_whole_data = jobs_to_nodes_whole_data.drop_duplicates(subset=['job_id_string', 'node_id'])
    print("after the clean, jobs_to_nodes_whole_data contains {} records".format(jobs_to_nodes_whole_data.shape[0]))

    # remove the jobs with missing end-time
    completed_jobs_data = jobs_whole_data[jobs_whole_data.end_time != '0000-00-00 00:00:00']
    print("completed_jobs_data contains {} records".format(completed_jobs_data.shape[0]))


    ### drop jobs with running time shorter than 1 min
    large_jobs = completed_jobs_data[pd.to_datetime(completed_jobs_data['end_time']) - pd.to_datetime(completed_jobs_data['run_start_time']) > np.timedelta64(1, 'm')]
    print("large jobs (running time > 1 min) contains {} jobs".format(large_jobs.shape[0]))
    ### merge the info from the two tables
    merged_jobs_to_nodes = pd.merge(jobs_to_nodes_whole_data, large_jobs, how='left', on='job_id_string')
    ### drop useless columns (we use the job_id_string to identify jobs)
    large_jobs = large_jobs.drop(['job_id', 'job_name', 'queue', 'user', 'deleted', 'dependency', 'pbs_exit_code'], axis=1)

    # drop the jobs out of time interval
    jobs_data_interval = large_jobs.loc[(pd.to_datetime(large_jobs['run_start_time']) >= start_time) & (pd.to_datetime(large_jobs['end_time']) <= end_time)]
    print("There are {} jobs executed in the interval".format(jobs_data_interval.shape[0]))





### TODO I probably have to load more than one node measurements file
def calculate_job_consumption(job_id_string):
    
    job_real_pow_consumption = 0
    job_runned_alone = True
    job_pow_quality = 1.0
    job_good_nodes = 0 # node where we have at most 1 measurement in the interval
    job_measurements_tot = 0
    job_measurements_missing_tot = 0
    
    partial_pow_consumption = 0
    
    job_to_nodes = merged_jobs_to_nodes[merged_jobs_to_nodes['job_id_string'] == job_id_string]
    job_row = job_to_nodes.iloc[0]
    job_to_nodes_unique = job_to_nodes.node_id.unique() # series with the nodes id
    ### print some jobs info
    print("job {} use {} cores in this node".format(job_id_string, str(job_row['ncpus'])))
    print("job {} start at {} and end at {}".format(job_id_string, str(job_row['run_start_time']), str(job_row['end_time'])))
    print("job {} requires {} nodes and {} cores".format(job_id_string, str(job_row['node_req']), str(job_row['cpu_req'])))
    print(job_to_nodes_unique)
    
    
    for index, row in job_to_nodes.iterrows():
        ### add a 0 to the number if is less than 10 ("1" -> "01", ...)
        node_str = "0" + str(row['node_id']) if row['node_id'] < 10 else str(row['node_id'])
        print("node {}".format(node_str))
        
        ### open the measurements node file I/O
        infile_node = datadir + "CPUs/" + interval_comment + "/node" + node_str + suffix + interval_comment + "_active_cores_and_jobs.csv"
        node_measurements = pd.read_csv(infile_node)
    
        ### take only the entries of the interval where the job was running
        interval_measurements = node_measurements.loc[(pd.to_datetime(node_measurements['timestamp']) >= pd.to_datetime(job_row['run_start_time'])) & (pd.to_datetime(node_measurements['timestamp']) <= pd.to_datetime((job_row['end_time'])) - np.timedelta64(1, 'm'))]
        
        ### TODO drop missing measurements (calculate the percentage)
        n_node_measurements = interval_measurements.shape[0]
        job_measurements_tot += n_node_measurements
        
        interval_measurements = interval_measurements.dropna()
        # print("interval_measurements {}".format(interval_measurements.shape[0]))
        print("{}/{} intervals missing".format(n_node_measurements-interval_measurements.shape[0], n_node_measurements))
        job_measurements_missing_tot += n_node_measurements-interval_measurements.shape[0]
        
        # print(interval_measurements)
        ### drop where active cores is greater than 16 - only on node 30
        interval_measurements = interval_measurements[interval_measurements['active_cores'] <= 16]
        ### TODO drop row with 0 active_cores (or less than the current job used cores)
        interval_measurements = interval_measurements[interval_measurements['active_cores'] >= 1]  
        
        if(interval_measurements.shape[0] != 0):
            job_good_nodes += 1
        
        ### group the intervals wehere the partial_pow_cons can be obtaied with the same instance of the formula
        ### take the mean for the pow columns
        # interval_grouped = interval_measurements.reset_index().groupby(["active_cores", "active_jobs", "active_gpus", "active_mics"]).mean()
        interval_grouped = interval_measurements.groupby(["active_cores", "active_jobs", "active_gpus", "active_mics"])
        counts = interval_grouped.size().to_frame(name='counts')
        interval_grouped = (counts
         .join(interval_grouped.agg({'pow_tot_0':'mean'}).rename(columns={'pow_tot_0': 'pow_tot_0_mean'}))
         .join(interval_grouped.agg({'pow_tot_1':'mean'}).rename(columns={'pow_tot_1': 'pow_tot_1_mean'}))
         .reset_index()
        )
        
        if(interval_grouped.shape[0]>1):
            job_runned_alone = False
        
        ### apply the formula on each grouped data
        #for index_group, row_group in interval_grouped.iterrows():
            # n_active_cores = index_group[0]
        #    n_active_cores = row_group['active_cores']
        #    partial_pow_consumption += (row_group['pow_tot_0_mean'] + row_group['pow_tot_1_mean']) * row['ncpus'] / n_active_cores
        
        if(interval_grouped.shape[0] != 0):
            print(interval_grouped)
            interval_grouped['pow_tot'] = (interval_grouped['pow_tot_0_mean'] + interval_grouped['pow_tot_1_mean']) * row['ncpus'] / interval_grouped['active_cores'] 
            # not_alone_counts = interval_grouped['counts'].loc[interval_grouped['active_jobs'] != 1].sum()
            # interval_grouped.loc[interval_grouped['active_jobs'] == 1, ['counts']] += not_alone_counts        
            partial_pow_consumption += np.average(interval_grouped['pow_tot_0_mean'] + interval_grouped['pow_tot_1_mean'], weights=interval_grouped['counts'])
            partial_pow_consumption = partial_pow_consumption / interval_grouped.shape[0]
            # print(interval_grouped)
            
        print("partial measurement: {}".format(partial_pow_consumption))
        job_real_pow_consumption += partial_pow_consumption
        partial_pow_consumption = 0
    
    print("{}/{} node measurements are good".format(job_good_nodes, job_to_nodes_unique.shape[0]))
    
    # print("job_real_pow_consumption is {}".format(job_real_pow_consumption))
    ### TODO if some entire node data is missing, approximate the amount to add
    if(job_good_nodes != 0):
        job_real_pow_consumption = job_real_pow_consumption + (job_real_pow_consumption / job_good_nodes) * (job_to_nodes_unique.shape[0] - job_good_nodes)
       
    
    if(job_measurements_missing_tot != 0):
        job_pow_quality = 1 - (job_measurements_missing_tot / job_measurements_tot)
    else:
        job_pow_quality = 0
        
    return job_real_pow_consumption, job_runned_alone, job_pow_quality
    
    

if __name__ == "__main__":
    main()
