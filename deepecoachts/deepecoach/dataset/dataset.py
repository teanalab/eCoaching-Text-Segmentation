import logging
import re
from abc import ABCMeta, abstractmethod
from os import listdir
from numpy.random import shuffle as shuffle_list
from deepecoach.dataset import SentTokenizer

logger = logging.getLogger(__name__)
tokenizer = SentTokenizer()  # vocabulary


class DataSet(metaclass=ABCMeta):
    def __init__(self, text_dir=None, vocabulary=None):
        """
        Class to deal with an annotated corpus
        :param text_dir: a path to text files (.txt)
        """
        if vocabulary is not None and not tokenizer.loaded:
            tokenizer.load_vocabulary(vocabulary)
        self.texts = []
        self.word_texts = []
        self.word_id_texts = []

        if text_dir:
            self._read_text_dir(text_dir)
            self._fit_tokenizer()
            self._set_nb_attributes()

    def _read_text_dir(self, dname):
        slash = '' if dname[-1] == '/' else '/'
        for fname in sorted(listdir(dname)):
            try:
                self._read_text_file(dname + slash + fname)
            except:
                pass

    def _read_text_file(self, filename):
        f = open(filename, 'r', encoding='utf8')
        txt = self._clean_text_file(f.read().strip())
        tks = txt.strip().split()
        min_sentence_size = 5
        if txt.strip() and len(tks) >= min_sentence_size and tks[0] not in '.!?:;':
            self.texts.append(txt)
        else:
            print(txt)
        f.close()

    @abstractmethod
    def _clean_text_file(self, text):
        """
        should return a text with only one line and . as punctation mark
        """
        pass

    def _fit_tokenizer(self, build_vocab=True):
        if not self.texts:
            raise Exception('You should read the data before fit the tokenizer!')
        self.word_texts = tokenizer.fit_on_texts(self.texts, build_vocab=build_vocab)
        self.word_id_texts = tokenizer.word_texts_to_indexes(self.word_texts)
        self.vocabulary = tokenizer.word_index

    def _set_nb_attributes(self):
        self.nb_texts = len(self.word_texts)
        self.nb_sentences = sum(map(lambda x: x.count('newsegment'), self.word_texts))
        self.nb_words = sum(map(len, self.word_texts))
        self.max_sentence_size = max(map(lambda x: len(x) - x.count('newsegment'), self.word_texts))
        self.nb_classes = len(tokenizer.labels)
        self.vocab_size = len(tokenizer.word_index)

    def indexes_to_words(self, indexes_texts):
        return tokenizer.index_texts_to_text(indexes_texts)

    def info(self):
        logger.info(type(self).__name__)
        logger.info('Nb texts: {}'.format(self.nb_texts))
        logger.info('Nb sentences: {}'.format(self.nb_sentences))
        logger.info('Nb words: {}'.format(self.nb_words))
        logger.info('Nb classes: {}'.format(self.nb_words))


class DirDataSet(DataSet):
    def _clean_text_file(self, text):
        text = re.sub(r'(\S+)([\*\.])', r'\1 \2', text.strip())
        text = re.sub(r'([\*\.])(\S+)', r'\1 \2', text.strip())
        return re.sub(r'\ +', ' ', text.strip())


class DataSetTS:
    def __init__(self, binary=False):
        self.texts = []
        self.word_texts = []
        self.word_id_texts = []
        self.shuffle_indexes = []
        self.binary = binary

    def indexes_to_words(self, indexes_texts):
        return tokenizer.index_texts_to_text(indexes_texts)

    def as_matrix(self, ids=True):
        x, y = [], []
        end_period = tokenizer.word_index['.'] if ids else '.'
        texts = self.word_id_texts if ids else self.word_texts
        for i, text in enumerate(texts):
            x_, y_ = [], []
            for word in text:
                if word == end_period:
                    y_[-1] = 1
                else:
                    x_.append(word)
                    y_.append(0)
            y_[-1] = 1
            x.append(x_)
            y.append(y_)
        return x, y

    def shuffle(self, olds=[]):
        shuffle_list(self.shuffle_indexes)
        t, wt, wi, pt, si = [], [], [], [], []
        for i in self.shuffle_indexes:
            if i not in olds:
                t.append(self.texts[i])
                wt.append(self.word_texts[i])
                wi.append(self.word_id_texts[i])
                si.append(i)
        for i in self.shuffle_indexes:
            if i in olds:
                t.append(self.texts[i])
                wt.append(self.word_texts[i])
                wi.append(self.word_id_texts[i])
                si.append(i)
        self.texts = t
        self.word_texts = wt
        self.word_id_texts = wi
        self.shuffle_indexes = si

    def truncate(self, ratio):
        limit = len(self.texts) - int(len(self.texts) * ratio)
        ds_leftover = DataSetTS()
        ds_leftover.texts = self.texts[limit:]
        ds_leftover.word_texts = self.word_texts[limit:]
        ds_leftover.word_id_texts = self.word_id_texts[limit:]
        ds_leftover.shuffle_indexes = self.shuffle_indexes[limit:]
        self.texts = self.texts[:limit]
        self.word_texts = self.word_texts[:limit]
        self.word_id_texts = self.word_id_texts[:limit]
        self.shuffle_indexes = self.shuffle_indexes[:limit]
        return ds_leftover

    def info(self):
        nb_texts = len(self.texts)
        nb_sentences = sum(map(lambda x: x.count('.'), self.texts))
        nb_words = sum(map(len, self.word_id_texts))
        logger.info(type(self).__name__)
        logger.info('Nb texts: {}'.format(nb_texts))
        logger.info('Nb sentences: {}'.format(nb_sentences))
        logger.info('Nb words: {}'.format(nb_words))
        logger.info('Chance Baseline: {0:.4%}'.format(nb_sentences / (nb_sentences + nb_words + int(nb_words == 0))))
