import tensorflow as tf
from keras import layers
from keras.layers import Input
from keras.datasets import mnist

inputs = Input(shape=(28,28,1))
x = tf.keras.layers.Conv2D(filters=64, kernel_size=3, activation="relu")(inputs)
x = tf.keras.layers.MaxPooling2D(pool_size=2)(x)
x = tf.keras.layers.Conv2D(filters=128, kernel_size=3, activation="relu")(x)
x = tf.keras.layers.MaxPooling2D(pool_size=2)(x)
x = tf.keras.layers.Conv2D(filters=256, kernel_size=3, activation="relu")(x)
x = tf.keras.layers.GlobalAveragePooling2D()(x)
outputs = tf.keras.layers.Dense(10, activation="softmax")(x)

model = tf.keras.Model(inputs, outputs)
model.summary()

(train_images, train_labels), (test_images, test_labels) = mnist.load_data()
train_images = train_images.reshape(60000, 28, 28, 1)
train_images = train_images.astype("float32")/255
test_images = test_images.reshape(10000, 28, 28, 1)
test_images = test_images.astype("float32")/255

model.compile(optimizer="adam", loss="sparse_categorical_crossentropy", metrics=["accuracy"]);
model.fit(train_images, train_labels, epochs=5, batch_size=64)

test_loss, test_acc = model.evaluate(test_images, test_labels)
print(f'test accuracy: {test_acc:.3f}')
