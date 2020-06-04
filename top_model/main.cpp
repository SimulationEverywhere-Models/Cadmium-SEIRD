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

//C++ headers
#include <iostream>
#include <chrono>
#include <algorithm>
#include <string>
#include<fstream>

using namespace std;
using namespace cadmium;


using TIME = float;

/***** Define output ports for coupled model *****/
struct SEIRD_defs{
    struct susceptible : public out_port<int>{};
    struct exposed : public out_port<int>{};
    struct infective: public out_port<int>{};
    struct recovered : public out_port<int>{};
    struct deceased : public out_port<int>{};
};

/*************** Loggers *******************/
    static ofstream out_messages("../simulation_results/SEIRD_output_messages.txt");
    struct oss_sink_messages{
        static ostream& sink(){          
            return out_messages;
        }
    };
    static ofstream out_state("../simulation_results/SEIRD_output_state.txt");
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

   /* if (argc < 2) {
        cout << "Program used with wrong parameters. The program must be invoked as follow:";
        cout << argv[0] << " path to the input file " << endl;
        return 1; 
    }*/
    float mortality = 10.5;
    int infectivity_period = 14;
    float dt = 0.1;

    /****** recovered atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> pop_recovered = dynamic::translate::make_dynamic_atomic_model<accumulator, TIME>("recovered");

    /****** deceased atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> pop_deceased = dynamic::translate::make_dynamic_atomic_model<accumulator, TIME>("deceased");

    /****** infective atomic models instantiation *******************/
    shared_ptr<dynamic::modeling::model> pop_infective = dynamic::translate::make_dynamic_atomic_model<infective, TIME, float, int, float>("infective", 
                                                            move(mortality), move(infectivity_period), move(dt));
    
    /****** exposed atomic models instantiation *******************/
    //shared_ptr<dynamic::modeling::model> subnet2 = dynamic::translate::make_dynamic_atomic_model<Subnet, TIME>("subnet2");

    /****** susceptible atomic models instantiation *******************/

   
    /*******SEIRD COUPLED MODEL********/
    dynamic::modeling::Ports iports_SEIRD = {};
    dynamic::modeling::Ports oports_SEIRD  = {typeid(SEIRD_defs::susceptible),typeid(SEIRD_defs::exposed),
                    typeid(SEIRD_defs::infective),typeid(SEIRD_defs::recovered), typeid(SEIRD_defs::deceased)};
    dynamic::modeling::Models submodels_SEIRD  = {pop_recovered, pop_deceased, pop_infective};
    dynamic::modeling::EICs eics_SEIRD  = {};
    dynamic::modeling::EOCs eocs_SEIRD  = {
        dynamic::translate::make_EOC<accumulator_defs::report,SEIRD_defs::recovered>("recovered"),
        dynamic::translate::make_EOC<accumulator_defs::report,SEIRD_defs::deceased>("deceased"),
        dynamic::translate::make_EOC<infective_defs::total_infective,SEIRD_defs::infective>("infective")
    };
    dynamic::modeling::ICs ics_SEIRD  = {
        dynamic::translate::make_IC<infective_defs::deceased, accumulator_defs::in>("infective","deceased"),
        dynamic::translate::make_IC<infective_defs::recovered, accumulator_defs::in>("infective","recovered")
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