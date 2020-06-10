/**
* Cristina Ruiz Martin
* ARSLab - Carleton University
*
* infective
*/

#ifndef _SUSCEPTIBLE_HPP__
#define _SUSCEPTIBLE_HPP__

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include <limits>
#include <assert.h>
#include <string>
#include <algorithm>


using namespace cadmium;
using namespace std;

//Port definition
struct susceptible_defs{
    struct report : public out_port<int> { };
    struct new_exposed : public out_port<int> { };
    struct in : public in_port<int> { };
};

enum SDL_STATE {EMPTY = 0, NON_EMPTY =1};

    template<typename TIME>
    class susceptible{
        public:     
            //Model parameters to be overload in the constructor
            int total_population;
            float transmission_rate;
            int initial_infective;
            float dt; //integrative time
            // default constructor
            susceptible() noexcept{
              assert(false && "susceptible (constructor): default constructor is not available");
            }
            susceptible(int i_total_population, float i_transmission_rate, int i_initial_infective, float i_dt){
            
                total_population = i_total_population;
                transmission_rate = i_transmission_rate;
                initial_infective = i_initial_infective;
                dt = i_dt;
                state.sdl_state = NON_EMPTY;
                state.total_susceptible = total_population - initial_infective;
                state.new_exposed = 0;
                state.report = true;
                state.info = true;
            }
            // state definition
            struct state_type{
                SDL_STATE sdl_state;
                int total_susceptible; 
                int new_exposed;
                bool report;
                bool info;
            }; 


            state_type state;
            // ports definition
            using input_ports=std::tuple<typename susceptible_defs::in>;
            using output_ports=std::tuple<typename susceptible_defs::report, susceptible_defs::new_exposed>;

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
                if(get_messages<typename susceptible_defs::in>(mbs).size()>1){
                    assert(false && "susceptible (external): there is more than one message in the bag");
                }
                state.report = true;  
                state.info  = true;
                int infective = get_messages<typename susceptible_defs::in>(mbs)[0];
                if(state.sdl_state == NON_EMPTY){
                    int new_exposed_auxiliary = (int)((transmission_rate*state.total_susceptible*infective)/total_population)*dt;
                    state.new_exposed = min(state.total_susceptible, new_exposed_auxiliary); 
                    state.total_susceptible -= state.new_exposed;
                    if(state.total_susceptible > 0){
                        state.sdl_state = NON_EMPTY;
                    }else{
                        state.sdl_state = EMPTY;
                    }
                }else{
                    state.new_exposed=0;
                    state.total_susceptible=0;
                }              
                             
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
                get_messages<typename susceptible_defs::report>(bags).push_back(state.total_susceptible);
              } 
              if(state.info == true){
                get_messages<typename susceptible_defs::new_exposed>(bags).push_back(state.new_exposed);
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

            friend std::ostringstream& operator<<(std::ostringstream& os, const typename susceptible<TIME>::state_type& i) {
                os << "<total_susceptible " << i.total_susceptible << ">"; 
            return os;
            }
        };     
  

#endif // _SUSCEPTIBLE_HPP__
  



