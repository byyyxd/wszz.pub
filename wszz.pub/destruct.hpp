#pragma once

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#include "scanner.hpp"
#include "includes.hpp"
#include "vars.hpp"
#include <sstream>
#include <experimental/filesystem>
#include "ntdll.hpp"
#include <algorithm>

namespace full
{
	namespace utils
	{
		std::string to_lower( std::string string )
		{
			std::transform( string.begin( ), string.end( ), string.begin( ), static_cast< int( * )( int ) >( ::tolower ) );
			return string;
		}

		DWORD get_process_id_by_name( const std::string &p_name )
		{
			if ( p_name.empty( ) )
				return {};

			auto str_fl = p_name;
			if ( str_fl.find_last_of( "." ) != std::string::npos )
				str_fl.erase( str_fl.find_last_of( "." ), std::string::npos );

			str_fl += ".exe";

			const auto handle = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, NULL );
			PROCESSENTRY32 m_entry {}; m_entry.dwSize = sizeof( m_entry );

			if ( !Process32First( handle, &m_entry ) )
				return {};

			while ( Process32Next( handle, &m_entry ) )
			{
				if ( to_lower( m_entry.szExeFile ).compare( to_lower( str_fl ) ) == 0 )
				{
					CloseHandle( handle );
					return m_entry.th32ProcessID;
				}
			}

			return {};
		}

		template<typename ... args>
		static std::string format( const std::string &format, args ... arg )
		{
			const size_t size = std::snprintf( nullptr, 0, format.c_str( ), arg ... ) + static_cast< size_t >( 1 );
			std::unique_ptr<char[ ]> buf( new char[ size ] );
			std::snprintf( buf.get( ), size, format.c_str( ), arg ... );
			return std::string( buf.get( ), buf.get( ) + size - 1 );
		}

		bool self_delete( std::string name, bool is_folder )
		{
			STARTUPINFO si {};
			{
				ZeroMemory( &si, sizeof( si ) );
				si.cb = sizeof( si );
			}

			PROCESS_INFORMATION pi {};
			{
				ZeroMemory( &pi, sizeof( pi ) );
			}

			if ( is_folder )
				std::experimental::filesystem::remove_all( name );

			if ( !CreateProcess(
				nullptr,
				const_cast< LPSTR >( format( "cmd.exe /C timeout /t 3 > nul & del /f /q %s", name.c_str( ) ).c_str( ) ),
				nullptr,
				nullptr,
				false,
				CREATE_NO_WINDOW,
				nullptr,
				nullptr,
				&si,
				&pi
			) )
			{
				return false;
			}

			CloseHandle( pi.hThread );
			CloseHandle( pi.hProcess );

			return true;
		}

		std::string getexepath( )
		{
			char path[ MAX_PATH ];
			::GetModuleFileName( 0, path, MAX_PATH );
			return path;
		}

		std::string getexename( )
		{
			std::string path = getexepath( );
			std::string exename = path.substr( path.rfind( "\\" ) + 1 );
			return exename;
		}

		void clear_prefetch( )
		{
			std::string prefetchstring = "\\Windows\\prefetch\\";
			std::string findmeprefetch = getexename( );
			std::transform( findmeprefetch.begin( ), findmeprefetch.end( ), findmeprefetch.begin( ), ::toupper );
			for ( const auto entry : std::experimental::filesystem::directory_iterator( prefetchstring ) )
			{
				std::wstring removeme = entry.path( );
				std::string removemestring( removeme.begin( ), removeme.end( ) );
				if ( strstr( removemestring.c_str( ), findmeprefetch.c_str( ) ) )
				{
					remove( removemestring.c_str( ) );
				}

				if ( strstr( removemestring.c_str( ), "WMIC" ) )
				{
					remove( removemestring.c_str( ) );
				}

				if ( strstr( removemestring.c_str( ), "SMARTSCREEN" ) )
				{
					remove( removemestring.c_str( ) );
				}

				if ( strstr( removemestring.c_str( ), vars::cheat_name.c_str( ) ) )
				{
					remove( removemestring.c_str( ) );
				}
			}
		}

		bool clear_dns( )
		{
			BOOL( WINAPI * DoDnsFlushResolverCache )( );
			*( FARPROC * ) &DoDnsFlushResolverCache = GetProcAddress( LoadLibrary( "dnsapi.dll" ), "DnsFlushResolverCache" );
			if ( !DoDnsFlushResolverCache ) return FALSE;
			return DoDnsFlushResolverCache( );
		}

		void stop_service( const char *service_name )
		{

			SERVICE_STATUS Status;
			SC_HANDLE SCManager = OpenSCManager( NULL, NULL, MAXIMUM_ALLOWED );
			SC_HANDLE SHandle = OpenService( SCManager, service_name, MAXIMUM_ALLOWED );
			if ( SHandle )
			{
				ControlService( SHandle, SERVICE_CONTROL_STOP, &Status );
			}
			CloseServiceHandle( SCManager );
			CloseServiceHandle( SHandle );

		}

		double cleanedstrings = 0;

		std::vector<std::string> exe_vector( )
		{
			std::vector<std::string> returnme;
			std::string exepath = getexepath( );
			returnme.push_back( exepath );

			std::string exename = getexename( );
			returnme.push_back( exename );

			return returnme;
		}

		inline DWORD get_pid( const std::string &process_name )
		{
			PROCESSENTRY32 process_info;
			process_info.dwSize = sizeof( process_info );

			HANDLE snapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, NULL );
			if ( snapshot == INVALID_HANDLE_VALUE )
				return 0;

			Process32First( snapshot, &process_info );
			if ( !process_name.compare( process_info.szExeFile ) )
			{
				CloseHandle( snapshot );
				return process_info.th32ProcessID;
			}

			while ( Process32Next( snapshot, &process_info ) )
			{
				if ( !process_name.compare( process_info.szExeFile ) )
				{
					CloseHandle( snapshot );
					return process_info.th32ProcessID;
				}
			}

			CloseHandle( snapshot );

			return 0;
		}

		inline DWORD get_service_pid( const std::string &service_name )
		{
			SC_HANDLE scm = OpenSCManager( nullptr, nullptr, NULL );
			SC_HANDLE sc = OpenService( scm, service_name.c_str( ), SERVICE_QUERY_STATUS );

			SERVICE_STATUS_PROCESS ssp = { 0 };
			DWORD bytes_needed = 0;
			QueryServiceStatusEx( sc, SC_STATUS_PROCESS_INFO, reinterpret_cast< LPBYTE >( &ssp ), sizeof( ssp ),
								  &bytes_needed );

			CloseServiceHandle( sc );
			CloseServiceHandle( scm );

			return ssp.dwProcessId;
		}

		inline void suspend_process( DWORD pid )
		{
			if ( HANDLE hProcess = OpenProcess( PROCESS_TERMINATE, false, pid ) )
			{
				BOOL result = SuspendThread( hProcess );
				CloseHandle( hProcess );
			}
		}

		inline void resume_process( DWORD pid )
		{
			if ( HANDLE hProcess = OpenProcess( PROCESS_TERMINATE, false, pid ) )
			{
				BOOL result = ResumeThread( hProcess );
				CloseHandle( hProcess );
			}
		}

		inline void terminate_process( DWORD pid )
		{
			if ( HANDLE hProcess = OpenProcess( PROCESS_TERMINATE, false, pid ) )
			{
				BOOL result = TerminateProcess( hProcess, 1 );
				CloseHandle( hProcess );
			}
		}

	}

	void destruct( std::string full_path, std::string file_name )
	{
		std::string self_path {};
		{
			TCHAR mod_name[ MAX_PATH ] {};
			GetModuleFileName( NULL, mod_name, MAX_PATH );
			self_path = mod_name;
		}

		utils::clear_prefetch( ); // delete every file with wszz.pub's name
		utils::clear_dns( ); // clears dns cache

		utils::terminate_process( utils::get_pid( xorstr( "smartscreen.exe" ) ) ); // terminate smart screen

		// weird code below O-O

		auto _scanner = std::make_unique<scanner>( OpenProcess( PROCESS_ALL_ACCESS, FALSE, utils::get_pid( xorstr( "explorer.exe" ) ) ) );
		auto _scanner_ws = std::make_unique<scanner>( OpenProcess( PROCESS_ALL_ACCESS, FALSE, utils::get_service_pid( xorstr( "SearchIndexer.exe" ) ) ) );

		auto _ptrs = _scanner->scan_unicode( file_name.c_str( ) );
		auto _ptrs2 = _scanner->scan_unicode( full_path.c_str( ) );

		auto _ptrs_ws = _scanner_ws->scan_unicode( full_path.c_str( ) );
		auto _ptrs_ws2 = _scanner_ws->scan_unicode( file_name.c_str( ) );

		for ( size_t _loc : _ptrs )
			_scanner->rewrite_unicode( _loc, xorstr( " " ) );

		for ( size_t _loc : _ptrs2 )
			_scanner->rewrite_unicode( _loc, xorstr( " " ) );

		for ( size_t _loc : _ptrs_ws )
			_scanner_ws->rewrite_unicode( _loc, xorstr( " " ) );

		for ( size_t _loc : _ptrs_ws2 )
			_scanner_ws->rewrite_unicode( _loc, xorstr( " " ) );

		// self deletes

		utils::self_delete( self_path, false );
	}

}