/**
 * \file CQuiz.h
 * \version  $Id: CQuiz.h  $
 * \author 
 * \date 
 * \brief 定义答题管理器
 *
 */


#ifndef _CLASS_CQUIZ_H_
#define _CLASS_CQUIZ_H_

#include <vector>
#include <set>
#include "zMutex.h"
#include "zMisc.h"
#include "zType.h"
#include "zEntry.h"
#include "zEntryManager.h"
#include "Command.h"
#include "SessionCommand.h"
#include "zUser.h"
#include "zUniqueID.h"
#include "zSingleton.h"

class UserSession;
class zRWLock;
class CQuiz;
extern unsigned int quiz_active_time;
extern unsigned int quiz_ready_time;
typedef zUniqueID<DWORD> zUniqueDWORDID;

/**
 * \brief 答题记录管理类
 *
 * 管理答题记录的容器,使用Singleton模式
 */
class CQuizM : public zEntryManager<zEntryTempID>, 
	       public Singleton<CQuizM>
{
	friend class SingletonFactory<CQuizM>;

	private:
		zUniqueDWORDID *channelUniqeID;
		zRWLock rwlock;
		CQuizM();

	public:
		bool getUniqeID(DWORD &tempid);
		void putUniqeID(const DWORD &tempid);

		template <class YourEntry>
			bool execEveryOne(execEntry<YourEntry> &exec)
			{
				rwlock.rdlock();
				bool ret=execEveryEntry<>(exec);
				rwlock.unlock();
				return ret;
			}

		template <class YourEntry>
			void removeOne_if(removeEntry_Pred<YourEntry> &pred)
			{
				rwlock.wrlock();
				removeEntry_if<>(pred);
				rwlock.unlock();
			}

		~CQuizM();
		bool init();
		static void destroyMe();

		/**
		 * \brief 对战管理器的定时器回调函数
		 *
		 * 定时遍历所有对战记录，删除无效记录，结束达到时间的对战等相关处理。
		 */
		void timer();
		void printSize();


					


		/**
		 * \brief 处理用户的对战命令
		 *
		 * \param pUser 发送该命令的玩家相对应的UserSession对象
		 * \param ptNullCmd 收到的命令
		 * \param cmdLen 命令长度
		 * \return 是对战命令，并得到相应处理，返回为true, 否则为false
		 *
		 */
		bool processUserMessage(UserSession *pUser, const Cmd::stNullUserCmd *ptNullCmd, const unsigned int cmdLen);

		/**
		 * \brief 处理场景发送过来的服务器间消息
		 *
		 * \param cmd 场景服务器发送过来的命令指令
		 * \param cmdLen 命令长度
		 *
		 * \return 是已定义的服务器间命令，并得到相应处理，返回true, 否则为false.
		 *
		 */
		bool processSceneMessage(const Cmd::t_NullCmd *cmd, const unsigned int cmdLen);

		/**
		 * \brief 加入新的竞赛记录
		 *
		 * \param pCmd 命令指针
		 * \return 没有重复记录，并加入成功，返回true,否则返回false
		 *
		 */
		bool addNewQuiz_sceneSession(Cmd::Session::t_createQuiz_SceneSession* pCmd);



		/**
		 * \brief 查找全区竞赛记录
		 *
		 * \return 如果找到，返回对应指针，如果没有，返回NULL
		 */
		CQuiz* findWorldQuiz();

		/**
		 * \brief 查找个人问答记录
		 *
		 * \return 如果找到，返回对应指针，如果没有，返回NULL
		 */
		CQuiz* findPersonalQuiz(DWORD dwUserID);
		
		/**
		  * \brief 用户上线处理
		  *
		  * 用户上线时，同时判断该用户的竞赛状态，并做相应处理
		  *
		  * \param pUser 上线用户
		  *
		  */
		void userOnline(UserSession* pUser);
		
		/**
		  * \brief 用户下线处理
		  *
		  * 用户下线时，同时判断该用户的竞赛状态，并做相应处理
		  *
		  * \param pUser 上线用户
		  *
		  */
		void userOffline(UserSession* pUser);
};


/**
 * \brief 参赛者
 *
 */
class CPothunter
{
	public:
		DWORD dwUserID;   // 参赛者ID
		int dwScore;    // 得分
		DWORD dwGrace;    // 本次文采得分
		int dwLuck;	// 幸运星使用次数

		int dwAnswerStatus; // 本次答题状态
		
		CPothunter()
		{
			dwUserID = 0;
			dwScore = 0;
			dwGrace = 0;
			dwLuck = 0;

			dwAnswerStatus = -1;
		}
		/*bool operator ==(CPothunter& ref)
		{
			return ref.dwUserID==this->dwUserID;
		}*/
		
		~CPothunter(){}
};



class CSubject
{
	public:
		/*std::string title;
		std::string answer_a;
		std::string answer_b;
		std::string answer_c;
		std::string answer_d;
		*/
		char title[256];
		char answer_a[56];
		char answer_b[56];
		char answer_c[56];
		char answer_d[56];
		char answer_e[56];
		char answer_f[56];

		DWORD answer;
		int  quiz_type; // 0为全国,1为个人,2为２０级

		CSubject(const CSubject& ref)
		{
			strcpy(this->title, ref.title);
			strcpy(this->answer_a, ref.answer_a);
			strcpy(this->answer_b, ref.answer_b);
			strcpy(this->answer_c, ref.answer_c);
			strcpy(this->answer_d, ref.answer_d);
			strcpy(this->answer_e, ref.answer_e);
			strcpy(this->answer_f, ref.answer_f);

			this->answer = ref.answer;
			this->quiz_type = ref.quiz_type;
		}

		CSubject()
		{
			answer=0;
			bzero(title, sizeof(title));
			bzero(answer_a, sizeof(answer_a));
			bzero(answer_b, sizeof(answer_b));
			bzero(answer_c, sizeof(answer_c));
			bzero(answer_d, sizeof(answer_d));
			bzero(answer_e, sizeof(answer_e));
			bzero(answer_f, sizeof(answer_f));
			quiz_type = 0;
		}

		const CSubject & operator= (const CSubject &ref)
		{
			strcpy(this->title, ref.title);
			strcpy(this->answer_a, ref.answer_a);
			strcpy(this->answer_b, ref.answer_b);
			strcpy(this->answer_c, ref.answer_c);
			strcpy(this->answer_d, ref.answer_d);
			strcpy(this->answer_e, ref.answer_e);
			strcpy(this->answer_f, ref.answer_f);
			
			this->answer = ref.answer;
			this->quiz_type = ref.quiz_type;
			return *this;
		}

		~CSubject()
		{
#ifdef _ZJW_DEBUG			
			Zebra::logger->debug("~CSubject:%s", this->title);
#endif				
		}
		
};

class CSubjectM : public Singleton<CSubjectM>
{
	public:
		~CSubjectM();
		bool init();
		static void destroyMe();
		
		std::vector<CSubject> gsubjects;
		std::vector<CSubject> levels;
		std::vector<CSubject> personals;
		CSubjectM();
};

class CQuiz:public zEntry
{
	protected:
		zRWLock rwlock;                             // 读写锁

		/// 答题前的等待时间
		DWORD ready_time;

		/// ready_question持续时间,默认10秒
		DWORD ready_question_time;
		

		/// 竞赛当前状态
		BYTE state;

		/// 竞赛类型
		BYTE type;
		
		virtual void sendExitToAll(){};
		std::set<int> answered;


	public:
		/// 竞赛计时,从ready_question结束时重新置零后，再进行计时, 默认进行30分钟
		DWORD count;

		/// 题库类型
		BYTE subject_type;

		DWORD active_time;

		/// 当前已答题目数
		DWORD cur_subject;

		/// 本次答题的总题数
		DWORD total_subject;

		/// 答题计时
		DWORD question_count;

		enum{
			QUIZ_READY,		// 邀请等待状态
			QUIZ_READY_QUESTION,    // 等待被邀应者回应状态,保持10秒
			QUIZ_SEND_QUESTION,	// 发题状态
			QUIZ_ACTIVE_QUESTION,	// 答题状态
			QUIZ_END_QUESTION,      // 结束本轮问答,并做相应处理(全区竞赛时，进行排序处理并发送排名与得分，
						// 个人问答仅进行得分和文采的发送)
			QUIZ_READY_OVER,        // 结束竞赛，通知场景取消所有人的对战状态，并计算输赢,重置查看排名时间
			QUIZ_READ_SORT,		// 该状态只用于全国,等待五秒时间后，在此状态再发送一次结束
			QUIZ_RETURN_GOLD,	// 个人问答结束，但还未进行奖励
			QUIZ_OVER		// 结束清除状态
		};

		CQuiz();
		CQuiz(DWORD active_time, DWORD ready_time); 
				
		virtual ~CQuiz();

		/**
		 * \brief 时间事件处理回调函数，由TimeTick.cpp中函数回调
		 *
		 *
		 */
		void timer();	//时间事件处理回调


		// 以下为设置对战状态的处理方法。对战状态具体查看对战系统状态转换图
		// 状态转换函数默认情况，皆不需要重载。只需重载指定的几个方法。
		// 如果你想实现一个与默认行为不同的状态转换过程。可能需要重载所有状态转换方法
		
		virtual void setReadyState();
		virtual void setReadyQuestionState();
		virtual void setSendQuestionState();
		virtual void setActiveQuestionState();
		virtual void setEndQuestionState();
		virtual void setReadyOverState();
		virtual void setReturnGoldState();
		virtual void setReadSortState();
		virtual void setOverState();

		virtual bool addPothunters(UserSession* pUser) = 0;

		/**
		 * \brief 打出竞赛当前状态
		 *
		 */
		void printState();

		/**
		  * \brief 用户上线处理
		  *
		  * 用户上线时，同时判断该用户的竞赛状态，并做相应处理,个人问答上线不需处理。
		  * 世界竞赛时，需要判断其是否已经参加了竞赛,如果没有参加，则发出询问，如果已经参加，则不再处理
		  *
		  * \param pUser 上线用户
		  *
		  */
		virtual void userOnline(UserSession* pUser) = 0;
		
		/**
		  * \brief 用户下线处理
		  *
		  * 用户下线时，同时判断该用户的竞赛状态，并做相应处理, 个人问答下线时，进入setReadyOver状态，全国竞赛时不做处理
		  *
		  * \param pUser 上线用户
		  *
		  */
		virtual void userOffline(UserSession* pUser) = 0;

		/**
		  * \brief 判断是否仍在竞赛期间
		  *
		  * \return 如果仍在对战时期，返回TRUE,否则返回FALSE
		  */
		virtual bool isActivePeriod()=0;

		/**
		  * \brief 判断是否仍在准备期
		  *
		  * \return 如果仍在准备期时期，返回TRUE,否则返回FALSE
		  */
		virtual bool isReadyPeriod()
		{
			return this->count<this->ready_time ? true : false;
		}
		
		/**
		  * \brief 判断是否仍在准备期
		  *
		  * \return 如果仍在准备期时期，返回TRUE,否则返回FALSE
		  */
		virtual bool isReadyQuestionPeriod()
		{
			return this->count<ready_question_time ? true : false;
		}


		/**
		 * \brief  判断是否仍在查看排名期
		 *
		 */
		virtual bool isReadSortPeriod()
		{
			return this->count<5?true:false;
		}
		
		virtual bool isActiveQuestionPeriod()
		{
			return this->question_count<=20 ? true : false;
		}

		virtual bool isEndQuestionPeriod()
		{
			return this->question_count<=25 ? true : false;
		}
		
		/**
		  * \brief 判断是否仍在准备期
		  *
		  * \return 如果仍在准备期时期，返回TRUE,否则返回FALSE
		  */
		virtual void updateTime()
		{
			rwlock.wrlock();
			this->count += 1;
			this->question_count += 1;
			rwlock.unlock();
		}

		virtual BYTE getType()
		{
			return this->type;
		}

		virtual BYTE getState()
		{
			return this->state;
		}

		virtual int answer(Cmd::stAnswerQuiz* pCmd, DWORD dwUserID) = 0;
		virtual void exitQuiz(DWORD dwUserID) = 0;

		/// 本次竞赛的题目
		std::vector<CSubject>   subjects;
//		CSubject subjects[1];
//		int subjects[100];
};


class CQuizPersonal : public CQuiz
{
	public:
		CQuizPersonal();
		CQuizPersonal(DWORD active_time, DWORD ready_time);
		virtual ~CQuizPersonal();
		
		bool isActivePeriod()
		{
			return this->cur_subject<=total_subject;
		}
		
		void setSendQuestionState();
		void setEndQuestionState();
		void setReadyOverState();
		void setReturnGoldState();

		int answer(Cmd::stAnswerQuiz* pCmd, DWORD dwUserID);
		bool addPothunters(UserSession* pUser);
		void exitQuiz(DWORD dwUserID);
		
		/**
		  * \brief 用户上线处理
		  *
		  * 用户上线时，同时判断该用户的竞赛状态，并做相应处理
		  *
		  * \param pUser 上线用户
		  *
		  */
		void userOnline(UserSession* pUser);
		void userOffline(UserSession* pUser);
		CPothunter pothunter;
	protected:
		bool award();
		UserSession* user;
};

class CQuizWorld : public CQuiz
{
	public:
		CQuizWorld();
		CQuizWorld(DWORD active_time, DWORD ready_time, BYTE subject_type=0);
		virtual ~CQuizWorld();

		void setSendQuestionState();
		void setEndQuestionState();
		void setReadyOverState();
		
		bool addPothunters(UserSession* pUser);
		int answer(Cmd::stAnswerQuiz* pCmd, DWORD dwUserID);
		
		void exitQuiz(DWORD dwUserID);

		bool isActivePeriod()
		{
			return this->cur_subject<=total_subject;
		}

		/**
		  * \brief 用户上线处理
		  *
		  * 用户上线时，同时判断该用户的竞赛状态，并做相应处理
		  *
		  * \param pUser 上线用户
		  *
		  */
		void userOnline(UserSession* pUser);
		void userOffline(UserSession* pUser);
	public:		
		/// 参赛人员列表
		std::vector<CPothunter> pothunters;
//		int valid_pothunter; // 本轮有效人数
	private:
		virtual void sendExitToAll();

};

#endif

