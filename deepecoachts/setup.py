from setuptools import setup
from setuptools import find_packages

setup(name='deepecoach',
      version='0.0.2',
      description='Deep neural approach for ecoaching text segmentation',
      author='Mehedi Hasan',
      author_email='mehedi@wayne.edu',
      url='https://github.com/teanalab/eCoaching-Text-Segmentation',
      license='MIT',
      install_requires=['numpy>=1.11.0',
                        'keras>=2.1.5',
                        'pandas>=0.17.1',
                        'scikit_learn>=0.19.1',
                        'nltk>=3.2.5',
                        'nlpnet>=1.2.2',
                        'gensim>=2.0.0'],
      extras_require={
          'error_analysis': ['pandas_ml', 'DocumentFeatureSelection'],
          'crfmodels': ['sklearn_crfsuite>=1.4.1']
      },

      # To provide executable scripts, use entry points in preference to the
      # "scripts" keyword. Entry points provide cross-platform support and
      # allow pip to create the appropriate form of executable for the
      # target platform.
      entry_points=dict(
          console_scripts=[
              'deepecoach = deepecoach.__main__:cli',
          ],
      ),

      # See: http://pypi.python.org/pypi?%3Aaction=list_classifiers
      classifiers=[
          # How mature is this project? Common values are:
          #   3 - Alpha
          #   4 - Beta
          #   5 - Production/Stable
          'Development Status :: 3 - Alpha',

          # Indicate who your project is intended for
          'Intended Audience :: Developers',
          'Topic :: Software Development',

          # Pick your license as you wish (should match "license" above)
          'License :: OSI Approved :: MIT License',

          # Specify the Python versions you support here. In particular, ensure
          # that you indicate whether you support Python 2, Python 3 or both.
          'Programming Language :: Python :: 2',
          'Programming Language :: Python :: 2.7',
          'Programming Language :: Python :: 3',
          'Programming Language :: Python :: 3.3',
          'Programming Language :: Python :: 3.4',

          'Environment :: Console',
      ],

packages=find_packages())

