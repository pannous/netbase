#!/usr/bin/ruby
# gem install rice   # http://rice.rubyforge.org/
# require "mkmf-rice"
require "mkmf"

# $CFLAGS << "-O3"
# 
# HEADER_DIRS = [
#   ".."
# ]
# LIB_DIRS = [
#   "../../build/"
# ]
# 
# dir_config("libs", HEADER_DIRS, LIB_DIRS)

# have_library("libtools")
# have_library("libalglib")

# require 'mkmf'
# dir_config("netbase")
dir_config("../../")
# have_library("netbase")
have_library("../../netbase")
create_makefile("netbase")
