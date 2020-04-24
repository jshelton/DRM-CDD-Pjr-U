
#include <iostream>
#include "Hungarian.h"



using namespace std;
/**
   Sends a string of the contents of the matrix to output stream

   @param output stream: stream to output to 
   @param matrix or vector of vectors

   @return output stream
*/

const bool latex = true;

void latexOutput ( ostream &o, vector<vector<double> >& C,  vector<vector<double> >& answers, int linePosition ) 
{
  int i,j;

  // bool latex = true;

  o << endl;
  
  if (latex){
    o << "\\[" << endl << "\\begin{bmatrix}" ;

    if (C.size() != 0  && linePosition > 0 && linePosition < C[0].size() ){
      o << "[";
      for (i = 0; i < C[0].size(); i++) {
	if ( i == linePosition )
	  o << "|";
	o << "c";
      }
      o << "]";


    }
    
    o << " %drm position: " << linePosition;
    
    o << endl;
  }

  for(i=0; i< C.size(); i++) 
    {
      if (!latex) 
	o << " [";
		

      bool latexFirst = true;
      for(j=0; j< C[i].size(); j++) 
	{

	  if (latex) {
	    if (latexFirst){
	      if (answers[i][j]==1)
		o << "\\textbf{" << C[i][j] << "}";
	      else
		o << C[i][j];
	      latexFirst = false;
	    }
	    else {
	      if (answers[i][j]==1)
		o << " & \\textbf{" << C[i][j] << "}";
	      else
		o << " & " << C[i][j] ;
	    }
	    
	  }
	  else o << C[i][j] << ' ';

	  
	}
      if (latex) o << "\\\\" << endl;
      else o << "]" << endl;

		
    }


  if (latex) o << "\\end{bmatrix}" << endl << "\\]" << endl;
  o << endl;

}


ostream &operator << ( ostream &o,  vector<vector<double> >& C) 
{
  int i,j;
  //  bool latex = true;

  o << endl;
  
  if (latex) o << "\\[" << endl << "\\begin{bmatrix}" << endl;

  for(i=0; i< C.size(); i++) 
    {
      if (!latex) 
	o << " [";
		

      bool latexFirst = true;
      for(j=0; j< C[i].size(); j++) 
	{

	  if (latex) {
	    if (latexFirst){
	      o << C[i][j];
	      latexFirst = false;
	    }
	    else o << " & " << C[i][j] ;
	    
	  }
	  else o << C[i][j] << ' ';

	  
	}
      if (latex) o << "\\\\" << endl;
      else o << "]" << endl;

		
    }


  if (latex) o << "\\end{bmatrix}" << endl << "\\]" << endl;
  o << endl;

  return o;
}




/**
   Determines the weight of amount of time the DRM takes based on the
   position it occurs it.

   @param position that the drm occurs (0 for beginning and >n for not occurring)

   @return 
*/

class DRMCalculator{
public:
  float rate;
  float base;

  void setParams(float newRate, float newBase){
    rate = newRate;
    base = newBase;
  }

  const double MaintenanceAt(int position){
    return (rate * position + base);
  }
};

int main(void)
{
  // please use "-std=c++11" for this initialization of vector.

  /// INPUT DATA

  float dueDate;
  cin >> dueDate;
  
  // Create the DRM Formula
  float rate, base;
  cin >> rate >> base;
  DRMCalculator drmCalc; drmCalc.setParams(rate,base);

  int dimension_j, dimension_r;
  cin >> dimension_j >> dimension_r;

  // Pjr is the Processing time of per job per position before DRM
  vector <vector<double> > Pjr(dimension_j);

  for (int j = 0; j < dimension_j; j++){
    Pjr[j].resize(dimension_r);
    for (int i = 0; i < dimension_r; i++){
      cin >> Pjr[j][i];
    }
  }


  cout << Pjr << endl;
  
  // Qjr is the Processing time of per job per position after DRM
  vector <vector<double> > Qjr(dimension_j);


  for (int j = 0; j < dimension_j; j++){
    Qjr[j].resize(dimension_r);
    for (int i = 0; i < dimension_r; i++){
      cin >> Qjr[j][i];
    }
  }


  cout << Qjr << endl;

  // Keep Track of the best result

  double bestMakeSpan = -1;
  int bestDRMPosition = -1;
  int bestOnTimeJobs = -1;
  

  for ( int drmPosition = 0; drmPosition <= Pjr[0].size(); drmPosition++ )
    {
      //      bool costSet = false;
      double makespan = 0;

      cout << endl << "Next we will evaluate using rate modification that occurs just before the job at the position " << drmPosition+1 <<  ". " << "That is, " << (latex?"$":"") <<"r = " << drmPosition+1 << (latex?"$":"") << "." << endl;


      vector< vector<double> > drmSolution(
					   Pjr.size()  ,
					   vector<double>());


      vector< vector<double> > drmPotentialSolution(
						    Pjr.size()  ,
						    vector<double>());

      // Just print the combined matrix
      for (int j = 0; j < Pjr[0].size(); j++){
	for (int i = 0; i < drmPotentialSolution.size() ; i++){
	  if ( j <  drmPosition){
	    drmPotentialSolution[i].push_back(Pjr[i][j]);
	  } else {
	    drmPotentialSolution[i].push_back(Qjr[i][j]);
	  }
	}
      }

      for (int j = 0; j < Pjr[0].size() && makespan <= dueDate; j++){
	for (int i = 0; i < drmSolution.size() ; i++){
	  if ( j <  drmPosition){
	    drmSolution[i].push_back(Pjr[i][j]);
	  } else {
	    drmSolution[i].push_back(Qjr[i][j]);
	  }
	}

	


	HungarianAlgorithm HungAlgo;
	vector<int> assignment;
	
	double maintTime;

	if ( drmPosition < Pjr.size())
	  maintTime = 0;
	else
	  drmCalc.MaintenanceAt(drmPosition);
	  
	makespan = HungAlgo.Solve(drmSolution, assignment) + maintTime;


	if (makespan <= dueDate && bestOnTimeJobs < j){
	  bestOnTimeJobs = j;
	  bestDRMPosition = drmPosition;
	}
	  
	// if (!costSet || bestMakeSpan > makespan ) {
	//   costSet = true;
	//   bestMakeSpan = makespan;
	//   bestDRMPosition = drmPosition;

	// }


	vector< vector<double> > answer(
					drmSolution.size()  ,
					vector<double>(drmSolution[0].size()) );

	for (int i = 0; i < answer.size(); i++)
	  for (int j = 0; j < answer[i].size(); j++)
	    answer[i][j] = 0;

	// cout << "Job schedule: ";
      
	for (unsigned int x = 0; x < drmSolution.size(); x++){
	  if (assignment[x] != -1)
	  {
	    answer[x][assignment[x]] = 1;
	    //	    std::cout << "job " << x << ": " << assignment[x] << ", ";
	  }
	}

	// // 	    cout << (answer);

	//	cout << "Iteration " << j<< endl;
	latexOutput(cout, drmSolution, answer, drmPosition);
	latexOutput(cout, drmPotentialSolution, answer, drmPosition);

	if (makespan > dueDate){
	  cout << "Here we have at least one late job, so we will stop adding more potential jobs.";
	}

	{

	  cout << endl << "Thus far, the number of on-time jobs was ";
	  cout << (latex?"$":"") << bestOnTimeJobs+1 << (latex?"$":"") << ", ";

	  cout <<  "and the minimum time required to process " << (j+1) << " jobs with the given DRM position is ";
	  cout << (latex?"$":"") << makespan <<  (latex?"$":"");

	  cout << " (the time required for maintenance is ";
	  cout << (latex?"$":"") << maintTime <<  (latex?"$":"") << ")." << endl;
	}

      }
	  
      // cout << drmSolution;
      // cout << endl << "Best number of On time jobs:" << bestOnTimeJobs+1 << endl <<"makespan: " << makespan <<  endl;

    }

	
  cout << endl;
  //  cout << "Best Makespan resulting: " << bestMakeSpan << endl;
  cout << "The maximum number of on time jobs is " << (latex?"$":"") << bestOnTimeJobs+1 << (latex?"$":"") <<". ";
  cout << "This happens when the rate modification occurs just before processing the job at the position " << bestDRMPosition+1 << ". That is " << (latex?"$":"") << "r = " << bestDRMPosition+1 << (latex?"$":"") << ". " << endl;


  return 0;
}


