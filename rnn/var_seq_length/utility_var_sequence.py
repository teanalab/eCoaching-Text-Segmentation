# include required packages
import numpy
import re
import sys

from keras.utils import np_utils
from keras.preprocessing.sequence import pad_sequences


# load e-coaching data from datafile
def get_ecoaching_data(file_name='e-coaching_data_stem.txt'):
    data = []
    with open(file_name, "r") as file_stream:
        for line in file_stream:
            current_line = line.split(",")
            if len(current_line) > 1:
                data.append(clean_text(current_line[0]).strip())
    return data


# split data into 5 folds
def split_data(data, folds=5):
    fold_size = len(data)/folds
    folds_data = []
    for i in range(folds):
        start, end = i*fold_size, i*fold_size + fold_size
        folds_data.append(data[start: end])
    return folds_data


# encode text data into numeric values
def get_encoded_sequence(data, vocabulary_obj):
    x, y = [], []

    for text in data:
        sample_x = vocabulary_obj.vectorize(text)
        sample_y = [0]*(len(sample_x)-1)
        sample_y.append(1)

        sample_x = [a / float(vocabulary_obj.size()) for a in sample_x]
        x.append(sample_x)
        y.append(sample_y)

    return x, y


# clean text, removed weired chars
def clean_text(text):
    new_text = re.sub(r'[^\x00-\x7f]', r'', text)
    new_text = new_text.replace("\n", "").strip()
    new_text = re.sub(r'[?|`|>|<|$|.|!|;|:|=|&|\'|\"|\-|/|)|(|\]|[|]', r'', new_text)
    return new_text


# evaluate model with F1, Precision and Recall
def get_macro_average_performance(actual, predicted):
    f = open("/home/mehedi/Dropbox/results_sequence_GRU.txt", "a")
    f.write("fold number \n")
    f.write("actual: ")
    f.writelines(["%s," % item for item in actual])
    f.write("\npredicted: ")
    f.writelines(["%s," % item for item in predicted])
    f.write("\n")

    precision = 0.0
    recall = 0.0
    f_measure = 0.0
    accuracy = 0.0

    labels = [1, 0, 0, 1]

    for k in [0, 2]:

        tp = 0
        fp = 0
        tn = 0
        fn = 0

        for i in range(0, len(actual)):
            if actual[i] == predicted[i] and actual[i] == labels[k]:
                tp += 1
            elif actual[i] != predicted[i] and actual[i] == labels[k]:
                fn += 1
            elif actual[i] == predicted[i] and actual[i] == labels[k + 1]:
                tn += 1
            elif actual[i] != predicted[i] and actual[i] == labels[k + 1]:
                fp += 1

        local_precision = 0.0
        local_recall = 0.0
        local_f_measure = 0.0

        if (tp + fp) > 0:
            local_precision = (float(tp) / (tp + fp))
        if (tp + fn) > 0:
            local_recall = (float(tp) / (tp + fn))
        if (local_precision + local_recall) > 0:
            local_f_measure = (float(2 * local_precision * local_recall) / (local_precision + local_recall))
        local_accuracy = (float(tp + tn) / (tp + fp + tn + fn))

        # for checking calculation
        print tp, fp, tn, fn
        print local_accuracy, local_precision, local_recall, local_f_measure

        precision += local_precision
        recall += local_recall
        f_measure += local_f_measure
        accuracy += local_accuracy

        # Write results into file
        f.write(str(tp) + ", " + str(fp) + ", " + str(tn) + ", " + str(fn))
        f.write("\n")
        f.write(str(local_precision) + "," + str(local_recall) + "," + str(local_f_measure))
        f.write("\n")

    f.write("\n\n")
    f.write(str(precision / 2) + "," + str(recall / 2) + "," + str(f_measure / 2))
    f.write("\n\n")
    f.flush()
    f.close()

    return accuracy / 2, precision / 2, recall / 2, f_measure / 2