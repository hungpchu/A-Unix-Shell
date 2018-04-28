/*
 * Copyright (c) 2018, Yutaka Tsutano
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/*

NAME: HUNG CHU
NUID: 95060612

*/

#include <iostream>
#include <string>
#include <vector>
#include "command.hpp"
#include "parser.hpp"
#include <sys/wait.h>
#include <wait.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>


   int exec(const std::string& cmd, const std::vector<std::string>& args)
     
     {
                     // Make an ugly C-style args array.
         std::vector<char*> c_args = {const_cast<char*>(cmd.c_str())};
        for (const auto& a : args) {
            c_args.push_back(const_cast<char*>(a.c_str()));
         }
        c_args.push_back(nullptr);

        return execvp(cmd.c_str(), c_args.data());
     
     }


ostream_mode prevPipe;
int main(int argc, char *argv[])
{
    std::string input_line;
    int childId, out, out1, in;
    int status = 0;
    int pipeID = 0;
    int pipeID1 = 0;
    int checkStatus;
    std::string com;
    const char *fileIn;
    const char *fileOut;
    istream_mode inputMode;
    ostream_mode  outputMode;
    std::vector<std::string> argument;
    next_command_mode nextMode, prevMode;
    bool disablePrint = false;
    int  pipefd[2];

     
               
    for (;;) {

      // check for "-t"
     if ( argv[1] != NULL ){

        disablePrint = true;

     }

        // Print the prompt.
        if ( disablePrint == false){
        
        std::cout<< "osh> " << std::flush;

    }

        // Read a single line.
        if (!std::getline(std::cin, input_line) || input_line == "exit") {
            break;
        }

        try {
               

            // Parse the input line.
            std::vector<shell_command> shell_commands
                    = parse_command_string(input_line);

            prevPipe = ostream_mode::term;
           checkStatus = 0;
           prevMode = next_command_mode::always;
           
         // std::cout << "-------------------------\n";
          
            // for loop to iterate through each commands
            for (const auto & cmd : shell_commands) {
              
               // std::cout << cmd;
               
                 // std::cout << "-------------------------\n";    

              com = cmd.cmd;
              argument = cmd.args;
              inputMode = cmd.cin_mode; 
              outputMode = cmd.cout_mode;
              fileIn = cmd.cin_file.c_str();
              fileOut = cmd.cout_file.c_str();
              nextMode = cmd.next_mode;

              // check for the exec in phase 3
             if ( ( checkStatus != 0 && prevMode == next_command_mode::on_success ) || ( checkStatus == 0 && prevMode == next_command_mode::on_fail )  ){

                        status = checkStatus;
                        break;
                    
              }


          // create pipefd
          if ( pipe(pipefd) == -1 ) {

              perror(" fail to create pipe(pipefd)\n ");

          }

            // create child process to exec
            childId = fork();
               
            if ( childId == 0){

                    // > redirect In
                    if ( outputMode == ostream_mode::file   ){

                        out = open(fileOut, O_WRONLY | O_CREAT , 0700);
                         dup2(out, 1);
                         close(out);

                    }
                    // >> 
                     if ( outputMode == ostream_mode::append ){


                        out1 = open(fileOut,O_APPEND | O_CREAT| O_RDWR,0700);
                        dup2(out1, 1);
                        close(out1);
                    }
                    // <
                     if ( inputMode == istream_mode::file ){

                        in = open(fileIn,O_RDONLY , 0700);          
                        dup2(in, 0);
                        close(in);

                    }

                // check inputMode for pipe
                if ( inputMode ==  istream_mode::pipe){

                      dup2(pipeID, 0);
                     close(pipefd[0]);

                }

                // check outputMode for pipe
                if ( outputMode == ostream_mode::pipe){
      
                    prevPipe = ostream_mode::pipe;
                     dup2(pipefd[1], 1);
                     close(pipefd[1]);
                    
                }
                             
                    exec(com, argument);       
                               
                }

                else{

                    wait(&status);
                    pipeID = pipefd[0];
                    pipeID1 = pipefd[1];

                    // turn the output of command back to terminal
                    if ( outputMode != ostream_mode::term ){

                    dup2(1, pipeID1);

                }
                
            // check for status of command in phase 3   
            if ( nextMode == next_command_mode::on_success || nextMode == next_command_mode::on_fail ){

                    prevMode = nextMode;
                    checkStatus = status;
                  
              }
                    
                }

            }// for each command


        }// try loop
     
        catch (const std::runtime_error& e) {

            //std::cout << "osh: " << e.what() << "\n";
              std::cout << e.what() << "\n";
        }
    }// end for   

    if ( disablePrint == false){

    std::cout << std::endl;
  
  }

}