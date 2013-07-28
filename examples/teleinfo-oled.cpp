/* ======================================================================
Program : teleinfo oled
Purpose : grab teleinformation from serial line then write to OLED display
Version : 1.0
Author  : (c) Charles-Henri Hallard
Comments: some code grabbed from picocom and other from teleinfo
	: You can use or distribute this code unless you leave this comment
	: too see thos code correctly indented, please use Tab values of 2
	
03/05/2013 	Charles-Henri Hallard (http://hallard.me)
						Modified for compiling and use on Raspberry ArduiPi Board
						LCD size and connection are passed as arguments 
						ArduiPi project documentation http://hallard.me/arduipi

====================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <syslog.h>
#include <termios.h>
#include <netinet/in.h>
#include <getopt.h>

#include "ArduiPi_OLED_lib.h"
#include "Adafruit_GFX.h"
#include "ArduiPi_OLED.h"

// ----------------
// Constants
// ----------------
#define true 1
#define false 0

#define PRG_NAME        "teleinfo-oled"
#define PRG_VERSION     "1.1"
#define PRG_DIR			"/usr/local/bin" 				// Directory where we are when in daemon mode

// Define teleinfo mode, device or network
#define TELEINFO_DEVICE 	"/dev/ttyAMA0"
#define TELEINFO_PORT			1200							// Port used to receive frame over Network
#define TELEINFO_NETWORK	"192.168.1.255"		// Broadcast or specific IP where to send frame

// Local File for teleinfo
#define	TELEINFO_BUFSIZE	512

#define	STX 0x02
#define	ETX 0x03


enum parity_e 		{  P_NONE, 	P_EVEN, 	 P_ODD };
enum flowcntrl_e 	{ FC_NONE, 	FC_RTSCTS, FC_XONXOFF };
enum mode_e 			{ MODE_NET, MODE_SERIAL };
enum ptec_e 			{ PTEC_HP, PTEC_HC };

// Oled protocol 
enum oled_e 	{ OLED_I2C, OLED_SPI };


// Config Option
struct s_opts
{
	char port[128];
	int baud;
	enum flowcntrl_e flow;
	char flow_str[32];
	enum parity_e parity;
	char parity_str[32];
	int databits;
	int mode;
	char mode_str[32];
	int address;					// i2c slave address
	int oled;						// protocol used for OLED
	int netport;
	int verbose;
  int daemon;
} ;

s_opts opts = {
	TELEINFO_DEVICE,
	1200,
	FC_NONE,
	"none",
	P_EVEN,
	"even",
	7,
	MODE_SERIAL,
	"Serial",
	0,
	OLED_ADAFRUIT_SPI_128x32,
  TELEINFO_PORT,
	false,
  false
};


// ======================================================================
// Global vars 
// ======================================================================
int 	g_fd_teleinfo; 					// teleinfo serial handle
struct termios g_oldtermios ; // old serial config
int 	g_tlf_sock;							// teleinfo socket 
int		g_exit_pgm;							// indicate en of the program
char 	g_lockname[256] = ""; // Lock filename

ArduiPi_OLED display;		// display OLED class


// Valeur des label Téléinfo à afficher
struct s_values
{
  long hchp;		// Index Heures Pleines
	long hchc;		// Index Heures Creuses
	int	 papp;		// Puissance Apparente
	int	 isousc;	// Intensité souscrite
	int	 iinst;		// Intensité instantanée
	int	 ptec;		// Puissance Tarifaire en cours
} ;

s_values g_values;

// ======================================================================
// Global funct
// ======================================================================
void tlf_close_serial(int);

/* ======================================================================
Function: log_syslog
Purpose : write event to syslog
Input 	: stream to write if needed
					string to write in printf format
					printf other arguments
Output	: -
Comments: 
====================================================================== */
void log_syslog( FILE * stream, const char *format, ...)
{
	static char tmpbuff[512]="";
	va_list args;

	// do a style printf style in ou buffer
	va_start (args, format);
	vsnprintf (tmpbuff, sizeof(tmpbuff), format, args);
	tmpbuff[sizeof(tmpbuff) - 1] = '\0';
	va_end (args);

	// Write to logfile
	openlog( PRG_NAME, LOG_PID|LOG_CONS, LOG_USER);
 	syslog(LOG_INFO, tmpbuff);
 	closelog();
 	
 	// stream passed ? write also to it
 	if (stream && opts.verbose && !opts.daemon ) 
 	{
 		fprintf(stream, tmpbuff);
 		fflush(stream);
 	}
}


/* ======================================================================
Function: clean_exit
Purpose : exit program 
Input 	: exit code
Output	: -
Comments: 
====================================================================== */
void clean_exit (int exit_code)
{
	int r;	
	
	// close serials
  if (g_fd_teleinfo)
  {
		// Restore Old parameters.
  	if (  (r = tcsetattr(g_fd_teleinfo, TCSAFLUSH, &g_oldtermios)) < 0 )
			log_syslog(stderr, "cannot restore old parameters %s: %s", opts.port, strerror(errno));

		// then close
  	tlf_close_serial(g_fd_teleinfo);
  }
	
	 	// close socket
 	if (g_tlf_sock)
 		close(g_tlf_sock);
	
	// Release OLED and Raspberry I/O control
  display.close();

	if ( exit_code == EXIT_SUCCESS)
	{			
			log_syslog(stdout, "Succeded to do my job\n");
	}
	else
	{
			log_syslog(stdout, "Closing teleinfo due to error\n");
	}
	
	exit(exit_code);
}

/* ======================================================================
Function: fatal
Purpose : exit program due to a fatal error
Input 	: string to write in printf format
					printf other arguments
Output	: -
Comments: 
====================================================================== */
void fatal (const char *format, ...)
{
	char tmpbuff[512] = "";
	va_list args;

	va_start(args, format);
	vsnprintf(tmpbuff, sizeof(tmpbuff), format, args);
	tmpbuff[sizeof(tmpbuff) - 1] = '\0';
	va_end(args);

	// Write to logfile
	openlog( PRG_NAME, LOG_PID | LOG_CONS, LOG_USER);
 	syslog(LOG_INFO, tmpbuff);
 	closelog();

	fprintf(stderr,"\r\nFATAL: %s \r\n", tmpbuff );
	fflush(stderr);

	clean_exit(EXIT_FAILURE);
}



/* ======================================================================
Function: daemonize
Purpose : daemonize the pocess
Input 	: -
Output	: -
Comments: 
====================================================================== */
static void daemonize(void)
{
	pid_t pid, sid;
	
	// already a daemon
	if ( getppid() == 1 ) 
		return;
	
	// Fork off the parent process 
	pid = fork();
	if (pid < 0) 
   	fatal( "fork() : %s", strerror(errno));

	// If we got a good PID, then we can exit the parent process.
	if (pid > 0) 
	  exit(EXIT_SUCCESS);

	
	// At this point we are executing as the child process
	// ---------------------------------------------------

	// Change the file mode mask 
	umask(0);
	
	// Create a new SID for the child process 
	sid = setsid();
	if (sid < 0) 
   	fatal( "setsid() : %s", strerror(errno));

	// Change the current working directory.  This prevents the current
	// directory from being locked; hence not being able to remove it.
	if ((chdir(PRG_DIR)) < 0) 
   	fatal( "chdir('%s') : %s", PRG_DIR, strerror(errno));
	
	// Close standard files 
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);
}

/* ======================================================================
Function: isr_handler
Purpose : Interrupt routine Code for signal
Input 	: -
Output	: -
Comments: 
====================================================================== */
void isr_handler (int signum)
{
	// Does we received CTRL-C ?
	if ( signum==SIGINT || signum==SIGTERM)
	{
		// Indicate we want to quit
		g_exit_pgm = true;
		
		log_syslog(NULL, "Received SIGINT/SIGTERM");
	}
	// Our receive buffer is full
	else if (signum == SIGIO)
	{
		log_syslog(NULL, "Received SIGIO");
	
	}
	
}

/* ======================================================================
Function: tlf_init_serial
Purpose : initialize serial port for receiving teleinfo
Input 	: -
Output	: Serial Port Handle
Comments: -
====================================================================== */
int tlf_init_serial(void)
{
	int tty_fd, r ;
	struct termios  termios ;
	
  // Ouverture de la liaison serie (Nouvelle version de config.)
  if ( (tty_fd = open(opts.port, O_RDONLY | O_NOCTTY)) == -1 ) 
  	fatal( "tlf_init_serial %s: %s", opts.port, strerror(errno));
  else
		log_syslog( stdout, "'%s' opened.\n",opts.port);
 
	// Get current parameters.
  if (  (r = tcgetattr(tty_fd, &g_oldtermios)) < 0 )
		log_syslog(stderr, "cannot get current parameters %s: %s",  opts.port, strerror(errno));
		
	// clear struct
	bzero(&termios, sizeof(termios)); 

	// Even Parity with 7 bits data
	termios.c_cflag = (B1200 | PARENB | CS7 | CLOCAL | CREAD) ;
	//termios.c_iflag = IGNPAR ;
	termios.c_iflag =	(IGNPAR | INPCK | ISTRIP) ; ;
	termios.c_oflag = 0;	// Pas de mode de sortie particulier (mode raw).
	termios.c_lflag = 0;	// non canonique
	termios.c_cc[VTIME] = 0;     /* inter-character timer unused */
	termios.c_cc[VMIN]  = 1;     /* blocking read until 1 character arrives */

	tcflush(tty_fd, TCIFLUSH);

  if ( ( r = tcsetattr(tty_fd,TCSANOW,&termios) < 0 ) )
		log_syslog(stderr, "cannot get current parameters %s: %s",  opts.port, strerror(errno));

	// Sleep 500ms
	usleep(500000);

 	return tty_fd ;
}

/* ======================================================================
Function: tlf_close_serial
Purpose : close serial port for receiving teleinfo
Input 	: Serial Port Handle
Output	: -
Comments: 
====================================================================== */
void tlf_close_serial(int device)
{
  if (device)
  {	
		int r;
	
	  	// Clean up
		if ( (r = tcflush(device, TCIOFLUSH)) < 0 ) 
			fatal("cannot flush before quit %s: %s", opts.port, strerror(errno));
		
		// restore old settings
		tcsetattr(device,TCSANOW,&g_oldtermios);
	  	
		close(device) ;
	}
}

/* ======================================================================
Function: tlf_checksum_ok
Purpose : check if teleinfo frame checksum is correct
Input 	: -
Output	: true or false
Comments: 
====================================================================== */
int tlf_checksum_ok(char *etiquette, char *valeur, char checksum) 
{
	unsigned int i ;
	unsigned char sum = 32 ;		// Somme des codes ASCII du message + un espace

	for (i=0; i < strlen(etiquette); i++) 
		sum = sum + etiquette[i] ;
		
	for (i=0; i < strlen(valeur); i++) 
		sum = sum + valeur[i] ;
		
	sum = (sum & 63) + 32 ;

	// Return 1 si checkum ok.
	if ( sum == checksum) 
		return ( true ) ;	

	return ( false ) ;
}



/* ======================================================================
Function: tlf_treat_label
Purpose : do action when received a correct label / value + checksum line
Input 	: plabel : pointer to string containing the label
				: pvalue : pointer to string containing the associated value
Output	: 
Comments: 
====================================================================== */
void tlf_treat_label( char * plabel, char * pvalue) 
{
	// Intensité souscrite
	if (strcmp(plabel, "ISOUSC")==0 )
		g_values.isousc = atoi ( pvalue);

	// Intensité instantanée
	if (strcmp(plabel, "IINST")==0 )
		g_values.iinst = atoi ( pvalue);

	// Display current Power
	if (strcmp(plabel, "PAPP")==0 )
		g_values.papp = atoi ( pvalue);

	// Display current price index
	if (strcmp(plabel, "PTEC")==0 && strcmp(pvalue, "HP..")==0 )
		g_values.ptec = PTEC_HP;

	if (strcmp(plabel, "PTEC")==0 && strcmp(pvalue, "HC..")==0 )
		g_values.ptec = PTEC_HC;

		// Display Index
	if (strcmp(plabel, "HCHC")==0 || strcmp(plabel, "HCHP")==0)
	{
		long index = atol ( pvalue);
		//fprintf(stdout, "%s = %ld KWh\n", plabel, index);
		
		if (strcmp(plabel, "HCHC")==0 )
			g_values.hchc = index;
		else
			g_values.hchp = index;
		
	}
}


/* ======================================================================
Function: tlf_check_frame
Purpose : check for teleinfo frame
Input 	: -
Output	: the length of valid buffer, 0 otherwhile
Comments: 
====================================================================== */
int tlf_check_frame( char * pframe) 
{
	char * pstart; 
	char * pend; 
	char * pnext; 
	char * ptok; 
	char * pvalue; 
	char * pcheck;
	char	buff[TELEINFO_BUFSIZE];
	int frame_err , len;

	len = strlen(pframe); 
	
	strncpy( buff, pframe, len+1);

	pstart = &buff[0];
	pend   = pstart + len;

	if (opts.verbose)
	{
		fprintf(stdout, "------------------- Received %d char %s Frame.%s\n-------------------", len, opts.mode_str, buff);
		fflush(stdout);
	}	
	
	// just one vefification, buffer should be at least 100 Char
	if ( pstart && pend && (pend > pstart+100 ) )
	{
		//fprintf(stdout, "Frame to analyse [%d]%s\n", pend-pstart, pstart);
		//fflush(stdout);	

		// by default no error
		frame_err = false;
		
		//log_syslog(stderr, "Found %d Bytes Frame\n", pend-pstart);

		// ignore STX
		pstart++;

		// Init our pointers
		ptok = pvalue = pnext = pcheck = NULL;
		

		// Loop in buffer	
		while ( pstart < pend )
		{
			// start of token
			if ( *pstart=='\n' )
			{		
				// Position on token				
				ptok = ++pstart;
			}						

			// start of token value
			if ( *pstart==' ' && ptok)
			{						
				// Isolate token name
				*pstart++ = '\0';

				// no value yet, setit ?
				if (!pvalue)
					pvalue = pstart;
				// we had value, so it's checksum
				else
					pcheck = pstart;
			}						

			// new line ? ok we got all we need ?
			if ( *pstart=='\r' )
			{						
				
				*pstart='\0';

				// Good format ?
				if ( ptok && pvalue && pcheck )
				{
					// Checksum OK
					if ( tlf_checksum_ok(ptok, pvalue, *pcheck))
					{
						//fprintf(stdout, "%s=%s\n",ptok, pvalue);
						
						// In case we need to do things
						tlf_treat_label(ptok, pvalue);
					}
					else
					{
						frame_err = true;
						//fprintf(stdout, "%s=%s BAD",ptok, pvalue);
						log_syslog(stderr, "tlf_checksum_ok('%s','%s','%c') error\n",ptok, pvalue, *pcheck);
						
					}
				}
				else
				{
					frame_err = true;
					log_syslog(stderr, "tlf_check_frame() no correct frame '%s'\n",ptok);
				}
				
				// reset data
				ptok = pvalue = pnext = pcheck = NULL;

			}						
			
			// next
			pstart++;
	
		}

		// no error in this frame ?
		if (!frame_err)
		{
			if (opts.verbose)
				fprintf(stdout, "Frame OK (%d)\n", len);

			//debug_dump_sensors(0);
			return (len);
		}
	}	
	else
	{
		log_syslog(stderr, "tlf_check_frame() No correct frame found\n");
	}

	return (0);

}

/* ======================================================================
Function: tlf_get_frame
Purpose : check for teleinfo frame broadcasted on the network
Input 	: true if we need to wait for frame, false if async (take if any)
Output	: true if frame ok, else false
Comments: 
====================================================================== */
int tlf_get_frame(char block) 
{
	struct sockaddr_in tlf_from;
	int fromlen = sizeof(tlf_from);
	int n = 0;
	int timeout = 100; // (10 * 100ms)
	int ret  =false;
	char	rcv_buff[TELEINFO_BUFSIZE];

	// clear ou receive  buffer
	memset(rcv_buff,0, TELEINFO_BUFSIZE );

	// do until received or timed out
	while (n<=0 && timeout--)
	{
		// read data received on socket ?
		n = recvfrom(g_tlf_sock,rcv_buff,TELEINFO_BUFSIZE,0, (struct sockaddr *)&tlf_from,(socklen_t *)&fromlen);

		// Do we received frame on socket ?
		if (n > 0) 
		{
			//log_syslog( stderr, "recvfrom %d buffer='%s'\n",n, rcv_buff);
			// check the frame and do stuff
			ret = tlf_check_frame( rcv_buff );
		}
		else 
		{
			// want to wait frame ?
			if ( block)
			{
				// Letting time to the Operating system doing other jobs
				// Wait 100ms it won't bother us
				usleep(100000);
			}
			else
			{
				break;
			}
		}
	}
	
	// check for timed out
	if (block && timeout<=0)
			log_syslog( stderr, "tlf_get_frame() Time-Out Expired\n");
		
	return (ret);
}

/* ======================================================================
Function: usage
Purpose : display usage
Input 	: program name
Output	: -
Comments: 
====================================================================== */
void usage( char * name)
{

	printf("%s\n", PRG_NAME );
	printf("Usage is: %s --net|serial --oled type  [-a address] [options] <tty device>\n", PRG_NAME);
	printf("  --<s>erial :  receive teleinfo frame from serial\n");
	printf("  --<n>net   :  receive teleinfo frame from network\n");
	printf("  --<o>led type\n\tOLED type if one of :\n");
	for (int i=0; i<OLED_LAST_OLED;i++)
		printf("\t  %1d : %s\n", i, oled_type_str[i]);
	printf("Options are:\n");
	printf("  --<v>erbose  : speak more to user\n");
	printf("  --<d>aemon   : daemonize the process\n");
	printf("  --<h>elp\n");
	printf("<?> indicates the equivalent short option.\n");
	printf("Short options are prefixed by \"-\" instead of by \"--\".\n");
	printf("Example :\n");
	printf( "teleinfo -s -d /dev/ttyAMA0\nstart teleinfo as a daemon to receive frame from the serial port /dev/ttyAMA0\n\n");
	printf( "teleinfo -n -v\nstart teleinfo to wait for a network frame, then display it\n");
}


/* ======================================================================
Function: parse_args
Purpose : parse argument passed to the program
Input 	: -
Output	: -
Comments: 
====================================================================== */
void parse_args(int argc, char *argv[])
{
	static struct option longOptions[] =
	{
		{"verbose", no_argument,	  	0, 'v'},
		{"net"		, no_argument,	  	0, 'n'},
		{"serial"	, no_argument,	  	0, 's'},
		{"oled"   ,required_argument, 0, 'o' },
		{"port"   ,	required_argument,0, 'p'},
		{"daemon" ,	no_argument, 			0, 'd'},
		{"help"	  ,	no_argument, 			0, 'h'},
		{0, 0, 0, 0}
	};

	int optionIndex = 0;
	int c;
	char * pEnd;

	while (1) 
	{
		/* no default error messages printed. */
		opterr = 0;

    c = getopt_long(argc, argv, "vnshdp:o:", longOptions, &optionIndex);

		if (c < 0)
			break;

		switch (c) 
		{
		
			case 'v':opts.verbose = true;		break;
			case 'd':opts.daemon = true;		break;

			case 'n':
				opts.mode = MODE_NET;
				strcpy(opts.mode_str, "Network");
			break;

			case 's':
				opts.mode = MODE_SERIAL;
				strcpy(opts.mode_str,"Serial");
			break;
			
			case 'o':
				opts.oled = (int) atoi(optarg);
				
				if (opts.oled < 0 || opts.oled >= OLED_LAST_OLED )
				{
						fprintf(stderr, "--oled %d ignored must be 0 to %d.\n", opts.oled, OLED_LAST_OLED-1);
						fprintf(stderr, "--oled set to 0 now\n");
						opts.oled = 0;
				}
			break;

			case 'a':
				opts.address = strtol(optarg,&pEnd,0);
				
				if ( !*pEnd || opts.address < 1 || opts.address > 0x7F )
				{
						fprintf(stderr, "--address %d (0x%02x) ignored.\n", opts.address, opts.address);
						fprintf(stderr, "--address must be between 1 and 255 or 0x01 and 0xff\n");
						opts.address = 0;
						fprintf(stderr, "--setting slave to default 0x%02x\n", opts.address);
				}
			break;


			case 'p':
				opts.netport = (int) atoi(optarg);
				
				if (opts.netport < 1024 || opts.netport > 65534)
				{
						fprintf(stderr, "--port '%d' ignored.\n", opts.netport);
						fprintf(stderr, "--port must be between 1024 and 65534\n");
						opts.netport = TELEINFO_PORT;
				}
			break;

			case 'h':
				usage(argv[0]);
				exit(EXIT_SUCCESS);
			case '?':
			default:
				fprintf(stderr, "Unrecognized option.\n");
				fprintf(stderr, "Run with '--help'.\n");
				exit(EXIT_FAILURE);
		}
	} /* while */

	if ( opts.mode == MODE_SERIAL )
	{ 
		if ( (argc - optind) < 1 ) 
		{
			fprintf(stderr, "No serial port given setting up to default serial port (%s)\n", opts.port);
		}
		else
		{
			strncpy(opts.port, argv[optind], sizeof(opts.port) - 1);
			opts.port[sizeof(opts.port) - 1] = '\0';
		}
	}

	if (opts.verbose)
	{
		printf("%s v%s\n", PRG_NAME, PRG_VERSION);

		printf("-- OLED params -- \n");
		printf("Oled is        : %s\n", oled_type_str[opts.oled]);

		
		if (opts.mode == MODE_NET )
		{
			printf("-- Network Stuff -- \n");
			printf("udp port is    : %d\n", opts.netport);
		}
		else
		{
			printf("-- Serial Stuff -- \n");
			printf("port is        : %s\n", opts.port);
			printf("flowcontrol    : %s\n", opts.flow_str);
			printf("baudrate is    : %d\n", opts.baud);
			printf("parity is      : %s\n", opts.parity_str);
			printf("databits are   : %d\n", opts.databits);
		}
		
		printf("-- Other Stuff -- \n");
		printf("verbose is     : %s\n", opts.verbose? "yes" : "no");
		printf("\n");
	}	
}

/* ======================================================================
Function: main
Purpose : Main entry Point
Input 	: -
Output	: -
Comments: 
====================================================================== */
int main(int argc, char **argv)
{
	struct sockaddr_in server;
	struct sigaction exit_action;
	int length, flags;
  int n;
	unsigned char c;
	char	rcv_buff[TELEINFO_BUFSIZE];
	int		rcv_idx;
  char time_str[200];
  time_t t;
  struct tm *tmp;
	int frame_ok ;

	rcv_idx = 0;
	g_fd_teleinfo = 0; 
	g_exit_pgm = false;
	
	bzero(rcv_buff, TELEINFO_BUFSIZE);

	parse_args(argc, argv);

	// Set up the structure to specify the exit action.
	exit_action.sa_handler = isr_handler;
	sigemptyset (&exit_action.sa_mask);
	exit_action.sa_flags = 0;
	sigaction (SIGTERM, &exit_action, NULL);
	sigaction (SIGINT, &exit_action, NULL); 

	// Grab teleinfo frame from network
	if (opts.mode == MODE_NET )
	{
		// Init Sockets
		g_tlf_sock=socket(AF_INET, SOCK_DGRAM, 0);
		
		if (g_tlf_sock < 0) 
			fatal( "Error Opening Socket %d: %s\n",g_tlf_sock, strerror (errno));
		else
		{
			if (opts.verbose)
				log_syslog(stderr, "Opened Socket\n");
		}
		
		flags = fcntl(g_tlf_sock,F_GETFL,0);
		fcntl(g_tlf_sock, F_SETFL, flags | O_NONBLOCK);

		length = sizeof(server);
		bzero(&server,length);
		
		server.sin_family=AF_INET;
		server.sin_addr.s_addr=INADDR_ANY;
		server.sin_port=htons(opts.netport);
		
		if ( bind(g_tlf_sock,(struct sockaddr *)&server,length) < 0 ) 
			fatal("Error Binding Socket %d : %s\n", g_tlf_sock, strerror (errno));
		else
		{
			if (opts.verbose)
				log_syslog(stderr, "Binded on port %d\n",opts.netport);
		}

		if (opts.verbose)
			log_syslog(stdout, "Network Init succeded\n");
		
	}

	// Grab teleinfo frame from serial device
	if (opts.mode == MODE_SERIAL)
	{
		if ( (g_fd_teleinfo = tlf_init_serial()) >0 )
		{
			if (opts.verbose)
				log_syslog(stdout, "Serial device %s Init succeded\n", opts.port);
		}
	}
	
	 // SPI
	if (display.oled_is_spi_proto(opts.oled))
	{
		// SPI change parameters to fit to your LCD
		if ( !display.init(OLED_SPI_DC,OLED_SPI_RESET,OLED_SPI_CS, opts.oled) )
			exit(EXIT_FAILURE);
	}
	else
	{
		// I2C change parameters to fit to your LCD
		if ( !display.init(OLED_I2C_RESET,opts.oled) )
			exit(EXIT_FAILURE);
	}

	display.begin();
	display.clearDisplay();
	
	// show 
  display.display(); 

 	log_syslog(stdout, "Inits succeded, entering Main loop\n");
 	
  if (opts.daemon)
  {
	 	log_syslog(stdout, "Starting as a daemon\n");
  	daemonize();
  }
	

	// Do while not end
	while ( ! g_exit_pgm ) 
	{
		// by default no frame
		frame_ok = false;
	
		// Grab from network
		if (opts.mode == MODE_NET )
		{
			// Get frame from network
			frame_ok =  tlf_get_frame(true);
		}
		else
		{
			// loop from serial port until frame ok
			while ( ! frame_ok)
			{
				// Read from serial port
				n = read(g_fd_teleinfo, &c, 1);
			
				if (n == 0)
					fatal("nothing to read");
				else if (errno == EINTR  )
					break;
				else if ( n < 0 )
					fatal("read failed: %s", strerror(errno));

				//log_syslog(stdout, "%c", c);
				//printf("%c",c);

				// What we received ?
				switch (c)
				{
					// start of frame ???
					case  STX:
						// Clear buffer, begin to store in it
						rcv_idx = 0;
						bzero(rcv_buff, TELEINFO_BUFSIZE);
						rcv_buff[rcv_idx++]=c;
					break;
						
					// End of frame ?
					case  ETX:
						// We had STX ?
						if ( rcv_idx )
						{
							// Store in buffer and proceed
							rcv_buff[rcv_idx++]=c;
							
							// clear the end of buffer (paranoia inside)
							bzero(&rcv_buff[rcv_idx], TELEINFO_BUFSIZE-rcv_idx);
							
							// Is this frame valid ?
							if ( (length = tlf_check_frame(rcv_buff)) > 0 )
							{
								frame_ok = true;
							}
						}
						// May be begin of the program or other problem.
						else
						{
							rcv_idx = 0;
							bzero(rcv_buff, TELEINFO_BUFSIZE);
						}
					break;
					
					// other char ?
					default:
					{
						// If we are in a frame, store data recceived
						if (rcv_idx)
						{
							// If buffer is not full
							if ( rcv_idx < TELEINFO_BUFSIZE)
							{
								// Store data recceived
								rcv_buff[rcv_idx++]=c;
							}
							else
							{
								// clear buffer & restart
								rcv_idx=0;
								bzero(rcv_buff, TELEINFO_BUFSIZE);
							}
						}
					}
					break;
				}
			}
		} // while not frame ok from serial
		
		// here we received a frame, even serial or from network
		// If frame  ok
		if ( frame_ok )
		{
			//char oled_buff[1024];
			int percent=0;
			
			t = time(NULL);
			tmp = localtime(&t);
			if (tmp) 
			{
				if (strftime(time_str, sizeof(time_str), "%d %b %Y %T" , tmp) == 0) 
					strcpy( time_str, "No Time");
			}

			// good full frame received, do whatever you want here
			//fprintf(stdout, "==========================\nTeleinfo Frame of %d char\n%s\n==========================%s\n",
			//								strlen(rcv_buff), time_str, rcv_buff );

			//sprintf( oled_buff, "Creuses %09lu\nPleines %09lu\n%d W   %d \n%s\n",
			//					g_values.hchp, g_values.hchc, g_values.papp, (100*g_values.iinst)/g_values.isousc, time_str);
			//fprintf(stdout, oled_buff);
			
			display.clearDisplay();   // clears the screen and buffer	display.setTextSize(1);
			display.setTextColor(WHITE);
			display.setCursor(0,0);

			// Percent of total power 
			percent = (100.0 * g_values.iinst) / g_values.isousc ;

			
			if (display.height() == 32 )
			{
				if (g_values.ptec == PTEC_HP )
					display.setTextColor(BLACK, WHITE); // 'inverted' text
			
				display.print("Pleines ");
				display.printf("%09ld\n", g_values.hchp);
				display.setTextColor(WHITE); // normaltext
				
				if (g_values.ptec == PTEC_HC )
					display.setTextColor(BLACK, WHITE); // 'inverted' text
					
				display.print("Creuses ");
				display.printf("%09ld\n", g_values.hchc);
				display.setTextColor(WHITE); // normaltext

				// Calculate Bargraph display
				
				//display.setTextColor(BLACK, WHITE); // 'inverted' text
				//skip seconds
				time_str[17] = 0x00;
				display.printf("%d W %d%%  %3d A\n%s", g_values.papp, percent, g_values.iinst, time_str);
				display.drawVerticalBargraph(114,0,12,32,1, percent);
				display.display();
				display.setTextColor(BLACK, WHITE); // 'inverted' text
			}
			else
			{
				if (g_values.ptec == PTEC_HP )
					display.setTextColor(BLACK, WHITE); // 'inverted' text
			
				display.setTextSize(2); 

				display.printf("%09ld\n", g_values.hchp);
				display.setTextColor(WHITE); // normaltext
				
				if (g_values.ptec == PTEC_HC )
					display.setTextColor(BLACK, WHITE); // 'inverted' text
					
				display.printf("%09ld\n", g_values.hchc);
				display.setTextColor(WHITE); // normaltext

				
				//display.setTextColor(BLACK, WHITE); // 'inverted' text
				display.setTextSize(1); 
				display.printf("%d W %d%%  %3d A\n", g_values.papp, percent, g_values.iinst);
				display.printf("%s", time_str);

				// Calculate Bargraph display
				// Percent of total power 
				display.drawHorizontalBargraph(0, 49,128,14,1, percent);

				display.display();
				display.setTextColor(BLACK, WHITE); // 'inverted' text
			}

		}

		// We want to display results ?
		if (opts.verbose)
		{
			if ( (n = write(STDOUT_FILENO, &c, 1)) <= 0 )
				fatal("write to stdout failed: %s", strerror(errno));
		}
	}
	

  log_syslog(stderr, "Program terminated\n");
  
  clean_exit(EXIT_SUCCESS);
  
  // avoid compiler warning
  return (0);
}
