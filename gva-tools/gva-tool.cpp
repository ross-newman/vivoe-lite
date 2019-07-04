//
// @file gva_tool.c
// @author ross.e.newman@lmco.com
// @date 02 May 2019
// @brief TBC
//
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/sysinfo.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <ncurses.h>
#include <ctype.h>
#include <libxml/parser.h>
#include <libxml/xmlreader.h>

// Default device name if not supplied on the command line 
#define DEFAULT_GPS_DEVICE_NAME "/dev/ttyUSB0"
// Console configuration file 
#define XML_FILENAME "config.xml"
// Console application description 
#define APP_DESCRIPTION "gva-tool"

enum {
    ON = 0,
    OFF,
    DISABLED,
    ERROR
};

enum {
    LOG_DEBUG = 0,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
};

enum {
    MESSAGE_INFO = 1,
    MESSAGE_WARNING,
    MESSAGE_ERROR
};

typedef struct nmessages {
    int             lines;
    char            line[100][100];
} nmessage;

typedef enum {
    KEY_SA  = 0,
    KEY_WPN,
    KEY_DEF,
    KEY_SYS,
    KEY_DRV,
    KEY_STR,
    KEY_COM,
    KEY_BMS,
    KEY_F1,
    KEY_F2,
    KEY_F3,
    KEY_F4,
    KEY_F5,
    KEY_F6,
    KEY_F7,
    KEY_F8,
    KEY_F9,
    KEY_F10,
    KEY_F11,
    KEY_F12,
    KEY_F13,
    KEY_F14,
    KEY_F15,
    KEY_F16,
    KEY_F17,
    KEY_F18,
    KEY_F19,
    KEY_F20,
    KEY_POWER,
    KEY_BRIGHTNESS_UP,
    KEY_BRIGHTNESS_DOWN,
    KEY_BLACKOUT,
    MAX_KEYS
} gvaKeyEnum;

typedef struct gvaInputType {
    char            name[10];
    gvaKeyEnum      key;
    int             binding;
    bool            active;
} gvaInput;

typedef struct {
    char            logFile[256];
    char            device[256];
    int             boardId;
} gvaXml;

#define DEBUG 1
#define XML_FILENAME "config.xml"

static WINDOW  *root_win;
static int      fd;
static int      no_splash = 0;
static gvaInput gva_keys[MAX_KEYS] = {
    { "SA ", KEY_SA, '1', false },
    { "WPN", KEY_WPN, '2', false },
    { "DEF", KEY_DEF, '3', false },
    { "SYS", KEY_SYS, '4', false },
    { "DRV", KEY_DRV, '5', false },
    { "STR", KEY_STR, '6', false },
    { "COM", KEY_COM, '7', false },
    { "BMS", KEY_BMS, '8', false },
    { "F1 ", KEY_F1, '1', false },
    { "F2 ", KEY_F2, '2', false },
    { "F3 ", KEY_F3, '3', false },
    { "F4 ", KEY_F4, '4', false },
    { "F5 ", KEY_F5, '5', false },
    { "F6 ", KEY_F6, '6', false },
    { "F7 ", KEY_F7, '7', false },
    { "F8 ", KEY_F8, '8', false },
    { "F9 ", KEY_F9, '9', false },
    { "F10", KEY_F10, '0', false },
    { "F11", KEY_F11, 'q', false },
    { "F12", KEY_F12, 'w', false },
    { "F12", KEY_F13, 'e', false },
    { "F14", KEY_F14, 'r', false },
    { "F15", KEY_F15, 't', false },
    { "F16", KEY_F16, 'y', false },
    { "F17", KEY_F17, 'u', false },
    { "F18", KEY_F18, 'i', false },
    { "F19", KEY_F19, 'o', false },
    { "F20", KEY_F20, 'p', false },
    { "POWER", KEY_POWER, 'x', false },
    { "UP", KEY_BRIGHTNESS_UP, '+', false },
    { "DOWN", KEY_BRIGHTNESS_DOWN, '-', false },
    { "BO", KEY_BLACKOUT, 'b', false }
 };

static gvaXml   xml = { 0 };
char            gvaConfigFile[200] = XML_FILENAME;
const char      xmlContent[4096] = { 0 };
FILE            *errorfd = 0;
void            winPanicBox(char *str);

void gvaLog(char* message, int type);

void
xmlparseKeyword(xmlDocPtr doc, xmlNodePtr cur, xmlChar * keyword, char *value)
{

    unsigned char           *key;
    cur = cur->xmlChildrenNode;
    while (cur != NULL) {
        if ((!xmlStrcmp(cur->name, (const xmlChar *) keyword))) {
            key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
            strcpy(value, (char*)key);
            xmlFree(key);
        }
        cur = cur->next;
    }
    return;
}

void
xmlParse(xmlDocPtr * doc, char *parent, xmlChar * keyword, char *value)
{
    xmlNodePtr      cur;

    cur = xmlDocGetRootElement(*doc);
    cur = cur->xmlChildrenNode;
    while (cur != NULL) {
        if ((!xmlStrcmp(cur->name, (const xmlChar *) parent))) {
            xmlparseKeyword(*doc, cur, keyword, value);
        }

        cur = cur->next;
    }
}

int
xmlStreamFile(const char *filename)
{
    xmlDocPtr       doc;
    xmlNodePtr      cur;
    char            tmp[100] = {0};
    int             ret; 

    doc = xmlParseFile(filename);
    cur = xmlDocGetRootElement(doc);

    if (cur == NULL) {
        fprintf(stderr, "empty document\n");
        xmlFreeDoc(doc);
        return -1;
    }

    if (xmlStrcmp(cur->name, (const xmlChar *) "gva")) {
        fprintf(stderr, "document of the wrong type, root node != gva\n\n");
        xmlFreeDoc(doc);
        return -1;
    }

    xmlParse(&doc, "tool", (xmlChar *)"LogFile", tmp);
    strcpy(xml.logFile, tmp);
    xmlParse(&doc, "tool",  (xmlChar *)"Device", tmp);
    strcpy(xml.device, tmp);

    doc = xmlParseFile(filename);
    xmlParse(&doc, "board",  (xmlChar *)"Id", tmp);
    sscanf(tmp, "%d", &xml.boardId);

    return 0;
}

int
xmlReader(const char *content, int length)
{
    int             ret;

    ret = xmlStreamFile(gvaConfigFile);

    /*
     * Cleanup function for the XML library.
     */
    xmlCleanupParser();

    return ret;
}

WINDOW         *
winCreate(int height, int width, int starty, int startx)
{
    WINDOW         *local_win;

    local_win = newwin(height, width, starty, startx);
    box(local_win, 0, 0);       /* 0, 0 gives default characters for the vertical and horizontal
                                 * lines */
    wrefresh(local_win);        /* Show that box */

    return local_win;
}

void
winDestroy(WINDOW * local_win)
{
    wclear(local_win);
    wrefresh(local_win);
    delwin(local_win);
}

void
winPrintInMiddle(WINDOW * win, char *string)
{
    int             length,
                    row,
                    col,
                    x;
    float           temp;

    if (win == NULL)
        win = stdscr;

    getmaxyx(win, row, col);
    length = strlen(string);

    x = (col - length) / 2;

    mvwprintw(win, 0, x, "%s", string);
    wrefresh(win);
}

void
winPopup(nmessage message, int opt)
{
    WINDOW         *splash_win;
    int             startx,
                    starty,
                    width,
                    height,
                    x;
    char            header[100];

    width = 40;
    switch (opt) {
    case MESSAGE_INFO:
        strcpy(header, "Information:");
		for (x = 0; x < message.lines; x++) {
			gvaLog(message.line[x], LOG_INFO);
		}
        break;
    case MESSAGE_WARNING:
        strcpy(header, "Warning:");
        gvaLog(message.line[0], LOG_WARNING);
        break;
    case MESSAGE_ERROR:
        strcpy(header, "Critical Error:");
        gvaLog(message.line[0], LOG_ERROR);
        break;
    }
    height = message.lines + 4;

    starty = (LINES - height) / 2;      /* Calculating for a center placement */
    startx = (COLS - width) / 2;        /* of the window */
    splash_win = winCreate(height, width, starty, startx);

    mvwprintw(splash_win, 0, 2, header);

    for (x = 0; x < message.lines; x++) {
        mvwprintw(splash_win, 2 + x, 2, message.line[x]);
    }
    wrefresh(splash_win);       /* Print it on to the real screen */
    wgetch(splash_win);         /* Wait for user input */

    winDestroy(splash_win);
    endwin();
    if (opt == 3) {
		gvaLog("GVA tool finalized.", LOG_INFO);
		close(fd);
		endwin();                   /* End curses mode */
		if (errorfd) fclose(errorfd); /* End logging */ 
        exit(-1);
    }
    wrefresh(root_win);
}

void
winPanicBox(char *str)
{
    nmessage        message;
    message.lines = 1;
    strcpy(message.line[0], str);
    winPopup(message, MESSAGE_ERROR);
    gvaLog(str, LOG_ERROR);
}

void
winMessageBox(nmessage str)
{
    winPopup(str, MESSAGE_INFO);
}

//
// This function will display important information about the qva-tool setup
// @todo Take add parameters to configuration 
// @body Add parameters to and .xml file for more flexability
//
void
gvaInfo()
{
    char            tmp[100];
    nmessage        m;

    sprintf(tmp, "GVA info:");
    strcpy(m.line[0], tmp);
    sprintf(tmp, "    GPS Device : %s", DEFAULT_GPS_DEVICE_NAME);
    strcpy(m.line[1], tmp);
    m.lines = 3;
    winMessageBox(m);
}


void
winBezelRedraw(WINDOW *win_bezel, gvaInput * keys, int activekey)
{
  int             c = 0;
  int             line = 2;
  int             column = 0;
  int             column_offset = 10;
  char            keytext[10] = "F10";

  wattron(win_bezel, COLOR_PAIR(4));

  /* Top of screen */
  for (c = KEY_SA; c <= KEY_BLACKOUT; c++) {
    strcpy(keytext, keys[c].name);
    (c==activekey) ? wattron(win_bezel, COLOR_PAIR(4)) : wattron(win_bezel, COLOR_PAIR(1));
    mvwprintw(win_bezel, line, column_offset + column, keytext);
    column += 7;
    if (c==KEY_BMS) { column = 0; line +=2; } /* Left of screen */
    if (c==KEY_F6) { column = 0; line +=2; } /* Right of screen */
    if (c==KEY_F12) { column = 0; line +=2; } /* Botton of screen */
    if (c==KEY_F20) { column = 0; line +=2; } /* Botton of screen */
  }
	wattroff(win_bezel, COLOR_PAIR(1));
  mvwprintw(win_bezel, 12, 3, "Press 'ENTER' to send input.");
  mvwprintw(win_bezel, 13, 3, "Press 'q' to return to the help screen.");
}

void
winBezel(gvaInput * keys, int activekey)
{
  bool done = false;
  WINDOW         *win_bezel;
  int             startx,
                  starty,
                  width,
                  height;

  height = LINES;
  width = COLS;
  starty = 0;
  startx = 0;
  win_bezel = winCreate(height, width, starty, startx);
  mvwprintw(win_bezel, 0, 3, "GVA Tool (Bezel)");
  winBezelRedraw(win_bezel, keys, activekey);

  while (!done) {
    int c = wgetch(win_bezel);

    switch (c) {
      case '0':
          winBezelRedraw(win_bezel, keys, 0);
          activekey = 0;
          break;
      case '1':
          winBezelRedraw(win_bezel, keys, 1);
          activekey = 1;
          break;
      case '2':
          winBezelRedraw(win_bezel, keys, 2);
          activekey = 2;
          break;
      case '3':
          winBezelRedraw(win_bezel, keys, 3);
          activekey = 3;
          break;
      case '4':
          winBezelRedraw(win_bezel, keys, 4);
          activekey = 4;
          break;
      case '5':
          winBezelRedraw(win_bezel, keys, 5);
          activekey = 5;
          break;
      case '6':
          winBezelRedraw(win_bezel, keys, 6);
          activekey = 6;
          break;
      case '7':
          winBezelRedraw(win_bezel, keys, 7);
          activekey = 7;
          break;
      case '8':
          winBezelRedraw(win_bezel, keys, 8);
          activekey = 8;
          break;
      case '9':
          winBezelRedraw(win_bezel, keys, 9);
          activekey = 9;
          break;
      case '>':
      case KEY_RIGHT:
          (activekey<KEY_BLACKOUT) ? activekey+=1 : 0;
          winBezelRedraw(win_bezel, keys, activekey);
          break;
      case '<':
      case KEY_LEFT:
          (activekey>KEY_SA) ? activekey-=1 : 0;
          winBezelRedraw(win_bezel, keys, activekey);
          break;
      case 'Q':
      case 'q':
          done=true;
          break;
      default:
          break;
      }
    wrefresh(win_bezel);
  }
  winDestroy(win_bezel);
}

void
winSplash()
{
    WINDOW         *splash_win;
    int             startx,
                    starty,
                    width,
                    height;

    height = 8;
    width = 50;
    starty = (LINES - height) / 2;      /* Calculating for a center placement */
    startx = (COLS - width) / 2;        /* of the window */
    splash_win = winCreate(height, width, starty, startx);

    mvwprintw(splash_win, 0, 2, "GVA Tool");
    mvwprintw(splash_win, 2, 2, "Tools for testing GVA using SNMP and DDS");
    mvwprintw(splash_win, 3, 2, "Includes HUMS Inputs, GPS and sensor setup.");
    mvwprintw(splash_win, 5, 2, "Author: ross@rossnewman..com");
    wrefresh(splash_win);       /* Print it on to the real screen */
    wgetch(splash_win);         /* Wait for user input */

    winDestroy(splash_win);
}

void
winHelp()
{
  bool done = false;
  WINDOW         *help_win;
  int             startx,
                  starty,
                  width,
                  height;
  height = LINES;
  width = COLS;
  starty = 0;
  startx = 0;
  wattron(help_win, COLOR_PAIR(1));
  help_win = winCreate(height, width, starty, startx);
  mvwprintw(help_win, 0, 3, "GVA Tool (Help)");
  wattron(help_win, COLOR_PAIR(3));    
  mvwprintw(help_win, 3, 5, " _____ _____ _____    _____         _ ");
  mvwprintw(help_win, 4, 5, "|   __|  |  |  _  |  |_   _|___ ___| |");
  mvwprintw(help_win, 5, 5, "|  |  |  |  |     |    | | | . | . | |");
  mvwprintw(help_win, 6, 5, "|_____|\\___/|__|__|    |_| |___|___|_|");
  wattron(help_win, COLOR_PAIR(1));
  mvwprintw(help_win, 5, 44, "h - Help");
  mvwprintw(help_win, 6, 44, "Press 'q' to quit application.");
  mvwprintw(help_win, 10, 3, "Use these keys to switch to different helper screens:");
  mvwprintw(help_win, 11, 3, "  b - Bezel key inputs     a - Alarm generator");
  mvwprintw(help_win, 12, 3, "  g - GPS generation       ");

  while (!done) {
    int key=0;
    int c = wgetch(help_win);

    switch (c) {
    case 'b':
        winBezel(gva_keys, 0);
        break;
    case 'q':
    case 'Q':
        done = true;
        break;
    case 'i':
    case 'I':
        gvaInfo();
        break;
    default:
        break;
    }
    redrawwin(help_win);
    mvwprintw(help_win, 0, 3, "GVA Tool (Help)");
  }
  winDestroy(help_win);
}

void
gvaLog(char* message, int type)
{
	struct sysinfo info;
	char msgType[4] = "???";

	sysinfo(&info);
	switch(type) {
		case LOG_DEBUG :
			strcpy(msgType, "DBG");
			break;
		case LOG_INFO:
			strcpy(msgType, "INF");
			break;
		case LOG_WARNING:
			strcpy(msgType, "WAR");
			break;
		case LOG_ERROR:
			strcpy(msgType, "ERR");
			break;
	}

#if !DEBUG
	/** Discard debug message if DEBUG not enabled */
	if (type == LOG_DEBUG) return;
#endif	
	if (!errorfd)
	{
		errorfd=fopen(xml.logFile, "w");
	}
	fprintf(errorfd, "[%ld] *%s* %s\n", info.uptime, msgType, message);
	fflush(errorfd);
}

int
gvaOptions(int argc, char **argv)
{
    int             vflag = 0;
    int             index;
    int             c;
    opterr = 0;

    while ((c = getopt(argc, argv, "d::h::v::n::f::")) != -1)
        switch (c) {
        case 'h':
            printf
                ("Options:\n\t-h = help\n\t-d = device (default '/dev/ttyUSB0')\n\t-n = no splash\n\t-f = configuration file\n\t-x = User Vector channel numbers\n\t-v = version\n\n");
            return -1;
        case 'v':
            printf("Version %d.%d.%d\n\n", 0, 0, 1);
            return -1;
        case 'd':
	    // @todo Add support for USB GPS devices in gva-tool.
	    // @body GPC long/lat will be transmitted to GUI over DDS
            strcpy(xml.device, optarg); /* Overload the xml name */
            break;
        case 'n':
            no_splash = 1;
            break;
        case 'f':
            strcpy(gvaConfigFile, optarg);
            break;
        case '?':
            if (optopt == 'd')
                fprintf(stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint(optopt)) {
                fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                return -1;
            } else
                fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
            return -1;
        default:
            fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
            return -1;
        }

    for (index = optind; index < argc; index++) {
        printf("Non-option argument %s. Use -h for help.\n", argv[index]);
        return -1;
    }
    return 0;
}

/**
 * @brief Tool suite to generate various DDS packets.
 * This program simulates various devices that may be found on the 
 * network and also simulates some features of a vehicle.
 * 
 */
int
main(int argc, char **argv)
{
    int             ch,
                    i = 0;
    int             startx,
                    starty,
                    width,
                    height;
    char            tmp[100];
    nmessage        m = {0};
    int             status = 0;
    MEVENT event;
     
    strcpy(xml.device, DEFAULT_GPS_DEVICE_NAME);

    printf("%s version 0.1\n", APP_DESCRIPTION);

    if (gvaOptions(argc, argv) != 0)
        return -1;

    if (xmlReader(xmlContent, 6) != 0)
        return -1;

	  gvaLog("GVA tool application Started", LOG_INFO);

    initscr();                  /* Start curses mode */
  	clear();
    noecho();
    cbreak();                   /* Line buffering disabled, Pass on everty thing to me */
    keypad(stdscr, TRUE);       /* I need that nifty F1 */

    if (has_colors() == FALSE) {
        endwin();
        printf("Your terminal does not support color\n");
    } else {
        start_color();
        init_pair(1, COLOR_WHITE, COLOR_BLACK);
        init_pair(2, COLOR_YELLOW, COLOR_BLACK);
        init_pair(3, COLOR_GREEN, COLOR_BLACK);
        init_pair(4, COLOR_BLACK, COLOR_WHITE);
        init_pair(5, COLOR_BLACK, COLOR_WHITE);
    }

    /*
     * Setup the GPS board 
     */
    fd = open(xml.device, O_RDWR);
    if (fd < 0) {
#if 0
        sprintf(m.line[0], "Error opening device %s", xml.device);
        m.lines = 1;
        winMessageBox(m);
#endif
    }

    /*
     * Hardware OK setup console 
     */
    height = LINES;
    width = COLS;
    starty = 0;
    startx = 0;
    root_win = winCreate(height, width, starty, startx);
    winPrintInMiddle(root_win, "Press 'q' quit, 'i' info, 's' screen");

#if 0
    if (!no_splash) {
        winSplash();
    }
#endif

//    wtimeout(root_win, 1);
    noecho();
    cbreak();	//Line buffering disabled. pass on everything
      
    winHelp();

    gvaLog("GVA finalized.", LOG_INFO);
    close(fd);
    endwin();                   /* End curses mode */
    if (errorfd) fclose(errorfd); /* End logging */ 

    return 0;
}
