#pragma once
#include <dynaconf/include/Scope.h>

namespace dynaconf {

	/// Registry providing a many-to-one relations from definitions to classes.
	///
	/// Options exists to bootstrap configuration parsing. Multiple 
	/// definitions can be registered with a disambiguation key. For 
	/// example, if a program is passed '--my-opt=option-val', the parser
	/// might use:
	///
	///	set<MyOpt>( scope, 'option-val', options ); 
	///
	/// to set the chosen option in the appropriate scope.
	///
	/// TODO: Maybe virtualize clusters to allow for more generic option 
	/// handling--i.e. returning the value passed, const/enum generation,
	/// etc.
	///
	class Options {
	public:
		/// Define an option for a class.
		///
		/// @param defintion to set.
		/// @param key identifying that definition.
		/// @return boolean indication of success.
		///
		bool define( std::shared_ptr<Definition> && definition, const std::string & key );

		/// Resolve an option for a class.
		///
		/// @param type_index of class to resolve.
		/// @param key identifying a definition.
		/// @return pointer to definition or nullptr.
		///
		std::shared_ptr<Definition> resolve( const std::type_index & index, const std::string & key ) const;

		/// Default global option set
		///
		static const std::shared_ptr<Options> Global;

		/// Syntatic sugar for statically exporting a variable.
		///
		/// Example:
		/// 
		///   Options::Export myExport( make_singleton<MyType>( std:::make_shared<MyType> ) ), "MyType" );
		///
		///   /* ... */
		///
		///   set<MyType>( scope, "MyType", Options::Global );
		///
		/// TODO: Evaluate dropping this global linkage...
		///
		class Export {
		public:
			const bool valid;	///< indicates if the export was successful.

			/// Export constructor--performs option registration
			///
			/// @tparam DefinitionType for definition.
			/// @param definition to export
			/// @param key associated with definition
			/// @param options to export to -- default global.
			///
			template < typename DefinitionType >
			Export( std::shared_ptr<DefinitionType> && definition, const std::string & key, const std::shared_ptr<Options> & options = Global )
			: valid( set( options, key, definition ) )
			{}
		};

	protected:
		/// Collection of definitions sharing the same type_index.
		/// 
		struct Cluster {
			std::unordered_map<std::string, std::shared_ptr<Definition> > definitions;
		};
		
		mutable std::mutex mutex;
		std::unordered_map<std::type_index, Cluster > clusters;
	};


	/// Provide an option for a definition.
	///
	/// @tparam DefinitionType type of definition--deduced.
	/// @param options to add option to.
	/// @param key for this option.
	/// @param definition to set.
	/// @return boolean indication of success.
	///
	template < typename DefinitionType >
	bool set( const std::shared_ptr<Options> & options, const std::string & key, const std::shared_ptr<DefinitionType> & definition )
	{
		return options->define( std::static_pointer_cast<Definition>( definition ), key );
	}


	/// Get an option for a definition.
	///
	/// @tparam Class to resolve.
	/// @param options to query.
	/// @param key for definition.
	/// @return Provider<Class> pointer or nullptr.
	///
	template < typename Class >
	auto get( const std::shared_ptr<Options> & options, const std::string & key ) -> std::shared_ptr< Provider<Class> >
	{
		auto definition = options->resolve( std::type_index{ typeid(Class) }, key );
		return std::dynamic_pointer_cast< Provider<Class> >( definition );
	}


	/// Update a scope with a definition from options.
	///
	/// @tparam Class to resolve in options.
	/// @param scope to update with defition
	/// @param key for definition.
	/// @param options to query for defintion.
	/// @return boolean indication of success.
	///
	template < typename Class >
	bool set( const std::shared_ptr<Scope> & scope, const std::string & key, const std::shared_ptr<Options> & options )
	{
		auto definition = options->resolve( std::type_index{ typeid(Class) }, key );
		if( definition )
		{
			return scope->define( definition );
		}
		else
		{
			return false;
		}
	}	
}
