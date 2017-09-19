#include <catch.hpp>
#include <dynaconf/include/Options.h>
#include <dynaconf/include/NamedType.h>

using ValueType = dynaconf::NamedType<int, struct ValueTypeParameter >;

SCENARIO( "options should provide for multiple named definitions of object" )
{
	GIVEN( "a set of definitions options and a configuration scope" )
	{
		auto options = std::make_shared<dynaconf::Options>();
		auto scope = std::make_shared<dynaconf::Scope>();

		THEN( "options should resolve as expected" )
		{
			REQUIRE( dynaconf::get<ValueType>( options, "0" ) == nullptr );
			REQUIRE( dynaconf::get<ValueType>( options, "1" ) == nullptr );
			REQUIRE( dynaconf::get<ValueType>( options, "2" ) == nullptr );
			REQUIRE( dynaconf::get<ValueType>( options, "3" ) == nullptr );

			dynaconf::set( options, "1", dynaconf::make_singleton<ValueType>( std::make_shared<ValueType>( 1 ) ) );
			dynaconf::set( options, "2", dynaconf::make_singleton<ValueType>( std::make_shared<ValueType>( 2 ) ) );
			dynaconf::set( options, "3", dynaconf::make_singleton<ValueType>( std::make_shared<ValueType>( 2 ) ) );

			REQUIRE( dynaconf::get<ValueType>( options, "0" ) == nullptr );
			REQUIRE( dynaconf::get<ValueType>( options, "1" ) != nullptr );
			REQUIRE( dynaconf::get<ValueType>( options, "2" ) != nullptr );
			REQUIRE( dynaconf::get<ValueType>( options, "3" ) != nullptr );
		}

		THEN( "options should be selectable" )
		{
			dynaconf::set( options, "1", dynaconf::make_singleton<ValueType>( std::make_shared<ValueType>( 1 ) ) );
			dynaconf::set( options, "2", dynaconf::make_singleton<ValueType>( std::make_shared<ValueType>( 2 ) ) );
			dynaconf::set( options, "3", dynaconf::make_singleton<ValueType>( std::make_shared<ValueType>( 2 ) ) );

			REQUIRE_FALSE( dynaconf::set<ValueType>( scope, "0", options ) );
			REQUIRE( dynaconf::set<ValueType>( scope, "1", options ) );
			REQUIRE( dynaconf::get<ValueType>( scope )->get() == 1 );
		}
	}
}
