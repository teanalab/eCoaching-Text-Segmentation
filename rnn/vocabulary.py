import re
import codecs
import operator


class Vocabulary:
    def __init__(self, vocab_path='e-coaching.vocab'):
        self.vocab = dict()
        self.int_to_word = dict()
        self.vocab_path = vocab_path

    # build e-coaching dictionary
    def build(self):
        with codecs.open(self.vocab_path, 'r', 'UTF-8') as train_file:
            words = [x.strip().rstrip('\n') for x in train_file.readlines()]
            self.vocab = dict((c, i + 1) for i, c in enumerate(words))
            self.int_to_word = dict((i+1, c) for i, c in enumerate(words))
        return self

    # read all the data from file and create vocabulary, also find # classes
    def create(self, data_file='e-coaching_data_stem.txt'):
        word_dict = {}

        with open(data_file, "r") as file_stream:
            for line in file_stream:
                current_line = line.strip().split(",")
                if len(current_line) < 2:
                    continue

                words = re.split('(\W+)', self.clean_text(current_line[0]).strip())
                for word in words:
                    if len(word.strip()) > 0:
                        word_dict[word] = 1

        sorted_map = sorted(word_dict.items(), key=operator.itemgetter(0))
        file_pointer = open(self.vocab_path, "w")
        for key, val in sorted_map:
            file_pointer.write(key + "\n")
        file_pointer.close()

        self.build()
        return self

    def size(self):
        return len(self.vocab)

    def tokenize(self, text):
        return [x.strip() for x in re.split('(\W+)', text) if x.strip()]

    def vectorize(self, text):
        text = text.lower()
        words = filter(lambda x: x in self.vocab, self.tokenize(text))
        return [self.vocab[w] for w in words]

    def clean_text(self, text):
        new_text = re.sub(r'[^\x00-\x7f]', r'', text)
        new_text = new_text.replace("\n", "").strip()
        new_text = re.sub(r'[?|`|>|<|$|.|!|;|:|=|&|\'|\"|\-|/|)|(|\]|[|]', r'', new_text)
        return new_text
