#include <dynaconf/include/Options.h>

namespace dynaconf {

	/// Define an option for a class.
	///
	/// @param defintion to set.
	/// @param key identifying that definition.
	/// @return boolean indication of success.
	///
	bool Options::define( std::shared_ptr<Definition> && definition, const std::string & key )
	{
		std::unique_lock<std::mutex> lock( mutex );
		auto index = definition->index();

		// attempt update cluster
		//
		auto result = clusters[ index ].definitions.emplace( std::piecewise_construct,
			std::forward_as_tuple( key ),
			std::forward_as_tuple( std::move( definition ) ) );

		return result.second;
	}

	/// Resolve an option for a class.
	///
	/// @param type_index of class to resolve.
	/// @param key identifying a definition.
	/// @return pointer to definition or nullptr.
	///
	std::shared_ptr<Definition> Options::resolve( const std::type_index & index, const std::string & key ) const
	{
		std::unique_lock<std::mutex> lock( mutex );
		auto cluster = clusters.find( index );
		if( cluster != clusters.end() )
		{
			auto result = cluster->second.definitions.find( key );
			if( result != cluster->second.definitions.end() )
			{
				return result->second;
			}
		}
		return std::shared_ptr<Definition>{ nullptr };
	}

	/// Default global option set
	///
	static Options globals;

	const std::shared_ptr<Options> Options::Global{ &globals, [](Options*){} };
}
