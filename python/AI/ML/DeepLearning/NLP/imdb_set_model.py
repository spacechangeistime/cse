import keras
import os, pathlib, shutil, random
from keras.utils import text_dataset_from_directory
from keras import layers
import matplotlib.pyplot as plt

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

batch_size=32
train_ds = text_dataset_from_directory(train_dir, batch_size=batch_size)
val_ds = text_dataset_from_directory(val_dir, batch_size=batch_size)
test_ds = text_dataset_from_directory(test_dir, batch_size=batch_size)

max_tokens = 30000
text_vectorization = layers.TextVectorization(max_tokens=max_tokens,
                                              #learn a word level vocabulary
                                              split="whitespace",
                                              output_mode="multi_hot",
                                              ngrams=2,
                                              )
train_ds_no_labels = train_ds.map(lambda x, y: x)
text_vectorization.adapt(train_ds_no_labels)
set_train_ds = train_ds.map(lambda x, y: (text_vectorization(x), y), num_parallel_calls=8)
set_val_ds = val_ds.map(lambda x, y: (text_vectorization(x), y), num_parallel_calls=8)
set_test_ds = test_ds.map(lambda x, y: (text_vectorization(x), y), num_parallel_calls=8)
x, y = next(set_train_ds.as_numpy_iterator())
print(x.shape)
print(y.shape)

def build_linear_classifier(max_tokens, name):
    inputs = keras.Input(shape=(max_tokens,))
    outputs = layers.Dense(1, activation="sigmoid")(inputs)
    model = keras.Model(inputs, outputs, name=name)
    model.compile(optimizer="adam", loss="binary_crossentropy", metrics=["accuracy"])
    return model
model = build_linear_classifier(max_tokens, "bag_of_words_classifier")
model.summary()
early_stopping = keras.callbacks.EarlyStopping(monitor="val_loss", restore_best_weights=True, patience=2)
history = model.fit(set_train_ds, validation_data=set_val_ds,
                    epochs=10,
                    callbacks=[early_stopping])
accuracy = history.history["accuracy"]
val_accuracy = history.history["val_accuracy"]
epochs = range(1, len(accuracy) + 1)
plt.plot(epochs, accuracy, "r--", label="Training Accuracy")
plt.plot(epochs, val_accuracy, "b", label="Validation Accuracy")
plt.title("Training and Validation Accuracy")
plt.legend()
plt.show()

test_loss, test_acc = model.evaluate(set_test_ds)
print(test_acc)
