import sys
import re
from langdetect import detect_langs

if __name__ == "__main__":
    for line in sys.stdin:
        ln: str = line
        end_idx = ln[9].find("\",\"author\":\"")
        prefix = ln[0:9]
        body: str = ln[9:end_idx]

        is_a_bot = body.__contains__("I am a bot")
        if(is_a_bot): continue

        is_removed = body.__contains__("[deleted]") or body.__contains__("[removed]")
        if(is_removed): continue

        sanitized = re.sub(r"\\u0026gt;.*?\\n\\n", "", body)
        sanitized = re.sub(r"\\n", "", sanitized)
        is_empty = sanitized.strip() == ""
        if(is_empty): continue

        languages = detect_langs(sanitized)
        not_french = languages[0].lang != 'fr' or languages[0].prob < 0.8
        if(not_french): continue

        sys.stdout.write(prefix + sanitized + "\n")