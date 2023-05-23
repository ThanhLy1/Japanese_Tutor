###############################################################################
###             Japanese Text to Speech - Spring 2023
###
### english_to_kana is a script to convert english word to Japnese katakana
### The purpose of this is for converting english name to Japanese name and have
### voice synthesis work as it only synthesis english syllable and not whole word
### 
### @see     https://github.com/zomysan/alkana.py    
###
###
### @file    english_to_kana.py
### @author  Thanh Ly  hien689@gmail.com 
###############################################################################

import alkana
import sys

# print(alkana.get_kana("Hello"))
# (alkana.get_kana("World"))

# This does not print because it's not in the dictionary
# print(alkana.get_kana("abcdefg"))

# Output from above
# ハロー
# ワールド
# None

# If the word doesn't exist it return nothing

# Command Line

# $ alkana hello
# ハロー
# $ alkana world
# ワールド
# $ alkana abcdefg

# To add a new word that doesn't exist

# alkana.add_external_data('./sample.csv')
# print(alkana.get_kana('hogehoge)) # # ホゲホゲ

if len(sys.argv) < 2:
    print("Usage: python english_to_kana.py <text> [<text>...]")
    sys.exit(1)

texts = sys.argv[1:]
kana = [alkana.get_kana(text) for text in texts]
result = " ".join(filter(None, kana))
print(result)
