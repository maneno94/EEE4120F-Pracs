//==============================================================================
// Copyright (C) John-Philip Taylor
// tyljoh010@myuct.ac.za
//
// This file is part of the EEE4084F Course
//
// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNmakeU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>
//
// This is an adaptition of The "Hello World" example avaiable from
// https://en.wikipedia.org/wiki/Message_Passing_Interface#Example_program
//==============================================================================


/** \mainpage Prac3 Main Page
 *
 * \section intro_sec Introduction
 *
 * The purpose of Prac3 is to learn some basics of MPI coding.
 *
 * Look under the Files tab above to see documentation for particular files
 * in this project that have Doxygen comments.
 */



//---------- STUDENT NUMBERS --------------------------------------------------
//
// Please note:  put your student numbers here !!  <<<< NB!  NB!
//
//-----------------------------------------------------------------------------

/* Note that Doxygen comments are used in this file. */
/** \file Prac3
 *  Prac3 - MPI Main Module
 *  The purpose of this prac is to get a basic introduction to using
 *  the MPI libraries for prallel or cluster-based programming.
 */

// Includes needed for the program
#include "Prac3.h"

/** This is the master node function, describing the operations
    that the master will be doing */
void Master () 
{
       //! <h3>Local vars</h3>
       // The above outputs a heading to doxygen function entry
       //int window [] = new int (9);       //2D Median filtering example using a 3 x 3 sampling window
       //std::array<int, 9> window = {7, 4, 2, 8, 6, 1, 9, 0, 3}; 
       int  j;             //! j: Loop counter
       // char buff[BUFSIZE]; //! buff: Buffer for transferring message data
       MPI_Status stat;    //! stat: Status of the MPI application





       // Read the input image
       if(!Input.Read("Data/fly.jpg"))
       {
             printf("Cannot read image\n");
             return;
       }
       
       int Np=floor(Input.Height/(numprocs-1));
       int Mp=floor(Input.Width*Input.Components);
       int size= (Np)*(Mp);


        //int count=1;
       // Start of "Hello World" example..............................................
       printf("0: We have %d processors\n", numprocs);
       for(j = 1; j < numprocs; j++) 
       {
             
             int  buff0[3] = {Np, Mp,1};
             int * buff = new int[size];

             //int x = ((j-1)*Mp); x < (Mp+((j-1)*Mp)); x++
             //int y=0; y<Np; y++
            
             int index=0;
             for (int y = ((j-1)*Np); y < (Np+((j-1)*Np)); y++)
             {
               
         
                  for ( int x = 0; x < Mp; x++)
                  {
       
                           buff[index++] = Input.Rows[y][x];
                  } 
                  
         
             }

             //printf("size1:  %d %d  \n",  index, size);
 
  
             //printf("size1:  %d \n",  buff[1055]);
             MPI_Send(buff0, 3, MPI_INT, j, TAG0, MPI_COMM_WORLD);
             
             //printf("Hello %d \n", j);
       

             //printf("size1:  %d \n",  buff[709]);
             MPI_Send(buff, size, MPI_INT, j, TAG1, MPI_COMM_WORLD);
             //printf("size1:  %d \n",  buff[1055]);
             delete [] buff;
             
       }
       
       
       
       // Allocated RAM for the output image
       if(!Output.Allocate(Input.Width, Input.Height, Input.Components)) 
          return;


         // int count=0;
      //Read from slave
       for(j = 1; j < numprocs; j++) 
       {
       
       
            
             //printf("yes\n"); 
             //int Np=Input.Height;
             //int Mp=floor(Input.Width*Input.Components/(numprocs-1));
             int * buff1 = new int [size];

             

             // This is blocking: normally one would use MPI_Iprobe, with MPI_ANY_SOURCE,
             // to check for messages, and only when there is a message, receive it
             // with MPI_Recv.  This would let the master receive messages from any
             // slave, instead of a specific one only.
             MPI_Recv(buff1, size, MPI_INT, j, TAG1, MPI_COMM_WORLD, &stat);
            
            
             int index2=0;

             // int count=0;           
             for (int y = ((j-1)*Np); y < (Np+((j-1)*Np)); y++)
             {
                  //int count=0;
                  //printf("coulumn:  %d\n", count++); 
                  //int index2=0;
                  for ( int x = 0; x < Mp; x++)
                  {
                         
                        Output.Rows[y][x] = buff1[index2++];
                         
                  } 
             }
             
             delete [] buff1;

      }
     
     // printf("yes\n");  
      
      if(!Output.Write("Data/Output.jpg"))
      {
           printf("Cannot write image\n");
           return;
      }
}
  

//------------------------------------------------------------------------------

/** This is the Slave function, the workers of this MPI application. */
void Slave(int ID)
{
   // Start of "Hello World" example..............................................
   std::array<int, 5> window = { 2, 8, 6,0,0};
   MPI_Status stat;
   int buff0 [3] = {};


   
   MPI_Recv(buff0, 3, MPI_INT, 0, TAG0, MPI_COMM_WORLD, &stat);
   
   int Mp = buff0[1];
   int Np = buff0[0];
   int c = (Np)*(Mp);
   int * buff = new int [c];
     
   MPI_Recv(buff, c, MPI_INT, 0, TAG1, MPI_COMM_WORLD, &stat);
   
   //printf("size2:  %d \n",  buff[1055] );

   
   
   //printf("yes\n"); 
   //int p=0; 
   for (int y=0; y<Np; y++)
   {
       //printf("yes\n");
       for (int x=0; x<Mp; x++)
       {
             //printf("yes\n");
             if(x==0 || y==0 || x==Mp-1 || y==Np-1)
             {
     //            p++;
             }
             else
             { 
             //window[0] = buff[(y-1)*Mp + x-1];
             window[0] = buff[(y-1)*Mp+x]; 
             //window[2] = buff[(y-1)*Mp+x+1]; 
             window[1] = buff[y*Mp+x-1]; 
             window[2] = buff[y*Mp+x]; 
             window[3] = buff[y*Mp+x+1]; 
             //window[6] = buff[(y+1)*Mp+x-1]; 
             window[4] = buff[(y+1)*Mp+x]; 
             //window[8] = buff[(y+1)*Mp+x+1];
    
             }
             //  sort using the default operator<
             //printf("yes\n");
             std::sort(window.begin(), window.end()); 
             buff[y*Mp+x] = window[2];
             //printf("yes\n");
      }
      
      //printf("p is %d\n", p++); 
      
   }



   
   
   // send to rank 0 (master):
   MPI_Send(buff, c, MPI_INT, 0, TAG1, MPI_COMM_WORLD);
   
   delete[] buff;

    // End of "Hello World" example................................................
    
}




//------------------------------------------------------------------------------

/** This is the entry point to the program. */
int main(int argc, char** argv)
{
   int myid;

   // MPI programs start with MPI_Init
   MPI_Init(&argc, &argv);

   // find out how big the world is
   MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

   // and this processes' rank is
   MPI_Comm_rank(MPI_COMM_WORLD, &myid);

   // At this point, all programs are running equivalently, the rank
   // distinguishes the roles of the programs, with
   // rank 0 often used as the "master".
   //printf("%d\n", myid);
   if(myid == 0)
   {
     printf(" master id: %d\n", myid); 
     Master();
     
   }
        
   else
   { 
     printf(" slave id: %d\n", myid);         
     Slave (myid);
     
   }

   // MPI programs end with MPI_Finalize
   MPI_Finalize();
   return 0;
  
}
//------------------------------------------------------------------------------
