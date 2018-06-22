from abc import ABCMeta, abstractmethod
import numpy as np


class Embedding(metaclass=ABCMeta):
    def __init__(self, lowercase=False, dimensions=50, min_count=5, workers=4, window_size=5, epochs=5):
        self.lowercase = lowercase
        self.dimensions = dimensions
        self.min_count = min_count
        self.workers = workers
        self.window_size = window_size
        self.epochs = epochs
        self.model = dict()
        self.random_vector = None
        self._vocabulary = None

    @abstractmethod
    def load(self, load_file):
        pass

    @abstractmethod
    def save(self, save_file):
        pass

    @abstractmethod
    def train(self, train_file):
        pass

    @property
    def vocabulary(self):
        if self._vocabulary is None:
            self._vocabulary = dict(zip(self.model.keys(), range(len(self.model))))
        return self._vocabulary

    @property
    def vocabulary_size(self):
        return len(self.vocabulary)

    def get_vector(self, word):
        if word in self.model:
            return self.model[word]
        if self.lowercase and word.lower() in self.model:
            return self.model[word.lower()]
        if self.random_vector is None:
            self.random_vector = self.generate_random_vector()
        return self.random_vector

    def generate_random_vector(self):
        epsilon = np.sqrt(6) / np.sqrt(self.dimensions)
        return np.random.random(self.dimensions) * 2 * epsilon - epsilon
