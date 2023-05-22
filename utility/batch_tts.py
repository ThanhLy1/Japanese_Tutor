import subprocess
import os
import requests
import sys
import time

class TextToSpeech:
    BASE_URL = "http://127.0.0.1:50021"
    HEADERS = {"Accept": "application/json"}

    def __init__(self, speaker):
        self.speaker = speaker

    def audio_query(self, text):
        url = f"{self.BASE_URL}/audio_query"
        params = {"text": text, "speaker": self.speaker}
        response = requests.post(url, headers=self.HEADERS, params=params)

        if response.status_code == 200:
            data = response.json()
            return data
        else:
            print("Error:", response.text)
            return None

    def audio_query_from_preset(self, text, preset_id):
        url = f"{self.BASE_URL}/audio_query_from_preset"
        params = {"text": text, "preset_id": preset_id}
        response = requests.post(url, headers=self.HEADERS, params=params)

        if response.status_code == 200:
            data = response.json()
            return data
        else:
            print("Error:", response.text)
            return None

    def synthesize_and_save_audio(self, query_data, speed_scale, volume_scale, intonation_scale, pre_phoneme_length, post_phoneme_length, audio_file_path):
        synthesis_params = {
            "speaker": self.speaker,
            "speedScale": speed_scale,
            "volumeScale": volume_scale,
            "intonationScale": intonation_scale,
            "prePhonemeLength": pre_phoneme_length,
            "postPhonemeLength": post_phoneme_length
        }

        synthesis_url = f"{self.BASE_URL}/synthesis"
        response = requests.post(synthesis_url, json=query_data, params=synthesis_params, headers=self.HEADERS)

        if response.status_code == 200:
            # Wait for the synthesis process to complete
            time.sleep(5)

            audio_content = response.content
            with open(audio_file_path, "wb") as audio_file:
                audio_file.write(audio_content)
            print(f"Audio file saved: {audio_file_path}")
        else:
            print("Error:", response.text)


def convert_text_to_speech(word, speaker, preset_id):
    tts = TextToSpeech(speaker)

    # Split the word using ":" as the delimiter
    parts = word.split(":")
    text = parts[0].strip()
    
    # Determine the new audio file name
    new_audio_file_name = text[:10] if len(text) > 10 else text
    if len(parts) > 1 and parts[1].strip():
        new_audio_file_name = parts[1].strip()[:10]

    # Generate the audio file path
    audio_file_path = f"{new_audio_file_name}.wav"

    result = tts.audio_query(text)
    result2 = tts.audio_query_from_preset(text, preset_id)

    speed_scale = 1.0
    volume_scale = 1.0
    intonation_scale = 2.0
    pre_phoneme_length = 0.1
    post_phoneme_length = 0.1

    tts.synthesize_and_save_audio(result2, speed_scale, volume_scale, intonation_scale, pre_phoneme_length, post_phoneme_length, audio_file_path)


# Read the words from the file, ignoring empty lines and lines starting with "#" and "//"
with open('words.txt', 'r') as file:
    words = [line.strip() for line in file if line.strip() and not line.strip().startswith("#") and not line.strip().startswith("//")]

# Specify the speaker and preset_id
speaker = 20
preset_id = 1

# Convert each word to speech
for word in words:
    convert_text_to_speech(word, speaker, preset_id)
