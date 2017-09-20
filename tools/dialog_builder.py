from __future__ import print_function
import csv
import copy
from dataset import *

# 0. body 1. score_hidden 2. archived 3. name 4. author
# 5. author_flair_text 6. downs 7. created_utc 8. subreddit_id
# 9. link_id 10. parent_id 11. score 12. retrieved_on
# 13. controversiality 14. gilded 15. id 16. subreddit 17. ups
# 18. distinguished 19. author_flair_css_class 20. removal_reason


parents = set()
thread_by_id = dict()

with open('reddit_france.csv', 'rb') as f:
    reader = csv.reader(f)
    row = reader.next()
    for i in range(100000):
        # print("name: " + i[3] + ", author: " + i[4] + ", link_id: " + i[9] + ", id:" + i[15] + ", pid:" + i[10] + ", sr: " + i[16])
        row = reader.next()
        # if(i % 1000 == 0):
        #     print("link_id: " + row[9] + ", parent_id: " + row[10] + ", id: " + row[15])
        thread_by_id[row[15]] = list([row])
        parents.add(row[10])

print("Unique parent_id:", parents.__len__())
flag = True

iterations = 0
while flag:
    flag = False
    print(iterations)
    iterations+=1
    for last_id in thread_by_id:
        topmost_comment = thread_by_id[last_id][0]
        parent_id = topmost_comment[10][3:]
        # print(parent_id)
        # print(parent_id in thread_by_id)
        if (parent_id in thread_by_id and
                    last_id != parent_id and
                    thread_by_id[parent_id][-1][15] != topmost_comment[15]):
            thread_by_id[last_id] = copy.copy(thread_by_id[parent_id]) + thread_by_id[last_id]
            flag = True

full_threads = list()

for last_id in thread_by_id:
    thread = thread_by_id[last_id]
    if (thread.__len__() > 1 and thread[thread.__len__() -1][15] not in parents):
        full_threads.append(thread)

print(full_threads.__len__())

dialog = Dialog()

for thread in full_threads:
    conv = Conversation()
    dialog.addConversation(conv)
    for i in range(thread.__len__()):
        body = thread[i][0].replace('\n', '\\n').replace('\r', '\\r')
        conv.addUtterance(thread[i][4].__hash__(), body)

f = open('francais.xml', 'w+')
f.write(dialog.toXML())
