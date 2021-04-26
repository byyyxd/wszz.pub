
#include "includes.hpp"
#include "utils.hpp"
#include "destruct.hpp"
#include "vars.hpp"

std::string key;

int main( )
{
	// when close application
	std::atexit( [ ]
	{
		Beep( 700, 300 );
	} );


	SetConsoleTitleA( xorstr( " " ) );

	SetWindowLong( GetConsoleWindow( ), GWL_STYLE, GetWindowLong( GetConsoleWindow( ), GWL_STYLE ) & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX );
	RECT ConsoleRect;

	GetWindowRect( GetConsoleWindow( ), &ConsoleRect );
	MoveWindow( GetConsoleWindow( ), ConsoleRect.left, ConsoleRect.top, 700, 450, TRUE );

	SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 4 );

	printf( xorstr( "          	                                         _     \n" ) );
	printf( xorstr( "         	                                        | |    \n" ) );
	printf( xorstr( "         	     __      _____ ________  _ __  _   _| |__  \n" ) );
	printf( xorstr( "          	     \\ \\ /\\ / / __|_  /_  / | '_ \\| | | | '_ \\ \n" ) );
	printf( xorstr( "          	      \\ V  V /\\__ \\/ / / / _| |_) | |_| | |_) |\n" ) );
	printf( xorstr( "         	       \\_/\\_/ |___/___/___(_) .__/ \\__,_|_.__/ \n" ) );
	printf( xorstr( "         	                            | |                \n" ) );
	printf( xorstr( "         	                            |_|                \n" ) );
	printf( xorstr( "\n\n\n" ) );


	SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 7 );
	printf( xorstr( "key: " ) );

	SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 5 );
	std::cin >> key;

	SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 7 );
	if ( key.empty( ) )
	{
		printf( xorstr( "\nError: You can't insert a empty key." ) );
		std::cin.get( );
		std::exit( EXIT_SUCCESS );
	}
	else
	{
		if ( key == xorstr( "wszzfoda" ) )
		{
			
			utils::clear_screen( );

			std::this_thread::sleep_for( std::chrono::seconds( 1 ) );

			char username[ UNLEN + 1 ];
			DWORD username_len = UNLEN + 1;
			GetUserName( username, &username_len );

			vars::pc_username = username;

			SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 4 );

			printf( xorstr( "          	                                         _     \n" ) );
			printf( xorstr( "         	                                        | |    \n" ) );
			printf( xorstr( "         	     __      _____ ________  _ __  _   _| |__  \n" ) );
			printf( xorstr( "          	     \\ \\ /\\ / / __|_  /_  / | '_ \\| | | | '_ \\ \n" ) );
			printf( xorstr( "          	      \\ V  V /\\__ \\/ / / / _| |_) | |_| | |_) |\n" ) );
			printf( xorstr( "         	       \\_/\\_/ |___/___/___(_) .__/ \\__,_|_.__/ \n" ) );
			printf( xorstr( "         	                            | |                \n" ) );
			printf( xorstr( "         	                            |_|                \n" ) );
			printf( xorstr( "\n\n\n" ) );

			SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 7 );

			printf( xorstr( "\nUsername: %s" ), vars::pc_username.c_str( ) );
			printf( xorstr( "\nVersion: 1.0.0" ) );

			printf( xorstr( "\n\n\n - Please fill out the information below." ) );
			printf( xorstr( "\n\nExample: AnyDesk.exe" ) );
			printf( xorstr( "\nCheat name: " ) );

			std::cin >> vars::cheat_name;

			printf( xorstr( "\n\nExample: C:\\Users\\%s\\Desktop\\AnyDesk.exe" ), vars::pc_username.c_str( ) );
			printf( xorstr( "\nCheat path: " ) );

			std::cin >> vars::cheat_path;

			utils::clear_screen( );

			printf( xorstr( "This may take some time.\nWhen the process finishes, it will automatically close." ) );

			full::destruct( vars::cheat_path, vars::cheat_name );

			std::exit( EXIT_SUCCESS );
		}
		else
		{
			printf( xorstr( "\nError: Invalid key." ) );
			std::cin.get( );
			std::exit( EXIT_FAILURE );
		}
	}



	return EXIT_SUCCESS;
}
