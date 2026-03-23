#include <unistd.h>
#include <iostream>
#include "zFSM.h"
/*
class StateMachine
{
	public:
		StateMachine()
		{
		}

		virtual ~StateMachine()
		{
		}

		virtual bool process_user_cmd(){};
		virtual bool process_scene_cmd(){};

		/// 定时器, 由系统定时器线程调用
		virtual void timer();
		
};

class Dare : public StateMachine<Dare>
{
	public:
		virtual void init(const std::string& name)
		{
			State<Dare>* ready_state = new State<Dare>;
			
			EventHandler<Dare> question_yes_handler(this);
			question_yes_handler.handler = &Dare::question_yes;
			
			EventHandler<Dare> question_no_handler(this);
			question_no_handler.handler = &Dare::question_no;

			ready_state->set_machine(this);

			ready_state->add_transition("question_yes", question_yes_handler, "active");
			ready_state->add_transition("question_no", question_no_handler);
			this->add_state("ready", ready_state);
			this->set_act("ready");
		}
		
		int question_yes(Event* ev)
		{
			std::cout << "handler question_yes" << std::endl;
			return 1;
		}
		
		int question_no(Event* ev)
		{
			std::cout << "handler question_no" << std::endl;
			return 1;
		}
};
*/
int main(int argc,char *argv[])
{
/*
	Dare *union_dare = new Dare;
	union_dare->init("dare");
	
	Event ev("question_no");
	union_dare->dispatch_event(&ev);

	while (union_dare->is_over()) delete union_dare;
*/	
	return 0;
}
