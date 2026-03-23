#pragma once



#include "CGuiRightFadeNotifyDialog.h"

class CGuiLogin;
class CGuiKeyBoardDlg;
class CGuiMain;
class CGuiChatOutput;
class CGuiMessageBox;
class CGuiSystem;
class CGuiSelectCharDialog;
class CGuiCreateCharDialog;
//class CGuiRegisterDialog;
//class CGuiModifyPasswd;
class CGuiHelp;
class CGuiItem;
class CGameGuiFloatItem;
class CGuiInputBox;
class CGuiUserInfo;
class CGuiPrivateChat;
class CGuiNpcDialog;
class CGuiTrade;
class CGuiNpcTrade;
class CGuiSalaryTrade;
class CGuiTicketTrade; //积分配置存储标识
class CGuiChongzhiTrade; //充值点配置存储标识
class CGuiSavingBoxDialog;
class CGuiMiniMap;
class CGuiTeam;
class CGuiSkill;
class CGuiFloatControl;
class CGuiFloatAccessKey;
class CGuiOtherAccessKey;
class CGuiShortcutItem;
//class CGuiEquip;
class CGuiWorldConn;
class CGuiWorldBoard;
class CGuiChatSelect;
class CGuiItemDetailInfo;

class CGuiTaskDialog;
class CGuiFace;
class CGuiSysChat;

class CGuiOthersShopDialog;
class CGuiShopDialog;
class CGuiNpcMake;
class CGuiColorDialog;
class CMakeGuess;
class CGuiReliveDlg;
class CGuiPetCtrlDialog;
class CGuiSelectMarryTypeDlg;
class CGuiSelectSeptDareTypeDlg;

class CGuiLageMap;
class CGuiBallot;
class CGuiCharacterProperty;
class CGuiHairStyleDialog;
class CGuiHeadListDialog;
class CGuiClothColorDialog;
class CGuiPetDialog;
class CGuiPrivateChatSelect;
class CGuiSaveBoxContinueDialog;
class CNationalMapDialog;
class CGuiSendMailDialog;
class CGuiRecvMailDialog;

class CGuiGiveGiftDialog;

//小游戏相关
class CMiniGameListDlg;
class CMGMoneyInputDlg;
class CMGNumberDlg;
//推荐人系统相关
class CGuiRecoSys_A_InputDlg;
class CGuiRecoSys_B_Query_TakeDlg;
class CGuiRecoSys_A_Query_TakeDlg;
//师徒系统
class CGuiMasterBountyDlg;
class CGuiPrenticeBountyDlg;

class CGuiAuctionDialog;
class CGuiPetPackDialog;
class CGuiInputBox2Dialog;

class CGuiPackage;
class CGuiMakeTo;
class CGuiChatSelectButton;
class CGuiChatInvateDialog;
class CGuiCompetition;

class CGuiPasswordDlg;
class CGuiModifyPasswordDlg;
class CGuiEmotionDialog;
class CGuiChangeCountry;
class CGuiChangeCountrySalary;
class CGuiChangeWarField;
class CGuiCountryWarQuery;
class CGuiUnoinPower;

class CGuiGmMsgBoxDialog;
class CGuiSelectDialog;
class CGuiKingSelectDialog;

class CGuiAuctionMoneyInputDialog;
class CGuiAuctionGoldCoinInputDialog;
class CGuiGoldExchangeDialog;
class CGuiStockDialog;

class CGuiModifyRevenueBox;

class CGuiCountrySavingBoxDlg;
class CGuiAutoReplySettingDlg;
class CGuiAdoptCartoonDialog;
class CGuiCartoonAddCentDialog;
class CGuiAddMemberDialog;
class CGuiNameMoneyInputBoxDialog;

class CGuiCountryTechDlg;
class CGuiCountry;

class CGuiClearPointDlg;
class CGuiApplyConn;
class CGuiExitGameDlg;
class CGuiSafeGameDlg;
class CGuiTiaoGameDlg;
class CGuiAddSalaryDlg;
class CGuiQuestGameDlg;
class CGuiLoginGameDlg;
class CGuiQuestVipGameDlg;
class CGuiBoardGameDlg;
class CGuiGoodHelperDlg;
class CGuiAddNearByChat;

class CGuiInputBoxFiveDialog;
class CGuiUserListDialog;

class CGuiTopDialog;
class CGuiHalofFameTopDialog;
class CGuiTaiMiaoTopDialog;
class CGuiAutoAttack; ///////////////star100515

class CChatChannel;
class CRoleItem;
class CDlgProgressBar;
class CDlg2ProgressBar;
class CGuiFriendLevelup;//ZH
class CGuiCongratulateInfo;//ZH
class CGuiBossInfoDlg;
class CGuiCowrybox;

class CDlgEquipRecast; //装备助手
class CDlgEquipAnnexpack; //附件助手
class CDlgEquiphandbook; //soke 百兽图鉴
class CDlgEquipbookRecast; //soke 百兽图鉴助手
class CDlgEquipFabaoRecast; //soke 法宝助手
class CDlgEquipYuanshenRecast; //soke 元神助手
class CDlgMall;  //商城新
//经脉
class CGuijingmaiDlg;
//剑冢
class CGuijianzhongDlg;
//生肖
class CGuishengxiaoDlg;
//圣器
class CGuishengqiDlg;
//征途传
class CGuizhengtuzhuanDlg;
//功勋竞猜
class CGuigongxunDlg;
//理财
class CGuiLicaiDlg;
//头衔
class CGuiTouxianDlg;
//称号
class CGuiChenghaoDlg;
//副本
class CGuiFubenDlg;
//儿女
class CGuiBabyDlg;
//充值
class CGuichongzhiDlg;
//天下第一
class CGuiTianxiaDlg;
//自行车
class CGuiBikeDlg;
//云天别墅
class CGuiBieShuDlg;
//回收系统
class CGuiHuishouDlg;
//新手引导
class CGuiXinshouDlg;
//每日任务
class CGuiMeiriDlg;
//自动使用
class CGuiAutoUseDlg;
//能力修炼
class CGuiXiulianDlg;
//修仙系统
class CGuiXiuxianDlg;
//封印系统
class CGuiFengyinDlg;
//装备转换
class CGuiZhuanhuanDlg;
//---------
class CGuiMagicBoxDlg; //魔盒
//后门面板
class CGuiMianbanDlg;
//战车列表
class CGuiZhanchelistDlg;
//战车控制台
class CGuiZhancheDlg;
//战车小面板
class CGuiZhancheCmdDlg;
//家族BOSS
class CGuiJiazubossDlg;
//坐骑图鉴
class CGuiZuoqiDlg;
enum PROGRESSBAR_TYPE;
enum PROGRESSBAR2_TYPE;
/**
* \brief 简短描述
* 
* 详细描述
* 
*/
class CGuiMoneyInputBox;

//#include "../gui/include/guiTypes.h"

enum enumGuiActionType{
	eAction_Message,
	eAction_ExitGame,
	eAction_Logout,
	eAction_SplitItem,
	eAction_PrivateChat,
	eAction_RequestTrade,		// 请求交易
	eAction_SelfRequestTrade,	// 确认是否真要与别人交易
	eAction_BuyItem,			// 买物品
	eAction_SalaryBuyItem,		//soke 工资买物品
	eAction_AnswerTeam_Normal,		// 回答组队请求
	eAction_AnswerTeam_Honor,
	eAction_ExitTeam,			// 离开队伍
	eAction_DelUser,			// 删除角色
	eAction_World,				//社会关系
	eAction_Tong,				//帮会邀请
	eAction_TongUser,				//玩家要求加入帮会邀请
	eAction_Sept,				//家族邀请
	eAction_SeptUser,				//玩家要求加入家族邀请
	eAction_Friend,				//好友邀请
	eAction_Teacher,			//师徒邀请
	eAction_CancelTask,			//取消任务
	eAction_EnterShopSaleNum,	//输入销售数量(shop)
	eAction_EnterShopSalePrice,	//输入销售价格(shop)
	eAction_TongBoard,			//帮会公告
	eAction_SeptBoard,			//家族公告
	eAction_SchoolBoard,		//门派公告
	eAction_FriendBoard,		//好友公告
	eAction_ExitTong,			//离开帮会
	eAction_ExitSept,			//离开家族
	eAction_ExitSchool,			//离开门派
	eAction_DelTong,			//开除成员帮会
	eAction_DelSept,			//开除成员家族
	eAction_DelSchool,			//开除成员门派
	eAction_DelFriend,			//解除好友
	eAction_DelBad,				//解除黑名单
	eAction_ConnectLoginFailed,	// 连接登陆服务器失败
	eAction_BuyShopItem,		//购买别人摊位的物品
	eAction_CloseShop,			//关闭摊位
	eAction_MarryTask,			//结婚任务
	eAction_Color_Default,		//默认返回颜色行为(对colorDialog的行为定义eAction_Color_***规范)
	eAction_MoneyRelive,		//银子复活
	eAction_MaxeyRelive,		//soke 原地银子复活
	eAction_HorseSellConfirm,	//Horse sell confirm message box
	eAction_Color_SystemSetting,//系统设置
	eAction_TongAttack,			//帮会战斗
	eAction_SeptAttack,			//家族战斗
	eAction_SchoolAttack,	    //门派战斗
	eAction_SeptAttackNpc,       //家族争夺NPC战斗
	eAction_LeaveMarry,       //离婚
	eAction_TongExplain,	    //帮会说明
	eAction_SeptExplain,	    //家族说明
	eAction_DelItemConfirm,		//Give up item
	eAction_RemoveHorseConfirm,//确认弃马
	eAction_TongChangeName, //改变帮众称号
	eAction_SeptChangeName, //改变家族称号
	eAction_UseClearPoint,  // 清除属性点 || 清除技能点 
	eAction_SkillUp,	//升级技能确认
	eAction_Bind,		// 卖绑定物品
	eAction_MaxSkillLevel,	//Already up to max skill level
	eAction_SkillUpFail,	//Can not study skill

	eAction_TongBallot_OK,			//帮会投票
	eAction_TongBallot_CANCEL,		//帮会弃票
	eAction_SeptBallot_OK,			//家族投票
	eAction_SeptBallot_CANCEL,		//家族弃票

	eAction_Competition,		//全国竞赛
	eAction_CompetitionNpc,		//NPC答题
	eAction_HonorToPk,		//NPC答题
	eAction_StartShop,			//开始摆摊,输入广告语
	eAction_InputMailSendMoney,	//邮件送出银子
	eAction_InputMailSendGoldCoin,	//邮件送出银子
	eAction_ConfirmReceiveMailItem,	//确认收取邮件附件
	eAction_ConfirmStartAuction,	//确认开始拍卖
	eAction_ConfirmCannelAuction,	//确认取消拍卖

	eAction_LeaveCountry,		//离开国家
	eAction_AddCountry,		    //加入国家（金子）
	eAction_AddCountrySalary,   //加入国家（工资）
	eAction_SendCountryChat,       // 用世界频道发言
	eAction_BuyMonthCard,			//购买月卡包月
	eAction_GMBox,
	eAction_Recommend,             // 填写推荐人姓名 
	eAction_CreateGen,            // 成为将军
	eAction_LeaveArmy,             // 离开军队
	eAction_StopOneTalk,             // 国王禁止某人说话

	eAction_ExitGameByUnlaw,        // 发现非法程序，自动关闭客户端

	eAction_TakeCountryWar,         // 参加国战

	eAction_BecomeResearcher,       // 成为科技研究员
	eAction_GiveUnoinMoney,	       //帮会资金捐献


	eAction_ChangeArmyName,	       //更改军队名称
	eAction_RecruitArmy,           // 招收队长
#ifdef __SPRING__
	eAction_SendBlessMessage,		//输入祝福语
#endif
	eAction_AskSeptExp,				//请求每日经验值
	eAction_ConsignCartoon,			//指定宠物领养者
	eAction_AddFriendMember,		//增加好友
	eAction_AddSeptMember,			//增加家族成员
	eAction_AddTongMember,			//增加帮会成员	
	eAction_AddSchoolMember,		//招收徒弟

	eAction_EmperorRequDailyMoney,	//皇帝请求每日奖励

	eAction_KingRequDailyMoney,	//soke 国王请求每日奖励
	eAction_YuanshuaiRequDailyMoney, //soke 元帅请求每日奖励
	eAction_ZaixiangRequDailyMoney,  //soke 宰相请求每日奖励
	eAction_CatcherRequDailyMoney,   //soke 捕头（左）请求每日奖励
	eAction_CensorRequDailyMoney,  //soke 御史（左）请求每日奖励
	eAction_CatchexRequDailyMoney,   //soke 捕头（右）请求每日奖励
	eAction_CensosRequDailyMoney,  //soke 御史（右）请求每日奖励

	//
	eAction_SeptGiveupOccupy,		//族长放弃当前占领
	eAction_UnionGiveupOccupy,		//帮主放弃当前占领
	//
	eAction_ConfirmMarriage,		//是否答应结婚请求
	eAction_ConfirmMarryMoney,		//确认结婚需要缴纳的钱

	//
	eAction_ConfirmSeptDareType,	//确认家族挑战赌声望
	eAction_InputBribePrisonMoney,	//输入贿赂银两界面
	eAction_InputBailPrison,		//输入取保就医信息

	eAction_ConfirmAllyCountry,    // 确认国家结盟
	eAction_CancelAllyCountry,  // 解除国家结盟

	eAction_ChangeDisplaySettings,//重新设置用户的桌面分辨率

	eAction_PrisonCountry,		  //关玩家
	eAction_NpcDlgInput,			//从npcdialog发出的打开输入对话框

	eAction_MiniGame_MoneyInput,	//小游戏投注 moneyinputdlg
	eAction_MiniGame_Invite,		//小游戏邀请 messagebox
	eAction_MiniGame_Start,			//小游戏开始
	eAction_MiniGame_Continue,		//小游戏继续 messagebox
	eAction_MiniGame_GameOver,		//结束
	//Call duty
	eAction_SelectCallDuty,
	eAction_ExitLordStrikeLobby,             // 退出斗地主画面
	//eAction_ExitLordStrikeRoom,
	eAction_ExitLordStrikeGame,
	eAction_ExitLordStrikeGameIng,
	eAction_ExitLordStrikeLobbyAndGame,
	eAction_ExitLordStrikeGameFromRoom,

	eAction_AddSalary,   //soke  领取工资
	eAction_AddLogin,   //soke  领取登陆奖励
	eAction_AddVip,   //soke  领取vip奖励
	eAction_OpenSafety,
	eAction_Add_Property,      //soke 加属性弹框
	eAction_Fubenjhxt,	//副本激活系统
	eAction_RenameEquip,  //sky 一键换装命名
	eAction_Chibang,  //sky 翅膀
	eAction_Hanbing,  //sky 寒冰
	eAction_Chibang2,  //sky 翅膀
	eAction_Hanbing2,  //sky 寒冰
	eAction_Lua_InputBox,
	eAction_TicketBuyItem,		//soke 积分买物品
	eAction_ChongzhiBuyItem,	//soke 充值点买物品
	eAction_GotoMap,    //桃子地图1定时传送
	eAction_GotoMap2,    //桃子地图2定时传送
	eAction_biaoche1,    //国家镖车传送
	eAction_biaoche2,    //国家镖车传送
	eAction_FabaoShow,            //展示法宝
	eAction_DelBike,            //自行车退出
	eAction_Xiulian,            //能力修炼
	eAction_boss,    //世界boss
	eAction_bike,    //自行车

};	

enum enumPasswordType
{
	ePassword_SavingBox,		//储物箱密码
};

#define GUI_IDOK		1
#define GUI_IDCANCEL	2

extern const char* c_szGuiPack;
extern const char* c_szMBOK;
extern const char* c_szMBOKCancel;
extern const char* c_szMBYesNo;

/**
* \brief 简短描述
* 
* 详细描述
* 
*/
class CGameGuiManager
{
	bool m_bInitMainCharacterInfo;
public:
	bool IsPackgetFull();
	void InitMainCharacterInfo();

	CGameGuiManager(void);
	~CGameGuiManager(void);

	//typedef std::vector<CGuiEquip*> tListEquip;
	//tListEquip m_aGuiEquip;

	void Init();
	void Release();

	CGuiLogin* AddGuiLogin();
	CGuiKeyBoardDlg * AddKeyBoardDlg();
	CGuiMain*  AddMain();
	CGuiChatOutput* AddChatOutput();
	CGuiSystem* AddSystem();
	CGuiMessageBox* AddMessageBox(const char* pszText,enumGuiActionType type = eAction_Message,void* pData = NULL,const char* pszButtons = c_szMBOK,bool bModal = true);
	CGuiChatInvateDialog*	AddChatInvate(void* pData = NULL);
	CGuiChatInvateDialog*	AddTongInvate(void* pData = NULL);
	CGuiChatInvateDialog*	AddSeptInvate(void* pData = NULL);
	CGuiSelectCharDialog*	AddSelectCharDialog();
	CGuiCreateCharDialog*	AddCreateCharDialog();
	CGuiItem*				AddItem();
	CGuiHelp*				AddHelp(const char *pszHelpFileName = NULL);
	CGuiEmotionDialog*		AddEmotionDlg();
	CGuiSavingBoxDialog*	AddSavingBoxDlg();

	CGameGuiFloatItem*	  AddFloatItem();
	CGuiInputBox*		  AddInputBox(const char* pszText,enumGuiActionType type,void* pData = NULL );
	CGuiMoneyInputBox*	  AddMoneyInputBox(const char* pszText,enumGuiActionType type,void* pData = NULL );

	CGuiUserInfo*		  AddUserInfo(int nTABId = -1);
	CGuiUserInfo*		  AddUserInfoShowDlg();
	CGuiNpcDialog*		  AddNpcDialog();    //桃子对话页面
	CGuiNpcDialog*	      AddSalaryDialog(); //soke 工资配置
	CGuiNpcDialog*	      AddTicketDialog(); //soke 积分配置
	CGuiNpcDialog*	      AddChongzhiDialog(); //soke 充值点配置
	CGuiTrade*			  AddTrade();
	CGuiNpcTrade*		  AddNpcTrade(DWORD dwRepair);
	CGuiSalaryTrade*	  AddSalaryTrade(); //soke 工资
	CGuiTicketTrade*	  AddTicketTrade(); //soke 积分
	CGuiChongzhiTrade*	  AddChongzhiTrade(); //soke 充值点

	void				  EndNpcDialog();
	void				  CloseAllModalDialog();
	void AddServerSystemMessage(const char* pszMsg,DWORD color = COLOR_ARGB(255,255,0,0));
    void AddClientZiseMessage(const char* pszMsg,DWORD color = COLOR_ARGB(255,0,255,0));
	void AddClientSystemMessage(const char* pszMsg,DWORD color = COLOR_ARGB(255,255,0,0));
	void AddClientChatMessage(const char* pszMsg,DWORD color = COLOR_ARGB(255,255,0,0));
	void AddClientTaskMessage(const char* pszMsg,DWORD color = COLOR_ARGB(255,255,250,0));    //soke 特殊信息,获得经验、物品，在人物头上（黄）
	void AddClientSysScrollMessage(const char * pszMsg,DWORD color =  COLOR_ARGB(255,255,250,0));
	//soke 增加中间滚动颜色（黄、绿、紫、橙、、桔红、粉红、深红）
	void AddHsMessage(const char * pszMsg,DWORD color =  COLOR_ARGB(255,255,255,0));
	void AddLsMessage(const char * pszMsg,DWORD color =  COLOR_ARGB(255,255,255,0));
	void AddZsMessage(const char * pszMsg,DWORD color =  COLOR_ARGB(255,255,255,0));
    void AddCsMessage(const char * pszMsg,DWORD color =  COLOR_ARGB(255,255,255,0));
    void AddFhMessage(const char * pszMsg,DWORD color =  COLOR_ARGB(255,255,255,0));
	void AddFsMessage(const char * pszMsg,DWORD color =  COLOR_ARGB(255,255,255,0));
	void AddShMessage(const char * pszMsg,DWORD color =  COLOR_ARGB(255,255,255,0));
	void AddZfMessage(const char * pszMsg,DWORD color =  COLOR_ARGB(255,255,255,0));
	void AddTyMessage(const char * pszMsg,DWORD color);

	CGuiMiniMap*		  AddMiniMap();
	CGuiMiniMap*		  AddExitHelpGame();
	CGuiTeam*			  AddTeam();
	CGuiSkill*		  AddSkill(bool bShow = true);
	CGuiWorldBoard*		 AddWorldBoard(const char* pszCaption,const char* pszTitle,const char* pszText,
		enumGuiActionType type,bool bEdit,void* pData = NULL);

	CGuiWorldConn*       AddWorldConn(CChatChannel* pChannel = NULL);
	CGuiWorldConn*       AddNearby(CChatChannel* pChannel = NULL);
	CGuiFloatControl*	 AddFloatControl();
	CGuiFloatAccessKey*	 AddFloatAccessKey();
	CGuiOtherAccessKey*	 AddOtherAccessKey();
	CGuiShortcutItem*	 AddShortcutItem();
	CGuiChatSelect*		 AddChatSelect(bool bShow = true);
	CGuiItemDetailInfo*		AddItemDetailInfo();
	CGuiSysChat*          AddSysChat();

	CGuiPrivateChatSelect*  AddPrivateChatSelect();

	CGuiFace*           AddFace();
	CGuiChatSelectButton*  AddChatButton();
	CGuiNpcMake*        AddNpcMake(byte eState);
	CMakeGuess*        AddMakeGuess(stPointI pt);
	void				OnNpcActionEnd();
	CGuiCompetition*    AddCompetition(byte eState);


	CGuiLageMap*	AddLageMap();
	CGuiBallot*     AddBallot(enumGuiActionType Action);
	CGuiCharacterProperty*		AddCharacterProperty(bool bShow = true);
	CGuiPackage*    AddPackage1(CRoleItem* pItem);
	CGuiPackage*    AddPackage2(CRoleItem* pItem);
	CGuiPackage*    AddPackage(stObjectLocation pos,CRoleItem* pItem,bool bShow =false);
	CGuiMakeTo*	    AddMakeTo2(CRoleItem* pItem);
	CGuiMakeTo*	    AddMakeTo1(CRoleItem* pItem);
	CGuiMakeTo*	    AddMakeTo(stObjectLocation pos,CRoleItem* pItem,bool bShow =false);
	CGuiChangeCountry *	 AddChangeCountryDialog();
	CGuiChangeCountrySalary *	 AddChangeCountrySalaryDialog();
	CGuiChangeWarField * AddChangeWarFieldDialog();
	CGuiChangeWarField * AddChoiceWarFieldDialog();
	CGuiCountryWarQuery * AddCountryWarQueryDialog();
	CGuiUnoinPower*      AddUnoinPowerDialog(char* name);
	CGuiInputBoxFiveDialog*	AddInputBoxFiveDialog(const char* strTitle);

	void InitCreate();

	CGuiChatInvateDialog* m_guiInvite;
	CGuiLageMap * m_guiLageMap;
	CGuiLogin* m_guiLogin;
	CGuiKeyBoardDlg * m_guiKeyBoard;
	CGuiMain*  m_guiMain;
	CGuiChatOutput* m_guiChatOutput;
	CGuiSystem* m_guiSystem;
	CGuiSelectCharDialog* m_guiSelectChar;
	CGuiCreateCharDialog* m_guiCreateCharDialog;
	CGuiItem*			  m_guiItem;  
	CGuiHelp*              m_guiHelp;
	CGuiEmotionDialog *    m_guiEmotionDlg;
	CGuiSavingBoxDialog*	m_guiSavingBoxDlg;
	CGameGuiFloatItem*	  m_guiFloatItem;
	CGuiUserInfo*		  m_guiUserInfo;
	CGuiUserInfo*        m_guiUserInfoShow;
	CGuiNpcDialog*		  m_guiNpcDialog;
	CGuiTrade*			  m_guiTrade;
	CGuiNpcTrade*		  m_guiNpcTrade;
	CGuiSalaryTrade*	  m_guiSalaryTrade;  //soke 增加工资
	CGuiTicketTrade*	  m_guiTicketTrade;  //soke 增加积分
	CGuiChongzhiTrade*	  m_guiChongzhiTrade;  //soke 增加充值点
	CGuiMiniMap*		  m_guiMiniMap;
	CGuiTeam*			  m_guiTeam;
	CGuiSkill*			 m_guiSkill;
	CGuiWorldConn*       m_guiWorldConn;
	CGuiWorldConn*       m_guiNearby;
	CGuiFloatControl*	 m_guiFloatControl;
	CGuiFloatAccessKey*	 m_guiFloatAcessKey;
	CGuiOtherAccessKey*	 m_guiOtherAccessKey;
	CGuiShortcutItem*	 m_guiShortcutItem;
	CGuiChatSelect*		 m_guiChatSelect;
	CGuiItemDetailInfo*		m_guiItemDetailInfo;
	CGuiFace*           m_guiFace;
	CGuiNpcMake*        m_guiNpcMake;
	CMakeGuess*         m_guiMakeGuess;
	CGuiSysChat*        m_guiSysChat;
	CGuiBallot*         m_guiBallot;
	CGuiCharacterProperty*		m_guiCharacterProperty;
	CGuiPrivateChatSelect*     m_guiPrivateChatSelect;

	CGuiPackage*              m_guiPackage1;
	CGuiPackage*              m_guiPackage2;
	CGuiMakeTo*               m_guiMakeTo1;
	CGuiMakeTo*               m_guiMakeTo2;
	CGuiChatSelectButton*      m_guiChatButton;
	CGuiCompetition*          m_guiCompetition;
	CGuiChangeCountry*        m_guiChangeCountry;
	CGuiChangeCountrySalary*  m_guiChangeCountrySalary;
	CGuiChangeWarField*        m_guiChangeWarField;
	CGuiChangeWarField*        m_guiChoiceWarField;
	CGuiCountryWarQuery*       m_guiCountryWarQuery;
	CGuiUnoinPower*            m_guiUnoinPower;
	//ZH 
	CGuiFriendLevelup*     m_guiFriendLevelup;// 好友祝贺窗口
	CGuiCongratulateInfo*  m_guiCongratulateInfo;//祝贺信息窗口
	//////////////////////////////////////////////////////////////////////////
	

	///任务对话框
	CGuiTaskDialog *	 m_guitaskdialog;
	CGuiTaskDialog *	 AddTaskDialog();

	///换发型、发色对话框
	CGuiHairStyleDialog *m_guiHairStyleDlg;
	CGuiHairStyleDialog *AddHairDlg();

	///换服装颜色对话框
	CGuiClothColorDialog *m_guiClothColorDlg;
	CGuiClothColorDialog *AddClothColorDlg();

	///换头像
	CGuiHeadListDialog *m_guiHeadListDlg;
	CGuiHeadListDialog *AddHeadListDlg();

	CGuiBossInfoDlg*   m_guiBossInfoDlg;
	CGuiBossInfoDlg*   AddBossInfoDlg();	

	///收件箱
	CGuiRecvMailDialog* m_guiRecvMailDlg;
	CGuiRecvMailDialog* AddRecvMailDlg(bool bShow = true);

	///发件箱
	CGuiSendMailDialog* m_guiSendMailDlg;
	CGuiSendMailDialog* AddSendMailDlg(bool bShow = true);
////////////////////////////star100515   自动挂机
	CGuiAutoAttack*     m_guiAutoAttackDlg;
	CGuiAutoAttack*     AddAutoAttackDlg(bool bShow = true);
////////////////////////////end

	///礼品发放对话框
	CGuiGiveGiftDialog* m_guiGiveGiftDlg;
	CGuiGiveGiftDialog* AddGiveGiftDlg(bool bShow = true);

	//推荐人系统对话框(A推荐B)
	
	CGuiRecoSys_A_InputDlg* m_guiRecoSys_A_InputDlg;
	CGuiRecoSys_A_InputDlg* AddRecoSys_A_InputDlg(bool bShow = true);
	CGuiRecoSys_B_Query_TakeDlg* m_guiRecoSys_B_Query_TakeDlg;
	CGuiRecoSys_B_Query_TakeDlg* AddRecoSys_B_Query_TakeDlg(bool bShow = true);
	CGuiRecoSys_A_Query_TakeDlg* m_guiRecoSys_A_Query_TakeDlg;
	CGuiRecoSys_A_Query_TakeDlg* AddRecoSys_A_Query_TakeDlg(bool bShow = true);

	//师徒系统
	CGuiMasterBountyDlg* m_guiMasterBountyDlg;
	CGuiMasterBountyDlg* AddMasterBountyDlg(bool bShow = true);
	CGuiPrenticeBountyDlg* m_guiPrenticeBountyDlg;
	CGuiPrenticeBountyDlg* AddPrenticeBountyDlg(bool bShow = true);

	//add by wangy for MiniGame 
	CMiniGameListDlg* m_MiniGameListDlg;
	CMiniGameListDlg* AddMiniGameListDlg(bool bShow = true);
	//小游戏moneyinputdlg
	CMGMoneyInputDlg* m_MGMoneyInputDlg;
	CMGMoneyInputDlg* AddMGMoneyInputDlg(bool bShow = true);
	//比数字小游戏
	CMGNumberDlg* m_MGNumberDlg;
	CMGNumberDlg* AddMGNumberDlg(enumGuiActionType type,void* pData = NULL);

	//ZH
	CGuiFriendLevelup*	AddFriendLevelup();
	CGuiCongratulateInfo* AddCongratulateInfo();
	//////////////////////////////////////////////////////////////////////////
	
	///
	CGuiNameMoneyInputBoxDialog*	m_guiNameMoneyInputDlg;
	CGuiNameMoneyInputBoxDialog*	AddNameMoneyInputDlg(const char* pszText1,const char* pszText2,enumGuiActionType type,void* pData = NULL);

	CGuiSelectDialog* m_guiSelectDialog;
	CGuiSelectDialog* AddSelectDlg(bool bShow = true);

	CGuiKingSelectDialog* m_guiKingSelectDialog;
	CGuiKingSelectDialog* AddKingSelectDlg(bool bShow = true);

	CGuiAuctionDialog* m_guiAuctionDlg;
	CGuiAuctionDialog* AddAuctionDlg();

	CGuiPetPackDialog* m_guiPetPackDlg;
	CGuiPetPackDialog* AddPetPackDlg();

	// 2010 4
	CGuiTopDialog* m_guiTopDialog;
	CGuiTopDialog* AddTopDialog();

	CGuiTaiMiaoTopDialog* m_guiTaiMiaoTopDialog;
	CGuiTaiMiaoTopDialog* AddTaiMiaoTopDialog();

	
	//sky 重要信息公告
	CGuiRightFadeNotifyDialog* m_guiRightFadeNotifyDialog;
	CGuiRightFadeNotifyDialog* AddRightFadeNotifyDialog();

	//自动回复自定义设置
	CGuiAutoReplySettingDlg*	m_guiAutoReply;
	CGuiAutoReplySettingDlg*	AddAutoReply(CGuiMenu* pMenu = NULL);

	///GM 留言
    CGuiGmMsgBoxDialog *m_guiGmMsgBoxDialog; 
	CGuiGmMsgBoxDialog *AddGmMsgBoxDlg();

	///摆摊对话框
	CGuiShopDialog*		m_guiShop;
	CGuiShopDialog*		AddShop(bool bShow = true);

	///别人的摊位
	CGuiOthersShopDialog*		m_guiOthersShop;
	CGuiOthersShopDialog*		AddOthersShop(bool bShow = true);

	///颜色对话框
	CGuiColorDialog*	AddColorDialog(enumGuiActionType Action = eAction_Color_Default
		,DWORD color = COLOR_ARGB(255,255,255,255),void *pData = NULL);
	
	///重生对话框
	CGuiReliveDlg *m_guiReliveDlg;
	CGuiReliveDlg *AddReliveDlg(DWORD dwTempUserID,DWORD dwDeadType = 0);

	///选择婚礼类型对话框
	CGuiSelectMarryTypeDlg* m_guiSelectMarryTypeDlg;
	CGuiSelectMarryTypeDlg* AddSelectMarryTypeDlg();

	///选择家族挑战类型对话框
	CGuiSelectSeptDareTypeDlg* m_guiSelectSeptDareTypeDlg;
	CGuiSelectSeptDareTypeDlg* AddSelectSeptDareTypeDlg();

	///宠物对话框
	CGuiPetDialog*		m_guiPetDlg;
	CGuiPetDialog*		AddPetDialog();

	///宠物控制界面
	CGuiPetCtrlDialog*  m_guiPetFloatDlg;
	CGuiPetCtrlDialog*  AddPetFloatDialog(petType type);

	CGuiAuctionMoneyInputDialog*	m_guiAucMoneyInputDlg;
	CGuiAuctionMoneyInputDialog*	AddAucMoneyInputDlg(DWORD dwAuctionId,int type);

	CGuiAuctionGoldCoinInputDialog*	m_guiAucGodlCoinInputDlg;
	CGuiAuctionGoldCoinInputDialog*	AddAucGoldCoinInputDlg(DWORD dwAuctionId);

	CGuiGoldExchangeDialog*	m_guiGoldExchangeDlg;
	CGuiGoldExchangeDialog* AddGoldExchangeDlg();

	CGuiAdoptCartoonDialog* m_guiAdoptCartoonDlg;
	CGuiAdoptCartoonDialog* AddAdoptCartoonPetDialog();

	CGuiCartoonAddCentDialog* m_guiCartoonAddCentDlg;
	CGuiCartoonAddCentDialog* AddCartoonAddCentDialog(DWORD dwCartoonID);

	CGuiInputBox2Dialog*	m_guiInputBox2Dlg;
	CGuiInputBox2Dialog*	AddInputBox2(const char* pszText1,const char* pszText2,enumGuiActionType type,void* pData = NULL);

	//增加成员对话框
	CGuiAddMemberDialog*	m_guiAddMemberDlg;
	CGuiAddMemberDialog*	AddAddMemberDialog(enumGuiActionType type,const char* title);

	//股票对话框
	CGuiStockDialog*		m_guiStockDlg;
	CGuiStockDialog*		AddStockDialog();

	///召唤兽控制界面
	//CGuiPetCtrlDialog*  m_guiSummonFloatDlg;
	//CGuiPetCtrlDialog*  AddSummonFloatDialog();

	///仓库继续租赁输入天数界面
	CGuiSaveBoxContinueDialog* m_guiSaveBoxContinueDlg;
	CGuiSaveBoxContinueDialog* AddSaveBoxContinueDlg(BYTE box);

	///玩家列表
	CGuiUserListDialog*		m_pUserListDlg;
	CGuiUserListDialog*		AddUserListDialog(CChatChannel* pChannel);

	///国家地图
	CNationalMapDialog* m_guiNationalMap;
	CNationalMapDialog* AddNationalMap();

	CGuiPasswordDlg*	m_guiPasswordDlg;
	CGuiPasswordDlg*	AddPasswordDlg(enumPasswordType type,const char* pszLabel,void* pData = NULL);

	CGuiModifyPasswordDlg*	m_guiModifyPasswordDlg;
	CGuiModifyPasswordDlg*	AddModifyPasswordDlg(enumPasswordType type,const char* pszLabel1,const char* pszLabel2,void* pData = NULL);


	CGuiModifyRevenueBox * m_guiModifyRevenueBox;
	CGuiModifyRevenueBox * AddModifyRevenueBox();


	CGuiCountrySavingBoxDlg* m_guiCountrySaveingBoxDlg;
	CGuiCountrySavingBoxDlg* AddCountrySaveingBoxDlg();

	CGuiCountryTechDlg   *  m_guiCountryTechDlg;
	CGuiCountryTechDlg   * AddCountryTechDlg();

	CGuiCountry          * m_guiCountry;
	CGuiCountry          * AddCountryDlg();

	CGuiClearPointDlg      * m_guiClearPoint;
	CGuiClearPointDlg  * AddClearPointDlg();

	CGuiApplyConn*      m_guiApplyConn;
	CGuiApplyConn*      AddApplyConn();

	///小退菜单
    CGuiExitGameDlg*   m_guiExitGame;
	CGuiExitGameDlg*   AddExitGame();

	///财产保护菜单
	CGuiSafeGameDlg*   m_guiSafeGame;
	CGuiSafeGameDlg*   AddSafeGame();

	///跳舞系统菜单
	CGuiTiaoGameDlg*   m_guiTiaoGame;
	CGuiTiaoGameDlg*   AddTiaoGame();

	///领取工资菜单
	CGuiAddSalaryDlg*   m_guiAddSalary;
	CGuiAddSalaryDlg*   AddSalary();

	///每日任务系统
	CGuiQuestGameDlg*   m_guiQuestGame;
	CGuiQuestGameDlg*   AddQuestGame();

	///每日登陆系统
	CGuiLoginGameDlg*   m_guiLoginGame;
	CGuiLoginGameDlg*   AddLoginGame();

	CGuiHalofFameTopDialog* m_guiHalofFameTopDialog;
	CGuiHalofFameTopDialog* AddHalofFameTopDialog();


	///vip菜单系统
	CGuiQuestVipGameDlg*   m_guiQuestVipGame;
	CGuiQuestVipGameDlg*   AddQuestVipGame();

	///排行榜菜单
	CGuiBoardGameDlg*   m_guiBoardGame;
	CGuiBoardGameDlg*   AddBoardGame();
	CDlgProgressBar*  m_pDlgProgressBar;
	CDlgProgressBar*  AddDlgProgressBar(PROGRESSBAR_TYPE eType);

	//soke 更换太庙进度条
	CDlg2ProgressBar*  m_pDlg2ProgressBar;
	CDlg2ProgressBar*  AddDlg2ProgressBar(PROGRESSBAR2_TYPE eType);

    //soke 装备助手
	CDlgEquipRecast* m_pDlgEquipRecast;
	CDlgEquipRecast* AddDlgEquipRecast(bool bShow = true);

    //soke 附件助手
	CDlgEquipAnnexpack* m_pDlgEquipAnnexpack;
	CDlgEquipAnnexpack* AddDlgEquipAnnexpack(bool bShow = true);

	//soke 百兽图鉴助手 对话框
	CDlgEquipbookRecast* m_pDlgEquipbookRecast;
	CDlgEquipbookRecast* AddDlgEquipbookRecast(bool bShow = true);

	//soke 百兽图鉴 对话框
	CDlgEquiphandbook* m_pDlgEquiphandbook;
	CDlgEquiphandbook* AddDlgEquiphandbook();	

	//soke 法宝助手 对话框
	CDlgEquipFabaoRecast* m_pDlgEquipFabaoRecast;
	CDlgEquipFabaoRecast* AddDlgEquipFabaoRecast(bool bShow = true);

	//soke 元神助手 对话框
	CDlgEquipYuanshenRecast* m_pDlgEquipYuanshenRecast;
	CDlgEquipYuanshenRecast* AddDlgEquipYuanshenRecast(bool bShow = true);

	///sky 好帮手
	CGuiGoodHelperDlg*   m_guiGoodHelper;
	CGuiGoodHelperDlg*   AddGoodHelper();

	//宝箱
	CGuiCowrybox* m_guiCowrybox;
	CGuiCowrybox* AddCowrybox();

	//魔盒
	CGuiMagicBoxDlg* m_guiMagicBox;
	CGuiMagicBoxDlg* AddMagicBox();

   	//商城新 对话框
	CDlgMall*		m_pDlgMall;
	CDlgMall*		AddDlgMall();

	///经脉
    CGuijingmaiDlg*   m_guijingmai;
	CGuijingmaiDlg*   Addjingmai();

	///剑冢
    CGuijianzhongDlg*   m_guijianzhong;
	CGuijianzhongDlg*   Addjianzhong();

	///生肖
    CGuishengxiaoDlg*   m_guishengxiao;
	CGuishengxiaoDlg*   Addshengxiao();

	///圣器
    CGuishengqiDlg*   m_guishengqi;
	CGuishengqiDlg*   Addshengqi();

	///征途传
    CGuizhengtuzhuanDlg*   m_guizhengtuzhuan;
	CGuizhengtuzhuanDlg*   Addzhengtuzhuan();

	///功勋竞猜
    CGuigongxunDlg*   m_guigongxun;
	CGuigongxunDlg*   AddGongxun();

	///理财
    CGuiLicaiDlg*   m_guilicai;
	CGuiLicaiDlg*   AddLicai();

	///头衔
    CGuiTouxianDlg*   m_guiTouxian;
	CGuiTouxianDlg*   AddTouxian();

	///称号
    CGuiChenghaoDlg*   m_guiChenghao;
	CGuiChenghaoDlg*   AddChenghao();

	///副本
    CGuiFubenDlg*   m_guiFuben;
	CGuiFubenDlg*   AddFuben();

	///儿女
    CGuiBabyDlg*   m_guiBaby;
	CGuiBabyDlg*   AddBaby();

	///充值
    CGuichongzhiDlg*   m_guichongzhi;
	CGuichongzhiDlg*   Addchongzhi();

	///天下第一
    CGuiTianxiaDlg*   m_guiTianxia;
	CGuiTianxiaDlg*   AddTianxia();

	///自行车
    CGuiBikeDlg*   m_guiBike;
	CGuiBikeDlg*   AddBike();

	//新手引导
	CGuiXinshouDlg*   m_guiXinshou;
	CGuiXinshouDlg*   AddXinshou();
	
	//云天别墅
	CGuiBieShuDlg*   m_guiBieShu;
	CGuiBieShuDlg*   AddBieShu();

	//回收系统
	CGuiHuishouDlg*   m_guiHuishou;
	CGuiHuishouDlg*   AddHuishou();

	//野外首领

	//每日任务
	CGuiMeiriDlg*   m_guiMeiri;
	CGuiMeiriDlg*   AddMeiri();

	//自动使用
	CGuiAutoUseDlg*   m_guiAutoUse;
	CGuiAutoUseDlg*   AddAutoUse();

	//能力修炼
	CGuiXiulianDlg*   m_guiXiulian;
	CGuiXiulianDlg*   AddXiulian();

	//封印系统
	CGuiFengyinDlg*   m_guiFengyin;
	CGuiFengyinDlg*   AddFengyin();

	//官职系统

	//饷银商店

	//官阶奖励

	//战功排行榜

	//装备转换
	CGuiZhuanhuanDlg*   m_guiZhuanhuan;
	CGuiZhuanhuanDlg*   AddZhuanhuan();

	//绝世无双锻造

	//坐骑图鉴
	CGuiZuoqiDlg*   m_guiZuoqi;
	CGuiZuoqiDlg*   AddZuoqi();
	
	//修仙系统
	CGuiXiuxianDlg*   m_guiXiuxian;
	CGuiXiuxianDlg*   AddXiuxian();

	//后门面板
	CGuiMianbanDlg*   m_guiMianban;
	CGuiMianbanDlg*   AddMianban();

	//战车列表
	CGuiZhanchelistDlg*   m_guiZhanchelist;
	CGuiZhanchelistDlg*   AddZhanchelist();

	//战车控制台
	CGuiZhancheDlg*   m_guiZhanche;
	CGuiZhancheDlg*   AddZhanche();

	//战车小面板
	CGuiZhancheCmdDlg*   m_guiZhancheCmd;
	CGuiZhancheCmdDlg*   AddZhancheCmd();

	//家族BOSS
	CGuiJiazubossDlg*   m_guiJiazuboss;
	CGuiJiazubossDlg*   AddJiazuboss();

	CGuiInputBoxFiveDialog*	m_guiInputBoxFiveDlg;

	void CloseAll1_15TypeWnd();
};

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
inline CGameGuiManager * GetGameGuiManager()
{
	extern CGameGuiManager g_GameGuiManager;
	return &g_GameGuiManager;
}

CGuiMessageBox* GameMessageBox(const char* pszText,enumGuiActionType type = eAction_Message,void* pData = NULL,const char* pszButtons = "确定|1|Enter|",bool bModal = true);

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
inline CGuiSystem* AddSystemDialog()
{
	return GetGameGuiManager()->AddSystem();
}

void OnOffItemDialog();
void ShowHelpDialog();
void OnOffNatioanalMapDialog();
void ShowEmotionDialog();
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述

inline CGuiModifyPasswd* AddModifyPasswdDialog()
{
	return GetGameGuiManager()->AddModifyPasswd();
}
 */
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述

inline CGuiRegisterDialog* AddRegisterDialog()
{
	return GetGameGuiManager()->AddRegisterDialog();
}
 */
/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
inline CGuiCreateCharDialog* AddCreateCharDialog()
{
	return GetGameGuiManager()->AddCreateCharDialog();
}


/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
inline CGuiUserInfo* AddUserInfoDialog()
{
	return GetGameGuiManager()->AddUserInfo();
}

/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \return 返回值的描述
 */
//soke 百兽图鉴
inline CDlgEquiphandbook* AddEquiphandbook()
{
	return GetGameGuiManager()->AddDlgEquiphandbook();
}


/**
 * \brief 简短描述
 * 
 * 详细描述
 * 
 * \param pszText : 描述
 * \return 返回值的描述
 */
inline void ShowMessage(const char* pszText)
{
	GameMessageBox(pszText);
}

inline void AddClientSystemMessageV(const char* szFmt,va_list argptr )
{
	char szString[1024];
	_vsnprintf(szString,1024,szFmt,argptr);
	szString[1023] = 0;
	GetGameGuiManager()->AddClientSystemMessage(szString);
}

/**
* \brief 简短描述
* 
* 详细描述
* 
* \param szFmt : 描述
* \return 返回值的描述
*/
inline void AddClientSystemMessageF(const char* szFmt,...)
{
	va_list args;
	va_start( args, szFmt );
	AddClientSystemMessageV(szFmt,args);
	va_end(args);
}

void AddClientMessageV(DWORD type,DWORD color,const char* szFmt,va_list argptr);
void AddClientMessageF(DWORD type,DWORD color,const char* szFmt,...);

extern void* g_pMoneyInputBox;
