# RNN with variable length input sequences to one word output
import numpy

from keras.models import Sequential
from keras.layers import Dense
from keras.layers import GRU
from keras.layers import LSTM
from keras.utils import np_utils
from keras.preprocessing.sequence import pad_sequences

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
max_len = 10
folds = 5
data = utility.get_ecoaching_data()
folds_data = utility.split_data(data, folds)
results = []

for fold_num in range(folds):

    train_data = []
    for i in range(folds):
        if i != fold_num:
            train_data = train_data + folds_data[i]

    # get encoded training data
    X, y = utility.get_encoded_data(train_data, vocabulary_obj, max_len)

    # reshape X to be [samples, time steps, features]
    X = numpy.reshape(X, (X.shape[0], max_len, 1))

    # normalize
    X = X / float(vocabulary_obj.size())

    # print total number of instances
    print("# of instances: ", len(y))

    # create and fit the model
    batch_size = 16
    model = Sequential()
    model.add(LSTM(32, input_shape=(X.shape[1], 1)))
    # model.add(GRU(32, input_shape=(X.shape[1], 1)))
    model.add(Dense(y.shape[1], activation='softmax'))
    model.compile(loss='categorical_crossentropy', optimizer='adam', metrics=['accuracy'])
    model.fit(X, y, epochs=5, batch_size=batch_size, verbose=2)

    # summarize performance of the model
    scores = model.evaluate(X, y, verbose=0)
    print("\nModel Accuracy: %.2f%%" % (scores[1]*100) + " for fold " + str(fold_num+1) + "\n\n")
    results.append([fold_num, scores[1]])

# print results
print("Avg. Accuracy of Model: ")
print(numpy.mean(results, axis=0))

# demonstrate some model predictions
# for i in range(20):
#     pattern_index = numpy.random.randint(len(dataX))
#     pattern = dataX[pattern_index]
#     x = pad_sequences([pattern], maxlen=max_len, dtype='float32')
#     x = numpy.reshape(x, (1, max_len, 1))
#     x = x / float(vocabulary_obj.size())
#     prediction = model.predict(x, verbose=0)
#     index = numpy.argmax(prediction)
#     result = int_to_char[index]
#     seq_in = [int_to_char[value] for value in pattern]
#     print seq_in, "->", result
