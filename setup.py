from distutils.core import setup, Extension

module = Extension("trie", sources = ["trie.cpp"])


setup(name="PackageName",version="1.0",description="This is a package for trie", ext_modules=[module])
