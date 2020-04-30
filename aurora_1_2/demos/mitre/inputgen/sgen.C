#include <stdlib.h>
#include <cmath>
#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>

#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#include <fstream>

#include "SocketClient.H"
 
const int X_MIN = 0;
const int X_MAX = 500;
const int Y_MIN = 0;
const int Y_MAX = 350;

const double BACK = 0.25;

const int SPEED_TANK = 2;
const int SPEED_AIRPLANE = 10;

const int OBJ_KIND = 5;
const int PERIODS = 3;

const double CHANGE_HEADING_PROB = 0.3;

struct Tuple {
   int	ID;
   int	X;
   int	Y;
   int	color;
   int t;
   int heading;
};

struct MoveInfo{
   int		x_min;
   int		x_max;
   int		y_min;
   int		y_max;
   int		x_step;
   int		y_step;
   int		heading;
   int		color;
};

int min(int a, int b) { return a < b? a : b ; }
int max(int a, int b) { return a > b? a : b ; }

int randHeading() {
   return (int)(drand48()*8);
}

main(int argc, char** argv)
{
   // read output filename and config file name from command line
   fstream outputfile;
   fstream configfile;
   if (argc == 3){
      outputfile.open(argv[1], ios::out|ios::trunc);
      configfile.open(argv[2], ios::in);
   }
   else if(argc == 2){
      outputfile.open(argv[1], ios::out|ios::trunc);
      configfile.open("config", ios::in);
   }
   else{
      outputfile.open("data.dat", ios::out|ios::trunc);
      configfile.open("config", ios::in);
   }

   // read configuration
   char pars[1024];

   int update_interval;
   configfile >> update_interval;
   configfile.getline(pars, sizeof(pars));

   int periods[PERIODS + 1];
   periods[0] = 0;
   for (int i = 1; i <= PERIODS; ++i){
      configfile >> periods[i];
      configfile.getline(pars, sizeof(pars));
   }
   int endtime = periods[PERIODS];

   int obj_nums[OBJ_KIND];
   int obj_num = 0; 
   for (int i = 0; i < OBJ_KIND; ++i){
      configfile >> obj_nums[i];
      configfile.getline(pars, sizeof(pars));
      obj_num += obj_nums[i];
   }

   int id_base[OBJ_KIND];
   for (int i = 0; i < OBJ_KIND; ++i){
      configfile >> id_base[i];
      configfile.getline(pars, sizeof(pars));
   }

   configfile.close();

/*
   cout << port << endl 
        << update_interval << endl ;
   cout << endl;
   for (int i = 1; i <= PERIODS; ++i){
      cout << periods[i] << endl;
   }
   cout << endl;
   for (int i = 0; i < OBJ_KIND; ++i){
      cout << obj_nums[i] << endl;
   }
   cout << endl;
   for (int i = 0; i < OBJ_KIND; ++i){
      cout << id_base[i] << endl;
   }
*/

   // setup move patten for each group

   MoveInfo move_info[PERIODS][OBJ_KIND];

   // by default all objects can move anywhere between y_min and y_max
   for (int i = 0; i < PERIODS; ++i){
      for (int j = 0; j < OBJ_KIND; ++j){
         move_info[i][j].y_min = Y_MIN;
         move_info[i][j].y_max = Y_MAX;
         move_info[i][j].heading = -1;
      }
   }
   
   // obj_kind1: foe aricraft back
   // randdom walk in the back, for all periods
   for (int i = 0; i < PERIODS; ++i){
      move_info[i][0].x_min = X_MIN + (int)((1 - BACK) *(X_MAX - X_MIN));
      move_info[i][0].x_max = X_MAX; 
      move_info[i][0].x_step = SPEED_AIRPLANE; 
      move_info[i][0].y_step = SPEED_AIRPLANE; 
      move_info[i][0].color = 1; 
   }

   // obj_kind2: foe aricraft front
   // period 1, random walk in y near the front. x fixed
   move_info[0][1].x_min = X_MIN + (X_MAX - X_MIN) / 2;
   move_info[0][1].x_max = X_MIN + (int)((1 - BACK) *(X_MAX - X_MIN));
   move_info[0][1].x_step = SPEED_AIRPLANE;
   move_info[0][1].y_step = SPEED_AIRPLANE;
   move_info[0][1].color = 4; 
   // period 2, heading west, y fixed, 
   // set speed such that all objects can pass front line in this period
   move_info[1][1].x_min = X_MIN;
   move_info[1][1].x_max = X_MAX;
   move_info[1][1].x_step = (move_info[0][1].x_max - 
                             move_info[0][1].x_min) / 
                             (periods[2] - periods[1]);
   move_info[1][1].y_step = 0;
   move_info[1][1].heading = 7;
   move_info[1][1].color = 6; 
   // period 3, continu west, speed normal
   move_info[2][1].x_min = X_MIN;
   move_info[2][1].x_max = X_MAX;
   move_info[2][1].x_step = move_info[1][1].x_step; 
   move_info[2][1].y_step = 0;
   move_info[2][1].heading = 7;
   move_info[2][1].color = 6; 

   // obj_kind3: foe tank back
   // randdom walk in the back, for all periods
   for (int i = 0; i < PERIODS; ++i){
      move_info[i][2].x_min = X_MIN + (int)((1 - BACK) *(X_MAX - X_MIN));
      move_info[i][2].x_max = X_MAX; 
      move_info[i][2].x_step = SPEED_TANK; 
      move_info[i][2].y_step = SPEED_TANK; 
      move_info[i][2].color = 3; 
   }

   // obj_kind4: foe tank front
   // period 1, random walk near the front. 
   move_info[0][3].x_min = X_MIN + (X_MAX - X_MIN) / 2;
   move_info[0][3].x_max = move_info[0][3].x_min + 
                           (int)((0.5 - BACK) * 0.5 *(X_MAX - X_MIN));
   move_info[0][3].x_step = SPEED_TANK;
   move_info[0][3].y_step = SPEED_TANK;
   move_info[0][3].color = 5; 
   // period 2, heading west 
   // set speed such that all objects can pass front line in this period
   move_info[1][3].x_min = X_MIN;
   move_info[1][3].x_max = X_MAX;
   move_info[1][3].x_step = (move_info[0][3].x_max - 
                             move_info[0][3].x_min) / 
                             (periods[2] - periods[1]);
   move_info[1][3].y_step = 0;
   move_info[1][3].heading = 7;
   move_info[1][3].color = 2; 
   // period 3, continu west, speed normal
   move_info[2][3].x_min = X_MIN;
   move_info[2][3].x_max = X_MAX;
   move_info[2][3].x_step = move_info[1][3].x_step; 
   move_info[2][3].y_step = 0;
   move_info[2][3].heading = 7;
   move_info[2][3].color = 2; 

   // obj_kind5: friendly aircraft back
   // randdom walk in the back, for all periods
   for (int i = 0; i < PERIODS; ++i){
      move_info[i][4].x_min = X_MIN;
      move_info[i][4].x_max = X_MIN + (int)(BACK * (double)(X_MAX - X_MIN)); 
      move_info[i][4].x_step = SPEED_AIRPLANE; 
      move_info[i][4].y_step = SPEED_AIRPLANE; 
      move_info[i][4].color = 0; 
   }
/*
   for (int i = 0; i < PERIODS; ++i){
      cout << endl;
      for (int j = 0; j < OBJ_KIND; ++j){
         cout << move_info[i][j].x_min << "   ";
         cout << move_info[i][j].x_max << "   ";
         cout << move_info[i][j].y_min << "   ";
         cout << move_info[i][j].y_max << "   ";
         cout << move_info[i][j].x_step << "   ";
         cout << move_info[i][j].y_step << "   ";
         cout << move_info[i][j].x_prob << "   ";
         cout << move_info[i][j].y_prob << "   ";
         cout << move_info[i][j].color << "   ";
         cout << endl;
      }
   }
   cout << endl;
   cout << endl;
*/

   // data[time_index][obj_index]
   // data holds all generated data
   Tuple** data;
   data = new Tuple*[endtime];
   for(int i = 0; i < endtime; ++i){
      data[i] = new Tuple[obj_num];
   }

   // generate initial data
   int m = 0;
   for (int i = 0; i < OBJ_KIND; ++i) {
      for (int j = 0; j < obj_nums[i]; ++j) {
         data[0][m].ID = id_base[i] + j;
         data[0][m].color = move_info[0][i].color;
         data[0][m].X = move_info[0][i].x_min + 
                        (int)(drand48() * (move_info[0][i].x_max - 
                                           move_info[0][i].x_min));
         data[0][m].Y = move_info[0][i].y_min + 
                        (int)(drand48() * (move_info[0][i].y_max - 
                                           move_info[0][i].y_min));
         data[0][m].t = 0;
         data[0][m].heading = randHeading();
         ++m;
      }
   }
  
   // generate data for all objects, in all periods
   for (int k = 0; k < PERIODS; ++k){
      for (int l = periods[k]; l < periods[k+1]; ++l){
         if (l == 0)
            continue;
         int m = 0;
         for (int i = 0; i < OBJ_KIND; ++i) {
            for (int j = 0; j < obj_nums[i]; ++j) {
               data[l][m].ID = id_base[i] + j;
               data[l][m].color = move_info[k][i].color;
               data[l][m].t = l * update_interval;

               // get heading
               if (move_info[k][i].heading == -1) {
                  if (drand48() < CHANGE_HEADING_PROB)
                     data[l][m].heading = randHeading();
                  else
                     data[l][m].heading = data[l-1][m].heading; 
               }
               else {
                  data[l][m].heading = move_info[k][i].heading; 
               }

               // move according to heading
               if (data[l][m].heading == 0 ||
                   data[l][m].heading == 1 || 
                   data[l][m].heading == 2)
                   data[l][m].Y = max(data[l-1][m].Y - move_info[k][i].y_step,
                                      move_info[k][i].y_min);
               else if(data[l][m].heading == 3 ||
                       data[l][m].heading == 4 || 
                       data[l][m].heading == 5)
                   data[l][m].Y = min(data[l-1][m].Y + move_info[k][i].y_step,
                                      move_info[k][i].y_max);
               else
                   data[l][m].Y = data[l-1][m].Y; 
               
               if (data[l][m].heading == 1 ||
                   data[l][m].heading == 4 || 
                   data[l][m].heading == 6)
                   data[l][m].X = min(data[l-1][m].X + move_info[k][i].x_step,
                                      move_info[k][i].x_max);
               else if(data[l][m].heading == 2 ||
                       data[l][m].heading == 5 || 
                       data[l][m].heading == 7)
                   data[l][m].X = max(data[l-1][m].X - move_info[k][i].x_step,
                                      move_info[k][i].x_min);
               else
                   data[l][m].X = data[l-1][m].X; 
               
               if (data[l][m].X == move_info[k][i].x_min) 
                 data[l][m].heading = (data[l][m].heading - 1 + 7) % 7;
               if (data[l][m].X == move_info[k][i].x_max) 
                 data[l][m].heading = (data[l][m].heading + 1) % 7;
               if (data[l][m].Y == move_info[k][i].y_min) 
                 data[l][m].heading = (data[l][m].heading + 3) % 7;
               if (data[l][m].Y == move_info[k][i].y_max) 
                 data[l][m].heading = (data[l][m].heading - 3 + 7) % 7;

               // next object 
               ++m;
            }
         }
      }
   }


   // write to outputfile
   for (int i = 0; i<endtime; ++i){
     for(int j=0; j< obj_num; ++j){
        outputfile << data[i][j].t << " ";
        outputfile << data[i][j].ID << " ";
        outputfile << data[i][j].X << " ";
        outputfile << data[i][j].Y << " ";
        outputfile << data[i][j].color << " ";
        outputfile << data[i][j].heading<< endl;
     }
   }
   outputfile.close();

   /*
   socket_tools::SocketClient client("localhost", port, SOCK_STREAM);
   int socket = client.connect_server();
   if (socket != -1){
      int t = 0;
      for (int i = 0; i<endtime; ++i){
        for(int j=0; j< obj_num; ++j){
           if(data[i][j].t != t){
             sleep(data[i][j].t - t);
             t = data[i][j].t;
           }
           write(socket, (char*)&(data[i][j]), sizeof(Tuple));
        }
      }
      close(socket);
   }
   */

   /*
   outputfile.open("data1.dat", ios::in); 
   Tuple tmp;
   char tmpchar[1024];
   while(!outputfile.eof()){
      outputfile >> tmp.t;
      outputfile >> tmp.ID;
      outputfile >> tmp.X;
      outputfile >> tmp.Y;
      outputfile >> tmp.color;

      cout << tmp.t << "      " 
           << tmp.ID  << "      "
           << tmp.X  << "      "
           << tmp.Y  << "      "
           << tmp.color  << "      "
           << endl;
   }
   outputfile.close();
   */
}

