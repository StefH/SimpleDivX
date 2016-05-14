#ifndef _TOOL_H
#define _TOOL_H

#include "SimpleDivX_Defines.h"

typedef struct _tooltips_info
{
	int  i_page;
	int  i_attribute;
	char *s_text;
} TOOLTIPS_INFO;

class CMyToolTips : public CToolTipCtrl  
{
public:
	int PrepareTooltips(CWnd*, int i_page, CToolTipCtrl *m_pTooltip);
	CMyToolTips();
	virtual ~CMyToolTips();

};

#endif