#!/usr/bin/env python3

import pandas as pd
import errno    
import os

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
    datadir = "/datasets/eurora_data/db/CPUs/"

    interval_comment = "April"
    start_time = pd.to_datetime('2014-04-01')
    end_time = pd.to_datetime('2014-05-01')

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
        infile = datadir + "cpu" + cpu + ".csv"
