#!/usr/bin/env python3

import pandas as pd
import errno    
import os, sys
import numpy as np

'''
Author: Enrico Ceccolini
    Given the raw data of Eurora (CORE_MEASUREMENT_FULL)
    and a time interval, it returns only the measurements
    of that period, sampled at 1 minute intervals
'''

def main():
    datadir = "/datasets/eurora_data/db/"

    jobs_file = datadir + 'jobs.csv'

    global_start_time = pd.to_datetime('2014-03-31')
    global_end_time = pd.to_datetime('2015-08-12')

    # interval_comment = "April_new"
    # start_time = pd.to_datetime('2014-04-01')
    # end_time = pd.to_datetime('2014-05-01')
    interval_comment = "WholeData"
    # start_time = pd.to_datetime('2014-03-31')
    # end_time = pd.to_datetime('2015-08-11')

    mkdir_p(datadir + 'CPUs/' + interval_comment)

    # notice that 43 doesn't exists in the db
    cpus=['01', '02', '03', '04', '05', '06', '07', '08', '09', '10', '11',
     '12', '13', '14', '15', '16', '17', '18', '19', '20', '21', '22', '23', 
     '24', '25', '26', '27', '28', '29', '30', '31', '32', '33', '34', '35', 
     '36', '37', '38', '39', '40', '41', '42', '44', '45', '46', '47', '48', 
     '49', '50', '51', '52', '53', '54', '55', '56', '57', '58', '59', '60', 
     '61', '62', '63', '64']
    #cpus=['01', '03', '04', '05', '06'] # test
    #cpus = ['07', '08', '09', '10', '11', '12', '13', '14', '15', '16', '17', '18', '19', '20', '21', '22', '23', '24', '25', '26', '27', '28', '29', '30', '31', '32', '33', '34', '35', '36', '37', '38', '39', '40', '41', '42', '44', '45', '46', '47', '48', '49', '50', '51', '52', '53', '54', '55', '56', '57', '58', '59', '60', '61', '62', '63', '64']
    #cpus=['01'] # test

    for cpu in cpus:
        print(cpu)
        infile = datadir + "CPUs/cpu" + cpu + ".csv"

        # read dataset
        whole_node_power_data = pd.read_csv(infile) # Read a comma-separated values (csv) file into DataFrame
        print("data rows {}".format(whole_node_power_data.shape))

        # drop useless columns
        whole_node_power_data = whole_node_power_data.drop([str(cpu)], axis=1)

        # drop the duplicates rows (same timestamp, cpu_id, ...)
        whole_node_power_data = whole_node_power_data.drop_duplicates() 
        print("data rows after drop_duplicates {}".format(whole_node_power_data.shape[0]))

        # select data in the specified interval
        #interval_data = whole_node_power_data.loc[(pd.to_datetime(whole_node_power_data['timestamp']) >= start_time) & (pd.to_datetime(whole_node_power_data['timestamp']) <= end_time)]
        #print("data row from {} to {}: {}".format(start_time, end_time, interval_data.shape[0]))
        # no more interval
        interval_data = whole_node_power_data


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

        # align cpu0 measurements with cpu1 measurements
        if cpu0.shape[0] < cpu1.shape[0]:
            cpu1 = cpu1[cpu1.timestamp >= cpu0['timestamp'].iloc[0]]
        if cpu1.shape[0] < cpu0.shape[0]:
            cpu0 = cpu0[cpu0.timestamp >= cpu1['timestamp'].iloc[0]]


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

        # insert padding to have the same indices on all nodes
        first_date = cpu0_1min.index[0]
        last_date = cpu0_1min.index[cpu0_1min.shape[0]-1]
        index_before = pd.date_range(start=global_start_time, end=first_date - np.timedelta64(1, 'm'), freq='1Min')
        index_after = pd.date_range(start=last_date + np.timedelta64(1, 'm'), end=global_end_time, freq='1Min')
        padding_before = pd.DataFrame(index=index_before, columns=['pow_cpu', 'pow_dram', 'pow_pkg'])
        padding_before.index.name = 'timestamp'
        padding_after = pd.DataFrame(index=index_after, columns=['pow_cpu', 'pow_dram', 'pow_pkg'])
        padding_after.index.name = 'timestamp'
        cpu0_1min = pd.concat([padding_before, cpu0_1min])
        cpu1_1min = pd.concat([padding_before, cpu1_1min])
        cpu0_1min = pd.concat([cpu0_1min, padding_after])
        cpu1_1min = pd.concat([cpu1_1min, padding_after])

        # rename columns for the concat
        cpu0_1min = cpu0_1min.rename(index=str, columns={"pow_cpu": "pow_cpu_0", "pow_dram": "pow_dram_0", "pow_pkg": "pow_pkg_0"})
        cpu1_1min = cpu1_1min.rename(index=str, columns={"pow_cpu": "pow_cpu_1", "pow_dram": "pow_dram_1", "pow_pkg": "pow_pkg_1"})

        # concat
        node_1min = pd.concat([cpu0_1min, cpu1_1min], axis=1)
        
        ### merge the power consumption to have only two values (one for each node's cpu)
        # add two new columns
        node_1min['pow_tot_0'] = cpu0_1min['pow_cpu_0'] + cpu0_1min['pow_dram_0'] + cpu0_1min['pow_pkg_0'] 
        node_1min['pow_tot_1'] = cpu1_1min['pow_cpu_1'] + cpu1_1min['pow_dram_1'] + cpu1_1min['pow_pkg_1']
        # remove the useless columns
        node_1min = node_1min.drop(['pow_cpu_0', 'pow_dram_0', 'pow_pkg_0', 'pow_cpu_1', 'pow_dram_1', 'pow_pkg_1'], axis=1)

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



