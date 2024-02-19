
#include <string>
#include <iostream>
#include <cmath>
#include <thread>
#include <vector>
#include <mutex>
std::mutex mu;


// this is a pretty epic program

namespace unit_math {
	int tape = 0;//souvenir (DO NOT REMOVE!!)
	bool lock = false;
	enum class type_of_unit { number, equation };
	enum class type_of_op { add, sub, mul, div, pow, log, abs};
	const char* types[7] = {"add","sub","mul","div","pow","log","abs"};
	long double log_base(long double input, long double base) {
		return log10(input) / log10(base);
	};
	class MATH_UNIT {
	public:
		type_of_unit typeofunit;
		long double number_value;
		bool solve_in_parallel;
		int size_of_equation;
		MATH_UNIT** equation;
		type_of_op typeofop;
		MATH_UNIT** parameters;
		int* loc_of_params;
		int len_of_loc_of_params;
		bool solved;
		bool garbage;
		bool copied;
		bool dori;
		MATH_UNIT* deri_or_inte;
		MATH_UNIT* copy;
	};
	MATH_UNIT* CREATE_NUM_UNIT(long double input, type_of_op operation) {
		MATH_UNIT* output = new MATH_UNIT;
		output->typeofunit = type_of_unit::number;
		output->typeofop = operation;
		output->size_of_equation = 0;
		output->solve_in_parallel = false;
		output->number_value = input;
		output->equation = new MATH_UNIT * [0];
		output->parameters = new MATH_UNIT * [0];
		output->loc_of_params = new int[0];
		output->len_of_loc_of_params = 0;
		output->solved = false;
		output->garbage = false;
		output->copied = false;
		output->dori = false;
		return output;
	}
	MATH_UNIT* CREATE_PARAM_UNIT() {
		MATH_UNIT* output = new MATH_UNIT;
		output->typeofunit = type_of_unit::equation;
		output->size_of_equation = 0;
		output->solve_in_parallel = false;
		output->typeofop = type_of_op::add;
		output->equation = new MATH_UNIT * [0];
		output->number_value = 0;
		output->parameters = new MATH_UNIT * [1];
		output->parameters[0] = output;
		output->loc_of_params = new int[1];
		output->loc_of_params[0] = 0;
		output->len_of_loc_of_params = 1;
		output->solved = false;
		output->garbage = false;
		output->copied = false;
		output->dori = false;
		return output;
	}
	MATH_UNIT** COMPACT(MATH_UNIT** input, int input_len, int start, int end) {
		MATH_UNIT** output = new MATH_UNIT * [(end - start)];
		for (int i = start; i < end; i++) {
			output[i - start] = input[i];
		}
		return output;
	}
	void COMBINE(MATH_UNIT* input1, MATH_UNIT* input2) {
		if (input1->typeofunit == type_of_unit::number) {
			input1->size_of_equation = input1->size_of_equation + 1;
			MATH_UNIT** new_values = new MATH_UNIT * [input1->size_of_equation];
			for (int i = 0; i < ((input1->size_of_equation) - 1); i++) {
				new_values[i] = input1->equation[i];
			}
			new_values[input1->size_of_equation - 1] = input2;
			delete[] input1->equation;
			input1->equation = new_values;
			if (input2->typeofunit == type_of_unit::equation or input2->len_of_loc_of_params > 0) {
				MATH_UNIT** REPLACE1 = new MATH_UNIT * [input1->len_of_loc_of_params + input2->len_of_loc_of_params];
				int* REPLACE2 = new int[input1->len_of_loc_of_params + input2->len_of_loc_of_params];
				for (int i = 0; i < (input1->len_of_loc_of_params + input2->len_of_loc_of_params); i++) {
					if (i < input1->len_of_loc_of_params) {
						REPLACE1[i] = input1->parameters[i];
						REPLACE2[i] = input1->loc_of_params[i];
					}
					else {
						REPLACE1[i] = input2->parameters[i - (input1->len_of_loc_of_params)];
						REPLACE2[i] = input1->size_of_equation - 1;
					}
				}
				input1->len_of_loc_of_params += input2->len_of_loc_of_params;
				delete[] input1->parameters;
				delete[] input1->loc_of_params;
				input1->parameters = REPLACE1;
				input1->loc_of_params = REPLACE2;
			}
		}
	}
	void COMBINE_TO_BACK(MATH_UNIT* input1, MATH_UNIT* input2) {
		if (input1->typeofunit == type_of_unit::number) {
			input1->size_of_equation = input1->size_of_equation + 1;
			MATH_UNIT** new_values = new MATH_UNIT * [input1->size_of_equation];
			for (int i = 0; i < ((input1->size_of_equation) - 1); i++) {
				new_values[i + 1] = input1->equation[i];
			}
			new_values[0] = input2;
			delete[] input1->equation;
			input1->equation = new_values;
			if (input2->typeofunit == type_of_unit::equation or input2->len_of_loc_of_params > 0) {
				MATH_UNIT** REPLACE1 = new MATH_UNIT * [input1->len_of_loc_of_params + input2->len_of_loc_of_params];
				int* REPLACE2 = new int[input1->len_of_loc_of_params + input2->len_of_loc_of_params];
				for (int i = 0; i < (input1->len_of_loc_of_params + input2->len_of_loc_of_params); i++) {
					if (i < input2->len_of_loc_of_params) {
						REPLACE1[i] = input2->parameters[i];
						REPLACE2[i] = 0;
					}
					else {
						REPLACE1[i] = input1->parameters[i-(input2->len_of_loc_of_params)];
						REPLACE2[i] = input1->loc_of_params[i - (input2->len_of_loc_of_params)] + 1;
					}
				}
				input1->len_of_loc_of_params += input2->len_of_loc_of_params;
				delete[] input1->parameters;
				delete[] input1->loc_of_params;
				input1->parameters = REPLACE1;
				input1->loc_of_params = REPLACE2;
			}
		}
	}
	void ADD(MATH_UNIT* input1, long double input2) {
		if (input1->typeofunit == type_of_unit::number) {
			MATH_UNIT* newunit1 = CREATE_NUM_UNIT(input2, type_of_op::add);
			input1->size_of_equation = input1->size_of_equation + 1;
			MATH_UNIT** new_values = new MATH_UNIT * [input1->size_of_equation];
			for (int i = 0; i < ((input1->size_of_equation) - 1); i++) {
				new_values[i] = input1->equation[i];
			}
			new_values[input1->size_of_equation - 1] = newunit1;
			delete[] input1->equation;
			input1->equation = new_values;
			input1->solve_in_parallel = false;
		}
	}
	void SUB(MATH_UNIT* input1, long double input2) {
		if (input1->typeofunit == type_of_unit::number) {
			MATH_UNIT* newunit1 = CREATE_NUM_UNIT(input2, type_of_op::sub);
			input1->size_of_equation = input1->size_of_equation + 1;
			MATH_UNIT** new_values = new MATH_UNIT * [input1->size_of_equation];
			for (int i = 0; i < ((input1->size_of_equation) - 1); i++) {
				new_values[i] = input1->equation[i];
			}
			new_values[input1->size_of_equation - 1] = newunit1;
			delete[] input1->equation;
			input1->equation = new_values;
			input1->solve_in_parallel = false;
		}
	}
	void MUL(MATH_UNIT* input1, long double input2) {
		if (input1->typeofunit == type_of_unit::number) {
			MATH_UNIT* newunit1 = CREATE_NUM_UNIT(input2, type_of_op::mul);
			input1->size_of_equation = input1->size_of_equation + 1;
			MATH_UNIT** new_values = new MATH_UNIT * [input1->size_of_equation];
			for (int i = 0; i < ((input1->size_of_equation) - 1); i++) {
				new_values[i] = input1->equation[i];
			}
			new_values[input1->size_of_equation - 1] = newunit1;
			delete[] input1->equation;
			input1->equation = new_values;
			input1->solve_in_parallel = false;
		}
	}
	void DIV(MATH_UNIT* input1, long double input2) {
		if (input1->typeofunit == type_of_unit::number) {
			MATH_UNIT* newunit1 = CREATE_NUM_UNIT(input2, type_of_op::div);
			input1->size_of_equation = input1->size_of_equation + 1;
			MATH_UNIT** new_values = new MATH_UNIT * [input1->size_of_equation];
			for (int i = 0; i < ((input1->size_of_equation) - 1); i++) {
				new_values[i] = input1->equation[i];
			}
			new_values[input1->size_of_equation - 1] = newunit1;
			delete[] input1->equation;
			input1->equation = new_values;
			input1->solve_in_parallel = false;
		}
	}
	void POW(MATH_UNIT* input1, long double input2) {
		if (input1->typeofunit == type_of_unit::number) {
			MATH_UNIT* newunit1 = CREATE_NUM_UNIT(input2, type_of_op::pow);
			input1->size_of_equation = input1->size_of_equation + 1;
			MATH_UNIT** new_values = new MATH_UNIT * [input1->size_of_equation];
			for (int i = 0; i < ((input1->size_of_equation) - 1); i++) {
				new_values[i] = input1->equation[i];
			}
			new_values[input1->size_of_equation - 1] = newunit1;
			delete[] input1->equation;
			input1->equation = new_values;
			input1->solve_in_parallel = false;
		}
	}
	void deletemu(MATH_UNIT* input, int lenny) {
		if (input != NULL) {
			delete[] input->equation;
			delete[] input->parameters;
			delete[] input->loc_of_params;
			delete[] input;
		}
	}
	MATH_UNIT** APPEND(MATH_UNIT** input1, MATH_UNIT* input2, int input1len, int loc) {
		MATH_UNIT** REPLACE = new MATH_UNIT * [input1len + 1];
		for (int i = 0; i < (input1len + 1); i++) {
			if (i < loc) {
				REPLACE[i] = input1[i];
			}
			else if (i == loc) {
				REPLACE[i] = input2;
			}
			else {
				REPLACE[i] = input1[i - 1];
			}
		}
		return REPLACE;
	}
	void DELETE_V(MATH_UNIT*** input, int input_len, int loc) {
		MATH_UNIT** REPLACE = new MATH_UNIT * [input_len - 1];
		for (int i = 0; i < (input_len - 1); i++) {
			if (i < loc) {
				REPLACE[i] = input[0][i];
			}
			else {
				REPLACE[i] = input[0][i + 1];
			}
		}
		delete[] input[0];
		input[0] = REPLACE;
	}
	void DELETE_V_I(int** input, int input_len, int loc) {
		int* REPLACE = new int[input_len - 1];
		for (int i = 0; i < (input_len - 1); i++) {
			if (i < loc) {
				REPLACE[i] = input[0][i];
			}
			else {
				REPLACE[i] = input[0][i + 1];
			}
		}
		delete[] input[0];
		input[0] = REPLACE;
	}
	void devalue(MATH_UNIT* input) {
		for (int i = 0;i < input->size_of_equation; i++) {
			devalue(input->equation[i]);
		}
		input->copied = false;
	}
	void revalue(MATH_UNIT* input, int* len) {
		if (not input->copied) {
			for (int i = 0; i < input->size_of_equation; i++) {
				revalue(input->equation[i], len);
				len[0]++;
			}
			input->copied = true;
		}
	}
	MATH_UNIT* HIDDEN_1(MATH_UNIT* input) {
		switch (input->typeofunit)
		{
		case type_of_unit::number:
			if (not input->copied) {
				MATH_UNIT* output = new MATH_UNIT;
				MATH_UNIT** params = new MATH_UNIT * [input->len_of_loc_of_params];
				int* loc_of_paramr = new int[input->len_of_loc_of_params];
				output->number_value = input->number_value;
				output->size_of_equation = input->size_of_equation;
				output->typeofop = input->typeofop;
				output->typeofunit = input->typeofunit;
				output->solved = input->solved;
				output->garbage = input->garbage;
				output->solve_in_parallel = input->solve_in_parallel;
				for (int i = 0; i < (input->len_of_loc_of_params);i++) {
					params[i] = input->parameters[i];
					loc_of_paramr[i] = input->loc_of_params[i];
				}
				output->parameters = params;
				output->loc_of_params = loc_of_paramr;
				output->len_of_loc_of_params = input->len_of_loc_of_params;
				output->equation = new MATH_UNIT * [input->size_of_equation];
				
				for (int i = 0; i < input->size_of_equation; i++) {
					MATH_UNIT* iii = input->equation[i];
					MATH_UNIT* ooo = HIDDEN_1(iii);
					output->equation[i] = ooo;
				}
				
				
				output->copied = false;
				output->dori = input->dori;
				input->copied = true;
				input->copy = output;
				return output;
			}
			else {
				return input->copy;
			}
			break;
		case type_of_unit::equation:
			return input;
			break;
		default:
			break;

		}
	}
	MATH_UNIT* COPY(MATH_UNIT* input) {
		devalue(input);
		MATH_UNIT* output = HIDDEN_1(input);
		return output;
	}
	void HIDDEN_2(MATH_UNIT* input, MATH_UNIT*** garbage, int* len) {
		if (input->typeofunit == type_of_unit::number) {
			for (int i = 0;i < input->size_of_equation; i++) {
				HIDDEN_2(input->equation[i], garbage, len);
			}
			if (not input->garbage) {
				input->garbage = true;
				MATH_UNIT** tty = APPEND(garbage[0], input, len[0], len[0]);
				delete[] garbage[0];
				garbage[0] = tty;
				len[0]++;
			}
		}
	}
	void DELETE(MATH_UNIT* input) {
		MATH_UNIT** njh = new MATH_UNIT * [0];
		int yyu = 0;
		HIDDEN_2(input, &njh, &yyu);
		for (int i = 0;i < yyu; i++) {
			deletemu(njh[i], i);
		}
		delete[] njh;
	}
	void dedori(MATH_UNIT* input) {
		for (int i = 0; i < input->size_of_equation; i++) {
			dedori(input->equation[i]);
		}
		input->dori = false;
	}
	MATH_UNIT* HIDDEN_1_OP(MATH_UNIT* input, type_of_op operation) {
		MATH_UNIT* output = CREATE_NUM_UNIT(0, operation);
		MATH_UNIT* copy = HIDDEN_1(input);
		copy->typeofop = type_of_op::add;
		COMBINE(output, copy);
		return output;
	};
	MATH_UNIT* HIDDEN_4(MATH_UNIT* input, MATH_UNIT* respect) {
		//reminder to self: do not forget that this function returns a math unit with no parameters in parameters pointer (fixed with combine to back)
		if (not input->dori) {
			input->dori = true;
			if (input->typeofunit == type_of_unit::number) {
				int* locations = new int[0];
				int len_loc = 0;
				for (int i = 0; i < input->len_of_loc_of_params; i++) {
					if (input->parameters[i] == respect) {
						if (len_loc > 0) {
							if (input->loc_of_params[i] != locations[len_loc - 1]) {
								int* REPLACE = new int[len_loc + 1];
								for (int i2 = 0; i2 < len_loc; i2++) {
									REPLACE[i2] = locations[i2];
								}
								REPLACE[len_loc] = input->loc_of_params[i];
								delete[] locations;
								locations = REPLACE;
								len_loc++;
							}
						}
						else {
							locations = new int[1];
							locations[0] = input->loc_of_params[i];
							len_loc++;
						}
					}
				}

				MATH_UNIT* output = CREATE_NUM_UNIT(input->number_value, input->typeofop);
				if (len_loc > 0) {
					bool solved = false;
					int iterator = input->size_of_equation - 1;
					int loc = 0;
					while (solved == false and iterator >= 0) {
						
						MATH_UNIT* working_value = input->equation[iterator];
						loc = input->size_of_equation - (iterator + 1);
						if (working_value->typeofop == type_of_op::add or working_value->typeofop == type_of_op::sub) {
							if (len_loc > 0) {
								if (iterator == locations[len_loc - 1]) {
									MATH_UNIT* ph1 = CREATE_NUM_UNIT(0, working_value->typeofop);
									MATH_UNIT* copy = HIDDEN_4(working_value, respect);
									copy->typeofop = type_of_op::add;
									COMBINE(ph1, copy);
									COMBINE_TO_BACK(output, ph1);
									DELETE_V_I(&locations, len_loc, (len_loc - 1));
									len_loc--;
								}
							}
						}
						else if (working_value->typeofop == type_of_op::mul) {
							if (len_loc > 0) {
								if (iterator == locations[len_loc - 1]) {
									// product rule
									MATH_UNIT* placeholder = CREATE_NUM_UNIT(0, type_of_op::add);
									MATH_UNIT* ph1 = CREATE_NUM_UNIT(0, type_of_op::add);
									MATH_UNIT* copy = HIDDEN_4(working_value, respect);
									copy->typeofop = type_of_op::add;
									COMBINE(ph1, copy);
									MATH_UNIT* ph2 = CREATE_NUM_UNIT(input->number_value, type_of_op::mul);
									for (int i = 0; i < iterator; i++) {
										COMBINE(ph2, HIDDEN_1_OP(input->equation[i], input->equation[i]->typeofop));
									}
									COMBINE(placeholder, ph1);
									COMBINE(placeholder, ph2);
									COMBINE_TO_BACK(output, placeholder);
									COMBINE_TO_BACK(output, HIDDEN_1_OP(working_value, working_value->typeofop));
									DELETE_V_I(&locations, len_loc, (len_loc - 1));
									len_loc--;
								}
								else {
									COMBINE_TO_BACK(output, HIDDEN_1_OP(working_value, type_of_op::mul));
								}
							}
							else {
								COMBINE_TO_BACK(output, HIDDEN_1_OP(working_value, type_of_op::mul));
							}
						}
						else if (working_value->typeofop == type_of_op::div) {
							if (len_loc > 0) {
								if (iterator == locations[len_loc - 1]) {
									//quotient rule
									MATH_UNIT* placeholder1 = CREATE_NUM_UNIT(0, type_of_op::sub);
									MATH_UNIT* placeholder2 = CREATE_NUM_UNIT(0, type_of_op::div);
									MATH_UNIT* ph1;
									MATH_UNIT* ph3 = HIDDEN_1_OP(working_value, type_of_op::add);
									COMBINE(placeholder2, ph3);
									COMBINE(placeholder2, CREATE_NUM_UNIT(2, type_of_op::pow));
									MATH_UNIT* rrt;
									ph1 = HIDDEN_4(working_value, respect);
									ph1->typeofop = type_of_op::add;
									MATH_UNIT* ph2 = CREATE_NUM_UNIT(input->number_value, type_of_op::mul);
									for (int i = 0; i < iterator; i++) {
										COMBINE(ph2, HIDDEN_1_OP(input->equation[i], input->equation[i]->typeofop));
									}
									COMBINE(placeholder1, ph1);
									COMBINE(placeholder1, ph2);
									COMBINE_TO_BACK(output, placeholder2);
									COMBINE_TO_BACK(output, placeholder1);
									rrt = HIDDEN_1_OP(working_value, working_value->typeofop);
									rrt->typeofop = type_of_op::mul;
									COMBINE_TO_BACK(output, rrt);
									DELETE_V_I(&locations, len_loc, (len_loc - 1));
									len_loc--;
								}
								else {
									COMBINE_TO_BACK(output, HIDDEN_1_OP(working_value, working_value->typeofop));
								}
							}
							else {
								COMBINE_TO_BACK(output, HIDDEN_1_OP(working_value, working_value->typeofop));
							}
						}
						else if (working_value->typeofop == type_of_op::pow) {
							if (len_loc > 0) {
								if (iterator == locations[len_loc - 1]) {
									// generalized power rule
									if (len_loc > 1) {
										MATH_UNIT* pht = CREATE_NUM_UNIT(input->number_value, type_of_op::add);
										for (int i = 0; i < iterator; i++) {
											COMBINE(pht, HIDDEN_1_OP(input->equation[i], input->equation[i]->typeofop));
										}
										MATH_UNIT* ph1_0 = HIDDEN_1_OP(working_value, type_of_op::pow);
										MATH_UNIT* ph2 = HIDDEN_1_OP(pht, type_of_op::mul);
										COMBINE(ph2, CREATE_NUM_UNIT(0, type_of_op::log));
										COMBINE(ph2, HIDDEN_1_OP(working_value, type_of_op::mul));
										MATH_UNIT* repl = HIDDEN_4(ph2, respect);
										DELETE(ph2->equation[ph2->size_of_equation-2]);
										deletemu(ph2, 0);
										MATH_UNIT* ph1 = HIDDEN_1_OP(pht, type_of_op::add);
										COMBINE(ph1, ph1_0);
										DELETE(pht);
										COMBINE_TO_BACK(output, ph2);
										COMBINE_TO_BACK(output, ph1);
										DELETE_V_I(&locations, len_loc, (len_loc - 1));
										solved = true;
									}
									else {
										MATH_UNIT* ph1 = CREATE_NUM_UNIT(input->number_value, type_of_op::mul);
										for (int i = 0; i < iterator; i++) {
											COMBINE(ph1, HIDDEN_1_OP(input->equation[i], input->equation[i]->typeofop));
										}
										
										MATH_UNIT* ph2 = HIDDEN_1_OP(ph1, type_of_op::mul);
										COMBINE(ph2, HIDDEN_1_OP(working_value, type_of_op::pow));
										MATH_UNIT* ph3 = CREATE_NUM_UNIT(0, type_of_op::add);
										MATH_UNIT* copy = HIDDEN_4(working_value, respect);
										copy->typeofop = type_of_op::add;
										COMBINE(ph3, copy);
										COMBINE(ph1, CREATE_NUM_UNIT(0, type_of_op::log));
										COMBINE_TO_BACK(output, ph1);
										COMBINE_TO_BACK(output, ph2);
										COMBINE_TO_BACK(output, ph3);
										DELETE_V_I(&locations, len_loc, (len_loc - 1));
										solved = true;
									}
									len_loc--;
								}
								else{
									// power rule with chain rule
									MATH_UNIT* ph1 = HIDDEN_1_OP(working_value, type_of_op::mul);
									MATH_UNIT* ph2 = CREATE_NUM_UNIT(input->number_value, type_of_op::mul);
									MATH_UNIT* ph2_1 = CREATE_NUM_UNIT(0, type_of_op::pow);
									for (int i = 0; i < iterator; i++) {
										COMBINE(ph2, HIDDEN_1_OP(input->equation[i], input->equation[i]->typeofop));
									}
									COMBINE(ph2_1, HIDDEN_1_OP(working_value, type_of_op::add));
									COMBINE(ph2_1, CREATE_NUM_UNIT(1, type_of_op::sub));
									COMBINE(ph2, ph2_1);
									COMBINE_TO_BACK(output, ph2);
									COMBINE_TO_BACK(output, ph1);
								}
							}
							else {
								// power rule with chain rule
								MATH_UNIT* ph1 = HIDDEN_1_OP(working_value, type_of_op::mul);
								MATH_UNIT* ph2 = CREATE_NUM_UNIT(input->number_value, type_of_op::mul);
								MATH_UNIT* ph2_1 = CREATE_NUM_UNIT(0, type_of_op::pow);
								for (int i = 0; i < iterator; i++) {
									COMBINE(ph2, HIDDEN_1_OP(input->equation[i], input->equation[i]->typeofop));
								}
								COMBINE(ph2_1, HIDDEN_1_OP(working_value, type_of_op::add));
								COMBINE(ph2_1, CREATE_NUM_UNIT(1, type_of_op::sub));
								COMBINE(ph2, ph2_1);
								COMBINE_TO_BACK(output, ph2);
								COMBINE_TO_BACK(output, ph1);
							}
						}
						else if (working_value->typeofop == type_of_op::log) {
							MATH_UNIT* ph1 = CREATE_NUM_UNIT(input->number_value, type_of_op::div);
							for (int i = 0; i < iterator; i++) {
								COMBINE(ph1, HIDDEN_1_OP(input->equation[i], input->equation[i]->typeofop));
							}
							COMBINE_TO_BACK(output, ph1);
							len_loc--;
						}
						iterator--;
					}
					input->deri_or_inte = output;
					delete[] locations;
					return output;
				}
				else {
					MATH_UNIT* output = CREATE_NUM_UNIT(0, input->typeofop);
					input->deri_or_inte = output;
					delete[] locations;
					return output;
				}
			}
			else{
				if (input == respect) {
					MATH_UNIT* output = CREATE_NUM_UNIT(1, input->typeofop);
					input->deri_or_inte = output;
					return output;
				}
				else {
					MATH_UNIT* output = CREATE_NUM_UNIT(0, input->typeofop);
					input->deri_or_inte = output;
					return output;
				}
			}
		}
		else {
			return input->deri_or_inte;
		}
	};
	MATH_UNIT* HIDDEN_4_OP(MATH_UNIT* input, MATH_UNIT* respect, type_of_op operation) {
		MATH_UNIT* output = CREATE_NUM_UNIT(0, operation);
		MATH_UNIT* copy = HIDDEN_4(input, respect);
		copy->typeofop = type_of_op::add;
		COMBINE(output, copy);
		return output;
	};
	MATH_UNIT* DERIVATIVE(MATH_UNIT* input, MATH_UNIT* respect) {
		devalue(input);
		MATH_UNIT* output = HIDDEN_4(input, respect);
		dedori(input);
		dedori(output);
		devalue(input);
		return output;
	}
	void HIDDEN_3(MATH_UNIT* input, MATH_UNIT*** garbage, int* len) {
		mu.lock();
		if (input->typeofunit == type_of_unit::number and (not input->solved)) {
			input->solved = true;
			mu.unlock();
			MATH_UNIT* output = CREATE_NUM_UNIT(input->number_value, input->typeofop);
			int loc_of_lif = -1;
			bool irfunc = true;
			int iterator = 0;
			bool trigger = false;
			int ppv = input->size_of_equation;
			int cop_len = 0;
			MATH_UNIT** REPLACE;
			MATH_UNIT** REPLACEMENT;
			std::vector<std::thread> threads;
			for (int i = 0;i < ppv; i++) {
				if (input->equation[i]->size_of_equation > 0) {
					threads.push_back(std::thread(HIDDEN_3, input->equation[i], garbage, len));
				}
			}
			for (std::thread& th : threads){
				if (th.joinable()){
					th.join();
				}
			}
			int iter = 0;
			for (int i = 0;i < ppv; i++) {
				if (input->equation[iter]->typeofunit == type_of_unit::number and input->equation[iter]->len_of_loc_of_params == 0) {
					switch (input->equation[iter]->typeofop) {
					case type_of_op::add:
						if (irfunc) {
							output->number_value = output->number_value + input->equation[iter]->number_value;
						}
						else {
							iterator = output->size_of_equation - 1;
							trigger = true;
							while (iterator > loc_of_lif and trigger) {
								switch (output->equation[iterator]->typeofop) {
								case type_of_op::add:
									output->equation[iterator]->number_value += input->equation[iter]->number_value;
									trigger = false;
									break;
								case type_of_op::sub:
									output->equation[iterator]->number_value -= input->equation[iter]->number_value;
									trigger = false;
									break;
								case type_of_op::mul:
									COMBINE(output, COPY(input->equation[iter]));
									trigger = false;
									break;
								case type_of_op::div:
									COMBINE(output, COPY(input->equation[iter]));
									trigger = false;
									break;
								default:
									break;
								}
								iterator--;
							}
							if (trigger) {
								COMBINE(output, COPY(input->equation[iter]));
							}
							iterator = 0;
							trigger = true;
						}
						mu.lock();
						if (not input->equation[iter]->garbage) {
							input->equation[iter]->garbage = true;
							garbage[0][len[0]] = input->equation[iter];
							len[0]++;
						}
						mu.unlock();
						iter++;
						break;
					case type_of_op::sub:
						if (irfunc) {
							output->number_value = output->number_value - input->equation[iter]->number_value;
						}
						else {
							iterator = output->size_of_equation - 1;
							trigger = true;
							while (iterator > loc_of_lif and trigger) {
								switch (output->equation[iterator]->typeofop) {
								case type_of_op::add:
									output->equation[iterator]->number_value -= input->equation[iter]->number_value;
									trigger = false;
									break;
								case type_of_op::sub:
									output->equation[iterator]->number_value += input->equation[iter]->number_value;
									trigger = false;
									break;
								case type_of_op::mul:
									COMBINE(output, COPY(input->equation[iter]));
									trigger = false;
									break;
								case type_of_op::div:
									COMBINE(output, COPY(input->equation[iter]));
									trigger = false;
									break;
								default:
									break;
								}
								iterator--;
							}
							if (trigger) {
								COMBINE(output, COPY(input->equation[iter]));
							}
							iterator = 0;
							trigger = true;
						}
						mu.lock();
						if (not input->equation[iter]->garbage) {
							input->equation[iter]->garbage = true;
							garbage[0][len[0]] = input->equation[iter];
							len[0]++;
						}
						mu.unlock();
						iter++;
						break;
					case type_of_op::mul:
						if (irfunc) {
							output->number_value = output->number_value * input->equation[iter]->number_value;
						}
						else {
							iterator = output->size_of_equation - 1;
							trigger = true;
							while (iterator > loc_of_lif and trigger) {
								switch (output->equation[iterator]->typeofop) {
								case type_of_op::add:
									output->equation[iterator]->number_value *= input->equation[iter]->number_value;
									break;
								case type_of_op::sub:
									output->equation[iterator]->number_value *= input->equation[iter]->number_value;
									break;
								case type_of_op::mul:
									output->equation[iterator]->number_value *= input->equation[iter]->number_value;
									trigger = false;
									break;
								case type_of_op::div:
									output->equation[iterator]->number_value /= input->equation[iter]->number_value;
									trigger = false;
									break;
								default:
									break;
								}
								iterator--;
							}
							if (trigger) {
								output->size_of_equation++;
								REPLACEMENT = new MATH_UNIT * [output->size_of_equation];
								for (int i2 = 0; i2 < output->size_of_equation;i2++) {
									if (i2 < iterator + 1) {
										REPLACEMENT[i2] = output->equation[i2];
									}
									else if (i2 == iterator + 1) {
										REPLACEMENT[i2] = CREATE_NUM_UNIT(input->equation[iter]->number_value, type_of_op::mul);
									}
									else if (i2 > iterator + 1) {
										REPLACEMENT[i2] = output->equation[i2 - 1];
									}
								}

								delete[] output->equation;
								output->equation = REPLACEMENT;
							}
							iterator = 0;
							trigger = true;
						}
						mu.lock();
						if (not input->equation[iter]->garbage) {
							input->equation[iter]->garbage = true;
							garbage[0][len[0]] = input->equation[iter];
							len[0]++;
						}
						mu.unlock();
						iter++;
						break;
					case type_of_op::div:
						if (irfunc) {
							output->number_value = output->number_value / input->equation[iter]->number_value;
						}
						else {
							iterator = output->size_of_equation - 1;
							trigger = true;
							while (iterator > loc_of_lif and trigger) {
								switch (output->equation[iterator]->typeofop) {
								case type_of_op::add:
									output->equation[iterator]->number_value /= input->equation[iter]->number_value;
									break;
								case type_of_op::sub:
									output->equation[iterator]->number_value /= input->equation[iter]->number_value;
									break;
								case type_of_op::mul:
									output->equation[iterator]->number_value /= input->equation[iter]->number_value;
									trigger = false;
									break;
								case type_of_op::div:
									output->equation[iterator]->number_value *= input->equation[iter]->number_value;
									trigger = false;
									break;
								default:
									break;
								}
								iterator--;
							}
							if (trigger) {
								output->size_of_equation++;
								REPLACEMENT = new MATH_UNIT * [output->size_of_equation];
								for (int i2 = 0; i2 < output->size_of_equation;i2++) {
									if (i2 < iterator + 1) {
										REPLACEMENT[i2] = output->equation[i2];
									}
									else if (i2 == iterator + 1) {
										REPLACEMENT[i2] = CREATE_NUM_UNIT(input->equation[iter]->number_value, type_of_op::div);
									}
									else if (i2 > iterator + 1) {
										REPLACEMENT[i2] = output->equation[i2 - 1];
									}
								}
								delete[] output->equation;
								output->equation = REPLACEMENT;
							}
							iterator = 0;
							trigger = true;
							}
						mu.lock();
						if (not input->equation[iter]->garbage) {
							input->equation[iter]->garbage = true;
							garbage[0][len[0]] = input->equation[iter];
							len[0]++;
						}
						mu.unlock();
						iter++;
						break;
					case type_of_op::pow:
						if (irfunc) {
							output->number_value = pow(output->number_value, input->equation[iter]->number_value);
						}
						else {
							COMBINE(output, COPY(input->equation[iter]));
							loc_of_lif = output->size_of_equation - 1;
						}
						mu.lock();
						if (not input->equation[iter]->garbage) {
							input->equation[iter]->garbage = true;
							garbage[0][len[0]] = input->equation[iter];
							len[0]++;
						}
						mu.unlock();
						iter++;
						break;
					case type_of_op::log:
						if (irfunc) {
							output->number_value = log(output->number_value);
						}
						else {
							COMBINE(output, COPY(input->equation[iter]));
							loc_of_lif = output->size_of_equation - 1;
						}
						mu.lock();
						if (not input->equation[iter]->garbage) {
							input->equation[iter]->garbage = true;
							garbage[0][len[0]] = input->equation[iter];
							len[0]++;
						}
						mu.unlock();
						iter++;
						break;
					default:
						break;
					}
				}
				else {
					irfunc = false;
					COMBINE(output, input->equation[iter]);
					loc_of_lif = output->size_of_equation - 1;
					iter++;
				}
			}
			delete[] input->equation;
			input->equation = output->equation;
			input->size_of_equation = output->size_of_equation;
			delete[] input->parameters;
			input->parameters = output->parameters;
			delete[] input->loc_of_params;
			input->loc_of_params = output->loc_of_params;
			input->number_value = output->number_value;
			input->solve_in_parallel = false;
			delete[] output;
			mu.lock();
		}
		mu.unlock();
	}
	void set_as_false(MATH_UNIT* input) {
		if (input->solved) {
			for (int i = 0;i < input->size_of_equation; i++) {
				set_as_false(input->equation[i]);
			}
			input->solved = false;
		}
	}
	void SOLVE(MATH_UNIT* input) {
		int lenny = 0;
		revalue(input, &lenny);
		devalue(input);
		MATH_UNIT** garbage = new MATH_UNIT * [lenny];
		int jjj = 0;
		set_as_false(input);
		HIDDEN_3(input, &garbage, &jjj);
		for (int i = 0;i < jjj; i++) {
			deletemu(garbage[i], i);
		}
		delete[] garbage;
	}
	void SET_PARAM(MATH_UNIT* input, MATH_UNIT* param, long double value) {
		int yy = 0; 
		for (int i = 0; i < input->len_of_loc_of_params; i++) {
			if (input->parameters[i - yy] == param) {
				DELETE_V(&input->parameters, input->len_of_loc_of_params, i - yy);
				DELETE_V_I(&input->loc_of_params, input->len_of_loc_of_params, i - yy);
				input->len_of_loc_of_params--;
				yy++;
			}
		}
		for (int i = 0; i < input->size_of_equation; i++) {
			if (input->equation[i] == param) {
				input->equation[i] = CREATE_NUM_UNIT(value, param->typeofop);
			}
			SET_PARAM(input->equation[i], param, value);
		}
	}
	void PRINT(MATH_UNIT* input) {
		std::cout << input << "\n";
		std::cout << "Type: " << types[int(input->typeofop)] << "\n";
		std::cout << "Number val: " << input->number_value << "\n";
		std::cout << "unit type : " << int(input->typeofunit) << "\n";
		std::cout << "copied : " << input->copied << "\n";
		std::cout << "New Input\n";
		for (int i = 0; i < input->size_of_equation; i++) {
			std::cout << "Math Unit " << i << "\n";
			std::cout << "\n";
			
			PRINT(input->equation[i]);
		}
		std::cout << "End\n";
	}
};

using namespace unit_math;

int main() {
	long double value1 = 78;
	long double value2 = 89;

	unit_math::MATH_UNIT* num_unit = unit_math::CREATE_NUM_UNIT(0, unit_math::type_of_op::add);
	unit_math::MATH_UNIT* num_unit2 = unit_math::CREATE_PARAM_UNIT();
	unit_math::MATH_UNIT* param_unit = CREATE_NUM_UNIT(0,type_of_op::pow);
	COMBINE(num_unit, CREATE_NUM_UNIT(2, type_of_op::add));
	COMBINE(param_unit, num_unit2);
	COMBINE(num_unit, param_unit);
	

	MATH_UNIT* num_unit3 = DERIVATIVE(num_unit, num_unit2);

	MATH_UNIT* num_unit4 = COPY(num_unit);
	MATH_UNIT* num_unit5 = COPY(num_unit3);

	for (int i = 0; i < 50; i++) {
		

		SET_PARAM(num_unit4, num_unit2, i);
		SET_PARAM(num_unit5, num_unit2, i);

		SOLVE(num_unit4);
		SOLVE(num_unit5);
		std::cout << "x: " << i << "\n";
		std::cout << "y: " << num_unit4->number_value << "\n";
		std::cout << "y': " << num_unit5->number_value << "\n";

		DELETE(num_unit4);
		DELETE(num_unit5);

		num_unit4 = COPY(num_unit);
		num_unit5 = COPY(num_unit3);
	}

	//cout << "test test test\n";
	//cout << num_unit->number_value << "\n";
	//cout << value1 + value2 << "\n";

	return 0;
}
