import argparse
import pytubefix
import os
from pydub import AudioSegment

parser = argparse.ArgumentParser("downloader")
parser.add_argument("yt_link", help="Enter a Youtube Link", type=str)
parser.add_argument("song_name", help="Enter Song Name", type=str)
parser.add_argument("music_folder", help="Enter Music Folder Path", type=str)
args = parser.parse_args()

yt = pytubefix.YouTube(args.yt_link)
yt_title = yt.title

ys = yt.streams.get_audio_only()
downloaded_file = ys.download()

audio = AudioSegment.from_file(downloaded_file)
audio.export(args.music_folder + args.song_name + ".mp3", format="mp3")

os.remove(downloaded_file)





