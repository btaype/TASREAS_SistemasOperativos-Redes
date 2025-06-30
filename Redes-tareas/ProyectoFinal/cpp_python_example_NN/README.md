#install

import torch
import torch.nn as nn
import numpy as np
import csv
import sys

#primero_compiler

python3 setup.py build_ext --inplace

#segundo-compiler-aqui corre el client

python3 main.py
