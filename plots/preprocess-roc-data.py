# include required packages
import os


# convert roc.arff to roc.csv for figure
def create_roc_csv(path):
    for file_name in os.listdir(path):
        file_name_writer = path + "/" + file_name.replace(".txt", ".csv")
        if 'csv' not in file_name:
            fw = open(file_name_writer, "w")
            fw.write("actual,predict_prob\n")
            truths = []
            preds = []
            with open(path + "/" + file_name, "r") as file_stream:
                flag = 0
                for line in file_stream:
                    current_line = line.strip().split()
                    if len(current_line) < 10:
                        current_line = line.strip().split(',')
                    if flag % 2 == 0:
                        for x in current_line:
                            truths.append(x)
                    else:
                        for x in current_line:
                            preds.append(x)
                    flag += 1

                print(path + "/" + file_name)
                for i in range(0, len(truths)):
                    fw.write(truths[i] + "," + preds[i] + "\n")

            fw.close()

# call above functions for the creation of new files
create_roc_csv('roc/base')
create_roc_csv('roc/propose')
print('conversion is done!')
