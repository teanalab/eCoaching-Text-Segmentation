from __future__ import absolute_import
from .glove import *
from .word2vec import *

class AvailableEmbeddings:
	map_emb = {
		'word2vec': Word2Vec,
		'glove': 	Glove,
		'id': 		IdVectors
	}
	@staticmethod
	def get(word):
		return AvailableEmbeddings.map_emb[word]
