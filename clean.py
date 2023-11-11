import os
import glob

files = glob.glob('Data/*') + glob.glob('Report/Figures/*')
for f in files:
    os.remove(f)