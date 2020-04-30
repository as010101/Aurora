#ifndef _WORKLOAD_GENERATOR_H_
#define _WORKLOAD_GENERATOR_H_


// Local headers
#include "SourceGenerator.H"	
#include "SourcePool.H"
#include "StreamBuffer.H"
#include "WGController.H"
#include "StreamSender.H"
#include "AggStreamSource.H"
#include "AggSourceFileReader.H"


using namespace std;
namespace workload_generator {

class WorkloadGenerator
{
public:
   WorkloadGenerator();
   WorkloadGenerator(const char* aurora_receiver_name, int port);

   // Start a command line interface
   // This interface provide all the functions the user needs
   void startCommandReceiver();

   // For connection to Aurora input receiver 
   // Default protocle TCP
   void connectAurora();
   void connectAurora(const char* aurora_receiver_name, int port);
   void disconnectAurora();

   // For GUI command receiver server 
   void addAggSource(AggStreamSource& agg_source);
   void changeRate(double ratio);
   void changRate(AggStreamSource& agg_source);
   void emptySourses();

   // For file based command 
   void readAggInfoFile(char* filename);
   void readAggInfoFiles(char* filename);
   void changRageByFile(char* filename);
   void printAggInfoFile(char* filename);
   void printPartialAggInfoFile(char* filename);

   // For control command
   void deleteAggSource(int numberID);
   void deleteAggSource(char* stringID);

private:
   SourceGenerator		_source_generator;
   SourcePool			_source_pool;
   StreamBuffer			_stream_buffer; 
   WGController			_controller;
   StreamSender			_stream_sender;

   void initialize();
   void printUsage();
};

}
#endif
