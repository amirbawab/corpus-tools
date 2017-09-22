import csv
import copy
from dataset import *

parents = set()
thread_by_id = dict()

# Columns of CSV data by index:

# 0. body 1. score_hidden 2. archived 3. name 4. author
# 5. author_flair_text 6. downs 7. created_utc 8. subreddit_id
# 9. link_id 10. parent_id 11. score 12. retrieved_on
# 13. controversiality 14. gilded 15. id 16. subreddit 17. ups
# 18. distinguished 19. author_flair_css_class 20. removal_reason

rows = 1
with open('reddit_france.csv', 'rb') as f:
    reader = csv.reader(f)
    for i in range(500000):
        row = reader.next()
        comment_id = row[15]
        thread_by_id[comment_id] = list([row])
        parent_id = row[10][3:]
        if (parent_id != comment_id):
            parents.add(parent_id)
        if (rows % 100000 == 0):
            print("Processed " + str(rows) + " rows...")
        rows += 1

print("Unique `parent_id`s:", len(parents))

removed = 0
processed = 0
total_rows = len(thread_by_id)
# Remove comments with no parents and no children
print("Filtering singletons...")
for thread in thread_by_id.keys():
    comment = thread_by_id[thread][0]
    processed += 1
    if processed % 100000 == 0:
        print("Processed " + str(processed) + " records...")
    comment_id = comment[15]
    parent_id = comment[10][3:]
    if (comment_id not in parents and comment_id == parent_id):
        thread_by_id.__delitem__(thread)
        removed += 1

print("Removed: " + str(removed) + ", remaining: " + str(len(thread_by_id)))

full_threads = list()
def merge_threads():
    merge_flag = False
    for thread in thread_by_id.values():
        topmost_comment = thread[0]
        comment_id = thread[-1][15]
        parent_id = topmost_comment[10][3:]
        if parent_id in thread_by_id and comment_id != parent_id:
            parent_thread = thread_by_id[parent_id]
            new_current_thread = copy.copy(parent_thread) + thread_by_id[comment_id]
            op = new_current_thread[0]
            op_comment_id = op[15]
            parent_of_op = op[10][3:]
            op_is_topmost_parent_in_thread = op_comment_id == parent_of_op
            last_comment_has_no_children = comment_id not in parents
            if(op_is_topmost_parent_in_thread and last_comment_has_no_children):
                full_threads.append(new_current_thread)
                thread_by_id.__delitem__(comment_id)
            else:
                thread_by_id[comment_id] = new_current_thread
            merge_flag = True
    return merge_flag

flag = True
iterations = 0
print("Longest thread...")
while flag:
    iterations += 1
    print(iterations)
    flag = merge_threads()

for last_id in thread_by_id:
    thread = thread_by_id[last_id]
    last_comment_id = thread[-1][15]
    last_comment_parent = thread[-1][10][3:]
    if (len(thread) > 1 and last_comment_id not in parents):
        full_threads.append(thread)

print("Full threads: " + str(len(full_threads)))

dialog = Dialog()

for thread in full_threads:
    conversation = Conversation()
    dialog.addConversation(conversation)
    for comment in thread:
        body = comment[0].replace('\n', '\\n').replace('\r', '\\r')
        conversation.addUtterance(comment[4].__hash__(), body)

f = open('francais.xml', 'w+')
f.write(dialog.toXML())
