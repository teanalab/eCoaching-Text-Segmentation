from deepecoach.dataset import DataSet
from deepecoach.helpers import Cleaner
from deepecoach.dataset import SentTokenizer
import re

tokenizer = SentTokenizer()  # vocabulary


class RawDataSet(DataSet):
    def __init__(self, texts=[], vocabulary=None):
        """
        Class to deal with an annotated corpus
        :param texts: list of raw texts as strings
        :param audios: list of paths to a prosodic file (.csv)
        ---
        The nth text and the nth audio should be mutual (same transcript)
        """
        if len(texts) == 0 and len(audios) == 0:
            raise Exception('You should pass a list of texts or a list of audios!')

        if len(texts) > 0 and not isinstance(texts[0], str):
            raise Exception('`texts` should be a list of strings')

        # if we have a saved vocab, then the tokenizer will load it
        if vocabulary is not None and not tokenizer.loaded:
            tokenizer.load_vocabulary(vocabulary)

        self.texts = []
        self.word_texts = []
        self.word_id_texts = []

        if len(texts) > 0:
            self._read_raw_texts(texts)
            self._fit_tokenizer(build_vocab=False)
            self._set_nb_attributes()

    def _read_raw_texts(self, texts, min_sentence_size=5):
        for txt in texts:
            txt = self._clean_text_file(txt)
            tks = txt.strip().split()
            if txt.strip() and len(tks) >= min_sentence_size and tks[0] not in '.!?:;':
                self.texts.append(txt)
            else:
                raise Exception('Text too short or begin with a punctuation:\n %s' % txt)

    def _clean_text_file(self, text):
        # since deepecoach works with transcript we remove all punctuation from text
        text = Cleaner.remove_punctuation(text, less='.-')
        text = Cleaner.lowercase(text)
        text = Cleaner.remove_newlines(text)
        text = Cleaner.trim(text)
        text = re.sub(r'(\S+)([\*\.])', r'\1 \2', text.strip())
        text = re.sub(r'([\*\.])(\S+)', r'\1 \2', text.strip())
        return text.strip()
