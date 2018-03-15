import inspect
import os
import glob
import fnmatch
import math

def mean(data):
  return float(float(sum(data)) / float(len(data)))

def stddev(data):
  data_mean = mean(data)
  sum_sq = 0.0
  for p in data:
    sum_sq += (float(p) - data_mean)**2.0
  return math.sqrt(sum_sq / float(len(data) - 1))

def PrintFrame():
  callerframerecord = inspect.stack()[1]    # 0 represents this line
                                            # 1 represents line at caller
  frame = callerframerecord[0]
  info = inspect.getframeinfo(frame)
  print info.filename                       # __FILE__     -> Test.py
  print info.function                       # __FUNCTION__ -> Main
  print info.lineno                         # __LINE__     -> 13

def make_dir(dir_path):
    try:
        os.makedirs(dir_path)
    except OSError:
        if not os.path.isdir(dir_path):
            raise

def find_files(directory, pattern):
    for root, dirs, files in os.walk(directory):
        for basename in files:
            if fnmatch.fnmatch(basename, pattern):
                filename = os.path.join(root, basename)
                yield filename

def remove_prefix(text, prefix):
    if text.startswith(prefix):
        return text[len(prefix):]
    return text

def chunks(l, n):
    """Yield successive n-sized chunks from l."""
    for i in xrange(0, len(l), n):
        yield l[i:i + n]

def reverse_readline(filename, buf_size=8192):
    """a generator that returns the lines of a file in reverse order"""
    with open(filename) as fh:
        segment = None
        offset = 0
        fh.seek(0, os.SEEK_END)
        file_size = remaining_size = fh.tell()
        while remaining_size > 0:
            offset = min(file_size, offset + buf_size)
            fh.seek(file_size - offset)
            buffer = fh.read(min(remaining_size, buf_size))
            remaining_size -= buf_size
            lines = buffer.split('\n')
            # the first line of the buffer is probably not a complete line so
            # we'll save it and append it to the last line of the next buffer
            # we read
            if segment is not None:
                # if the previous chunk starts right from the beginning of line
                # do not concact the segment to the last line of new chunk
                # instead, yield the segment first
                if buffer[-1] is not '\n':
                    lines[-1] += segment
                else:
                    yield segment
            segment = lines[0]
            for index in range(len(lines) - 1, 0, -1):
                if len(lines[index]):
                    yield lines[index]
        # Don't yield None if the file was empty
        if segment is not None:
            yield segment

def find_signal_mass_point(filepath):
  try:
    idx = filepath.index("_m")
    mass = int(filepath[idx+2:idx+6])
    return mass
  except:
    print "WARNING: Couldn't determine signal mass point for sample with file name: ", filepath
    print "WARNING: Defaulting to resonance mass = -1!"
    return -1



