project( 'c++ DynaConf', 'cpp', 
	version : '0.3.1', 
	license : 'BSD 3-clause' )

cpp_flags = [ '-std=c++11', '-g', '-Og' ]

base_includes = include_directories( '../' ) 

#install_subdir( 'include', 'dynaconf' )
subdir( 'source' )
subdir( 'test' )

# Example program from the readme
#
example_exe = executable( 'example', 'example.cpp',
	include_directories : base_includes,
	cpp_args : cpp_flags,
	link_with : libdynaconf )

