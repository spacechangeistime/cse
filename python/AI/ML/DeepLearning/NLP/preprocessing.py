import regex as re
import collections
import keras

class CharTokenizer:
    def __init__(self, vocabulary):
        self.vocabulary = vocabulary
        self.unk_id = vocabulary["[UNK]"]
    def standardize(self, text):
        return text.lower()
    def split(self, inputs):
        return re.findall(r'.', inputs)
    def index(self, tokens):
        return [self.vocabulary.get(t, self.unk_id) for t in tokens]
    def __call__(self, text):
        inputs = self.standardize(text)
        tokens = self.split(inputs)
        indices = self.index(tokens)
        return indices

def compute_char_vocabulary(inputs, max_size):
    char_counts = collections.Counter()
    for x in inputs:
        x = x.lower()
        tokens = re.findall(r'.', x)
        char_counts.update(tokens)
    vocabulary = ["[UNK]"]
    most_common = char_counts.most_common(max_size - len(vocabulary))
    for token, count in most_common:
        vocabulary.append(token)
    return dict((token, i) for i, token in enumerate(vocabulary))

class WordTokenizer:
    def __init__(self, vocabulary):
        self.vocabulary = vocabulary
        self.unk_id = vocabulary["[UNK]"]
    def standardize(self, text):
        return text.lower()
    def split(self, inputs):
        return re.findall(r'[\w]+|[.,;!?]', inputs)
    def index(self, tokens):
        return [self.vocabulary.get(t, self.unk_id) for t in tokens]
    def __call__(self, text):
        inputs = self.standardize(text)
        tokens = self.split(inputs)
        indices = self.index(tokens)
        return indices

def compute_word_vocabulary(inputs, max_size):
    char_counts = collections.Counter()
    for x in inputs:
        x = x.lower()
        tokens = re.findall(r'[\w]+|[.,;!?]', x)
        char_counts.update(tokens)
    vocabulary = ["[UNK]"]
    most_common = char_counts.most_common(max_size - len(vocabulary))
    for token, count in most_common:
        vocabulary.append(token)
    return dict((token, i) for i, token in enumerate(vocabulary))

def count_and_split_words(data):
    counts = collections.Counter()
    for line in data:
        for word in re.findall(r'[\w]+|[.,;!?]', line):
            chars = ' '.join(re.findall(r'.', word))
            counts[chars] += 1
    return dict(counts)

def count_pairs(counts):
    pairs = collections.Counter()
    for word, freq in counts.items():
        symbols = word.split()
        for pair in zip(symbols[:-1], symbols[1:]):
            pairs[pair] += freq
    return pairs

def merge_pair(counts, first, second):
    split = re.compile(f"(?<!\S){first} {second}(?!\S)")
    merged = f"{first}{second}"
    return {split.sub(merged, word): count for word, count in counts.items()}

def compute_subword_vacabulary(dataset, vocab_size):
    counts = count_and_split_words(dataset)
    char_counts = collections.Counter()
    for word, freq in counts.items():
        for char in word.split():
            char_counts[char] += freq
    vocab = ["[UNK]"] + [ char for char, freq in char_counts.most_common()]
    merges = []
    while len(vocab) < vocab_size:
        pairs = count_pairs(counts)
        if not pairs:
            break
        first, second = max(pairs, key=pairs.get)
        counts = merge_pair(counts, first, second)
        vocab.append(f"{first}{second}")
        merges.append(f"{first} {second}")
    vocab = dict((token, index) for index, token in enumerate(vocab))
    merges = dict((token, rank) for rank, token in enumerate(merges))
    return vocab, merges

class SubwordTokenizer:
    def __init__(self, vocab, merges):
        self.vocab = vocab
        self.merges = merges
        self.unk_id = vocab["[UNK]"]
    def standardize(self, inputs):
        return inputs.lower()
    def bpe_merge(self, word):
        while True:
            pairs = re.findall(r"(?<!\S)\S+ \S+(?!\S)", word, overlapped=True)
            if not pairs:
                break
            # we apply merge rules in "rank" order. More frequent pairs are merged first
            best = min(pairs, key=lambda pair: self.merges.get(pair, 1e9))
            if best not in self.merges:
                break
            first, second = best.split()
            split = re.compile(f"(?<!\S){first} {second}(?!\S)")
            merged = f"{first}{second}"
            word = split.sub(merged, word)
        return word
    def split(self, inputs):
        tokens = []
        for word in re.findall(r"[\w]+|[.,:!?]", inputs):
            #Make word characters separated by a space
            word = " ".join(re.findall(r'.', word))
            #Applies byte-pair encoding merge rules
            word = self.bpe_merge(word)
            tokens.extend(word.split())
        return tokens
    def index(self, tokens):
        return [self.vocab.get(t, self.unk_id) for t in tokens]
    def __call__(self, inputs):
        inputs = self.standardize(inputs)
        tokens = self.split(inputs)
        indices = self.index(tokens)
        return indices

filename = keras.utils.get_file(origin="https://www.gutenberg.org/files/2701/2701-0.txt",)
moby_dick = list(open(filename, "r"))
vocabulary, merges = compute_subword_vacabulary(moby_dick, vocab_size=2000)
swt = SubwordTokenizer(vocabulary, merges)
print("Vocabulary length:", len(vocabulary))
print("Vocabulary start:", list(vocabulary.keys())[:10])
print("Vocabulary end:", list(vocabulary.keys())[-10:])
data = "Call me Ishmael. Some years ago--never mind how long precisely."
indices = swt(data)
print("Line length:", len(indices))
print(indices)

vocabulary = compute_word_vocabulary(moby_dick, max_size=2_000)
wt = WordTokenizer(vocabulary)
print("Vocabulary length:", len(vocabulary))
print("Vocabulary start:", list(vocabulary.keys())[:10])
print("Vocabulary end:", list(vocabulary.keys())[-10:])
indices = wt(data)
print("Line length:", len(indices))
print(indices)

vocabulary = compute_char_vocabulary(moby_dick, max_size=100)
ct = CharTokenizer(vocabulary)
print("Vocabulary length:", len(vocabulary))
print("Vocabulary start:", list(vocabulary.keys())[:10])
print("Vocabulary end:", list(vocabulary.keys())[-10:])
indices = ct(data)
print("Line length:", len(indices))
print(indices)
