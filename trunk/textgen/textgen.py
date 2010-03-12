# markov text generator
from __future__ import division
import random

#_____________________________________________________________________
# Parameters

n = 5
source = 'shakespeare.txt' # 'drseuss.txt'

#_____________________________________________________________________
#

f = open(source,'r')
raw_text = f.read()

def parse(text):
    ''' make n-grams '''
    text = text.replace('"', ' " ').replace('.', ' . ').split()
    X = zip(*[text[i:] for i in range(n)])
    return map(' '.join, X)

# run parse on the raw text to extract n-grams
corpus         = []
initial_states = []         # get a list of words which begin sentences
for line in raw_text.split('.'):
    x = parse(line + '.')
    if len(x) < 1:
        continue
    initial_states.append(x[0])
    corpus += x

# learn the model
Model = {}
for i in range(1,len(corpus)):
    word = corpus[i]
    prev = corpus[i-1]      # remember these are n-grams
    if prev not in Model:
        Model[prev] = []
    Model[prev].append(word)

print 'Sample text contains', len(Model), 'words. \n'

def Prob(sentence):
    ''' calculate the probability of a sentence. '''
    sentence = parse(sentence)
    # P(initial state)
    p    = initial_states.count(sentence[0]) / len(initial_states)
    prev = sentence[0]
    for x in sentence[1:]:
        # P(curr|prev)
        p = p * Model[prev].count(x) / len(Model[prev])
        prev = x
    return p

# take a random walk
curr = random.choice(initial_states)    # pick an initial state
txt  = [curr]
while curr[-1] != '.':
    next = random.choice(Model[curr])
    curr = next
    txt.append(curr.split()[-1])
random_text = ' '.join(txt)
print random_text
print '\t>>> Prob =', Prob(random_text)

