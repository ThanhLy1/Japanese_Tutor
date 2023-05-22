import alkana

print(alkana.get_kana("Hello"))
print(alkana.get_kana("World"))

# This does not print because it's not in the dictionary
print(alkana.get_kana("abcdefg"))

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