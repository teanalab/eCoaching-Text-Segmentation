# include required packages
import os
import argparse
import numpy


# convert roc.arff to roc.csv for figure
def create_roc_csv(path):
    for file_name in os.listdir(path):
        file_name_writer = path + "/" + file_name.replace(".arff", "") + ".csv"
        if 'RNN' not in file_name and 'csv' not in file_name:
            fw = open(file_name_writer, "w")
            fw.write("fpr,tpr\n")
            with open(path + "/" + file_name, "r") as file_stream:
                for line in file_stream:
                    if '@' in line or len(line.strip()) == 0:
                        continue
                    current_line = line.split(",")
                    fw.write(current_line[5] + "," + current_line[6] + "\n")
            fw.close()


# convert rnn results to roc.csv for figure
def create_roc_csv_for_rnn(path):
    for file_name in os.listdir(path):
        if 'RNN' in file_name and 'csv' not in file_name:
            actual = []
            predicted = []
            count = 0
            with open(path + "/" + file_name, "r") as file_stream:
                for line in file_stream:
                    count += 1
                    if count == 1:
                        current_line = line.split(",")
                        actual += current_line
                    else:
                        current_line = line.split(",")
                        predicted += current_line
                        count = 0

            sorted_actual, sorted_predicted = [], []
            for i in numpy.fliplr([numpy.argsort(predicted)])[0]:
                sorted_actual.append(actual[i])
                sorted_predicted.append(predicted[i])

            # write prediction and actual values
            auc_file_name_writer = path + "/auc_" + file_name + ".csv"
            file_w = open(auc_file_name_writer, "w")
            file_w.write("actual,prediction\n")
            for i, j in zip(sorted_actual, sorted_predicted):
                if len(i.strip()) > 0:
                    file_w.write(i + "," + j + "\n")
            file_w.close()


# parse command line arguments
parser = argparse.ArgumentParser(description='Prepare data for ROC curve.')
parser.add_argument('--path', default='False',
                    help='Folder location containing raw roc data.')
args = parser.parse_args()

# set arguments value
folder_path = args.path

# call above functions for the creation of new files
create_roc_csv(folder_path)
create_roc_csv_for_rnn(folder_path)
print('conversion is done!')
