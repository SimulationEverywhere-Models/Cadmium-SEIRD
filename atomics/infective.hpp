/**
* Cristina Ruiz Martin
* ARSLab - Carleton University
*
* infective
*/

#ifndef __INFECTIVE_HPP__
#define __INFECTIVE_HPP__


#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include <limits>
#include <assert.h>
#include <string>


using namespace cadmium;
using namespace std;

//Port definition
    struct infective_defs{
        struct report : public out_port<int> { };
        struct total_infective : public out_port<int> { };
        struct deceased : public out_port<int> { };
        struct recovered : public out_port<int> { };
        struct in : public in_port<int> { };
    };
        

    template<typename TIME>
    class infective{
        public:     
            //Model parameters to be overload in the constructor
            float mortality;
            int infectivity_period;
            float dt; //integrative time
            // default constructor
            infective() noexcept{
              assert(false && "Infective (constructor): default constructor is not available");
            }
            infective(float i_mortality, int i_infectivity_period, float i_dt){
                mortality = i_mortality;
                infectivity_period = i_infectivity_period;
                dt = i_dt;
                state.total_infective = 0;
                state.new_deceased = 0;
                state.new_recovered = 0;
                state.report = false;
                state.info_infective = false;
                state.info_deceased = false;
                state.info_recovered = false;
            }
            // state definition
            struct state_type{
                int total_infective;
                int new_deceased; 
                int new_recovered;
                bool report;
                bool info_infective;
                bool info_deceased;
                bool info_recovered;

            }; 
            state_type state;
            // ports definition
            using input_ports=std::tuple<typename infective_defs::in>;
            using output_ports=std::tuple<typename infective_defs::report, 
                infective_defs::total_infective, infective_defs::deceased, infective_defs::recovered>;

            // internal transition
            void internal_transition() {
              if(state.report == true){
                state.report  = false;
              } 
              if(state.info_infective == true){
                state.info_infective  = false;
              } 
              if(state.info_deceased == true){
                state.info_deceased  = false;
              } 
              if(state.info_recovered == true){
                state.info_recovered  = false;
              } 
            }

            // external transition
            void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
                if(get_messages<typename infective_defs::in>(mbs).size()>1){
                    assert(false && "Infective (external): there is more than one message in the bag");
                }
                int new_infective= get_messages<typename infective_defs::in>(mbs)[0];
                state.new_deceased = ((state.total_infective/infectivity_period)*(mortality/100))*dt;
                state.new_recovered = ((state.total_infective/infectivity_period)*(1-(mortality/100)))*dt;
                state.total_infective += new_infective - state.new_deceased - state.new_recovered;
                state.report = true;  
                state.info_infective  = true;
                state.info_deceased  = true;
                state.info_recovered  = true;                         
            }

            // confluence transition
            void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
                internal_transition();
                external_transition(TIME(), std::move(mbs));
            }

            // output function
            typename make_message_bags<output_ports>::type output() const {
              typename make_message_bags<output_ports>::type bags;           
              
              if(state.report == true){
                get_messages<typename infective_defs::report>(bags).push_back(state.total_infective);
              } 
              if(state.info_infective == true){
                get_messages<typename infective_defs::total_infective>(bags).push_back(state.total_infective);
              } 
              if(state.info_deceased == true){
                get_messages<typename infective_defs::deceased>(bags).push_back(state.new_deceased);
              } 
              if(state.info_recovered == true){
                get_messages<typename infective_defs::recovered>(bags).push_back(state.new_recovered);
              } 
              return bags;
            }

            // time_advance function
            TIME time_advance() const { 
                TIME next_internal;
                if(state.report == true || state.info_infective == true || state.info_deceased  == true
                        || state.info_recovered  == true){
                    next_internal  = TIME(0);
                }else{
                    next_internal = std::numeric_limits<TIME>::infinity();
                }   
                return next_internal;
            }

            friend std::ostringstream& operator<<(std::ostringstream& os, const typename infective<TIME>::state_type& i) {
                os << "<total_infective: " << i.total_infective << ">"; 
            return os;
            }
        };     
  

#endif // __INFECTIVE_HPP__

  



