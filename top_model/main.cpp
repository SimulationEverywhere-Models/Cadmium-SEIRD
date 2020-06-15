//Cadmium Simulator headers
#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/dynamic_model.hpp>
#include <cadmium/modeling/dynamic_model_translator.hpp>
#include <cadmium/engine/pdevs_dynamic_runner.hpp>
#include <cadmium/logger/common_loggers.hpp>

//Time class header
#include <NDTime.hpp>


//Atomic model headers
#include "../atomics/accumulator.hpp"
#include "../atomics/infective.hpp"
#include "../atomics/exposed.hpp"
#include "../atomics/susceptible.hpp"

//C++ headers
#include <iostream>
#include <iomanip>
#include <chrono>
#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>

using namespace std;
using namespace cadmium;


using TIME = float;

/***** Define output ports for coupled model *****/
struct SEIRD_defs{
    struct susceptible : public out_port<float>{};
    struct exposed : public out_port<float>{};
    struct infective: public out_port<float>{};
    struct recovered : public out_port<float>{};
    struct deceased : public out_port<float>{};
};

/*************** Loggers *******************/
    static ofstream out_messages("simulation_results/SEIRD_output_messages.txt");
    struct oss_sink_messages{
        static ostream& sink(){          
            return out_messages;
        }
    };
    static ofstream out_state("simulation_results/SEIRD_output_state.txt");
    struct oss_sink_state{
        static ostream& sink(){          
            return out_state;
        }
    };
    
    using state=logger::logger<logger::logger_state, dynamic::logger::formatter<TIME>, oss_sink_state>;
    using log_messages=logger::logger<logger::logger_messages, dynamic::logger::formatter<TIME>, oss_sink_messages>;
    using global_time_mes=logger::logger<logger::logger_global_time, dynamic::logger::formatter<TIME>, oss_sink_messages>;
    using global_time_sta=logger::logger<logger::logger_global_time, dynamic::logger::formatter<TIME>, oss_sink_state>;

    using logger_top=logger::multilogger<state, log_messages, global_time_mes, global_time_sta>;
/******************************************************/

int main(int argc, char ** argv) {
   
  float mortality;
	float infectivity_period;
	float dt;
	float incubation_period;
	int total_population;
	int initial_infective;
	float transmission_rate;
	ifstream inputReader;
	
	//default filename if none is provided
	if(argc<2){
		inputReader.open("input_data/input.txt");
	}else{ //branch for providing filename
		std::string temp = argv[1];
		std::string filePath = "input_data/" + temp;
		inputReader.open(filePath);
	}
	
	//reading input from file
	if(inputReader.is_open()){
		std::string line; //store read in line
		
		//these blocks read the input for each variable and ensure that they have been provided
		getline(inputReader, line);
		line = line.substr(line.find("=")+1);
		if(line.empty() || line[1]==' '|| int(line[1])==13 || int(line[0])==13){ //the comparison to 13 here is to the ascii value of the carriage return character
			assert(false && "Please check the mortality input");
			//abort();
		}else{
			//cout<<"this is the line: "<<line<<endl;
			//cout<<"got here"<<endl;
			mortality = std::stof(line);
			//cout<<"got here"<<endl;
		}
		
		getline(inputReader, line);
		line = line.substr(line.find("=")+1);
		if(line.empty() || line[1]==' '|| int(line[1])==13 || int(line[0])==13){
			assert(false && "Please check the infectivity period input");
		}else{
			infectivity_period = std::stof(line);
		}
		
		getline(inputReader, line);
		line = line.substr(line.find("=")+1);
		if(line.empty() || line[1]==' '|| int(line[1])==13 || int(line[0])==13){
			assert(false && "Please check the dt input");
		}else{
			dt = std::stof(line);
		}
		
		getline(inputReader, line);
		line = line.substr(line.find("=")+1);
		if(line.empty() || line[1]==' '|| int(line[1])==13 || int(line[0])==13){
			assert(false && "Please check the incubation period input");
		}else{
			incubation_period = std::stof(line);
		}
		
		getline(inputReader, line);
		line = line.substr(line.find("=")+1);
		if(line.empty() || line[1]==' '|| int(line[1])==13 || int(line[0])==13){
			assert(false && "Please check the total population input");
		}else{
			total_population = std::stoi(line);
		}
		
		getline(inputReader, line);
		line = line.substr(line.find("=")+1);
		if(line.empty() || line[1]==' '|| int(line[1])==13 || int(line[0])==13){
			assert(false && "Please check the initial infective input");
		}else{
			initial_infective = std::stoi(line);
		}
		
		getline(inputReader, line);
		line = line.substr(line.find("=")+1);
		if(line.empty() || line[1]==' '|| int(line[1])==13 || int(line[0])==13){
			assert(false && "Please check the transmission rate input");
		}else{
			transmission_rate = std::stof(line);
		}
		
		//cout<<mortality<<","<<infectivity_period<<","<<dt<<","<<incubation_period<<","<<total_population<<","<<initial_infective<<","<<transmission_rate<<endl;
		
		inputReader.close();
	}
	



    /****** recovered atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> pop_recovered = dynamic::translate::make_dynamic_atomic_model<accumulator, TIME>("recovered");

    /****** deceased atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> pop_deceased = dynamic::translate::make_dynamic_atomic_model<accumulator, TIME>("deceased");

    /****** infective atomic models instantiation *******************/
    shared_ptr<dynamic::modeling::model> pop_infective = dynamic::translate::make_dynamic_atomic_model<infective, TIME, int, float, float, float>("infective", 
                                                           move(initial_infective), move(mortality), move(infectivity_period), move(dt));
    
    /****** exposed atomic models instantiation *******************/
    shared_ptr<dynamic::modeling::model> pop_exposed = dynamic::translate::make_dynamic_atomic_model<exposed, TIME, float, float>("exposed", 
                                                            move(incubation_period), move(dt));
    /****** susceptible atomic models instantiation *******************/
     shared_ptr<dynamic::modeling::model> pop_susceptible = dynamic::translate::make_dynamic_atomic_model<susceptible, TIME, int, float, int, float>("susceptible", 
                                                            move(total_population), move(transmission_rate), move(initial_infective), move(dt));
   
    /*******SEIRD COUPLED MODEL********/
    dynamic::modeling::Ports iports_SEIRD = {};
    dynamic::modeling::Ports oports_SEIRD  = {typeid(SEIRD_defs::susceptible),typeid(SEIRD_defs::exposed),
                    typeid(SEIRD_defs::infective),typeid(SEIRD_defs::recovered), typeid(SEIRD_defs::deceased)};
    dynamic::modeling::Models submodels_SEIRD  = {pop_recovered, pop_deceased, pop_infective, pop_exposed, pop_susceptible};
    dynamic::modeling::EICs eics_SEIRD  = {};
    dynamic::modeling::EOCs eocs_SEIRD  = {
        dynamic::translate::make_EOC<accumulator_defs::report,SEIRD_defs::recovered>("recovered"),
        dynamic::translate::make_EOC<accumulator_defs::report,SEIRD_defs::deceased>("deceased"),
        dynamic::translate::make_EOC<infective_defs::report,SEIRD_defs::infective>("infective"),
        dynamic::translate::make_EOC<exposed_defs::report,SEIRD_defs::exposed>("exposed"),
        dynamic::translate::make_EOC<susceptible_defs::report,SEIRD_defs::susceptible>("susceptible")
    };
    dynamic::modeling::ICs ics_SEIRD  = {
        dynamic::translate::make_IC<susceptible_defs::new_exposed, exposed_defs::in>("susceptible","exposed"),
        dynamic::translate::make_IC<exposed_defs::new_infective, infective_defs::in>("exposed","infective"),
        dynamic::translate::make_IC<infective_defs::deceased, accumulator_defs::in>("infective","deceased"),
        dynamic::translate::make_IC<infective_defs::recovered, accumulator_defs::in>("infective","recovered"),
        dynamic::translate::make_IC<infective_defs::total_infective, susceptible_defs::in>("infective","susceptible")    
    };
    shared_ptr<dynamic::modeling::coupled<TIME>> SEIRD ;
    SEIRD  = make_shared<dynamic::modeling::coupled<TIME>>(
        "SEIRD ", submodels_SEIRD , iports_SEIRD , oports_SEIRD , eics_SEIRD , eocs_SEIRD , ics_SEIRD  
    );


    
    /************** Runner call ************************/ 
    dynamic::engine::runner<float, logger_top> r(SEIRD, {0});
    r.run_until(3000);
    return 0;
}