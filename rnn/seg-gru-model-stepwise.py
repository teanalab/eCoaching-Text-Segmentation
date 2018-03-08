# RNN with variable length input sequences to one word output
import numpy

from random import random
from numpy import array
from numpy import cumsum
from keras.models import Sequential
from keras.layers import LSTM
from keras.layers import GRU
from keras.layers import Dense
from keras.layers import TimeDistributed
from keras.callbacks import EarlyStopping, ModelCheckpoint, Callback

import utility_stepwise

from vocabulary import Vocabulary

# fix random seed for reproducibility
numpy.random.seed(42)

# system configuration
max_len = 50
folds = 5
lstm_out = 25
batch_size = 1


class ResetStatesCallback(Callback):
    def __init__(self):
        self.counter = 0

    def on_batch_begin(self, batch, logs={}):
        if self.counter % max_len == 0:
            self.model.reset_states()
            # print("Reset States..............")
        self.counter += 1


# create e-coaching dictionary
vocabulary_obj = Vocabulary()
vocabulary_obj.create()

# create mapping of characters to integers (0-25) and the reverse
char_to_int = dict((c, i+1) for i, c in enumerate(vocabulary_obj.vocab))
int_to_char = dict((i+1, c) for i, c in enumerate(vocabulary_obj.vocab))

# prepare the data set of input to output pairs encoded as integers
data = utility_stepwise.get_ecoaching_data()
folds_data = utility_stepwise.split_data(data, folds)
results = []

for fold_num in range(folds):

    train_data, test_data = [], []
    for i in range(folds):
        if i == fold_num:
            test_data = folds_data[i]
        else:
            train_data = train_data + folds_data[i]

    # get encoded training data
    X, y = utility_stepwise.get_encoded_sequence(train_data, vocabulary_obj, max_len)

    # print total number of instances
    print("# of instances: ", len(y))

    # create and fit the model
    model = Sequential()
    model.add(LSTM(lstm_out, input_shape=(max_len, 1), return_sequences=True))
    model.add(TimeDistributed(Dense(1, activation='sigmoid')))
    model.compile(loss='binary_crossentropy', optimizer='adam', metrics=['accuracy'])
    # early_stop = EarlyStopping(monitor='val_acc', min_delta=0.00005, patience=20, verbose=1, mode='auto')
    # callbacks_list = [early_stop]

    for i in range(X.shape[0]):
        x_data = X[i]
        y_data = y[i]
        if i % 100 == 0:
            print("Records processed: ", i)
        model.fit(x_data, y_data, callbacks=[ResetStatesCallback()], shuffle=False, batch_size=batch_size, verbose=0)

    # get encoded test data
    test_x, test_y = utility_stepwise.get_encoded_sequence(test_data, vocabulary_obj, max_len)

    prediction_y = []
    original_y = []
    count_one = 0
    count_zero = 0

    for i in range(test_x.shape[0]):
        predict_y = model.predict_classes(test_x[i], verbose=0)
        orig_y = test_y[i]
        for j in range(max_len):
            if j == max_len-1:
                # print('Expected:', orig_y[0, j][0], 'Predicted', prediction_y[0, j][0])
                if orig_y[0, j][0] == predict_y[0, j][0]:
                    count_one += 1
            elif orig_y[0, j][0] == predict_y[0, j][0]:
                count_zero += 1

            prediction_y.append(predict_y[0, j][0])
            original_y.append(orig_y[0, j][0])


    f = open("/home/mehedi/Dropbox/results_sequence_GRU.txt", "a")
    f.write("\n\nCount one: " + str(count_one))
    f.write("\nCount zero: " + str(count_zero))
    f.flush()
    f.close()

    accuracy, precision, recall, f_measure = utility_stepwise.get_macro_average_performance(original_y, prediction_y)
    results.append([fold_num, precision, recall, f_measure])

# print results
print("Avg. performance of the model: ")
print(numpy.mean(results, axis=0))

