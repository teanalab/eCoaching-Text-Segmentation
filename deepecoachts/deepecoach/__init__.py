# -*- coding: utf-8 -*-
"""
deepecoach
~~~~~~~~~~~~~~~~~~~

Deep neural approach to Boundary and Disfluency Detection.

:copyright: (c) 2017 by Marcos Treviso
:licence: MIT, see LICENSE for more details
"""
from __future__ import absolute_import, unicode_literals
import logging


# Generate your own AsciiArt at:
# patorjk.com/software/taag/#f=Calvin%20S&t=deepecoach
__banner__ = r"""
╔╦╗┌─┐┌─┐┌─┐
 ║║├┤ ├┤ ├─┘ by Mehedi Hasan
═╩╝└─┘└─┘┴ 
"""

__prog__ = "deepecoach"
__title__ = 'DeepEcoach'
__summary__ = 'Deep neural approach for text segmentation'
__uri__ = 'https://github.com/teanalab/eCoaching-Text-Segmentation'

__version__ = '0.0.1-alpha'

__author__ = 'Mehedi Hasan'
__email__ = 'mehedi@wayne.edu'

__license__ = 'MIT'
__copyright__ = 'Copyright 2018 Mehedi Hasan'

# the user should dictate what happens when a logging event occurs
logging.getLogger(__name__).addHandler(logging.NullHandler())



# imports
from . import error_analysis
from . import train
from . import task
from .pipeline import Pipeline
