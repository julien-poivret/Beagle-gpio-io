#include <iostream>
#include <cstdlib>

/*              
   - Program tested.
   - This program is a personal tool for driving with ease
      the gpio on a beagel black from linux terminal 
      ( as always perfectly suitable for iot not for time critical task. )
     
     Quick help:
            compiled out name  | 0->127   | direction | on/off if out
     bash$:     programename   | pinumber | in/out    |     1/0

*/

struct GPIO {
    unsigned short gpio_pin;
    char direction[4];
    unsigned short value;
};

int main(int argc,char* argv[])
{
    if(argc > 1 ) {
	/*
	   main principe:
	   The first argument is the gpio pin number so we need first to check 
	   if the gpio range is valid, since we handling the arguments in char* argv[]
	   for arithmetical range evaluation, the ascii table is so offseted down to 48 units
	   in order to match the '0'->(value key of 48)  with  0->(valu key of 0).
	*/
        struct GPIO data;
        size_t count = 0;
	// if the byte is a valid numerical representation from 48 to 57 then the length of the value is so counted.
        while(argv[1][count] != 0 && count<3) {
            if (argv[1][count]>47 && argv[1][count]<58) {
                count++;
            }
            else {
                std::cout<<"The first argument must be a valid gpio number."<<std::endl;
                return EXIT_FAILURE;
            }
        }
        data.gpio_pin = 0;
        unsigned short factor = 1;
	// Make a big number, digit by digit... factor fit the scale range before incrementation
	// (1*1) + (1*10) + (1*100) = 111 , (5*1) + (3*10) + (7*100) = 735 ... easy 
        while(count>0) {
            data.gpio_pin += ((unsigned short) argv[1][count-1] - 48) * factor;
            factor *= 10;
            count--;
        }
	// gpio must be in a valid gpio pin range from 0 to 127 (hardware constraint).
        if(data.gpio_pin>=0 && data.gpio_pin<128) {
            count = 0;
	    // check the integrity of the second user argument.
            while(argv[2][count] != 0 && count<2 && argc >= 2) {
                    count++;
            }
            if(count != 3 && count != 2) {
                std::cout<<"The second argument is invalid must be \"in\" or \"out\" ->"<<count<<std::endl;
                return EXIT_FAILURE;
            }
            bool valid = false;
            bool input = false;
            count = 2;
            char out[4] = "out";
            char in[3] = "in";
            // Set the data structure with te right match in or out (low level code)
            while(count>0) {
                if(out[count] == argv[2][count]) {
                    count--;
                    if(!count) {
                        data.direction[0] ='o';
                        data.direction[1] ='u';
                        data.direction[2] ='t';
                        data.direction[3] ='\0';
                        valid = true;
                    }
                    continue;
                }
                else {
                    break;
                }
            }
            if(count) {
                count = 1;
                while(count>=0) {
                    if(in[count] == argv[2][count]) {
                        count--;
                        if(!count) {
                            data.direction[0] ='i';
                            data.direction[1] ='n';
                            data.direction[2] ='\0';
                            data.direction[3] ='\0';
                            valid = true;
                            input = true;
                        }
                        continue;
                    }
                    else {
                        break;
                    }
                }
            }
            if(valid) {
                // print the data structure on screen. 
                std::cout<<"result: Ok"<<std::endl
                         <<data.gpio_pin<<std::endl
                         <<data.direction<<std::endl;
                //Handle the third argument 1 or 0 .
                if (!input) {
                    if(argc<=3) {
                        std::cout<<"if gpio pin \"out\" mode then a value of 0 or 1 must be specified"<<std::endl;
                        return EXIT_FAILURE;
                    }
                    if((argv[3][0] != 48) && (argv[3][0] != 49)) {
                        std::cout<<"The third argument must be 1 or 0"<<std::endl;
                    }
                    else {
                        if (argv[3][0] == 48) {
                            data.value = 0;
                        }
                        else if (argv[3][0] == 49) {
                            data.value = 1;
                        }
                        std::cout<<"value: "<<data.value<<std::endl;
                    }
                }
            }
            else {
                std::cout<<"The second argument should be \"in\" or \"out\""<<std::endl;
                return EXIT_FAILURE;
            }
        }
        else {
            std::cout<<"\33[1;1H\33[2JThe GPIO number must be betwin 0 and 127"<<std::endl;
        }
	// format the terminal command before the call in background (behind the data structure summary on screen neat). 
        // stack buffer a bit to hight but ok.
	char test[100]="";
        std::sprintf((char*) &test, "echo %s > /sys/class/gpio/gpio%d/direction",data.direction,data.gpio_pin);
	system(test);
        std::sprintf((char*) &test, "echo %d > /sys/class/gpio/gpio%d/value",data.value,data.gpio_pin);
	system(test);
    }
    else {
        std::cout<<"\33[1;1H\33[2JPlease enter valid arguments like \"gpio pin in/out 1/0\" \n( the \
	third argument is optional only if arg 2 is out )"<<std::endl;
    }
    return EXIT_SUCCESS;
}
