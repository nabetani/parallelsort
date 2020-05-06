import math
import numpy as np
from matplotlib import pyplot
from matplotlib.backends.backend_pdf import PdfPages
import pandas as pd
from datetime import datetime, date, timedelta
from pathlib import Path

SQUARE_20cm = (20/2.54, 20/2.54)

def uniq( data, col ):
  return sorted(list(set(data[col])))

def marker(k):
  if k=="gnu_parallel":
    return "o"
  if k=="std":
    return "*"
  if k=="std(par)":
    return "x"
  if k=="std(par_unseq)":
    return "+"
  if k=="std(unseq)":
    return ""

def plot(data, sort_type, name):
  fig = pyplot.figure(figsize=SQUARE_20cm)
  fig.suptitle(name + " sort")
  procs = uniq(data, "proc")
  values = uniq(data, "value")
  print( procs )
  gcol=2
  for vix in range(0,len(values)):
    v = values[vix]
    graph = fig.add_subplot(math.ceil(len(values)/gcol), gcol, vix+1)  # nrows, ncols, index
    graph.set_xscale('log',  basex=2)
    graph.set_yscale('log')
    graph.set_title(v)
    for m in procs:
      rows = [ row for row in data.itertuples() if row.proc==m and row.value==v ]
      x= [ row.size for row in rows ]
      y= [ sort_type(row) for row in rows ]
      graph.plot( x, y, label=m, marker=marker(m))
    pyplot.legend()
    graph.grid(b=True, which='major', color='gray', linestyle='-')
  pyplot.tight_layout()
  pyplot.subplots_adjust(top=0.9)
  pyplot.savefig(name+'.png')

def main():
  data= pd.read_csv(
    "result.csv", 
    skipinitialspace=True,
    header=None,
    names=['proc', 'value', 'size', 'unstable', 'stable'])
  print( data )
  plot(data,  (lambda x:x.unstable), "unstable")
  plot(data,  (lambda x:x.stable), "stable")



main()
