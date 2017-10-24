project( 'c++ DynaConf', 'cpp', 
	version : '0.3.2', 
	license : 'BSD 3-clause' )

# TODO: add compiler version tests
#
cpp_flags = [ '-std=c++11',
	'-g',
	'-Wall',
	'-Wextra',
	'-Werror',
	'--pedantic',
	'-Wlogical-op',
	'-Wuseless-cast',
	'-Wdouble-promotion',
	'-Wshadow',
	'-Wformat=2',
	#gcc 6 '-Wnull-dereference',
	#gcc 6 '-Wduplicated-cond',
	#gcc 7 '-Wduplicated-branches',
	#gcc 7 '-Wrestrict',
	'-O3' ]

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

