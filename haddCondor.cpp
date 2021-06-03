


#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

class jobset{
	public:
	int _njobs;
	int _layer;
	int _nCombine;
	std::string _path;
	std::vector<std::vector<std::string> > jobinputs{};//each job has alist of inputs
	std::vector<std::string> joboutputs{}; //each job has 1 output
	jobset(std::vector<std::string> inputlist, int nCombine, int layer, std::string path );
	void printjobs();
//	void WriteJobs(std::ofstream& dagfile, std::string exe, std::string path);
	void WriteJobs( std::string exe, std::string outputpath);	
};
jobset::jobset(std::vector<std::string> inputlist, int nCombine, int layer, std::string path = ""){
	_layer = layer;
	_nCombine = nCombine;
	_path = path;	
	std::vector<std::string> inputs{};
	int jobctr=1;
	for(int i=0; i<inputlist.size(); i++){
		if( i!=0 && i%nCombine == 0){
			jobinputs.push_back( inputs );
			joboutputs.push_back( path+"f_"+std::to_string(layer)+"_"+std::to_string(jobctr)+".root" );
			jobctr++;
			inputs.clear();
		}
		inputs.push_back(inputlist[i]);
		if( inputlist.size()%nCombine != 0 && i == inputlist.size()-1 ){
			jobinputs.push_back( inputs );
			joboutputs.push_back( path+"f_"+std::to_string(layer)+"_"+std::to_string(jobctr)+".root" );
			jobctr++;
			inputs.clear();
		}
	}
	if( nCombine == inputlist.size() ){
		jobinputs.push_back( inputlist );
		joboutputs.push_back(path+"f_"+std::to_string(layer)+"_"+std::to_string(jobctr)+".root" );
	}
	
	_njobs = joboutputs.size();

	
}
void jobset::printjobs(){
	std::cout<<" layer: "<<_layer<<" nCombine: "<<_nCombine<<" nJobs: "<<_njobs<<std::endl;
	for(int i=0; i<_njobs; i++){
		std::cout<<"job "<< i+1<< " inputs: \n ";
		for(int j=0; j<jobinputs[i].size(); j++){
			std::cout<< jobinputs.at(i).at(j) <<std::endl;;
		}
		std::cout<<"outputs \n "<<joboutputs.at(i)<<std::endl;
		std::cout<<std::endl;
	}
}
/* wrote inline, seems condor isnt configured to handle this format
void jobset::WriteJobs(std::ofstream& dagfile, std::string exe, std::string path){
	std::string jobname;
	for(int i=0; i<_njobs; i++){
		jobname = "J"+std::to_string(_layer)+"_"+std::to_string(i+1);
		dagfile<< "JOB "+jobname+" {\n    ";
		dagfile<< "Exectuable = "+path+exe+"\n    ";
		dagfile<< "Arguments  = "+joboutputs.at(i)+" ";
		for(int j=0; j<jobinputs.at(i).size(); j++){
			dagfile<<jobinputs.at(i).at(j)<<" ";
		}
		dagfile<<"\n    ";
		dagfile<< "Log        = "+path+"log/log.log\n    ";
		dagfile<< "Output     = "+path+"output/"+jobname+".output\n    ";
		dagfile<< "Error      = "+path+"error/"+jobname+".err\n    ";
		dagfile<< "Universe   = vanilla\n";
		dagfile<<"}\n \n";
	}

}
*/
void jobset::WriteJobs( std::string exe, std::string outputpath){
	std::string jobname;
	for(int i=0; i<_njobs; i++){
                jobname = "l"+std::to_string(_layer)+"j"+std::to_string(i+1);
		std::ofstream subfile;
		subfile.open(outputpath+"/sub/"+jobname+".submit");
                subfile<< "Executable = "+exe+"\n";
                subfile<< "Arguments  = "+joboutputs.at(i)+" ";
                for(int j=0; j<jobinputs.at(i).size(); j++){
                        subfile<<jobinputs.at(i).at(j)<<" ";
                }
                subfile<<"\n";
		subfile<<"getenv = True\n";
    		subfile<<"use_x509userproxy = true\n";
    		subfile<<"Requirements = (Machine != \"red-node000.unl.edu\")\n";
                subfile<< "Log        = "+outputpath+"/logs/log.log\n";
                subfile<< "Output     = "+outputpath+"/logs/"+jobname+".output\n";
                subfile<< "Error      = "+outputpath+"/logs/"+jobname+".err\n";
                subfile<< "Universe   = vanilla\n";
                subfile<<"Queue\n";
		subfile.close();
        }


} 
int main(int argc, char *argv[]){

	//arguments:
	//1= outputdirectory name
	//2= path with .x and source
	//3= inputlist
	//4= nCombine
	


	std::string OutputDirName = std::string( argv[1] ); //test5
	std::string path= std::string( argv[2] ); ///home/t3-ku/janguian/dagtest/daghadd/
	std::string intpath = path+"/"+OutputDirName+"/root/";
	system(("rm -rf "+OutputDirName).c_str());
	system(("mkdir "+OutputDirName).c_str());
	system(("mkdir "+OutputDirName+"/sub").c_str());
	system(("mkdir "+OutputDirName+"/logs").c_str());
//	system(("mkdir "+OutputDirName+"/error").c_str());
//	system(("mkdir "+OutputDirName+"/output").c_str());
	system(("mkdir "+OutputDirName+"/root").c_str());
//	system(("mkdir "+OutputDirName+"/root_intermediate").c_str());
	system(("mkdir "+OutputDirName+"/dag").c_str());
	system(("mkdir "+OutputDirName+"/manual_submission").c_str());
	//read in text list of root files
	std::vector<std::string> fileList;
	std::string line;
	std::ifstream rootlist(argv[3]);//list.txt
	if( rootlist.is_open() ){
		while( getline(rootlist,line) ){
			std::cout<< line << '\n';
			fileList.push_back(line);
		}
		rootlist.close();
	}
	else{
		std::cout<< "unable to open file .. aborting";
		return 0;
	}

	int nCombine = std::stoi( std::string(argv[4]));
	if(nCombine < 2){
		std::cout<<" Choose nCombine > 1 .. aborting";
		return 0;
	}
	std::vector<jobset*> joblayers{};
	int layer =1 ;
	joblayers.push_back( new jobset(fileList, nCombine, layer, intpath));
	int Noutputs= joblayers.at(0)->joboutputs.size();
	while( Noutputs > 1  ){
		joblayers.push_back( new jobset( joblayers.at(layer-1)->joboutputs, nCombine, layer+1, intpath) );
		layer++;
		Noutputs = joblayers.at(layer-1)->joboutputs.size();	
	}
	//go to the last job layer and rename the last file to outputdirname.root
	joblayers.at(joblayers.size()-1)->joboutputs.at(0) = intpath+OutputDirName+".root";

	std::cout<<" JOB SUMMARY : "<<std::endl;
	for(int i=0; i< joblayers.size(); i++){
		joblayers.at(i)->printjobs();
	}		
	

	//generate dag file	
	//std::ofstream dagfile;
	std::string exe = "haddjob.x";
	//dagfile.open(path+OutputDirName+"/"+OutputDirName+".dag");
	for(int i=0; i<joblayers.size();i++){
	//	joblayers.at(i)->WriteJobs(dagfile, exe, path);
		joblayers.at(i)->WriteJobs( path+"/"+exe,path+"/"+OutputDirName);
	}
	//WriteJobs( path+outputDirName+"/src", exe, path);

	//write dependencies
	//write a separate dagfile
	std::ofstream dagfile;
	dagfile.open(path+"/"+OutputDirName+"/dag/"+OutputDirName+".dag");
	std::string jobname;
	//list jobs then write dependencies
	for(int i=0; i<joblayers.size();i++){
		for(int j=1; j<=joblayers.at(i)->_njobs; j++){
			jobname = "l"+std::to_string(joblayers.at(i)->_layer)+"j"+std::to_string(j);
			dagfile<<" Job "<< jobname << " " << path+"/"+OutputDirName+"/sub/"+jobname+".submit\n";
		}
	}
	
	for(int i=0; i<joblayers.size()-1;i++){
		std::ofstream manualsub_file;
		manualsub_file.open(path+"/"+OutputDirName+"/manual_submission/submission_layer"+std::to_string(i+1)+".sh");
        	dagfile<< "PARENT ";
		for(int j=1; j<=joblayers.at(i)->_njobs; j++){
			dagfile<< "l"+std::to_string(joblayers.at(i)->_layer)+"j"+std::to_string(j) << " ";
			manualsub_file<< "condor_submit "+path+"/"+OutputDirName+"/sub/";
			manualsub_file<<"l"+std::to_string(joblayers.at(i)->_layer)+"j"+std::to_string(j)+".submit\n"; 
		}
		manualsub_file.close();
		
		dagfile<<"CHILD ";
		for(int j=1; j<=joblayers.at(i+1)->_njobs; j++){
			dagfile<< "l"+std::to_string(joblayers.at(i+1)->_layer)+"j"+std::to_string(j) << " ";
		} 
		dagfile<<"\n";       	
	}
	//since we loop to joblayers_size()-1 we need to make a manual submission extra call
	//the last layer will always have 1 job
	std::ofstream manualsub_file;
	manualsub_file.open(path+"/"+OutputDirName+"/manual_submission/submission_layer"+std::to_string(joblayers.size())+".sh");
	manualsub_file<< "condor_submit "+path+"/"+OutputDirName+"/sub/";
        manualsub_file<<"l"+std::to_string(joblayers.size())+"j1.submit\n";
	manualsub_file.close();
	dagfile.close();

}
