/**
* Cristina Ruiz Martin
* ARSLab - Carleton University
*
* Accumulator
*/

#ifndef __ACCUMULATOR_HPP__
#define __ACCUMULATOR_HPP__


#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include <limits>
#include <assert.h>
#include <string>


using namespace cadmium;
using namespace std;

//Port definition
    struct accumulator_defs{
        struct report : public out_port<int> { };
        struct in : public in_port<int> { };
    };
   
    template<typename TIME>
    class accumulator{
        public:     

            // default constructor
            accumulator() noexcept{
              state.total_amount    = 0;
              state.report     = true;
            }
            // state definition
            struct state_type{
              int total_amount;
              bool report;
            }; 
            state_type state;
            // ports definition
            using input_ports=std::tuple<typename accumulator_defs::in>;
            using output_ports=std::tuple<typename accumulator_defs::report>;

            // internal transition
            void internal_transition() {
              if(state.report == true){
                state.report  = false;
              } 
            }

            // external transition
            void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
                if(get_messages<typename accumulator_defs::in>(mbs).size()>1){
                    assert(false && "Accumulator (external): there is more than one message in the bag");
                }
                state.total_amount += get_messages<typename accumulator_defs::in>(mbs)[0];
               
                state.report = true;                           
            }

            // confluence transition
            void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
                internal_transition();
                external_transition(TIME(), std::move(mbs));
            }

            // output function
            typename make_message_bags<output_ports>::type output() const {
              typename make_message_bags<output_ports>::type bags;           
              get_messages<typename accumulator_defs::report>(bags).push_back(state.total_amount);
              return bags;
            }

            // time_advance function
            TIME time_advance() const { 
              TIME next_internal;
               if(state.report == true){
                next_internal  = TIME(0);
              }else{
                next_internal = std::numeric_limits<TIME>::infinity();
              }   
              return next_internal;
            }

            friend std::ostringstream& operator<<(std::ostringstream& os, const typename accumulator<TIME>::state_type& i) {
                os << "<total_amount: " << i.total_amount << ">"; 
            return os;
            }
        };     
  

#endif // __ACCUMULATOR_HPP__