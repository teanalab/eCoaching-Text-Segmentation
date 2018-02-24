# include required packages
import numpy
import re

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
        input = vocabulary_obj.vectorize(text)
        sample_len = len(input)

        if sample_len <= max_len:
            x.append(input)
            y.append(1)
        else:
            for idx in range(sample_len-max_len, 0, -max_len):
                if idx == sample_len-max_len:
                    x.append(input[idx:sample_len])
                    y.append(1)
                else:
                    x.append(input[idx:idx+max_len])
                    y.append(0)

                if idx - max_len < 0:
                    x.append(input[0:idx])
                    y.append(0)

    data_x = numpy.array(pad_sequences(x, max_len, dtype='float32'))
    data_y = np_utils.to_categorical(y)
    return data_x, data_y


# clean text, removed weired chars
def clean_text(text):
    new_text = re.sub(r'[^\x00-\x7f]', r'', text)
    new_text = new_text.replace("\n", "").strip()
    new_text = re.sub(r'[?|`|>|<|$|.|!|;|:|=|&|\'|\"|\-|/|)|(|\]|[|]', r'', new_text)
    return new_text
