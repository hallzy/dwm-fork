/* See LICENSE file for copyright and license details. */

#include <X11/XF86keysym.h>
#include "gaplessgrid.c"

#define GAP 6

#define VOL_MUTE_BTN     XF86XK_AudioMute
#define VOL_DOWN_BTN     XF86XK_AudioLowerVolume
#define VOL_UP_BTN       XF86XK_AudioRaiseVolume
#define BRIGHT_UP_BTN    XF86XK_MonBrightnessUp
#define BRIGHT_DOWN_BTN  XF86XK_MonBrightnessDown


// more readable mouse buttons
#define LEFT_CLICK    Button1
#define MIDDLE_CLICK  Button2
#define RIGHT_CLICK   Button3
#define SCROLL_UP     Button4
#define SCROLL_DOWN   Button5
#define SCROLL_LEFT   Button6
#define SCROLL_RIGHT  Button7
#define MOUSE_BACK    Button8
#define MOUSE_FORWARD Button9

#define BIN "/home/steven/Documents/git-repos/remote-github/dotfiles/.bin"

/* appearance */
static const unsigned int borderpx  = 6;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int gappih    = GAP;       /* horiz inner gap between windows */
static const unsigned int gappiv    = GAP;       /* vert inner gap between windows */
static const unsigned int gappoh    = GAP;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = GAP;       /* vert outer gap between windows and screen edge */
static const int smartgaps          = 1;        /* 1 means no outer gap when there is only one window */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = {
	"monospace:size=10",
	"Noto Color Emoji:pixelsize=12:antialias=true:autohint=true"
};
static const char dmenufont[]       = "monospace:size=10";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  col_cyan  },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Firefox",  NULL,       NULL,       1 << 8,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
	{ "HHH",      gaplessgrid },
	{ NULL,       NULL },
};

/* key definitions */
#define MODKEY Mod4Mask
#define ALTKEY Mod1Mask


// Super+num             ==> Change view to tag numbered num
// Super+shift+num       ==> Move current window to tag 'num'
// Super+ctrl+num        ==> Temporarily show all windows in tag 'num' in
//                           current tag
// Super+ctrl+shift+num  ==> Show the active window on tag 'num' as well
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ALTKEY,                KEY,      viewAllMon,     {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

#define STACKKEYS(MOD,ACTION) \
	{ MOD, XK_j,     ACTION##stack, {.i = INC(+1) } }, \
	{ MOD, XK_k,     ACTION##stack, {.i = INC(-1) } }, \
	{ MOD, XK_grave, ACTION##stack, {.i = PREVSEL } }, \
	{ MOD, XK_q,     ACTION##stack, {.i = 0 } }, \
	{ MOD, XK_a,     ACTION##stack, {.i = 1 } }, \
	{ MOD, XK_z,     ACTION##stack, {.i = 2 } }, \
	{ MOD, XK_x,     ACTION##stack, {.i = -1 } },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { BIN "/dmenu_shortcut", "-m", dmenumon, NULL };
static const char *termcmd[]  = { BIN "/st_tabbed", NULL };
static const char *wificmd[]  = { BIN "/wifi-connect", NULL };


static const char *mutecmd[]    = { BIN "/special_keys.sh", "voltoggle"  , NULL };
static const char *volupcmd[]   = { BIN "/special_keys.sh", "volup"      , NULL };
static const char *voldowncmd[] = { BIN "/special_keys.sh", "voldown"    , NULL };
static const char *brupcmd[]    = { BIN "/special_keys.sh", "brightup"   , NULL };
static const char *brdowncmd[]  = { BIN "/special_keys.sh", "brightdown" , NULL };

static const char *mouseleftcmd[]  = { BIN "/special_keys.sh", "mouseleft"  , NULL };
static const char *mouserightcmd[] = { BIN "/special_keys.sh", "mouseright" , NULL };
static const char *mousedowncmd[]  = { BIN "/special_keys.sh", "mousedown"  , NULL };
static const char *mouseupcmd[]    = { BIN "/special_keys.sh", "mouseup"    , NULL };

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	STACKKEYS(MODKEY,                          focus)
	STACKKEYS(MODKEY|ShiftMask,                push)
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.02} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.02} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY|ControlMask,           XK_comma,  cyclelayout,    {.i = -1 } },
	{ MODKEY|ControlMask,           XK_period, cyclelayout,    {.i = +1 } },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },

	// Cycle the monitors
	{ MODKEY,                       XK_comma,  focusmon,       {.i = +1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = -1 } },

	// Move window to previous/next monitor
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = -1 } },

	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_BackSpace,    quit,     {0} },
	{ 0,                            VOL_MUTE_BTN,    spawn,    {.v = mutecmd    } },
	{ 0,                            VOL_DOWN_BTN,    spawn,    {.v = voldowncmd } },
	{ 0,                            VOL_UP_BTN,      spawn,    {.v = volupcmd   } },
	{ 0,                            BRIGHT_UP_BTN,   spawn,    {.v = brupcmd    } },
	{ 0,                            BRIGHT_DOWN_BTN, spawn,    {.v = brdowncmd  } },

	{ ControlMask | ALTKEY,         XK_h,  spawn,              {.v = mouseleftcmd  } },
	{ ControlMask | ALTKEY,         XK_j,  spawn,              {.v = mousedowncmd  } },
	{ ControlMask | ALTKEY,         XK_k,  spawn,              {.v = mouseupcmd    } },
	{ ControlMask | ALTKEY,         XK_l,  spawn,              {.v = mouserightcmd } },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */

// ClkClientWin   ==> A window on the screen
// ClkLtSymbol    ==> Click Layout Symbol in Top Bar
// ClkRootWin     ==> Click monitor??
// ClkStatusText
// ClkTagBar      ==> Clicking one of the numbered tags
// ClkWinTitle    ==> Window Title

static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              LEFT_CLICK,     setlayout,      {0} },
	{ ClkLtSymbol,          0,              RIGHT_CLICK,    setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              MIDDLE_CLICK,   zoom,           {0} },
	{ ClkStatusText,        0,              MIDDLE_CLICK,   spawn,          {.v = termcmd } },
	{ ClkStatusText,        0,              LEFT_CLICK,     spawn,          {.v = wificmd } },
	{ ClkClientWin,         MODKEY,         LEFT_CLICK,     movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         MIDDLE_CLICK,   togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         RIGHT_CLICK,    resizemouse,    {0} },
	{ ClkTagBar,            0,              LEFT_CLICK,     view,           {0} },
	{ ClkTagBar,            0,              RIGHT_CLICK,    toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         LEFT_CLICK,     tag,            {0} },
	{ ClkTagBar,            MODKEY,         RIGHT_CLICK,    toggletag,      {0} },
};
