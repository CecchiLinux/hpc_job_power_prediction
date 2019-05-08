#!/usr/bin/env python3

import pandas as pd
import errno    
import os
import numpy as np
import math

'''
Author: Enrico Ceccolini
    TODO write the description
'''

# Create a folder in the specified path
def mkdir_p(path):
    try:
        os.makedirs(path)
    except OSError as exc:  # Python > 2.5
        if exc.errno == errno.EEXIST and os.path.isdir(path):
            pass
        else:
            raise

def main():
    datadir = "/datasets/eurora_data/db/"
    infile_jobs_to_nodes = datadir + "job_nodes.csv" # contains where each job executed
    infile_jobs = datadir + "jobs.csv" # contains all the jobs

    # settings
    suffix = "_1min_"

    # interval_comment = "April_v2"
    # start_time = pd.to_datetime('2014-04-01')
    # end_time = pd.to_datetime('2014-05-01')

    interval_comment = "WholeData"
    start_time = pd.to_datetime('2014-03-24')
    end_time = pd.to_datetime('2015-08-12')

    # save CPU data (one file per node)
    # notice that 43 doesn't exists in the db
    cpus = ['01', '02', '03', '04', '05', '06', '07', '08', '09', '10', '11', '12', '13', '14', '15', '16', '17', '18', '19', '20', '21', '22', '23', '24', '25', '26', '27', '28', '29', '30', '31', '32', '33', '34', '35', '36', '37', '38', '39', '40', '41', '42', '44', '45', '46', '47', '48', '49', '50', '51', '52', '53', '54', '55', '56', '57', '58', '59', '60', '61', '62', '63', '64']
    # cpus = ['07']
    # cpus = ['01'] # test

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

    ### merge the info from the two tables
    merged_jobs_to_nodes = pd.merge(jobs_to_nodes_whole_data, completed_jobs_data, how='left', on='job_id_string')

    # drop the jobs out of time range
    interval_data_jobs = merged_jobs_to_nodes[pd.to_datetime(merged_jobs_to_nodes['start_time']) >= start_time]
    interval_data_jobs = interval_data_jobs[pd.to_datetime(interval_data_jobs['end_time']) <= end_time]
    interval_data_jobs = interval_data_jobs.sort_values('run_start_time')
    print("interval_data_jobs contains {} records".format(interval_data_jobs.shape[0]))

    for cpu in cpus:
        print(cpu)
        # take the jobs runned on this node
        interval_jobs_to_node = interval_data_jobs[interval_data_jobs['node_id'] == int(cpu)]
        print("interval_jobs_node_data contains {} records".format(interval_jobs_to_node.shape[0]))

        # read the power mesuraments file obtained with "keep_nodes_data_by_period_1min_sample.py"
        infile_node = datadir + "CPUs/" + interval_comment + "/node" + cpu + suffix + interval_comment
        node_data = pd.read_csv(infile_node + ".csv")

        
        num_minutes = node_data.shape[0]
        active_cores = pd.Series(0, index=np.arange(num_minutes))
        active_jobs = pd.Series(0, index=np.arange(num_minutes))
        active_gpus = pd.Series(0, index=np.arange(num_minutes))
        active_mics = pd.Series(0, index=np.arange(num_minutes))
        active_cores.shape

        # i = 0
        num_jobs = interval_jobs_to_node.shape[0]
        node_start_time = node_data['timestamp'].iloc[0]

        for index, row in interval_jobs_to_node.iterrows():
            # print("{}/{}   {}".format(i, num_jobs, row['job_id_string']))
            # i = i + 1
            start_time = pd.to_datetime(row['run_start_time'])
            # print(start_time)
            end_time = pd.to_datetime(row['end_time'])
            job_cores = row['ncpus']
            job_gpus = row['ngpus']
            job_mics = row['nmics']


            
            # before_minutes = int((start_time - pd.to_datetime(node_start_time)) / np.timedelta64(1, 'm'))
            before_minutes = int((start_time + np.timedelta64(1, 'm') - pd.to_datetime(node_start_time)) / np.timedelta64(1, 'm'))
            # running_minutes = math.ceil(((end_time + np.timedelta64(1, 'm') - start_time) / np.timedelta64(1, 'm')))
            running_minutes = int(((end_time - start_time) / np.timedelta64(1, 'm')))
            after_minutes = num_minutes - running_minutes - before_minutes
            # print("{} - {} - {}".format(before_minutes, running_minutes, after_minutes))
            before_serie = pd.Series(0, index=np.arange(before_minutes))
            after_serie = pd.Series(0, index=np.arange(after_minutes))
            running_serie = pd.Series(job_cores, index=np.arange(running_minutes))
            concat_series = pd.concat([before_serie, running_serie, after_serie], ignore_index=True)
            active_cores = active_cores.add(concat_series, fill_value=0)
            
            running_serie = pd.Series(1, index=np.arange(running_minutes))
            concat_series = pd.concat([before_serie, running_serie, after_serie], ignore_index=True)
            active_jobs= active_jobs.add(concat_series, fill_value=0)
            
            running_serie = pd.Series(job_gpus, index=np.arange(running_minutes))
            concat_series = pd.concat([before_serie, running_serie, after_serie], ignore_index=True)
            active_gpus = active_gpus.add(concat_series, fill_value=0)
            
            running_serie = pd.Series(job_mics, index=np.arange(running_minutes))
            concat_series = pd.concat([before_serie, running_serie, after_serie], ignore_index=True)
            active_mics = active_mics.add(concat_series, fill_value=0)

        node_data['active_cores'] = active_cores
        node_data['active_jobs'] = active_jobs
        node_data['active_gpus'] = active_gpus
        node_data['active_mics'] = active_mics

        outfile_node = infile_node + "_active_cores_and_jobs"
        node_data.to_csv(outfile_node + ".csv")

if __name__ == "__main__":
    main()