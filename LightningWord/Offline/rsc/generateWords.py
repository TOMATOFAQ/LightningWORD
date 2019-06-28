import re
file = open("wordTable.txt", "r")
en_pat = re.compile('[a-zA-Z]+')
words = set()
for line in file.readlines():
    words = words.union(set(en_pat.findall(line)))
words = list(words)

wordlist = [[] for i in range(10)]
for w in words:
    wordlist[int((w.__len__() / 2)) % 10].append(w)

file_w = open("wordTable.json", "w")
print(str(wordlist).replace("'", '"'), file=file_w)
