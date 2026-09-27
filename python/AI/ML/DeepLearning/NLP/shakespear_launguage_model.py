import keras
from keras import layers
import tensorflow as tf
import numpy as np

file = keras.utils.get_file(origin=("https://storage.googleapis.com/download.tensorflow.org/data/shakespeare.txt"),)
shakespeare = open(file, "r").read()

print(shakespeare[:250])
# The chunk size we will use during training. We only train on sequence of 100 characters at a time.
sequence_length = 100

def split_input(input, sequence_length):
    for i in range(0, len(input), sequence_length):
        yield input[i: i+sequence_length]

features = list(split_input(shakespeare[:-1], sequence_length))
labels = list(split_input(shakespeare[1:], sequence_length))
dataset = tf.data.Dataset.from_tensor_slices((features, labels))

x, y = next(dataset.as_numpy_iterator())
print(x[:50], y[:50])

tokenizer = layers.TextVectorization(standardize=None,
                                           split="character",
                                           output_sequence_length=sequence_length,
                                           )
tokenizer.adapt(dataset.map(lambda text, labels: text))
vocabulary_size = tokenizer.vocabulary_size()

dataset = dataset.map(lambda text, labels: (tokenizer(text), tokenizer(labels)), num_parallel_calls=8)
training_data = dataset.shuffle(10000).batch(64).cache()

embedding_dim = 256
hidden_dim = 1024

inputs = layers.Input(shape=(sequence_length,), dtype=int, name="token_ids")
x = layers.Embedding(vocabulary_size, embedding_dim)(inputs)
x = layers.GRU(hidden_dim, return_sequences=True)(x)
x = layers.Dropout(0.1)(x)
outputs = layers.Dense(vocabulary_size, activation="softmax")(x)
model = keras.Model(inputs, outputs)
model.summary()
model.compile(optimizer="adam",
              loss="sparse_categorical_crossentropy",
              metrics=["sparse_categorical_accuracy"],
              )
model.fit(training_data, epochs=20)

inputs = layers.Input(shape=(1,), dtype=int, name="token_ids")
input_state = layers.Input(shape=(hidden_dim,), name="state")
x = layers.Embedding(vocabulary_size, embedding_dim)(inputs)
x, output_state = layers.GRU(hidden_dim, return_state=True)(x, initial_state=input_state)
outputs = layers.Dense(vocabulary_size, activation="softmax")(x)
generation_model = keras.Model(inputs=(inputs, input_state), 
                               outputs=(outputs, output_state),
                               )
generation_model.set_weights(model.get_weights())

tokens = tokenizer.get_vocabulary()
token_ids = range(vocabulary_size)
char_to_id = dict(zip(tokens, token_ids))
id_to_char = dict(zip(token_ids, tokens))

prompt = """
KING RICHARD III:
"""

input_ids = [char_to_id[c] for c in prompt]
state = keras.ops.zeros(shape=(1, hidden_dim))
for token_id in input_ids:
    inputs = keras.ops.expand_dims([token_id], axis=0)
    # Feed the prompt char by char to update state
    predictions, state = generation_model.predict((inputs, state), verbose=0)

generated_ids = []
max_length = 250
# Generate characters one by one, computing a new state each iteration
for i in range(max_length):
    # Next char is the output index with highest probability
    next_char = int(np.argmax(predictions, axis=-1)[0])
    generated_ids.append(next_char)
    inputs = keras.ops.expand_dims([next_char], axis=0)
    predictions, state = generation_model.predict((inputs, state), verbose=0)

output = "".join([id_to_char[token_id] for token_id in generated_ids])
print(prompt + output)
