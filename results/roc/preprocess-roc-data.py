# include required packages
import os
import argparse
import numpy


# convert roc.arff to roc.csv for figure
def create_roc_csv(path):
    for file_name in os.listdir(path):
        file_name_writer = path + "/" + file_name.replace(".arff", "") + ".csv"
        if 'summit' not in file_name and 'csv' not in file_name:
            fw = open(file_name_writer, "w")
            fw.write("fpr,tpr\n")
            with open(path + "/" + file_name, "r") as file_stream:
                for line in file_stream:
                    if '@' in line or len(line.strip()) == 0:
                        continue
                    current_line = line.split(",")
                    fw.write(current_line[5] + "," + current_line[6] + "\n")
            fw.close()

# parse command line arguments
parser = argparse.ArgumentParser(description='Prepare data for ROC curve.')
parser.add_argument('--path', default='False',
                    help='Folder location containing raw roc data.')
args = parser.parse_args()

# set arguments value
folder_path = args.path

# call above functions for the creation of new files
create_roc_csv(folder_path)
print('conversion is done!')
