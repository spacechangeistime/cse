import argparse
import tensorflow as tf
import keras
from keras import layers, Input
from keras.utils import image_dataset_from_directory
from PIL import Image
import kagglehub
import zipfile
import os, shutil, pathlib
import matplotlib.pyplot as plt

parser = argparse.ArgumentParser()
parser.add_argument('--train', action='store_true', help='train or use model')

args = parser.parse_args()

batch_size = 32
image_size=(180, 180)
model_name = "dogs-vs-cats-with-data-augmentation.keras"
image_shape = image_size + (3,)
batch_shape = (batch_size,) + image_shape

file_exts = ['.jpg', '.jpeg']

def filter_valid_jpgs(directory):
    for filename in os.listdir(directory):
        if filename.lower().endswith(tuple(file_exts)):
            filepath = os.path.join(directory, filename)
            try:
                with Image.open(filepath) as img:
                    img.verify()
                img_bytes = tf.io.read_file(filepath)
                decoded_img = tf.io.decode_image(img_bytes)
                if img.mode not in ['L', 'RGB', 'RGBA']:
                    print(f'Invalid image mode: {filepath}, Channels: {img.mode}')
                    img.convert('RGB')
                    img.save(filepath)
            except tf.errors.InvalidArgumentError as e:
                print(f'Corrupted image {filepath}... {e}')
                os.remove(filepath)
            except Exception as e:
                print(f'Invalid image: {filepath} - Error: {e}')
                os.remove(filepath)

data_dir = pathlib.Path("dogs-vs-cats-small")
extracted_path = pathlib.Path("train")

def make_subset(subset_name, start_index, end_index):
    for category in ("cat", "dog"):
        dir = data_dir / subset_name / category
        os.makedirs(dir)
        fnames = [f"{category}.{i}.jpg" for i in range(start_index, end_index)]
        for fname in fnames:
            shutil.copyfile(src=extracted_path / category / fname, dst=dir / fname)
        filter_valid_jpgs(dir)

if not extracted_path.is_dir():
    kagglehub.login()

    download_path = kagglehub.competition_download("dogs-vs-cats")

    with zipfile.ZipFile(download_path + "/train.zip", "r") as zip_ref:
        zip_ref.extract_all(".")

    make_subset("train", 0, 1000)
    make_subset("validation", 1000, 1500)
    make_subset("test", 1500, 2500)

def filter_by_shape(x, y):
    return tf.reduce_all(tf.equal(tf.shape(x), batch_shape))

data_augmentation_layers = [
        layers.RandomFlip("horizontal"),
        layers.RandomRotation(0.1),
        layers.RandomZoom(0.2),
]
def data_augmentation(images, targets):
    for layer in data_augmentation_layers:
        images = layer(images)
    return images, targets

def display_augmented_images(ds):
    plt.figure(figsize=(10,10))
    for image_batch, _ in ds.take(1):
        image = image_batch[0]
        for i in range(9):
            ax = plt.subplot(3, 3, i+1)
            augmented_image, _ = data_augmentation(image, None)
            augmented_image = keras.ops.convert_to_numpy(augmented_image)
            plt.imshow(augmented_image.astype("uint8"))
            plt.axis("off")

if args.train:
    train_dataset = image_dataset_from_directory(data_dir/"train", batch_size = None, image_size=image_size, color_mode='rgb')
    train_dataset = train_dataset.batch(batch_size, drop_remainder=True)
    validation_dataset = image_dataset_from_directory(data_dir/"validation", batch_size = None, image_size=image_size, color_mode='rgb')
    validation_dataset = validation_dataset.batch(batch_size, drop_remainder=True)

    train_ds = train_dataset.filter(filter_by_shape)
    validation_ds = validation_dataset.filter(filter_by_shape)
    for data_batch, labels_batch in train_ds:
        if not tf.reduce_all(tf.equal(tf.shape(data_batch), batch_shape)):
            print(f'train features batch shape: {data_batch.shape}')
            print(f'train labels batch shape: {labels_batch.shape}')
    for data_batch, labels_batch in validation_ds:
        if not tf.reduce_all(tf.equal(tf.shape(data_batch), batch_shape)):
            print(f'validation features batch shape: {data_batch.shape}')
            print(f'validation labels batch shape: {labels_batch.shape}')

    display_augmented_images(train_ds)

    augmented_train_ds = train_ds.map(data_augmentation, num_parallel_calls=8)
    augmented_train_ds = augmented_train_ds.prefetch(tf.data.AUTOTUNE)

    inputs = Input(shape=image_shape)
    x = layers.Rescaling(1.0/255)(inputs)
    x = layers.Conv2D(filters=32, kernel_size=3, activation="relu")(x)
    x = layers.MaxPooling2D(pool_size=2)(x)
    x = layers.Conv2D(filters=64, kernel_size=3, activation="relu")(x)
    x = layers.MaxPooling2D(pool_size=2)(x)
    x = layers.Conv2D(filters=128, kernel_size=3, activation="relu")(x)
    x = layers.MaxPooling2D(pool_size=2)(x)
    x = layers.Conv2D(filters=256, kernel_size=3, activation="relu")(x)
    x = layers.MaxPooling2D(pool_size=2)(x)
    x = layers.Conv2D(filters=512, kernel_size=3, activation="relu")(x)
    x = layers.GlobalAveragePooling2D()(x)
    x = layers.Dropout(0.25)(x)
    outputs = layers.Dense(1, activation="sigmoid")(x)

    model = keras.Model(inputs, outputs)
    model.summary()
    model.compile(loss="binary_crossentropy", optimizer="adam", metrics=["accuracy"])
    callbacks = [keras.callbacks.ModelCheckpoint(filepath=model_name, save_best_only=True, monitor="val_loss")]
    history = model.fit(augmented_train_ds, epochs=100, validation_data=validation_ds, callbacks=callbacks)

    accuracy = history.history["accuracy"]
    val_accuracy = history.history["val_accuracy"]
    loss = history.history["loss"]
    val_loss = history.history["val_loss"]
    epochs = range(1, len(accuracy) + 1)

    plt.plot(epochs, accuracy, "r--", "training accuracy")
    plt.plot(epochs, val_accuracy, "b", "validation accuracy")
    plt.title("Training and Validation accuracy")
    plt.legend()
    plt.figure()

    plt.plot(epochs, loss, "r--", "training loss")
    plt.plot(epochs, val_loss, "b", "validation loss")
    plt.title("Training and Validation loss")
    plt.legend()
    plt.show()

test_dataset = image_dataset_from_directory(data_dir/"test", batch_size = None, image_size=image_size, color_mode='rgb')
test_dataset = test_dataset.batch(batch_size, drop_remainder=True)
test_ds = test_dataset.filter(filter_by_shape)
for data_batch, labels_batch in test_ds:
    if not tf.reduce_all(tf.equal(tf.shape(data_batch), batch_shape)):
        print(f'test features batch shape: {data_batch.shape}')
        print(f'test labels batch shape: {labels_batch.shape}')
test_model = keras.models.load_model(model_name)
test_loss, test_acc = test_model.evaluate(test_ds)
print(f'Test accuracy: {test_acc:.3f}')
