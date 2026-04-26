import torch
from torch.distributions import MultivariateNormal

input_dim = 2
output_dim = 1

class LinearModel(torch.nn.Module):
    def __init__(self):
        super().__init__()
        self.W = torch.nn.Parameter(torch.rand(input_dim, output_dim))
        self.b = torch.nn.Parameter(torch.zeros(output_dim))

    @torch.compile
    def forward(self, inputs):
        return torch.matmul(inputs, self.W) + self.b

model = LinearModel()
learning_rate = 0.1

def mean_squared_error(targets, predictions):
    per_sample_losses = torch.square(targets - predictions)
    return torch.mean(per_sample_losses)

def train_step(inputs, targets):
    predictions = model(inputs)
    loss = mean_squared_error(targets, predictions)
    loss.backward()
    optimizer = torch.optim.SGD(model.parameters(), lr=learning_rate)
    optimizer.step()
    model.zero_grad()
    return loss

num_samples = 1000
mean = torch.zeros(2)
covariance = torch.tensor([[1.0, 0.5], [0.5, 1.0]])
dist = MultivariateNormal(mean, covariance)
first = dist.sample((num_samples,))
second = dist.sample((num_samples,))
inputs = torch.vstack((first, second))
targets = torch.vstack((torch.zeros(num_samples, 1), torch.ones(num_samples, 1)))

epochs = 50
for step in range(epochs):
    loss = train_step(inputs, targets)
    print(f'loss at step {step}: {loss}')

model.eval()
total_loss = 0
with torch.no_grad():
    output = model(inputs)
    loss = mean_squared_error(targets, output)
    print(f'test loss: {loss}')
