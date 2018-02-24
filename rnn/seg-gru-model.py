# RNN with variable length input sequences to one word output
import numpy

from keras.models import Sequential
from keras.layers import Dense
from keras.layers import GRU
from keras.layers import LSTM

import utility

from vocabulary import Vocabulary

# fix random seed for reproducibility
numpy.random.seed(7)

# create e-coaching dictionary
vocabulary_obj = Vocabulary()
vocabulary_obj.create()

# create mapping of characters to integers (0-25) and the reverse
char_to_int = dict((c, i+1) for i, c in enumerate(vocabulary_obj.vocab))
int_to_char = dict((i+1, c) for i, c in enumerate(vocabulary_obj.vocab))

# prepare the data set of input to output pairs encoded as integers
f = open("results.txt", "a")
max_len = 5
folds = 5
data = utility.get_ecoaching_data()
folds_data = utility.split_data(data, folds)
results = []

for fold_num in range(folds):

    train_data, test_data = [], []
    for i in range(folds):
        if i == fold_num:
            test_data = folds_data[i]
        else:
            train_data = train_data + folds_data[i]

    # get encoded training data
    X, y = utility.get_encoded_data(train_data, vocabulary_obj, max_len)

    # print total number of instances
    print("# of instances: ", len(y))

    # create and fit the model
    batch_size = 1
    model = Sequential()
    # model.add(LSTM(32, input_shape=(X.shape[1], 1)))
    model.add(GRU(32, input_shape=(X.shape[1], 1)))
    model.add(Dense(y.shape[1], activation='softmax'))
    model.compile(loss='categorical_crossentropy', optimizer='adam', metrics=['accuracy'])
    model.fit(X, y, epochs=200, batch_size=batch_size, verbose=2)

    # summarize performance of the model
    scores = model.evaluate(X, y, verbose=0)
    print("\nModel Accuracy: %.2f%%" % (scores[1]*100) + " for fold " + str(fold_num+1) + "\n\n")

    # get encoded test data
    test_x, test_y = utility.get_encoded_data(test_data, vocabulary_obj, max_len)
    prediction_y = model.predict(test_x, verbose=0)
    index_prediction_y = numpy.argmax(prediction_y, axis=1)
    index_original_y = numpy.argmax(test_y, axis=1)
    accuracy, precision, recall, f_measure = utility.get_macro_average_performance(index_original_y, index_prediction_y)
    results.append([fold_num, precision, recall, f_measure])

    # Write results into file
    f.write("fold number " + str(fold_num+1) + ": ")
    f.write(str(precision) + "," + str(recall) + "," + str(f_measure))
    f.write("\n")

f.close()

# print results
print("Avg. performance of the model: ")
print(numpy.mean(results, axis=0))
