# RNN with variable length input sequences to one word output
import numpy

from keras.models import Sequential
from keras.layers import Dense
from keras.layers import GRU
from keras.layers import LSTM
from keras.layers import Bidirectional
from keras.layers.embeddings import Embedding
from keras.callbacks import EarlyStopping, ModelCheckpoint
from keras.regularizers import l1_l2

import utility

from vocabulary import Vocabulary

# fix random seed for reproducibility
numpy.random.seed(42)

# create e-coaching dictionary
vocabulary_obj = Vocabulary()
vocabulary_obj.create()

# create mapping of characters to integers (0-25) and the reverse
char_to_int = dict((c, i+1) for i, c in enumerate(vocabulary_obj.vocab))
int_to_char = dict((i+1, c) for i, c in enumerate(vocabulary_obj.vocab))

# system configuration
max_len = 100
folds = 5
embed_dim = 30
lstm_out = 32
dropout_x = 0.5
batch_size = 64

# prepare the data set of input to output pairs encoded as integers
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
    X, y = utility.get_encoded_long_sequence(train_data, vocabulary_obj, max_len)

    # print total number of instances
    print("# of instances: ", len(y))

    # create and fit the model
    model = Sequential()
    model.add(Embedding(vocabulary_obj.size(), embed_dim, input_length=X.shape[1]))
    # model.add(GRU(lstm_out, recurrent_regularizer=l1_l2(l1=0.0, l2=0.033), dropout=dropout_x))
    # model.add(LSTM(lstm_out, input_shape=(X.shape[1], 1)))
    model.add(Bidirectional(GRU(lstm_out)))
    model.add(Dense(y.shape[1], activation='softmax'))
    model.compile(loss='categorical_crossentropy', optimizer='adam', metrics=['accuracy'])
    # early_stop = EarlyStopping(monitor='val_acc', min_delta=0.00005, patience=20, verbose=1, mode='auto')
    # callbacks_list = [early_stop]
    # model.fit(X, y, epochs=1500, callbacks=callbacks_list, batch_size=batch_size, verbose=2, validation_split=0.2)
    model.fit(X, y, epochs=300, batch_size=batch_size, verbose=2)

    # summarize performance of the model
    scores = model.evaluate(X, y, verbose=0)
    print("\nModel Accuracy: %.2f%%" % (scores[1]*100) + " for fold " + str(fold_num+1) + "\n\n")

    # get encoded test data
    test_x, test_y = utility.get_encoded_data_rnn(test_data, vocabulary_obj, max_len)
    prediction_y = model.predict(test_x, verbose=0)
    index_prediction_y = numpy.argmax(prediction_y, axis=1)
    index_original_y = numpy.argmax(test_y, axis=1)
    accuracy, precision, recall, f_measure = utility.get_macro_average_performance(index_original_y, index_prediction_y)
    results.append([fold_num, precision, recall, f_measure])

# print results
print("Avg. performance of the model: ")
print(numpy.mean(results, axis=0))
