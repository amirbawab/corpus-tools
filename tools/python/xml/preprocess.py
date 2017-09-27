import sys
import re
import json
import argparse
from langdetect import detect_langs

# Usage: `head <bigquery_reddit_comments>.json | python3 preprocess.py | ...`

parser = argparse.ArgumentParser(description='Pre-processing tool. Filters data from JSON.')
parser.add_argument('--frenchThreshold', type=float, default=0.8,
                    help='Lowest ratio of French to non-French text. Enter a value between 0 and 1.')
args = parser.parse_args()

class Stats:
    """Filtering stats"""
    def __init__(self):
        """Initialize an Utterance"""
        self.bots = 0
        self.total = 0
        self.removed = 0
        self.deleted = 0
        self.empties = 0
        self.non_french = 0
        self.low_french = 0
        self.discards = list()

if __name__ == "__main__":
    stats = Stats()
    for line in sys.stdin:
        ln: str = line
        end_idx = ln[9:].find("\",\"author\":\"")
        prefix = ln[0:9]
        body: str = ln[9:end_idx]
        tail = ln[end_idx:]
        stats.total += 1

        is_a_bot = body.__contains__("I am a bot")
        if (is_a_bot): stats.removed += 1; continue

        is_removed = body.__contains__("[deleted]")
        if (is_removed): stats.removed += 1; continue
        is_deleted = body.__contains__("[removed]")
        if (is_deleted): stats.deleted += 1; continue

        sanitized = re.sub(r"\\u0026gt;.*?\\n\\n", "", body)
        sanitized = re.sub(r"\\n", "", sanitized)

        is_empty = sanitized.strip() == ""
        if (is_empty): stats.empties += 1; continue

        try:
            languages = detect_langs(sanitized)
        except:
            stats.non_french += 1; continue

        not_french = languages[0].lang != 'fr'
        if (not_french): stats.discards.append(sanitized); stats.non_french += 1; continue

        low_french = languages[0].prob < args.frenchThreshold
        if(low_french): stats.low_french += 1; continue

        output_line = prefix + sanitized + tail
        sys.stdout.write(output_line)

    with open('filter.log', 'w') as log_file:
        log_file.write(json.dumps(stats.__dict__))