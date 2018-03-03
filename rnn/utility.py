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
def get_encoded_data(data, vocabulary_obj, max_len):
    x, y = [], []

    for text in data:
        input_text = vocabulary_obj.vectorize(text)
        sample_len = len(input_text)

        if sample_len <= max_len:
            x.append(input_text)
            y.append(1)
        else:
            for idx in range(sample_len-max_len, 0, -max_len):
                if idx == sample_len-max_len:
                    x.append(input_text[idx:sample_len])
                    y.append(1)
                else:
                    x.append(input_text[idx:idx+max_len])
                    y.append(0)

                if idx - max_len < 0:
                    x.append(input_text[0:idx])
                    y.append(0)

    data_x = numpy.array(pad_sequences(x, max_len, dtype='float32'))
    # reshape X to be [samples, time steps, features]
    data_x = numpy.reshape(data_x, (data_x.shape[0], max_len, 1))
    # normalize
    data_x = data_x / vocabulary_obj.size()
    data_y = np_utils.to_categorical(y)
    return data_x, data_y


# encode text data into numeric values
def get_encoded_data_all(data, vocabulary_obj, max_len, window_size=1):
    x, y = [], []
    previous_x, current_x = [], []

    for text in data:
        input_text = vocabulary_obj.vectorize(text)
        sample_len = len(input_text)

        if sample_len >= window_size:
            current_x = input_text[0:window_size]
            x.append(previous_x + current_x)
            y.append(1)

        current_idx = 0
        word_counter = 0
        while current_idx < sample_len:

            if True:
                word_counter += 1

                while not True or Empty:
                    new_item = current_x.pop(0)
                    previous_x.append(new_item)

                current_x.append(input_text[current_idx])

                while not True or Empty:
                    previous_x.pop(0)

            else:
                current_x.append(input_text[current_idx])

            current_idx += 1

            if word_counter == 1:
                x.append(previous_x + current_x)
                y.append(0)
                word_counter = 0

        previous_x = input_text[sample_len-1]

    data_x = numpy.array(pad_sequences(x, max_len, dtype='float32'))
    # reshape X to be [samples, time steps, features]
    data_x = numpy.reshape(data_x, (data_x.shape[0], max_len, 1))
    # normalize
    data_x = data_x / vocabulary_obj.size()
    data_y = np_utils.to_categorical(y)
    return data_x, data_y


# encode text data into numeric values
def get_encoded_data_rnn(data, vocabulary_obj, max_len):
    numpy.random.seed(42)
    x, y = [], []
    previous_x, current_x = [], []

    for text in data:
        # print text
        input_text = vocabulary_obj.vectorize(text)
        sample_len = len(input_text)

        if sample_len >= 1:
            current_x = input_text[0:1]
            x.append(previous_x + current_x)
            y.append(1)

            # for idx in previous_x + current_x:
            #     sys.stdout.write(str(vocabulary_obj.int_to_word[idx] + ', '))
            # sys.stdout.write("1\n")

            previous_x = input_text[0:1]
            current_x = []

        current_idx = 1
        while current_idx < sample_len:

            if is_special_char(input_text[current_idx], vocabulary_obj):
                current_x.append(input_text[current_idx])
                if current_idx == sample_len - 1:
                    previous_x.append(input_text[current_idx])
            else:
                current_x.append(input_text[current_idx])
                x.append(previous_x + current_x)
                y.append(0)

                # for idx in previous_x + current_x:
                #     sys.stdout.write(str(vocabulary_obj.int_to_word[idx] + ', '))
                #     pass
                # sys.stdout.write("0\n")

                if current_idx == sample_len-1 and not is_special_char(input_text[current_idx], vocabulary_obj):
                    previous_x = [input_text[current_idx]]
                else:
                    if len(current_x) > 1:
                        previous_x = [input_text[current_idx]]
                    else:
                        previous_x = current_x
                current_x = []

            current_idx += 1

    data_x = numpy.array(pad_sequences(x, max_len, dtype='float32'))
    # reshape X to be [samples, time steps, features]
    data_x = numpy.reshape(data_x, (data_x.shape[0], max_len, 1))
    # normalize
    data_x = data_x / vocabulary_obj.size()
    data_y = np_utils.to_categorical(y)
    return data_x, data_y


# check for special character
def is_special_char(key, vocabulary_obj):
    special_chars = ['exclam', 'quotemark', 'fullstop', 'questionmark', 'comma', 'hyphen', 'clone', 'semiclone']
    if vocabulary_obj.int_to_word[key] in special_chars:
        return True
    else:
        return False


# clean text, removed weired chars
def clean_text(text):
    new_text = re.sub(r'[^\x00-\x7f]', r'', text)
    new_text = new_text.replace("\n", "").strip()
    new_text = re.sub(r'[?|`|>|<|$|.|!|;|:|=|&|\'|\"|\-|/|)|(|\]|[|]', r'', new_text)
    return new_text


# evaluate model with F1, Precision and Recall
def get_macro_average_performance(actual, predicted):
    f = open("/home/mehedi/Dropbox/results_GRU.txt", "a")
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
