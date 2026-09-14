import tensorflow as tf
import keras
from keras import layers
import pathlib, shutil, os, random

# Words to the left or right of label
context_size = 4
# Total window size
window_size = 9

def window_data(token_ids):
    num_windows = tf.maximum(tf.size(token_ids) - 2 * context_size, 0)
    windows = tf.range(window_size)[None, :]
    windows = windows + tf.range(num_windows)[:, None]
    windowed_tokens = tf.gather(token_ids, windows)
    return tf.data.Dataset.from_tensor_slices(windowed_tokens)

def split_label(window):
    left = window[:context_size]
    right = window[context_size+1:]
    bag = tf.concat((left, right), axis=0)
    label = window[context_size]
    return bag, label

zip_path = keras.utils.get_file(origin="https://ai.stanford.edu/~amaas/data/sentiment/aclImdb_v1.tar.gz",
                                fname="imdb",
                                extract=True,
                                )
imdb_extract_dir = pathlib.Path(zip_path) / "aclImdb"
for path in imdb_extract_dir.glob("*/*"):
    if path.is_dir():
        print(path)
print(open(imdb_extract_dir / "train" / "pos" / "4077_10.txt", "r").read())

train_dir = pathlib.Path("imdb_train")
test_dir = pathlib.Path("imdb_test")
val_dir = pathlib.Path("imdb_val")
shutil.copytree(imdb_extract_dir / "test", test_dir, dirs_exist_ok=True)

val_percentage = 0.2
for category in ("pos", "neg"):
    src_dir = imdb_extract_dir / "train" / category
    src_files = os.listdir(src_dir)
    random.Random(1337).shuffle(src_files)
    num_val_samples = int(len(src_files) * val_percentage)
    os.makedirs(val_dir / category, exist_ok=True)
    for file in src_files[:num_val_samples]:
        shutil.copy(src_dir / file, val_dir / category / file)
    os.makedirs(train_dir / category, exist_ok=True)
    for file in src_files[num_val_samples:]:
        shutil.copy(src_dir / file, train_dir / category / file)

max_length = 600
max_tokens = 30000

batch_size=32
train_ds = keras.utils.text_dataset_from_directory(train_dir, batch_size=batch_size)
val_ds = keras.utils.text_dataset_from_directory(val_dir, batch_size=batch_size)
test_ds = keras.utils.text_dataset_from_directory(test_dir, batch_size=batch_size)
text_vectorization = layers.TextVectorization(
        max_tokens=max_tokens,
        split="whitespace",
        output_mode="int",
        output_sequence_length=max_length,
        )
train_ds_no_labels = train_ds.map(lambda x, y: x)
text_vectorization.adapt(train_ds_no_labels)
sequence_train_ds = train_ds.map(lambda x, y: (text_vectorization(x), y), num_parallel_calls=8)
sequence_val_ds = val_ds.map(lambda x, y: (text_vectorization(x), y), num_parallel_calls=8)
sequence_test_ds = test_ds.map(lambda x, y: (text_vectorization(x), y), num_parallel_calls=8)

imdb_vocabulary = text_vectorization.get_vocabulary()
tokenize_no_padding = keras.layers.TextVectorization(
        vocabulary=imdb_vocabulary,
        split="whitespace",
        output_mode="int",
        )
dataset = keras.utils.text_dataset_from_directory(imdb_extract_dir / "train", batch_size=None)
# Drop label
dataset = dataset.map(lambda x, y: x, num_parallel_calls=8)
# Tokenize
dataset = dataset.map(tokenize_no_padding, num_parallel_calls=8)
# Creates context windows
dataset = dataset.interleave(window_data, cycle_length=8, num_parallel_calls=8)
dataset = dataset.map(split_label, num_parallel_calls=8)

hidden_dim = 64
inputs = layers.Input(shape=(2*context_size,))
cbow_embedding = layers.Embedding(input_dim=max_tokens,
                         output_dim=hidden_dim,
                         )
x = cbow_embedding(inputs)
x = layers.GlobalAveragePooling1D()(x)
outputs = layers.Dense(max_tokens, activation="sigmoid")(x)
cbow_model = keras.Model(inputs, outputs)
cbow_model.compile(optimizer="adam",
                   loss="sparse_categorical_crossentropy",
                   metrics=["sparse_categorical_accuracy"],
                   )
cbow_model.summary()
dataset = dataset.batch(1024).cache()
cbow_model.fit(dataset, epochs=4)

model_name = "imdb_word_embedding_sequence_model.keras"
callbacks = [keras.callbacks.ModelCheckpoint(filepath=model_name, save_best_only=True, monitor="val_loss")]

inputs = layers.Input(shape=(max_length,))
lstm_embedding = layers.Embedding(max_tokens, hidden_dim, mask_zero=True)
x = lstm_embedding(inputs)
x = layers.Bidirectional(layers.LSTM(hidden_dim))(x)
x = layers.Dropout(0.5)(x)
outputs = layers.Dense(1, activation="sigmoid")(x)
model = keras.Model(inputs, outputs, name="lstm_with_cbow")
lstm_embedding.embeddings.assign(cbow_embedding.embeddings)
model.compile(optimizer="adam",
              loss="binary_crossentropy",
              metrics=["accuracy"],
              )
model.fit(sequence_train_ds,
          validation_data=sequence_val_ds,
          epochs=10,
          callbacks=callbacks,
          )
test_loss, test_acc = model.evaluate(sequence_test_ds)
print(test_acc)

