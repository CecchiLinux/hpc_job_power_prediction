#!/usr/bin/env python3

import pandas as pd
import errno    
import os, sys

'''
Author: Enrico Ceccolini
    Given the raw data of Eurora (CORE_MEASUREMENT_FULL)
    and a time interval, it returns only the measurements
    of that period, sampled at 1 minute intervals
'''

def main():
    datadir = "/datasets/eurora_data/db/"

    jobs_file = datadir + 'jobs.csv'

    interval_comment = "April"
    start_time = pd.to_datetime('2014-04-01')
    end_time = pd.to_datetime('2014-05-01')

    mkdir_p(datadir + 'CPUs/' + interval_comment)

    ### filter the jobs

    # whole_data_jobs = pd.read_csv(jobs_file)
    # print("there are {} jobs in the whole dataset".format(whole_data_jobs.shape[0]))
    
    # # completed_data_jobs = whole_data_jobs[whole_data_jobs.exit_status == 'COMPLETED']
    # completed_data_jobs = whole_data_jobs[whole_data_jobs.end_time != '0000-00-00 00:00:00']
    # print("there are {} completed jobs in the whole dataset".format(completed_data_jobs.shape[0]))
    
    # dropped_data_jobs = completed_data_jobs[pd.to_datetime(completed_data_jobs['start_time']) > start_time]
    # dropped_data_jobs = dropped_data_jobs[pd.to_datetime(dropped_data_jobs['end_time']) < end_time]
    # print("there are {} completed jobs in the selected interval".format(dropped_data_jobs.shape[0]))

    # outfile = datadir + 'CPUs/' + interval_comment + "/jobs_" + interval_comment + ".csv"
    # dropped_data_jobs.to_csv(outfile)

    # sys.exit(0)

    ### filter the measurement 
    # save CPU data (one file per node)
    # notice that 43 doesn't exists in the db
    cpus=['01', '02', '03', '04', '05', '06', '07', '08', '09', '10', '11',
     '12', '13', '14', '15', '16', '17', '18', '19', '20', '21', '22', '23', 
     '24', '25', '26', '27', '28', '29', '30', '31', '32', '33', '34', '35', 
     '36', '37', '38', '39', '40', '41', '42', '44', '45', '46', '47', '48', 
     '49', '50', '51', '52', '53', '54', '55', '56', '57', '58', '59', '60', 
     '61', '62', '63', '64']
    # cpus=['01'] # test

    for cpu in cpus:
        print(cpu)
        infile = datadir + "CPUs/cpu" + cpu + ".csv"

        # read dataset
        whole_node_power_data = pd.read_csv(infile) # Read a comma-separated values (csv) file into DataFrame
        print("data rows {}".format(whole_node_power_data.shape))

        # drop useless columns
        # whole_node_power_data = whole_node_power_data.drop(['01'], axis=1)

        # drop the duplicates rows (same timestamp, cpu_id, ...)
        whole_node_power_data = whole_node_power_data.drop_duplicates() 
        print("data rows after drop_duplicates {}".format(whole_node_power_data.shape[0]))

        # select data in the specified interval
        interval_data = whole_node_power_data[pd.to_datetime(whole_node_power_data['timestamp']) > start_time]
        interval_data = interval_data[pd.to_datetime(interval_data['timestamp']) < end_time]
        print("data row from {} to {}: {}".format(start_time, end_time, interval_data.shape[0]))

        # split the data referring to the cpu0 and cpu0
        cpu0 = interval_data[interval_data['cpu_id'] == 0]
        cpu1 = interval_data[interval_data['cpu_id'] == 1]
        print("data row after split: {}".format(cpu0.shape[0]))
        print("data row after split: {}".format(cpu1.shape[0]))

        # drop the duplicates rows 
        cpu0 = cpu0.drop_duplicates(subset=['timestamp'])
        cpu1 = cpu1.drop_duplicates(subset=['timestamp'])
        print("data row after split: {}".format(cpu0.shape[0]))
        print("data row after split: {}".format(cpu1.shape[0]))

        # convert the timestamp column to the Pandas timestamp format
        cpu0.timestamp = pd.to_datetime(cpu0.timestamp)
        cpu1.timestamp = pd.to_datetime(cpu1.timestamp)
        # set the timestamp as the dataFrame index
        cpu0 = cpu0.set_index(["timestamp"])
        cpu1 = cpu1.set_index(["timestamp"])

        # resample data from 5 sec to 1 min, using the mean for the new values
        cpu0_1min = cpu0.resample('1Min').mean()
        print("data rows {}".format(cpu0_1min.shape[0]))
        cpu1_1min = cpu1.resample('1Min').mean()
        print("data rows {}".format(cpu1_1min.shape[0]))

        # remove useless columns
        cpu0_1min = cpu0_1min.drop(['cpu_id'], axis=1)
        cpu1_1min = cpu1_1min.drop(['cpu_id'], axis=1)

        # rename columns for the concat
        cpu0_1min = cpu0_1min.rename(index=str, columns={"pow_cpu": "pow_cpu_0", "pow_dram": "pow_dram_0", "pow_pkg": "pow_pkg_0"})
        cpu1_1min = cpu1_1min.rename(index=str, columns={"pow_cpu": "pow_cpu_1", "pow_dram": "pow_dram_1", "pow_pkg": "pow_pkg_1"})

        # concat
        node_1min = pd.concat([cpu0_1min, cpu1_1min], axis=1)

        # write
        outfile = datadir + 'CPUs/' + interval_comment + "/node" + cpu + "_1min_" + interval_comment + ".csv"
        node_1min.to_csv(outfile)
        
        print("-------------------")

# Create a folder in the specified path
def mkdir_p(path):
    try:
        os.makedirs(path)
    except OSError as exc:  # Python > 2.5
        if exc.errno == errno.EEXIST and os.path.isdir(path):
            pass
        else:
            raise

if __name__ == "__main__":
    main()



