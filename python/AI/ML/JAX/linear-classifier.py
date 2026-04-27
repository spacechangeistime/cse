import jax
import jax.numpy as jnp
from jax import random

num_samples = 1000
input_dim = 2
output_dim = 1
key = random.PRNGKey(0)
first = random.multivariate_normal(key, mean=jnp.array([3, 0]), cov = jnp.array([[1.0, 0.5], [0.5, 1.0]]), shape = (num_samples,))
key = random.split(key, num=1)[0]
second = random.multivariate_normal(key, mean=jnp.array([0, 3]), cov = jnp.array([[1.0, 0.5], [0.5, 1.0]]), shape = (num_samples,))
inputs = jnp.vstack((first, second))
targets = jnp.vstack((jnp.zeros(shape=(num_samples, 1)), jnp.ones(shape=(num_samples, 1))))

key = random.split(key, num=1)[0]
W = random.uniform(key, shape=(input_dim, output_dim))
b = jnp.zeros(shape=(output_dim,))

def model(inputs, W, b):
    return jnp.matmul(inputs, W) + b

def mean_squared_error(targets, predictions):
    per_sample_losses = jnp.square(targets - predictions)
    return jnp.mean(per_sample_losses)

def compute_loss(state, inputs, targets):
    W, b = state
    predictions = model(inputs, W, b)
    loss = mean_squared_error(targets, predictions)
    return loss

grad_fn = jax.value_and_grad(compute_loss)
learning_rate = 0.1
@jax.jit
def training_step(inputs, targets, W, b):
    state = (W, b)
    loss, grads = grad_fn(state, inputs, targets)
    grad_wrt_W, grad_wrt_b = grads
    W -= grad_wrt_W * learning_rate
    b -= grad_wrt_b * learning_rate
    return loss, W, b

epochs = 50
for step in range(epochs):
    loss, W, b = training_step(inputs, targets, W, b)
    print(f'loss at step {step}: {loss:.3f}')
