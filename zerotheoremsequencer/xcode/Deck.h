

#define DECKDEVICE 0
//#define DECKVIDEOMODE 16
#define DECKVIDEOMODE 9
#define DECKWIDTH 1280
#define DECKSTRIDE (DECKWIDTH*2)
#define DECKHEIGHT 720
#define DECKBPP 4
#define DECKBYTES (DECKWIDTH*DECKHEIGHT*DECKBPP)
#define NDECKS 4

#define DECK_EMPTY  0
#define DECK_FULL   1
#define DECK_WRITER 2
#define DECK_READER 3

#define DECKRATIO 2

Surface* deckGetRead();
void deckReleaseRead();
