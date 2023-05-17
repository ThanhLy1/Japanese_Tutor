import requests
import sys

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
        synthesis_response = requests.post(synthesis_url, json=query_data, params=synthesis_params)

        if synthesis_response.status_code == 200:
            audio_content = synthesis_response.content
            with open(audio_file_path, "wb") as audio_file:
                audio_file.write(audio_content)
            print("Audio file saved:", audio_file_path)
        else:
            print("Error:", synthesis_response.text)

    def process_audio(text, preset_id, speaker):
        # Process the audio with the provided parameters
        # Your audio processing logic here
        audio_content = f"Text: {text}, Preset ID: {preset_id}, Speaker: {speaker}"
        
        # Save the audio content to a file or perform other actions as needed
        with open("audio.txt", "w") as audio_file:
            audio_file.write(audio_content)

if __name__ == "__main__":
    tts = TextToSpeech(speaker=20)
    text = "私はAIです。私の作成にはChatGPTと音声合成技術が使用されました."
    preset_id = 1

    result = tts.audio_query(text)
    print(result)

    result2 = tts.audio_query_from_preset(text, preset_id)
    # print(result2)

    speed_scale = 1.0
    volume_scale = 1.5
    intonation_scale = 1.0
    pre_phoneme_length = 0.1
    post_phoneme_length = 0.1
    audio_file_path = "audio.wav"

    tts.synthesize_and_save_audio(result2, speed_scale, volume_scale, intonation_scale, pre_phoneme_length, post_phoneme_length, audio_file_path)
