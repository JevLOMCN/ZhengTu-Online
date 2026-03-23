
/**
*\file		GuiNpcDialog.h
*\version	$Id$
*\author	$$$01公司
*\			$$$01公司@###.com
*\date		2006-7-19  21:11:00 中国标准时间
*\brief	    NPC对话框
*
* 
*
*/

#pragma once
#include "..\gui\include\guidialog.h"
#include "../xml_parse/XmlParseEx.h"
class CLuaState;
/**
 * \brief NPC对话框
 * 
 * NPC对话框
 * 
 */
class CGuiNpcDialog :
	public CGuiDialog
{
	///NPC的ID
	 DWORD			m_dwNpcId;
public:
	~CGuiNpcDialog();
	enum enumAction{
		eTrade,			// 买卖
		eTradeRepair,	// 买卖/修理
		eTradeRepairByGold,	// 买卖/修理（金子)
		eDialog,		// 对话
		eClose,			// 关闭
		eMake,			//打造
		eMakeFood,     //烹饪
		eChange,		//合成
		eUp,			//升级
		eKaChange,      //灵魂合成
		eKaMake,       //灵魂镶嵌
		eHoleMake,      //打孔
		eTong,			//帮会
		eSept,          //家族
		eSchool,        //门派
		eTask,
		eDestroyTong,			//帮会
		eDestroySept,         //家族
		eDestroySchool,       //门派
		eMarry,              //结婚
		eLeaveMarry,              //离婚
		eGiveGift,				//发放礼品
        eStore,             //仓库
		eTongBallot,             //帮会投票
		eSeptBallot,             //家族投票
		eHair,				//发型、发色
		eHead,                    // 头像
		eClothColor,		//服装颜色
		eBackout,             //装备拆除
		eTongAttack,         //帮会战
		eSeptAttack,         //家族战
		eSchoolAttack,         //师门战
		eScriptButton,		
		eBuySavingBox,		//买储物箱
		eMakeDrug,             //炼药
		eSaleHorse,		//卖马
		eCompetitionNpc, //NPC答题
		eCompetition,  //全国竞赛
		eSeptAttackNpc,         //家族争夺NPC战
		eSeptNpcCustom,          //家族获得NPC税金
		eTongAttackTown,         //帮会争夺城镇战
		eTongTownCustom,         //帮会获得城镇税金
		eEmperorRequestDailyMoney,	//皇帝每天可以在皇城大臣处领取5D金钱作为奖励
		eKingRequestDailyMoney,	//soke 国王每天可以在大臣处领取5D金子薪资
		eYuanshuaiRequestDailyMoney,	//soke 元帅每天可以在大臣处领取3D金子薪资
		eZaixiangRequestDailyMoney,	    //soke 宰相每天可以在大臣处领取3D金子薪资
		eCatcherRequestDailyMoney,	    //soke 捕头（左）每天可以在大臣处领取2D金子薪资
		eDiplomatRequestDailyMoney,	    //soke 御史（左）每天可以在大臣处领取2D金子薪资
		eCatchexRequestDailyMoney,	    //soke 捕头（右）每天可以在大臣处领取2D金子薪资
		eDiplomaxRequestDailyMoney,	    //soke 御史（右）每天可以在大臣处领取2D金子薪资

		// [Conglin]
		eRenascenceRequest,		// 转生
		eGraceChangeToExp,		// 文采值换经验
		eSeptMPBuff,			// 领取家族BUFF
		eEnterBattle,			// 传往战场		


		// [sky] 训马师
		eAddHorseTraining_one,		// 1级训练
		eAddHorseTraining_two,		// 2级训练
		eAddHorseTraining_three,	// 3级训练
		eClearHorseTraining,		// 清除训练
		// [sky] 大臣
		eExchangeExp,

		eChangeCountryBySalary,	    // 使用工资移民
		eAddSalary,		            // 领取工资
		eAddChoujiang,		        // 抽奖
		eSeptTownExp,			//家族占城获得经验
		eBuyItem,				//购买物品
		eHonorToPk,				//荣誉点换PK值
		eChangeCountry,         //更换国家
		eChangeWarField,         //更换战场
		eChoiceWarField,         //更换战场
		eCountryWarQuery,         //查询国战
		eWarOperator,         //查询国战

		eSendMail,				//发件箱
		eRecvMail,				//收件箱		
		eAuction,				//拍卖界面
		eGoldExchange,			//点数兑换金子
		eBuyMonthCard,			//包月
		eModifyRevenue,              // 调税
		eQueryRevenue,                // 查询税率
		eQueryCountrySaveBox,         // 查询国库
		eContributeSaveBox,           // 捐献到国库
		eCheckValidQuest,				//检查可以接的任务
		eRecommend,                  // 填写推荐人
		eTechResearcher,       //国王指定研究员
		eTechUpdate,           // 科技人员升级
		eTechBallot,           // 国名投票

		eCreateArmy,          // 建立军队
		eLeaveArmy,            // 离开军队
		eQueryArmy,            // 查询军队
		eRecruitArmyer,          // 招收队长

	    eArmymemLevel,          // 国王指定玩家等级
		eKingStopOneTalk,        // 国王禁止某人说话
		eBuyCartoonPetCmd,		//购买替身宠物
		eStock,      // 股票系统
		eGoldGive,		//领取奖品
		eDialogEx,		//带参数的函数
		eConfirmSaleCartoon,		//确认卖出替身宝宝
		eTranslateToMoreExp,      //传送到增值地宫
		eGiveUnionMoney,         //帮会资金捐献
		eQueryUnionMoney,        //帮会资金查询
		eApplyConn,              //申请加入帮会家族

		//////////////////////////////////////
		// 家族和帮会排行榜
		eSeptNativeSort,
		eSeptWorldSort,
		eUnionNativeSort,
		eUnionWorldSort,
#ifdef __SPRING__
		eSpringCmd,
#endif
		//////////////////////////////////////
		eQuestGoldGive,  // 请求宝珠个数

		//////////////////////////////////////
		eQuestDragonSprite,		//请求龙精
		eQuestTigerSprite,		//请求虎魄

		eSeptGiveupOccupy,		//族长放弃当前占领
		eUnionGiveupOccupy,		//帮主放弃当前占领

		eSelectMarriageType,	//
		eSendOutPrisonCmd,		//发送请求出狱消息
		eOpenBribeInputBox,		//打开输入贿赂银两界面
		eOpenBailInputBox,		//打开取保就医(帮别人洗PK值)输入界面

		eAntiDareCountry,     // 国战反攻
		eRequestGiftDetails,	//请求活动细节
		eRequestGiftItem,		//请求获得礼品
		eRequestWinnerList,		//请求获奖列表

		eRequestItemCardForGift,	//请求道具卡换取礼品

		eRequestAllyCountry,          //国王缔结盟约
		eCancelAllyCountry,           // 国王取缔盟约
		eRequestAllyCountryInfo,        //国王请求盟国信息

		eLeavePrison,					//请求出牢狱

		eDaojuGet,		//获取道具材料

		//Add By David For 2006-3-20
		eImmure,						//国王每天可以关一个一小时

		eQeustForGenInfo,
		eFubenjhGenInfo,
		eAppointMember,

		//add by wyc
		//显示最弱3国
		eShowCountryWeak3,

		//推荐人系统(A推荐B)
		eRecoSys_A_Input,		//输入推荐人姓名
		eRecoSys_B_Query_Take,	//被推荐人奖励查询和领取
		eRecoSys_A_Query_Take,	//推荐人奖金查询和领取

		//请求领取家族普通经验
		eRequestSeptNormalExp,

		//师徒关系
		eMasterBounty,     //做为师傅查询进贡
		ePrenticeBounty,   //做为徒弟查询进贡

		//soke
		eFetchPearl,// 取太庙砖
		ePutPearl,// 放太庙砖
		eOpenPearlRank, // 打开太庙排行榜
		eFetch2Pearl,//更换太庙砖
		eSeptGoToMale,          // 男温泉
		eSeptGoToFemale,        // 女温泉
		eSeptExitHotspring,     // 离开温泉
		eSalary,   //soke 工资买卖
		eTicket,   //soke 积分买卖
        eChongzhi,   //soke 充值点买卖

		
		eQuestOccupierList,
		//ZH
		eExchangeEquipment,//低级装备换高级
		eEquipmentLevelUp,//升级道具
		eEquipmentHole,//道具打孔
		eEquipmentName,//道具签名
		eFubenOpen,	   // 开启副本
		eFubenEnter,   // 进入副本
		eFubenLeave,   // 离开副本
		eJiaZuOpen,	   // 家族地图
		eJiaZuBoss,	   // 家族BOSS
		
		//soke 天工巧匠
		eModifyAttri,           // 改造属性
		eAddReBind ,			// 重新绑定
		eAddReMaker ,			// 修改签名
		eAddAddMaker ,			// 装备签名
		eAddModifySoulA ,		// 修改灵魂属性
		eAddLVSoullineMD,		// 保留灵魂锁链改造
		eAddModifyDxSoulA,		// 修改第二灵魂属性
		eAddMDSkillA,			// 修改技能属性
		eAddEquipRound,			// 非转生装备改为转生
		eAddMDMake,			    // 马匹装备重新锻造
		eAddEquipMake,			// 装备祝福改造
		eAddEquipZsMake,		// 马匹装备祝福改造
		eAddEquipMaklv,			// 装备升级改造

		eGongxunjingcai,//功勋竞猜
		eLicai,//理财
		eTianxia,//天下第一
		eBikeGame,//自行车
		eDelBikeGame,//自行车
		eNextBikeGame,//自行车


		eZhanCheList,//战车列表
		eMiGong,//迷宫
		eTianxiaLingqu,//天下第一


		
	};

	struct stAction{
		int actionid;
		CGuiImage*	image;					//image
		//CGuiButton* button;					// UI
		enumAction action;					// 事件
		std::vector<std::string> params;	// 参数

		stAction()
		{
			image = NULL;
			//button = NULL;
		}
	};

	struct stButtonInfo{
		std::string label;
		int id;
		int index;
		int soundindex;
		DWORD color;
	};
	///LUA脚本对象
	CLuaState* m_pNpcScript;
	///编辑框对象
	CGuiMLTextBox* m_guiTalk;
	///NPC的类别
	std::vector<stAction> m_actions;
	///说话的内容
	TiXmlDocument		m_xmlTalk;
	///编辑按钮
	std::vector<stButtonInfo> m_buttons;
	///当前触发的脚本ID
	int m_iActionId;
	///是否结束当前的NPC对话框
	bool m_bEndAction;
	///显示NPC交易对话框
	bool m_bShowTradeDlg;
	///NPC对话框与其他对话框绑定
	void* m_pMsgDlg;

	CClassFunMap<CGuiNpcDialog,int>	m_FunMapVoid;

public:
	CGuiNpcDialog(void);
	void UpdateClientRect();
	void OnClose(void);
	void OnCreate();
	void InitCustomElements();
	void SetNpcScript(const char* script);
	void ResetControlLocation();
	bool OnGuiEvent(UINT nEvent,UINT nID,CGuiControl* pControl);
	void OnNpcAction(int index);
	void OnActionEnd();
	void OnActionBegin();
	HRESULT OnRender(float fElapsedTime);
	void OnSetCloseLocation();
	void OnInitScript();

	void ShowTradeDlg(bool bShow = true) { m_bShowTradeDlg = bShow; }

	void ShowRevenue();
	void ShowCountrySaveBox();
	//---------------------------------------------
	// script functions
	//---------------------------------------------

	void BeginAddDialog();
	void EndAddDialog();
	void AddDialog(const char* pszFunction);
	void AddDialogEx(const char* pszFunction,const char* pszPara);
	void AddTalk(const char* pszt1);

	// [Conglin]
	int AddRenascenceDlg( const char* pzText );
	int AddGraceChangeToExp( const char* pzText );
	int AddChangeCountryBySalary( const char* pzText );
	int AddEnterBattle( const char* pText );
	int AddSeptMPBuff( const char* pText );


	//[sky]训马师
	int AddHorseTraining_one( const char* pzText );
	int AddHorseTraining_two( const char* pzText );
	int AddHorseTraining_three( const char* pzText );
	int ClearHorseTraining( const char* pzText );
	// [sky]大臣
	int ExchangeExp( const char* pzText );   //功勋换经验

	void AddTalkEx(const char* pszText,int npcID,int ObjID);
	int AddButtonItem(const char* pszLabel);
	int AddButtonItemEx(const char* pszLabel,DWORD dwColor);
	int AddTradeItem(const char* pszLabel,const char* pszBuyObjects,const char* pszSellObjects,const char* pszTalk);
	int AddManagerSalaryDlg(const char* pszLabel,const char* pszBuyObjects,const char* pszSellObjects,const char* pszTalk); //soke 工资购买
	int AddManagerTicketDlg(const char* pszLabel,const char* pszBuyObjects,const char* pszSellObjects,const char* pszTalk); //soke 积分点数购买
	int AddManagerChongzhiDlg(const char* pszLabel,const char* pszBuyObjects,const char* pszSellObjects,const char* pszTalk); //soke 充值点数购买

	int AddTradeRepairItem(const char* pszLabel,const char* pszBuyObjects,const char* pszSellObjects,const char* pszRepairObjects,const char* pszTalk);
	int AddTradeRepairItemByGold(const char* pszLabel,const char* pszBuyObjects,const char* pszSellObjects,const char* pszRepairObjects,const char* pszTalk);
	int AddGoodsStore(const char* pszLabel);
	int AddChangeItem(const char* pszLabel,const char* pszBuyObjects,const char* pszTalk);
	int AddMakeItem(const char* pszLabel,const char* pszBuyObjects,const char* pszTalk);
	int AddMakeFood(const char* pszLabel,const char* pszBuyObjects,const char* pszTalk);
	int AddMakeDrug(const char* pszLabel,const char* pszBuyObjects,const char* pszTalk);
	int AddUpItem(const char* pszLabel,const char* pszBuyObjects,const char* pszTalk);
	int AddKaChangeItem(const char* pszLabel,const char* pszBuyObjects,const char* pszTalk);
	int AddKaMakeItem(const char* pszLabel,const char* pszBuyObjects,const char* pszTalk);
	int AddHoleMakeItem(const char* pszLabel,const char* pszBuyObjects,const char* pszTalk);
	int AddDialogItem(const char* pszLabel,const char* pszFunction);
	int AddDialogItemEx(const char* pszLabel,const char* pszFunction,const char* pszPara); //正常副本
	int AddFubenOpen(const char* pszLabel, int type); //副本开启
	int AddFubenEnter(const char* pszLabel, int type); //副本进入
	int AddFubenLeave(const char* pszLabel); //副本退出
	int AddJiaZuOpen(const char* pszLabel); //家族地图
	int AddJiaZuBoss(const char* pszLabel); //家族BOSS
		
	int AddCloseItem(const char* pszLabel);
	int AddeSaleHorseCmd(const char* pszLabel);
	int AddeHonorToPk(const char* pszLabel);

	//改造属性
	int AddModifyAttri(const char* pszLabel,const char* pszBuyObjects,const char* pszTalk);
	//重新绑定
	int AddReBind(const char* pszLabel,const char* pszBuyObjects,const char* pszTalk);
	//修改签名
	int AddReMaker(const char* pszLabel,const char* pszBuyObjects,const char* pszTalk);
	//装备签名
	int AddAddMaker(const char* pszLabel,const char* pszBuyObjects,const char* pszTalk);
	//修改灵魂属性
	int AddModifySoulA(const char* pszLabel,const char* pszBuyObjects,const char* pszTalk);
	//保留灵魂锁链改造
	int AddLVSoullineMD(const char* pszLabel,const char* pszBuyObjects,const char* pszTalk);
	//修改第二灵魂属性
	int AddModifyDxSoulA(const char* pszLabel,const char* pszBuyObjects,const char* pszTalk);
	//修改技能属性
	int AddMDSkillA(const char* pszLabel,const char* pszBuyObjects,const char* pszTalk);
	//非转生装备改造成转生装备
	int AddEquipRound(const char* pszLabel,const char* pszBuyObjects,const char* pszTalk);
	//马匹装备重新锻造
	int OpenHorseEquipAttributeModify(const char* pszLabel,const char* pszBuyObjects,const char* pszTalk);
	//装备升级改造
	int AddEquipLV(const char* pszLabel,const char* pszBuyObjects,const char* pszTalk);

	int AddGiveUnionMoney(const char* pszLabel);
	int AddQueryUnionMoney(const char* pszLabel);
	int AddApplyConn(const char* pszLabel);

	int AddStock(const char* pszLabel);
	int AddAuctionDialog(const char* pszLabel);
	int AddSendMailDialog(const char* pszLabel);
	int AddRecvMailDialog(const char* pszLabel);
	int AddHeadListDialog(const char* pszLabel);
	int AddHairDialog(const char* pszLabel);
	int AddClothColorDialog(const char* pszLabel);
	int AddBackoutItem(const char* pszLabel,const char* pszBuyObjects,const char* pszTalk);
	
	int AddScriptButton(const char* pszLabel,const char* pszFunction);
	void CloseNpcDialog();

	int AddTaskCmdItem(const char* pszLabel,const char* pszCmd, int pszCmd1, int pszCmd2);
	int AddCountryStrategy(const char* pszLabel,const char* pszCmd, int pszCmd1, int pszCmd2);

	void SendTaskCmd(const char* pszTarget,int taskID,int selectID);

	int AddTongBallot(const char* pszLabel);
	int AddSeptBallot(const char* pszLabel);

	int AddSeptAttackNpc(const char* pszLabel);
	int AddSeptNpcCustom(const char* pszLabel);
	int AddTongAttackTown(const char* pszLabel, const char* operatorID);
	int AddTongTownCustom(const char* pszLabel);
	int AddSeptTownExp(const char* pszLable);
	int AddEmperorRequestDailyMoney(const char* pszLabel); //soke 皇帝领取每日薪资
	int AddKingRequestDailyMoney(const char* pszLabel); //soke 国王领取每日薪资
	int AddYsRequestDailyMoney(const char* pszLabel); //soke 元帅领取每日薪资
	int AddZxRequestDailyMoney(const char* pszLabel); //soke 宰相领取每日薪资
	int AddCrRequestDailyMoney(const char* pszLabel); //soke 捕头（左）领取每日薪资
	int AddDtRequestDailyMoney(const char* pszLabel); //soke 御史（左）领取每日薪资
	int AddCxRequestDailyMoney(const char* pszLabel); //soke 捕头（右）领取每日薪资
	int AddDxRequestDailyMoney(const char* pszLabel); //soke 御史（右）领取每日薪资

	int AddQuestDragonSprite(const char* pszLable);
	int AddQuestTigerSprite(const char* pszLable);

	int SeptGiveupOccupy(const char* pszLable);
	int UnionGiveupOccupy(const char* pszLable);

	int AddTong(const char* pszLabel);
	int AddSept(const char* pszLabel);
	int AddSchool(const char* pszLabel);
	int AddMarry(const char* pszLabel);
	int AddLeaveMarry(const char* pszLabel);
	int AddChangeCountry(const char* pszLabel);
	int AddChangeWarField(const char* pszLabel);
	int AddChoiceWarField(const char* pszLabel);
	int AddCountryWarQuery(const char* pszLabel);
	int	AddWarOperator(const char* pszLabel,const char* operatorID);

	int AddCompetitionNpc(const char* pszLabel);
	int AddGoldExchangeDialog(const char* pszLabel);
	int AddBuyMonthCardCmd(const char* pszLabel);


	int DestroyTong(const char* pszLabel);
	int DestroySept(const char* pszLabel);
	int DestroySchool(const char* pszLabel);
	
	void PlaySound(const char* pszFileName);

	bool IsAttackPop(int nState);
	int AddLaunchAttack(const char* pszLabel,int nState);
	
	void Action(int id);
	
	int  GetHorseID();
	void ResetHorseOwnStatus();
	bool IsHasHorse();
	bool IsCanBuyFirstSavingBox();
	bool IsCanBuy2ndSavingBox();
	bool IsCanBuy3rdSavingBox();
	bool IsHaveNewMail();

	int SellHorse();
	int SetSavingBoxPassword();

	void ResetSavingBoxOwnNum();
	int  GetSavingBoxNum();
	int  GetRealSavingBoxNum();
	void AddBuySavingBoxCmd(const char* pszLabel);
	void AddBuyItemCmd(const char* pszLabel,int iItemID,int iItemnum);
	int  GetCartoonPetNum();
	void AddBuyCartoonPetCmd(const char* pszLabel,int npcID);

	void AddSelectMarriageTypeCmd(const char* pszLable,int type);

	int  QuerySeptAndUnionSort(const char * pszLabel,int type);

	void AddSaleCartoonPetCmd(int cartoonindex,const char* pszLabel);
	void ConfirmSaleCartoonPet(const char* pszLabel,const char* pszCartoonID);
	void ShowSavingBox();
	int AddCheckValidQuestCmd(const char* pszLabel);

	void EndDialog() { m_bEndAction = true;SetVisible(false);}

	bool IsCanClose();		//右上角的关闭按钮是否可以起作用
	bool IsMoveCanClose();	//人物移动是否可以关闭界面
	
	int AddGoldGiveCmd(const char* pszLabel);
	int AddModifyRevenue(const char* pszLabel);
	int AddQueryRevenue(const char * pszLabel);
	int AddCountryBoxGuard(const char* pszLabel);
	int AddCountryRevenue(const char* pszLabel);
	int AddRecommend(const char* pszLabel);
	int AddTechResearcher(const char * pszLabel);
	int AddTechUpdate(const char * pszLabel);
	int AddTechBallot(const char * pszLabel);

	int CreateArmy(const char * pszLabel);
	int LeaveArmy(const char * pszLabel);
	int QueryArmy(const char * pszLabel);
	int RecruitArmyer(const char *pszLabel);

	int ArmymenLevel(const char * pszLabel,int nLevel);
	int KingStopOneTalk(const char * pszLabel);

	// 传送
	int TranslateToMoreExp(const char * pszLabel);

	int AntiDareCountry(const char * pszLabel);

	void  AddQuestGoldGiveNum(const char * pszLabel);

	int AddSendOutPrisonCmd(const char* pszLabel);
	int AddOpenBribeInputBox(const char* pszLabel);
	int AddOpenBailPersonInputBox(const char* pszLabel);

	int	 RequestGiftList();
	void AddGiftListByCmd(stListGift* pCmd);
	void AddGiftDetailByCmd(stDetailGift* pCmd);
	void AddWinnerListByCmd(stWinnerGift* pCmd);

	int AddRequestItemCardForGift(const char* pszLable);


	int AddRequestAllyCountry(const char* pszLable);
	int AddCancelAllyCountry(const char * pszLable);
	int AddRequestAllyInfo(const char* pszLable);

	//soke
	int RecvTMTask( const char* pszLable );
	int ReTTMTask( const char* pszLable );
	int AddViewTMListBoard( const char* pszLabel );
	int ExcRecvTMTask( const char* pszLable );

	int AddGoToMaleHotspringItem( const char* pzText );
	int AddGoToFemaleHotspringItem( const char* pzText );
	int AddExitHotspringItem( const char* pzText );

	int AddLeavePrisonCmd(const char* pszLable);

	int AddDaojuGetBtn(const char* pszLable,int typeID);

#ifdef __SPRING__
	int AddSpringCmdItem(const char* pszLabel);
#endif
	
	//礼品发放F_NPC对话框接口 add by wyc 0603171607
	int AddGiveGift(const char* pszLabel);
	//显示最弱3国家F_NPC对话框接口 add by wyc 0603311650
	int AddShowCountryWeak3(const char* pszLabel);

	//推荐人系统(A推荐B)
	int AddRecoSys_A_Input(const char* pszLabel);
	int AddRecoSys_B_Query_Take(const char* pszLabel);
	int AddRecoSys_A_Query_Take(const char* pszLabel);

	//Add By David For 2006-3-20
	int AddImmureDlg( const char* pszLabel ); 
	/////////////////////////
	// 家族和帮会排行榜

	int AddInputDialog( const char* pszText,int type );
	int SendGoTrainUserCmd(const char* pszLevel);
	int SendGoFubenUserCmd(const char* pszLevel);
	int AddQueryGenInfo(const char* pszLabel);
	int AdFubenjhGenInfo(const char* pszLabel);

	//请求领取家族普通经验
	int AddRequestSeptNormalExp(const char* pszLabel);

	//做为师傅查询进贡
	int AddReqMasterBounty(const char* pszLabel, const char* operatorID);
	//做为徒弟查询进贡
	int AddReqPrenticeBounty(const char* pszLabel, const char* operatorID);

	int AddAppointMember(const char* pszLabel,int type);
	int  GetMainSkillType();
	int  GetLevel();	

	int AddQuestOccupierList(const char* pszLabel,int type);
	int AddSalaryDialog(const char* pszLabel); //soke 领取工资
	int AddChouJiang(const char* pszLabel);
	int AddExpGame(const char* text1,const char* text2,const char* text3);
	int StartBikeGame(const char* pszLabel);//自行车
	int DeleteBikeRance(const char* pszLabel);//自行车
	int NextBikeGame(const char* text1,const char* text2);//自行车
	int AddForTuneFundDlg(const char* pszLabel);
	int AddTianxiaDlg(const char* pszLabel);//天下第一
	int AddTianxiaLingquDlg(const char* pszLabel);//天下第一
	int AddZhanCheListDlg(const char* pszLabel);//战车列表
	int AddMiGong(const char* text1,const char* text2);//迷宫
	//////////////////////////////////////////////////////////////////////////


	static void bind_npc_dlg_lua(lua_State* L);
public: 
	///////////////sort//////////////////////////
	void ShowSeptNativeSort();
	void ShowSeptWorldSort();
	void ShowUnionNativeSort();
	void ShowUnionWorldSort();
	//////////////end sort////////////////////////

	void ShowGoldGiveNum(int Storn_num,int Matarial_num,int Card_num);

	void RefreshAllyCountryInfo();

	void ResetState()
	{
		m_state = STATE_NORMAL;
	};
private:
	enum npcdlg_state
	{
		STATE_NORMAL,           
		STATE_QUREY_TAX,	      // 查询税率中
		STATE_QUERY_COUNTRY_SAVEBOX,  //查询国税中
		STATE_CONTRIBUTE_COUNTRY,    //捐献中

		/////////sort////////////////////
		STATE_SEPT_NATIVE_SORT,
		STATE_SEPT_WORLD_SORT,
		STATE_UNION_NATIVE_SORT,
		STATE_UNION_WORLD_SORT,
		//////////end sort//////////////


		///////////////////////
		STATE_ARMY_QUERY,
		////////////////////////

		////////////////////////
		STATE_QUEST_GOLDGIVE_NUM,
		/////////////////////////


		////////////////////////
		STATE_QUEST_ALLYCOUNTRY_INFO,
		////////////////////////

		STATE_QUEST_FOR_GEN_INFO,
	};
	npcdlg_state m_state;

	int  AddActionItem(const char * pLabel,const enumAction & Action);	
};

