/**
* Cristina Ruiz Martin
* ARSLab - Carleton University
*
* infective
*/

#ifndef _EXPOSED_HPP__
#define _EXPOSED_HPP__

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include <limits>
#include <assert.h>
#include <string>


using namespace cadmium;
using namespace std;

//Port definition
    struct exposed_defs{
        struct report : public out_port<int> { };
        struct total_exposed : public out_port<int> { };
        struct new_infective : public out_port<int> { };
        struct in : public in_port<int> { };
    };
        


    template<typename TIME>
    class exposed{
        public:     
            //Model parameters to be overload in the constructor
            int incubation_period;
            float dt; //integrative time
            // default constructor
            exposed() noexcept{
              assert(false && "exposed (constructor): default constructor is not available");
            }
            exposed(int i_incubation_period, float i_dt){
            
                incubation_period = i_incubation_period;
                dt = i_dt;
                state.total_exposed = 0;
                state.new_infective = 0;
                state.report = true;
                state.info = true;
            }
            // state definition
            struct state_type{
                int total_exposed;
                int new_infective; 
                bool report;
                bool info;
            }; 



            state_type state;
            // ports definition
            using input_ports=std::tuple<typename exposed_defs::in>;
            using output_ports=std::tuple<typename exposed_defs::report, 
                exposed_defs::total_exposed, exposed_defs::new_infective>;

            // internal transition
            void internal_transition() {
              if(state.report == true){
                state.report  = false;
              } 
              if(state.info == true){
                state.info  = false;
              } 
            }

            // external transition
            void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
                if(get_messages<typename exposed_defs::in>(mbs).size()>1){
                    assert(false && "Infective (external): there is more than one message in the bag");
                }
                int new_exposed = get_messages<typename exposed_defs::in>(mbs)[0];
                state.new_infective = (state.total_exposed/incubation_period)*dt;
                state.total_exposed = state.total_exposed + new_exposed - state.new_infective;
                state.report = true;  
                state.info  = true;                        
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
                get_messages<typename exposed_defs::report>(bags).push_back(state.total_exposed);
              } 
              if(state.info == true){
                get_messages<typename exposed_defs::new_infective>(bags).push_back(state.new_infective);
              } 
              return bags;
            }

            // time_advance function
            TIME time_advance() const { 
                TIME next_internal;
                if(state.report == true || state.info == true){
                    next_internal  = TIME(dt);
                }else{
                    next_internal = std::numeric_limits<TIME>::infinity();
                }   
                return next_internal;
            }

            friend std::ostringstream& operator<<(std::ostringstream& os, const typename exposed<TIME>::state_type& i) {
                os << "<total_exposed " << i.total_exposed << ">"; 
            return os;
            }
        };     
  

#endif // __EXPOSED_HPP__

  



