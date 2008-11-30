from distutils.core import setup, Extension
import sys

libNames = ''
if sys.platform == 'win32':
    libNames = ['verve.dll', '_pyverve.dll']
    # Later versions of Visual C++ (e.g. 8.0) might require things like ['msvcp80.dll', 'msvcr80.dll']
else:
    libNames = ['libverve.so', '_pyverve.so']

#extension_module = Extension('pyverve', ['pyverve.i'], extra_compile_flags = ['-shadow', '-c++'])
#extension_module = Extension('pyverve', ['pyverve.i'])

setup(
	name = 'pyverve', 
	version = '0.1.0', 
    author = 'Tyler Streeter', 
    author_email = 'tylerstreeter@gmail.com', 
    url = 'http://www.vrac.iastate.edu/~streeter', 
	description = 'Open Source general purpose reinforcement learning',
    #ext_package = 'pyverve', 
    #ext_modules = [Extension('pyverve.pyverve', ['pyverve_wrap.cxx'])]
    #ext_modules = [extension_module]
    packages = ['pyverve'], 
    package_dir = {'pyverve': 'pyverve'}, 
    package_data = {'pyverve': libNames}
	)
