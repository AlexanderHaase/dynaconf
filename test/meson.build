test_includes = include_directories( '../Catch/single_include/' )
test_sources = [ 'main.cpp', 'Scope.cpp', 'Options.cpp' ]
test_exe = executable( 'all_tests', test_sources,
	include_directories : [ base_includes, test_includes ],
	cpp_args : cpp_flags,
	link_with : libdynaconf )

test( 'combined tests', test_exe )
