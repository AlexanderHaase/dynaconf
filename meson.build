project( "c++ DynaConf", 'cpp', version : '1.0.0', license : 'BSD 3-clause' )
#add_global_arguments( language : 'cpp' )


includes = include_directories( 'include' ) 

subdir( 'include' )
subdir( 'source' )


libdyanconf = shared_library( "dynaconf", [ 'Scope.cpp' ], include_directories : includes, install : true )
