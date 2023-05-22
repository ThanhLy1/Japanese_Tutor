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
        response = requests.post(synthesis_url, json=query_data, params=synthesis_params, headers=self.HEADERS)

        if response.status_code == 200:
            audio_content = response.content
            with open(audio_file_path, "wb") as audio_file:
                audio_file.write(audio_content)
            print(f"Audio file saved: {audio_file_path}")
        else:
            print("Error:", response.text)

    def get_presets(self):
        url = 'http://127.0.0.1:50021/presets'
        headers = {'accept': 'application/json'}

        try:
            response = requests.get(url, headers=headers)
            response.raise_for_status()  # Raises an exception for non-2xx status codes
            data = response.json()
            return data
        except requests.exceptions.RequestException as e:
            print(f"Error: {e}")
            return {'error': str(e)}

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: python3 script.py <text> <preset_id> <speaker>")
        sys.exit(1)  

    text = sys.argv[1]
    preset_id = int(sys.argv[2])
    speaker = int(sys.argv[3])

    tts = TextToSpeech(speaker)
    result = tts.audio_query(text)
    result2 = tts.audio_query_from_preset(text, preset_id)

    presets = tts.get_presets()

    if 'error' in presets:
        print(presets['error'])
    else:
        print(presets)

    speed_scale = 1.0
    volume_scale = 1.0
    intonation_scale = 2.0
    pre_phoneme_length = 0.1
    post_phoneme_length = 0.1
    audio_file_path = "audio.wav"

    tts.synthesize_and_save_audio(result2, speed_scale, volume_scale, intonation_scale, pre_phoneme_length, post_phoneme_length, audio_file_path)