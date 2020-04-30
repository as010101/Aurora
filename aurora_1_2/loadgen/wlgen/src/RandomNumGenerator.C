#include "RandomNumGenerator.H"

#include <iostream>			// for cerr
#include <sstream>			// for istringstream

using namespace std;

namespace workload_generator{

RandomNumGenerator::RandomNumGenerator(const distribution_t& par):
      _type(par.first), 
      _parameters(par.second),
      _input_file(0),
      _first_time(true),
      _open(false)
{
   if (checkDistribution(par) == true) {
      _valid = true;   
   }
   else {
      _valid = false;
   }

   i = 0;
}

RandomNumGenerator::~RandomNumGenerator()
{
   if (!_input_file) {
      delete _input_file;
   }
}

bool RandomNumGenerator::checkDistribution(const distribution_t& distr)
{
   distributiontype_t type = distr.first; 
   parlist_t parameters = distr.second;
   int par_size = parameters.size();

   switch (type) {
      case CONSTANT:
         {
            if (par_size != 1) {
               cerr << "Invalid number of parameters for CONSTANT"
                    << " distribution: "
                    << par_size
                    << endl;
               cerr << "Correct value shall be 1. "
                    << endl;
               cerr << "Parameter: constant value."
                    << endl;
               return false;
            }
            return true;
         }
      case UNIFORM:
         {
            if (par_size != 2) {
               cerr << "Invalid number of parameters for UNIFORM"
                    << " distribution: "
                    << par_size
                    << endl;
               cerr << "Correct value shall be 2. "
                    << endl;
               cerr << "Parameters: lower bound, upper bound"
                    << endl;
               return false;
            }
            if (parameters.front() > parameters.back()) {
               cerr << "Invalid parameters for UNIFORM"
                    << " distribution: "
                    << "lower bound ("
                    << parameters.front() << ")" 
                    << " upper bound ("
                    << parameters.back() << ")" 
                    << endl;
               cerr << "Lower bound must be less than upper bound."
                    << endl;
               return false;
            }
            return true;
         }
      case NORMAL:
         {
            if (par_size != 2) {
               cerr << "Invalid number of parameters for NORMAL"
                    << " distribution: "
                    << par_size
                    << endl;
               cerr << "Correct value shall be 2. "
                    << endl;
               cerr << "Parameters: mean, standard deviation."
                    << endl;
               return false;
            }
            if (parameters.back() < 0) {
               cerr << "Invalid parameters for NORMAL"
                    << " distribution: "
                    << "standard deviation ("
                    << parameters.back() << ")" 
                    << endl;
               cerr << "Standard deviation must be greater than zero."
                    << endl;
               return false;
            }
            return true;
         }
      case EXPONENTIAL:
         {
            if (par_size != 1) {
               cerr << "Invalid number of parameters for EXPONENTIAL"
                    << " distribution: "
                    << par_size
                    << endl;
               cerr << "Correct value shall be 1. "
                    << endl;
               cerr << "Parameter: mean. "
                    << endl;
               return false;
            }
            if (parameters.back() < 0) {
               cerr << "Invalid parameters for EXPONENTIAL"
                    << " distribution: "
                    << "mean ("
                    << parameters.back() << ")" 
                    << endl;
               cerr << "Mean must be greater than zero."
                    << endl;
               return false;
            }
            return true;
         }
      case LINEAR:
         {
            if (par_size != 4) {
               cerr << "Invalid number of parameters for LINEAR"
                    << " distribution: "
                    << par_size
                    << endl;
               cerr << "Correct value shall be 4. "
                    << endl;
               cerr << "Parameters: initial value, step, lower bound,"
                    << " upper bound."
                    << endl;
               return false;
            }
            parlist_it_t it = parameters.begin();
            // pass initial value
            ++it;  
            // pass step
            ++it;  
            double lower_bound = *it;
            ++it;
            double upper_bound = *it;
            if (lower_bound > upper_bound) {
               cerr << "Invalid parameters for LINEAR"
                    << " distribution: "
                    << "lower bound ("
                    << lower_bound << ")" 
                    << " upper bound ("
                    << upper_bound << ")" 
                    << endl;
               cerr << "Lower bound must be less than upper bound."
                    << endl;
               return false;
            }
            return true;
         }
      case RANDOMWALK:
         {
            if (par_size != 5) {
               cerr << "Invalid number of parameters for "
                    << "RANDOMWALK distribution: "
                    << par_size
                    << endl;
               cerr << "Correct value shall be 5. "
                    << endl;
               cerr << "Parameters: initial value, step, probability, "
                    << "lower bound, upper bound."
                    << endl;
               return false;
            }
            parlist_it_t it = parameters.begin();
            // pass initial value
            ++it;  
            // pass step
            ++it;  
            double prob = *it;
            ++it;
            double lower_bound = *it;
            ++it;
            double upper_bound = *it;

            if ((prob < 0) || (prob > 1)) {
               cerr << "Invalid parameters for RANDOMWALK"
                    << " distribution: "
                    << "probability("
                    << prob<< ")" 
                    << endl;
               cerr << "Probability must be between [0, 1]."
                    << endl;
               return false;
            }

            if (lower_bound > upper_bound) {
               cerr << "Invalid parameters for RANDOMWALK"
                    << " distribution: "
                    << "lower bound ("
                    << lower_bound << ")" 
                    << " upper bound ("
                    << upper_bound << ")" 
                    << endl;
               cerr << "Lower bound must be less than upper bound."
                    << endl;
               return false;
            }
            return true;
         }
      case RANDOMDIRECTIONWALK:
         {
            if (par_size != 5) {
               cerr << "Invalid number of parameters for "
                    << "RANDOMDIRECTIONWALK distribution: "
                    << par_size
                    << endl;
               cerr << "Correct value shall be 5. "
                    << endl;
               cerr << "Parameters: initial value, step, probability, "
                    << "lower bound, upper bound."
                    << endl;
               return false;
            }
            parlist_it_t it = parameters.begin();
            // pass initial value
            ++it;  
            // pass step
            ++it;  
            double prob = *it;
            ++it;
            double lower_bound = *it;
            ++it;
            double upper_bound = *it;

            if ((prob < 0) || (prob > 1)) {
               cerr << "Invalid parameters for RANDOMDIRECTIONWALK"
                    << " distribution: "
                    << "probability("
                    << prob<< ")" 
                    << endl;
               cerr << "Probability must be between [0, 1]."
                    << endl;
               return false;
            }

            if (lower_bound > upper_bound) {
               cerr << "Invalid parameters for RANDOMDIRECTIONWALK"
                    << " distribution: "
                    << "lower bound ("
                    << lower_bound << ")" 
                    << " upper bound ("
                    << upper_bound << ")" 
                    << endl;
               cerr << "Lower bound must be less than upper bound."
                    << endl;
               return false;
            }
            return true;
         }
      case TIMESTAMP:
         return true;
      case INPUTFILE:
         {
            if (par_size != 2) {
               cerr << "Invalid number of parameters for INPUTFILE"
                    << " distribution: "
                    << par_size
                    << endl;
               cerr << "Correct value shall be 2. "
                    << endl;
               cerr << "Parameter: filename, column."
                    << endl;
               return false;
            }
            return true;
         }
      case FILENAME:
         {
            if (par_size != 1) {
               cerr << "Invalid number of parameters for FILENAME"
                    << " distribution: "
                    << par_size
                    << endl;
               cerr << "Correct value shall be 1. "
                    << endl;
               cerr << "Parameter: filename."
                    << endl;
               return false;
            }
            return true;
         }
      case OTHER:
         return true;
      default:
         return false;
   }
}

double RandomNumGenerator::nextRand(double interval)
{
   if (_valid == false)
      return 0;
   switch (_type) {
      case CONSTANT:
         return _parameters.front();
      case UNIFORM:
         return 
            _parameters.front() + 
            drand48() * (_parameters.back() - _parameters.front());
      case NORMAL:
         return normal(_parameters.front(), _parameters.back());
      case EXPONENTIAL:
         return exponential(_parameters.front());
      case LINEAR:
         return linear(interval);
      case RANDOMWALK:
         return randomWalk(interval);
      case RANDOMDIRECTIONWALK:
         return randomDirectionWalk(interval);
      case INPUTFILE:
         return inputFile();
      case FILENAME:
         return _parameters.front();
      default:
         return 0;
   }
}

double RandomNumGenerator::getRandNum()
{
   return nextRand(1.0);
}

double RandomNumGenerator::getRandNum(double interval)
{
   return nextRand(interval);
}

double RandomNumGenerator::normal(double m, double s)
/* ========================================================================
 * Returns a normal (Gaussian) distributed real number.
 * NOTE: use s > 0.0
 *
 * Uses a very accurate approximation of the normal idf due to Odeh & Evans, 
 * J. Applied Statistics, 1974, vol 23, pp 96-97.
 * ========================================================================
 */
{ 
  const double p0 = 0.322232431088;     const double q0 = 0.099348462606;
  const double p1 = 1.0;                const double q1 = 0.588581570495;
  const double p2 = 0.342242088547;     const double q2 = 0.531103462366;
  const double p3 = 0.204231210245e-1;  const double q3 = 0.103537752850;
  const double p4 = 0.453642210148e-4;  const double q4 = 0.385607006340e-2;
  double u, t, p, q, z;

  u   = drand48();
  if (u < 0.5)
    t = sqrt(-2.0 * log(u));
  else
    t = sqrt(-2.0 * log(1.0 - u));
  p   = p0 + t * (p1 + t * (p2 + t * (p3 + t * p4)));
  q   = q0 + t * (q1 + t * (q2 + t * (q3 + t * q4)));
  if (u < 0.5)
    z = (p / q) - t;
  else
    z = t - (p / q);
  return (m + s * z);
}

double RandomNumGenerator::exponential(double m)
/* =========================================================
 * Returns an exponentially distributed positive real number. 
 * NOTE: use m > 0.0
 * =========================================================
 */
{
  return (-m * log(1.0 - drand48()));
}


double RandomNumGenerator::linear(double interval)
{
   parlist_it_t it = _parameters.begin();
   double last_value = *it;
   ++it;  
   double step  = *it;
   ++it;  
   double lower_bound = *it;
   ++it;
   double upper_bound = *it;

//   double new_value = last_value + step * interval;
   double new_value;
   if (_first_time) {
      new_value = last_value;
      _first_time = false;
   }
   else {
      new_value = last_value + step;
   }

   // check bound
   new_value = new_value >= lower_bound ? new_value : upper_bound;
   new_value = new_value <= upper_bound ? new_value : lower_bound;

   it = _parameters.begin();
   *it = new_value;

   return new_value;
}

double RandomNumGenerator::randomWalk(double interval)
{
   parlist_it_t it = _parameters.begin();
   double last_value = *it;
   ++it;  
   double step  = *it;
   ++it;  
   double prob = *it;
   ++it;  
   double lower_bound = *it;
   ++it;
   double upper_bound = *it;

   double new_value;
   if (drand48() > prob)
      step = -1 * step;

   new_value = last_value + step * interval;

   // check bound
   new_value = new_value >= lower_bound ? new_value : lower_bound;
   new_value = new_value <= upper_bound ? new_value : upper_bound;

   // record last value
   it = _parameters.begin();
   *it = new_value;

   return new_value;
}

double RandomNumGenerator::randomDirectionWalk(double interval)
{
   parlist_it_t it = _parameters.begin();
   double last_value = *it;
   ++it;  
   double step  = *it;
   ++it;  
   double prob = *it;
   ++it;  
   double lower_bound = *it;
   ++it;
   double upper_bound = *it;

   double new_value;
   if (drand48() > prob)
      step = -1 * step;

   new_value = last_value + step * interval;

   // check bound
   if (new_value < lower_bound){
      new_value = lower_bound;
      step = -1 * step;
   }
   if (new_value > upper_bound){
      new_value = upper_bound;
      step = -1 * step;
   }

   // record last value
   it = _parameters.begin();
   *it = new_value;
   // record direction
   ++it;
   *it = step;

   return new_value;
}

double RandomNumGenerator::inputFile()
{
   if (_first_time) {
      long file_address = static_cast<long>(_parameters.front());
      char* filename = reinterpret_cast<char*>(file_address);
      _input_file = new fstream;
      _input_file->open(filename, ios::in);
      if (!(*_input_file)) {
         cerr << "Can not open file: " << filename << endl;
         _open = false;
      }
      else {
         _open = true;
      }
      delete[] filename;
      _first_time = false;
   }

   if (_open) {
      static const int BUFFERSIZE = 1024 * 64;
      char buf[BUFFERSIZE];
      strcpy(buf, "");
      if (_input_file->peek() == EOF){
         _input_file->seekg(0, ios::beg);
      }
      _input_file->getline(buf, BUFFERSIZE);
      int i = 0;
      while (!strcmp(buf, "")) {
         if (_input_file->peek() == EOF){
            _input_file->seekg(0, ios::beg);
         }
         _input_file->getline(buf, BUFFERSIZE);
         ++i;
         if (i > 1000) {
            cout << "Invalid data file:" << endl; 
            cout << "No data contained or too many empty lines." << endl; 
            _input_file->close();
            _open = false;
            return 0;
         }
      }

      int column = static_cast<int>(_parameters.back());
      istringstream line(buf);
      double data = 0;
      char tmp;
      for (i = 1; i < column; ++i) {
         int j = 0;
         while (line.peek() == ' ' ||
                line.peek() == ',' ||
                line.peek() == ';' ){
            line >> tmp;
            ++j;
            if (j > 1000) {
               cout << "Invalid data file:" << endl;
               cout << "Invalid data line" << endl;
               _input_file->close();
               _open = false;
               return 0;
            }
         }

         if(!(line >> data)) {
            cout << "Invalid data file:" << endl;
            cout << "Invalid data line" << endl;
            _input_file->close();
            _open = false;
            return 0;
         }
      }
      
      int j = 0;
      while (line.peek() == ' ' ||
             line.peek() == ',' ||
             line.peek() == ';' ){
         line >> tmp;
         ++j;
         if (j > 1000) {
            cout << "Invalid data file:" << endl;
            cout << "Invalid data line" << endl;
            _input_file->close();
            _open = false;
            return 0;
         }
      }

      if (line >> data) {
         return data;
      }
      else {
         cout << "Invalie data file" << endl;
         _input_file->close();
         _open = false;
         return 0;
      }
   }

   return 0;
}

}
