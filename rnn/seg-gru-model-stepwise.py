import numpy

from keras.models import Sequential
from keras.layers import GRU, LSTM
from keras.layers import Dense
from keras.layers import TimeDistributed
from keras.callbacks import Callback

import utility_stepwise_gru

from vocabulary import Vocabulary

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

# create mapping of word to integers and the reverse
char_to_int = dict((c, i+1) for i, c in enumerate(vocabulary_obj.vocab))
int_to_char = dict((i+1, c) for i, c in enumerate(vocabulary_obj.vocab))

# prepare the data set of input to output pairs encoded as integers
data = utility_stepwise_gru.get_ecoaching_data()
folds_data = utility_stepwise_gru.split_data(data, folds)
results = []

for fold_num in range(folds):
    train_data, test_data = [], []
    for i in range(folds):
        if i == fold_num:
            test_data = folds_data[i]
        else:
            train_data = train_data + folds_data[i]

    # get encoded training data
    X, y = utility_stepwise_gru.get_encoded_sequence(train_data, vocabulary_obj, max_len)

    # print total number of instances
    print("# of instances: ", len(y))

    # create and fit the model
    model = Sequential()
    model.add(GRU(lstm_out, input_shape=(max_len, 1), return_sequences=True))
    model.add(TimeDistributed(Dense(1, activation='sigmoid')))
    model.compile(loss='binary_crossentropy', optimizer='adam', metrics=['accuracy'])

    for i in range(X.shape[0]):
        x_data = X[i]
        y_data = y[i]
        if i % 100 == 0:
            print("Records processed: ", i)
        model.fit(x_data, y_data, callbacks=[ResetStatesCallback()], shuffle=False, batch_size=batch_size, verbose=0)

    # get encoded test data
    test_x, test_y = utility_stepwise_gru.get_encoded_sequence(test_data, vocabulary_obj, max_len)

    prediction_y = []
    prediction_prob_y = []
    original_y = []
    count_one = 0
    count_zero = 0

    for i in range(test_x.shape[0]):
        predict_y = model.predict_classes(test_x[i], verbose=0)
        predict_prob_y = model.predict_proba(test_x[i], verbose=0)
        orig_y = test_y[i]

        for j in range(max_len):
            if j == max_len-1:
                # print('Expected:', orig_y[0, j][0], 'Predicted', prediction_y[0, j][0])
                if orig_y[0, j][0] == predict_y[0, j][0]:
                    count_one += 1
            elif orig_y[0, j][0] == predict_y[0, j][0]:
                count_zero += 1

            prediction_y.append(predict_y[0, j][0])
            prediction_prob_y.append(predict_prob_y[0, j][0])
            original_y.append(orig_y[0, j][0])


    f = open("results_GRU_summit.txt", "a")
    f.write("\n\nCount one: " + str(count_one))
    f.write("\nCount zero: " + str(count_zero))
    f.flush()
    f.close()

    f = open("roc_GRU_summit.txt", "a")
    for i in range(len(prediction_prob_y)):
        f.write("\n" + str(original_y[i]) + "," + str(prediction_y[i]) + "," + str(prediction_prob_y[i]))
    f.flush()
    f.close()

    accuracy, precision, recall, f_measure = utility_stepwise_gru.get_macro_average_performance(original_y, prediction_y)
    results.append([fold_num, precision, recall, f_measure])

# print results
print("Avg. performance of the model: ")
print(numpy.mean(results, axis=0))

