# Proof of Concept: Measuring Learning Quality for Foreign Words
import random
import numpy as np

# Simulated vocabulary
WORDS = ['apple', 'banana', 'cat', 'dog', 'elephant', 'fish', 'grape', 'hat', 'ice', 'jungle']

# Simulate user memory as a dict: word -> memory strength (0-1)
class Learner:
    def __init__(self, words):
        self.memory = {w: 0.0 for w in words}

    def review(self, word, strategy):
        # Each strategy can affect memory differently
        if strategy == 'simple':
            self.memory[word] = min(1.0, self.memory[word] + 0.2)
        elif strategy == 'spaced':
            self.memory[word] = min(1.0, self.memory[word] + 0.3 if self.memory[word] < 0.5 else 0.1)
        elif strategy == 'random':
            self.memory[word] = min(1.0, self.memory[word] + random.uniform(0.1, 0.3))

    def recall(self, word):
        # Probability of recall = memory strength
        return random.random() < self.memory[word]

def simulate_learning(strategy, sessions=10, reviews_per_session=5):
    learner = Learner(WORDS)
    recall_history = []
    for session in range(sessions):
        # Select words to review
        if strategy == 'simple':
            words_to_review = WORDS[:reviews_per_session]
        elif strategy == 'spaced':
            words_to_review = sorted(WORDS, key=lambda w: learner.memory[w])[:reviews_per_session]
        elif strategy == 'random':
            words_to_review = random.sample(WORDS, reviews_per_session)
        for word in words_to_review:
            learner.review(word, strategy)
        # Test recall for all words
        recalls = [learner.recall(w) for w in WORDS]
        recall_history.append(sum(recalls) / len(WORDS))
    return recall_history

if __name__ == '__main__':
    strategies = ['simple', 'spaced', 'random']
    for strat in strategies:
        history = simulate_learning(strat)
        print(f"Strategy: {strat}, Recall over time: {np.round(history, 2)}")
