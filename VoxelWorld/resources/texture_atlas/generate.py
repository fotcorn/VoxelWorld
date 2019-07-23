"""
This script generates a texture atlas from the zip downloaded from https://opengameart.org/content/free-low-poly-game-asset-3d-blocks
Unpack the zip file into the unpacked/ subdirectory where this script is located.
You need the "convert" command from the ImageMagick package installed.
This script probably only workd on Linux/Unix.
"""
import os
import shutil

# unpack zip from https://opengameart.org/content/free-low-poly-game-asset-3d-blocks to this directory:
path = './unpacked/'
temp_dir = './temp/'
if os.path.exists(temp_dir):
    shutil.rmtree(temp_dir)

os.mkdir(temp_dir)
os.system(f'cp unpacked/*/tex/*.gif {temp_dir}')

files = os.listdir(temp_dir)
for f in files:
    new_name = f.split('_')[0].lower() + '.gif'
    os.rename(os.path.join(temp_dir, f), os.path.join(temp_dir, new_name))

files = os.listdir(temp_dir)
files = [os.path.join(temp_dir, f) for f in files]
files = sorted(files)

os.system('convert ' + ' '.join(files) + ' +append texture_atlas.gif')

shutil.rmtree(temp_dir)
