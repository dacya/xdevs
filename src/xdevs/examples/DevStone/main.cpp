#include <ctime>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <sys/resource.h>
#include <iostream>
#include <chrono>
#include "../../core/util/Constants.h"
#include "../../core/simulation/Coordinator.h"
#include "DevStoneGenerator.h"
#include "DevStoneCoupledLI.h"
#include "DevStoneCoupledHI.h"
#include "DevStoneCoupledHO.h"
#include "DevStoneCoupledHOmod.h"

#define BENCH_LI      0
#define BENCH_HI      1
#define BENCH_HO      2
#define BENCH_HOmod   3

const char* bench_names[]={"LI", "HI", "HO", "HOmod"};

void usage(char* name) {
  fprintf(stderr, "Usage: %s <-w width> <-d depth> [-m max_events=1] <-b {HI | HO | HOmod }>\nwith w>=2, d>=2, m>=1\n", name);
}

int get_gamma_i(int i, int width) {
  int res = (width - i);
  if (res < 0) {
    return 0;
  }
  return res;
}


long compute_external_transitions(int benchmark, int max_events, int width, int depth) {
  long num_events = 0;
  switch(benchmark) {
  case BENCH_LI:
    num_events = max_events * ((width - 1) * (depth - 1) + 1);
    break;
  case BENCH_HI:
    num_events = max_events * (((width * width - width) / 2) * (depth - 1) + 1);
    break;
  case BENCH_HO:
    num_events = max_events * (((width * width - width) / 2) * (depth - 1) + 1);
    break;
  case BENCH_HOmod: {
    int gamma1 = width - 1;
    int gamma2 = width * (width - 1) / 2;
    int gamma3 = gamma1;
    int delta1 = depth - 1;
    int delta3 = (depth - 1) * (depth - 2) / 2;
    num_events =  max_events * (1 + delta1 * gamma1 * gamma1 + (delta1 + gamma3 * delta3) * (gamma1 + gamma2));
  }
    break;
  default: // We assume HO here
    num_events = max_events * (((width * width - width) / 2) * (depth - 1) + 1);
    break;
  }
  return num_events;
}

long compute_internal_transitions(int benchmark, int max_events, int width, int depth) {
  long num_events = 0;
  switch(benchmark) {
  case BENCH_LI:
    num_events = compute_external_transitions(benchmark, max_events, width, depth);
    break;
  case BENCH_HI:
    num_events = compute_external_transitions(benchmark, max_events, width, depth);
    break;
  case BENCH_HO:
    num_events = compute_external_transitions(benchmark, max_events, width, depth);
    break;
  case BENCH_HOmod:
    num_events = compute_external_transitions(benchmark, max_events, width, depth);
    break;
  default: // We assume HO here
    num_events = compute_external_transitions(benchmark, max_events, width, depth);
  }
  return num_events;
}

long compute_total_events(int benchmark, int max_events, int width, int depth) {
  long num_events = 0;
  switch(benchmark) {
  case BENCH_LI:
    num_events = compute_external_transitions(benchmark, max_events, width, depth);
    break;
  case BENCH_HI:
    num_events = compute_external_transitions(benchmark, max_events, width, depth);
    break;
  case BENCH_HO:
    num_events = compute_external_transitions(benchmark, max_events, width, depth);
    break;
  case BENCH_HOmod: {
    long temp_events = 1;
    int gamma1 = get_gamma_i(1, width);
    int k = 1; // k for level 1
    long* bag{new long[1 + 1 + depth * gamma1]{}}; // Maximum number of propagations
    // (asumming first index is 1, index 0 is not used)
    bag[1] = 1; // bag for level 1
    for (int d = 1; d < depth; ++d) {
      // Update numEvents
      long* next_bag{new long[1 + 1 + depth * gamma1]{}}; // Maximum number of propagations
      // (asumming first index is 1, index 0 is not used)
      for (int c = 1; c <= (k + gamma1); ++c) {
	long popSum1 = 0;
	long popSum2 = 0;
	for (int i = 1; i <= width; ++i) {
	  if (c - i + 1 > 0) {
	    popSum1 += gamma1 * bag[c - i + 1];
	    popSum2 += get_gamma_i(i, width) * bag[c - i + 1];
	  }
	}
	temp_events += (popSum1 + popSum2);
	next_bag[c] = popSum1;
      }
      // Compute k and bag for the next level
      k += gamma1;
      delete [] bag;
      bag = next_bag;
    }
    num_events =  max_events * temp_events;
    delete [] bag;
  }
    break;
  default: // We assume HO here
    num_events = compute_external_transitions(benchmark, max_events, width, depth);
  }
  return num_events;
}

int main(int argc, char *argv[]) {
  if(argc==1) {
    usage(argv[0]);
    exit(EXIT_FAILURE);
  }
  // Common parameters
  double preparationTime = 0.0;
  double period = 1.0;
  long maxEvents = 1;
  double intDelayTime = 0;
  double extDelayTime = 0;
  struct rusage resources;
  int benchmark = -1;
  int width = -1, depth = -1;
  int opt;
	
  // TODO
  // Evaluate parameteres: benchmark, width and depth
  while ((opt = getopt(argc, argv, "w:d:b:m:")) != -1) {
    switch (opt) {
    case 'w':
      width = atoi(optarg);
      break;
    case 'd':
      depth = atoi(optarg);
      break;
    case 'b':
      if (strncmp(optarg, "LI", 2) == 0) {
	benchmark = BENCH_LI;
      } else if (strncmp(optarg, "HI", 2) == 0) {
	benchmark = BENCH_HI;
      } else if (strncmp(optarg, "HOmod", 5) == 0) { // Be carefull!, this comparison must go before the next one!
	benchmark = BENCH_HOmod;
      } else if (strncmp(optarg, "HO", 2) == 0) {
	benchmark = BENCH_HO;
      } else {
	usage(argv[0]);
	exit(EXIT_FAILURE);
      }
      break;
    case 'm':
      maxEvents = atoi(optarg);
      break;
    default: /* '?' */
      usage(argv[0]);
      exit(EXIT_FAILURE);
    }
  }
  
  if ((width < 2) || (depth < 2) || (maxEvents < 1)) {
    usage(argv[0]);
    exit(EXIT_FAILURE);
  }
	
  Coupled framework(bench_names[benchmark]);
  DevStoneGenerator generator("Generator", preparationTime, period, 1);
  framework.addComponent(&generator);
  Coupled* coupledStone = 0;

  auto ts_start = std::chrono::steady_clock::now();

  switch (benchmark) {
    
    /*********************************************************************************************/
    // LI
  case BENCH_LI:
    coupledStone = new DevStoneCoupledLI("C", width, depth, preparationTime, intDelayTime, extDelayTime);
    framework.addComponent(coupledStone);
    framework.addCoupling(&generator, &generator.oOut, coupledStone, &((DevStoneCoupledLI*)coupledStone)->iIn);
    break;
  case BENCH_HI:
    coupledStone = new DevStoneCoupledHI("C", width, depth, preparationTime, intDelayTime, extDelayTime);
    framework.addComponent(coupledStone);
    framework.addCoupling(&generator, &generator.oOut, coupledStone, &((DevStoneCoupledHI*)coupledStone)->iIn);
    break;
  case BENCH_HO:
    coupledStone = new DevStoneCoupledHO("C", width, depth, preparationTime, intDelayTime, extDelayTime);
    framework.addComponent(coupledStone);
    framework.addCoupling(&generator, &generator.oOut, coupledStone, &((DevStoneCoupledHO*)coupledStone)->iIn);
    framework.addCoupling(&generator, &generator.oOut, coupledStone, &((DevStoneCoupledHO*)coupledStone)->iInAux);
    break;
  case BENCH_HOmod:
    coupledStone = new DevStoneCoupledHOmod("C", width, depth, preparationTime, intDelayTime, extDelayTime);
    framework.addComponent(coupledStone);
    framework.addCoupling(&generator, &generator.oOut, coupledStone, &((DevStoneCoupledHOmod*)coupledStone)->iIn);
    framework.addCoupling(&generator, &generator.oOut, coupledStone, &((DevStoneCoupledHOmod*)coupledStone)->iInAux);
    break;
  }

  auto ts_end_model_creation = std::chrono::steady_clock::now();

  //struct timespec ts_start1, ts_start2, ts_end;
  //	time_t start = clock();
  Coordinator coordinator(&framework);
  coordinator.initialize();
  
  auto ts_end_engine_setup = std::chrono::steady_clock::now();
  
  coordinator.simulate(std::numeric_limits<double>::infinity());

  auto ts_end_simulation = std::chrono::steady_clock::now();
  
  coordinator.exit();
  delete coupledStone;
  //	time_t end = clock();
  //	double time = (double)(end-start)/CLOCKS_PER_SEC;
  double time_model = ((ts_end_model_creation - ts_start).count()) * std::chrono::steady_clock::period::num / static_cast<double>(std::chrono::steady_clock::period::den);
  double time_engine = ((ts_end_engine_setup - ts_end_model_creation).count()) * std::chrono::steady_clock::period::num / static_cast<double>(std::chrono::steady_clock::period::den);
  double time_sim = ((ts_end_simulation - ts_end_engine_setup).count()) * std::chrono::steady_clock::period::num / static_cast<double>(std::chrono::steady_clock::period::den);
  
  if (getrusage(RUSAGE_SELF, &resources)) {
    perror ("rusage");
  }
  
  std::cout << "STATS\n";
  std::cout << "Benchmark: " << bench_names[benchmark] << " (" << benchmark << ")" << std::endl;
  std::cout << "PreparationTime: " << preparationTime << std::endl;
  std::cout << "Period: " << period << std::endl;
  std::cout << "MaxEvents: " << maxEvents << std::endl;
  std::cout << "Width: " << width << std::endl;
  std::cout << "Depth: " << depth << std::endl;
  std::cout << "IntDelayTime: " << intDelayTime << std::endl;
  std::cout << "ExtDelatTime: " << extDelayTime << std::endl;
  std::cout << "Num delta_int: " << DevStoneAtomic::NUM_DELT_INTS << ", [" << compute_internal_transitions(benchmark, maxEvents, width, depth) << "]" << std::endl;
  std::cout << "Num delta_ext: " << DevStoneAtomic::NUM_DELT_EXTS << ", [" << compute_external_transitions(benchmark, maxEvents, width, depth)  << "]" << std::endl;
  std::cout << "Num event_ext: " << DevStoneAtomic::NUM_EVENT_EXTS << ", [" << compute_total_events(benchmark, maxEvents, width, depth) << "]" << std::endl;
  std::cout << "Model creation time (s): " << time_model << std::endl;
  std::cout << "Engine setup time (s): " << time_engine << std::endl;
  std::cout << "Simulation time (s): " << time_sim << std::endl;
  std::cout << "TOTAL time (s): " << time_model + time_engine + time_sim << std::endl;
  std::cout << "MEMORY (KiB): " << resources.ru_maxrss << std::endl;
  
  return 0;
}

