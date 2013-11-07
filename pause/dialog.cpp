
#include "../nx.h"
#include "dialog.h"
#include "dialog.fdh"
#include "../l10n_strings.h"

using namespace Options;
extern FocusStack optionstack;

#ifdef _480X272 // 480x272 widescreen fix
#define DLG_X		((SCREEN_WIDTH / 2) - 110)
#define DLG_Y		((SCREEN_HEIGHT / 2) - 90)
#define DLG_W		240
#define DLG_H		180
#else
#define DLG_X		((SCREEN_WIDTH / 2) - 88)
#define DLG_Y		((SCREEN_HEIGHT / 2) - 90)
#define DLG_W		190
#define DLG_H		180
#endif

#define REPEAT_WAIT	30
#define REPEAT_RATE	4

Dialog::Dialog()
{
	onclear = NULL;
	ondismiss = NULL;
	
	fCoords.x = DLG_X;
	fCoords.y = DLG_Y;
	fCoords.w = DLG_W;
	fCoords.h = DLG_H;
	fTextX = (fCoords.x + 48);
	
	fCurSel = 0;
	fNumShown = 0;
	fRepeatTimer = 0;
	
	optionstack.AddItem(this);
}

Dialog::~Dialog()
{
	for(int i=0;ODItem *item = ItemAt(i);i++)
		delete item;
	
	optionstack.RemoveItem(this);
}

void Dialog::SetSize(int w, int h)
{
	fCoords.w = w;
	fCoords.h = h;
	fCoords.x = ((DLG_W / 2) - (w / 2)) + DLG_X;
	fCoords.y = ((DLG_H / 2) - (h / 2)) + DLG_Y;
	fTextX = (fCoords.x + 34);
}

void Dialog::offset(int xd, int yd)
{
	fCoords.x += xd;
	fCoords.y += yd;
	fTextX += xd;
}

/*
void c------------------------------() {}
*/

ODItem *Dialog::AddItem(const char *text, \
						void (*activate)(ODItem *, int), \
						void (*update)(ODItem *),\
						int id, int type)
{
	ODItem *item = new ODItem;
	memset(item, 0, sizeof(ODItem));
	
	strcpy(item->text, text);
	
	item->activate = activate;
	item->update = update;
	item->id = id;
	item->type = type;
	
	fItems.AddItem(item);
	
	if (update)
		(*update)(item);
	
	return item;
}

ODItem *Dialog::AddSeparator()
{
	return AddItem("", NULL, NULL, -1, OD_SEPARATOR);
}

ODItem *Dialog::AddDismissalItem(const char *text)
{
    if (!text) text = LC_RETURN;
	return AddItem(text, NULL, NULL, -1, OD_DISMISS);
}

/*
void c------------------------------() {}
*/

void Dialog::Draw()
{
	TextBox::DrawFrame(fCoords.x, fCoords.y, fCoords.w, fCoords.h);
	
	int x = fTextX;
	int y = (fCoords.y + 18);
	for(int i=0;;i++)
	{
		ODItem *item = (ODItem *)fItems.ItemAt(i);
		if (!item) break;
		
		if (i < fNumShown)
			DrawItem(x, y, item);
		
		if (i == fCurSel)
			draw_sprite(x - 16, y, SPR_WHIMSICAL_STAR, 1);
		
		y += GetFontHeight();
	}
	
	if (fNumShown < 99)
		fNumShown++;
}

void Dialog::DrawItem(int x, int y, ODItem *item)
{
char text[132];

	strcpy(text, item->text);
	strcat(text, item->suffix);
	
	// for replay times
	// comes first because it can trim the text on the left side if needed
	if (item->raligntext[0])
	{
		int rx = (fCoords.x + fCoords.w) - 10;
		rx -= GetFontWidth(item->raligntext, 5);
		font_draw(rx, y, item->raligntext, 5);
		
		// ... ellipses if too long
		int maxx = (rx - 4);
		//FillRect(maxx, 0, maxx, SCREEN_HEIGHT, 0,255,0);
		for(;;)
		{
			int wd = GetFontWidth(text, 0);
			if (x+wd < maxx) break;
			
			int len = strlen(text);
			if (len <= 3) { *text = 0; break; }
			
			text[len-1] = 0;
			text[len-2] = '.';
			text[len-3] = '.';
			text[len-4] = '.';
		}
	}
	
	font_draw(x, y, text, 0);
	
	// for key remaps
	if (item->righttext[0])
	{
		font_draw((fCoords.x + fCoords.w) - 62, y, item->righttext, 0);
	}
}


/*
void c------------------------------() {}
*/

void Dialog::RunInput()
{
	if (inputs[UPKEY] || inputs[DOWNKEY])
	{
		int dir = (inputs[DOWNKEY]) ? 1 : -1;
		
		if (!fRepeatTimer)
		{
			fRepeatTimer = (lastinputs[UPKEY] || lastinputs[DOWNKEY]) ? REPEAT_RATE : REPEAT_WAIT;
			sound(SND_MENU_MOVE);
			
			int nitems = fItems.CountItems();
			for(;;)
			{
				fCurSel += dir;
				if (fCurSel < 0) fCurSel = (nitems - 1);
							else fCurSel %= nitems;
				
				ODItem *item = ItemAt(fCurSel);
				if (item && item->type != OD_SEPARATOR) break;
			}
		}
		else fRepeatTimer--;
	}
	else fRepeatTimer = 0;
	
	
	if (buttonjustpushed() || justpushed(RIGHTKEY) || justpushed(LEFTKEY))
	{
		int dir = (!inputs[LEFTKEY] || buttonjustpushed() || justpushed(RIGHTKEY)) ? 1 : -1;
		
		ODItem *item = ItemAt(fCurSel);
		if (item)
		{
			if (item->type == OD_DISMISS)
			{
				if (dir > 0)
				{
					sound(SND_MENU_MOVE);
					if (ondismiss) (*ondismiss)();
					return;
				}
			}
			else if (item->activate)
			{
				(*item->activate)(item, dir);
				
				if (item->update)
					(*item->update)(item);
			}
		}
	}
	
	/*if (justpushed(ESCKEY))
	{
		Dismiss();
		return;
	}*/
}

void Dialog::SetSelection(int sel)
{
	if (sel < 0) sel = fItems.CountItems();
	if (sel >= fItems.CountItems()) sel = (fItems.CountItems() - 1);
	
	fCurSel = sel;
}


void Dialog::Dismiss()
{
	delete this;
}

void Dialog::Refresh()
{
	for(int i=0;ODItem *item = ItemAt(i);i++)
	{
		if (item->update)
			(*item->update)(item);
	}
}

void Dialog::Clear()
{
	if (onclear)
		(*onclear)();
	
	for(int i=0;ODItem *item = ItemAt(i);i++)
		delete item;
	
	fItems.MakeEmpty();
	fNumShown = 0;
	fCurSel = 0;
}




