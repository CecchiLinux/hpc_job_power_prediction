#!/usr/bin/env python3

import pandas as pd
import errno    
import os, sys
import numpy as np

'''
Author: Enrico Ceccolini
'''

def main():
    datadir = "/datasets/eurora_data/db1/"


    # settings
    interval_comment = "Andrea"
    start_time = pd.to_datetime('2014-03-31')
    end_time = pd.to_datetime('2014-05-01')
    interval_comment = "Alina"
    start_time = pd.to_datetime('2014-09-30')
    end_time = pd.to_datetime('2014-11-01')
    interval_comment_whole = "WholeData"
    # start_time = pd.to_datetime('2014-03-31')
    # end_time = pd.to_datetime('2015-08-11')
    suffix = "_1min_"  

    infile_jobs = datadir + "CPUs/" + interval_comment_whole + "/large_jobs_real_pow.csv"

    mkdir_p(datadir + 'CPUs/' + interval_comment)


    ### select the jobs that rans in the interval
    whole_data_jobs = pd.read_csv(infile_jobs, index_col=0) # Read a comma-separated values (csv) file into DataFrame
    interval_data_jobs = whole_data_jobs[pd.to_datetime(whole_data_jobs['run_start_time']) >= start_time]
    interval_data_jobs = interval_data_jobs[pd.to_datetime(interval_data_jobs['start_time']) <= end_time]
    print("there are {} jobs in the interval".format(interval_data_jobs.shape[0]))
    outfile_jobs = datadir + "CPUs/" + interval_comment + "/large_jobs_real_pow_" + interval_comment + ".csv"
    interval_data_jobs.to_csv(outfile_jobs)

    # notice that 43 doesn't exists in the db
    nodes = ['01', '02', '03', '04', '05', '06', '07', '08', '09', '10', '11', '12', '13', '14', '15', '16', '17', '18', '19', '20', '21', '22', '23', '24', '25', '26', '27', '28', '29', '30', '31', '32', '33', '34', '35', '36', '37', '38', '39', '40', '41', '42', '44', '45', '46', '47', '48', '49', '50', '51', '52', '53', '54', '55', '56', '57', '58', '59', '60', '61', '62', '63', '64']
    # nodes = ['01', '02'] # test

    for node in nodes:
        print(node)
        infile_node = datadir + "CPUs/" + interval_comment_whole + "/node" + node + suffix + interval_comment_whole + "_active_cores_and_jobs"
        node_data = pd.read_csv(infile_node + ".csv", index_col=0)

        ### select data in the specified interval
        interval_data = node_data.loc[(pd.to_datetime(node_data['timestamp']) >= start_time) & (pd.to_datetime(node_data['timestamp']) <= end_time)]
        print("data row from {} to {}: {}".format(start_time, end_time, interval_data.shape[0]))

        # write
        outfile = datadir + 'CPUs/' + interval_comment + "/node" + node + "_1min_" + interval_comment + "_active_cores_and_jobs.csv"
        interval_data.to_csv(outfile)
        
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



