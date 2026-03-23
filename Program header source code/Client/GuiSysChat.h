#pragma once
#include "..\gui\include\guidialog.h"


/**
 * \brief ºÚ∂Ã√Ë ˆ
 * 
 * œÍœ∏√Ë ˆ
 * 
 */
class CGuiSysChat :
	public CGuiDialog
{
	struct stSysChat
	{
		char szChat[MAX_CHATINFO];
		float height;
		SIZE size;
		DWORD color;
		float time;
		stSysChat()
		{
			time = 0.0f;
			color = -1;
			height = 0;
		}
	};

	typedef std::vector<stSysChat> tListSysChat;
	tListSysChat	m_listSysChat;


	

public:
	CGuiSysChat(void);
	
	void OnClose(void);
	void RenderBackground(float fElapsedTime);
	HRESULT OnRender( float fElapsedTime ); 
	void Draw(float fElapsedTime);

	void InsertSysChat(const char* szChat,DWORD color);
};
