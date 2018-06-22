import logging
import numpy as np

from deepecoach.dataset import SentTokenizer
from deepecoach.features import AvailableEmbeddings, POS

logger = logging.getLogger(__name__)
tokenizer = SentTokenizer()  # vocabulary


class Features:
    def __init__(self, embedding_type='random', embedding_file=None,
                 POS_type='nlpnet', POS_file=None, use_pos=True, use_embeddings=True):

        # POS
        self.use_pos = use_pos
        self.pos = POS(POS_type)
        if self.use_pos:
            self.pos.load(POS_file)

        # EMBEDDINGS
        self.use_embeddings = use_embeddings
        self.embedding_type = embedding_type
        self.embedding_file = embedding_file
        self.embeddings = AvailableEmbeddings.get(embedding_type)()
        self.embeddings.load(tokenizer.word_index if embedding_type == 'id' else embedding_file)

    def save(self, filename):
        import json
        data = {
            'use_pos': self.use_pos,
            'use_embeddings': self.use_embeddings,
            'POS_type': self.pos.type,
            'POS_file': self.pos.filename,
            'embedding_type': self.embedding_type,
            'embedding_file': self.embedding_file,
        }
        with open(filename, 'w') as f:
            json.dump(data, f)

    def load(self, filename):
        import json
        with open(filename, 'r') as f:
            data = json.load(f)
        self.__init__(**data)

    def info(self):
        if self.use_embeddings:
            logger.info('Embeddings type: {}'.format(type(self.embeddings).__name__))
            logger.info('Embeddings dim: {}'.format(self.embeddings.dimensions))
            logger.info('Embeddings vocab size: {}'.format(len(self.embeddings.vocabulary)))
        if self.use_pos:
            logger.info('POS type: {}'.format(self.pos.type))
            logger.info('POS vocab size: {}'.format(len(self.pos.vocabulary)))
        logger.info('Prosodic type: {}'.format(self.prosodic.type))
        logger.info('Prosodic classify: {}'.format(self.prosodic.classify))
        logger.info('Prosodic nb features: {}'.format(self.prosodic.nb_features))
        logger.info('Prosodic first: {}'.format(self.prosodic.nb_first))
        logger.info('Prosodic last: {}'.format(self.prosodic.nb_last))

    def get_embeddings(self, vocabulary):
        vocab_size = max(vocabulary.values()) + 1
        weights = np.random.randn(vocab_size, self.embeddings.dimensions)
        for word, index in vocabulary.items():
            weights[index] = self.embeddings.get_vector(word)
        return weights

    def get_POS(self, texts):
        return self.pos.get(texts)
