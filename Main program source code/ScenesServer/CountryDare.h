/**
 * \file
 * \version  $Id: CountryDare.h  $
 * \author  
 * \date 
 * \brief 国战管理器
 *
 * 
 */

#ifndef _COUNTRY_DARE
#define _COUNTRY_DARE
#include "zTime.h"
#include <time.h>       
#include <sys/time.h> 
#include <vector>
#include "zRWLock.h"

class Scene;

class CountryDare
{
	public:
		CountryDare()
		{
			last_attack_time=0;
			id=0;last_gen_attack_time=0;
			last_king_attack_time=0;
		};
		
		~CountryDare(){};
		
		DWORD id;
		time_t last_attack_time;
		time_t last_gen_attack_time;
		time_t last_att_gen_attack_time;
		time_t last_king_attack_time;
};

class CountryDareM
{
	public:
		static CountryDareM& getMe()
		{
			if (!_instance)
			{
				_instance = new CountryDareM();
			}

			return *_instance;
		}
		
		~CountryDareM(){}

		/**
		  * \brief 时间回调函数
		  */
		void timer();


		/**
		 * \brief 判断是否可以攻击国旗
		 *
		 */
		bool isAttackMainFlag(Scene* scene, DWORD dwNpcID);

		/**
		 * \brief 判断是否可以攻击大将军
		 *
		 */
		bool isAttackMainGen(Scene* scene);

		
		/**
		  * \brief 复活副旗
		  *
		  */
		void reliveSecondFlag(Scene* scene, DWORD dwNpcID);

		/**
		  * \brief 延长副旗复活时间
		  *
		  */
		void delaySecondFlag(Scene* scene);

		/**
		 * \brief 更新骚扰国战被攻击时间
		 *
		 */
		void updateAttackTime(DWORD countryid, time_t uptime);
		void updateKingAttackTime(DWORD countryid, time_t uptime);
		void setCountryDare(DWORD dwCountryID, DWORD dwDareCountryID, bool isAtt);

		/**
		 * \brief 
		 *
		 */
		void updateGenAttackTime(DWORD countryid, time_t uptime);

		void updateAttGenAttackTime(DWORD countryid, time_t uptime);

		/**
		 * \brief 初始化
		 *
		 * 初始化该场景所有国家的国战信息
		 *
		 */
		void init();
		
	protected:
		std::vector<CountryDare> dares;
		zRWLock rwlock;	
		CountryDareM(){}
		static CountryDareM* _instance;
};

#endif

