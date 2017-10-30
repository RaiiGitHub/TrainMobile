#pragma once
class TRAINONLINE_API FSM_Component
{
public:
	FSM_Component() {}
public:
	virtual ~FSM_Component() {}
	virtual bool Explain() { return false; }
};

class TRAINONLINE_API FSM_Symbol :
	public FSM_Component
{
public:
	FSM_Symbol() {}
	virtual ~FSM_Symbol() {}
public:
	FString symbol_;

};//included fsm input symbol and output symbol

typedef TSharedPtr<FSM_Symbol> FSM_SymbolPtr;
typedef TDoubleLinkedList<FSM_SymbolPtr> FSM_SymbolList;

class TRAINONLINE_API FSM_State :
	public FSM_Component
{
public:
	FSM_State() {}
	virtual ~FSM_State() {}
public:
	FString state_name_;
};
typedef TSharedPtr<FSM_State> FSM_StatePtr;
typedef TDoubleLinkedList<FSM_StatePtr> FSM_StateList;

class TRAINONLINE_API FSM_Function :
	public FSM_Component
{
public:
	FSM_Function(const FSM_StatePtr& own_state) :own_state_(own_state) {}
	virtual ~FSM_Function() {}
public:
	void SetupNextSymbol(const FSM_SymbolPtr& next_symbol){
		next_symbol_.Reset();
		next_symbol_ = next_symbol;
	}
public:
	FSM_StatePtr own_state_;
	FSM_SymbolPtr next_symbol_;
};//include fsm next state function and output function

typedef TSharedPtr<FSM_Function> FSM_FunctionPtr;

class TRAINONLINE_API FSM_Object
{
public:
	FSM_Object() {};
	virtual ~FSM_Object() {};
	virtual void Clear() {
		input_symbol_list_.Empty();
		output_symbol_list_.Empty();
		finite_state_list_.Empty();
		begining_state_.Reset();
		input_next_state_func_.Reset();
		output_func_.Reset();
	}
public:
	FSM_SymbolList input_symbol_list_;
	FSM_SymbolList output_symbol_list_;
	FSM_StateList  finite_state_list_;
	FSM_StatePtr   begining_state_;
	FSM_StatePtr   current_state_;
	FSM_FunctionPtr input_next_state_func_;
	FSM_FunctionPtr output_func_;
};

