#include "SetCovSolver.h"

namespace mclp{

  SetCovSolver::SetCovSolver(string gurobi_dir){
    gurobi_path = gurobi_dir;
  }

  void SetCovSolver::solve_set_covering(list < set<int> >& bins, set<int> boxes , int nb_box_types){
    exportToTxtSCP(&bins, nb_box_types, boxes);
    string filename = "bins_scp" + to_string(getpid()) + ".txt";

    const string MAX_TIME = "10";
    const string SEED = "1";
    string run2 = string("python " + gurobi_path + "/Solver.py " + filename);
    FILE *p = popen(run2.c_str(), "r");
    list<int> first_bins;
    list<set<int>> bins_gurobi;

    if(p != NULL) {
      cout << endl;
      cout << "running Gurobi-Solver" << endl;
        cout << "Time: " << MAX_TIME << endl;
        cout << "Seed: " << SEED << endl;

        char output[1000], last_output[1000],caracteres[1000], containers[1000];
        string str;
        vector <string> line;
        string check_container = "";
        const char *pointer;
        int buffer;
        string delimiter = " ";
      size_t pos = 0;
      string token;

      /*while(fgets(output, sizeof(output), p) != NULL) {
        strcpy(last_output, output);
      }*/

      while (feof(p) == 0)
      {
        fgets(caracteres,1000,p);
        if(caracteres[0] == '[') strcpy(containers, caracteres);
        else strcpy(last_output, caracteres);
      }

      last_output[strlen(last_output)-1]=0;
      containers[strlen(containers)-1]=0;
      str = output;

      //Se recorre la lista entregada por gurobi
      for(int i = 1; containers[i] != ']' ;i++){
        if(containers[i] == ','){
          pointer = check_container.c_str();
          buffer = std::atoi(pointer);
          first_bins.push_back(buffer);
          check_container = "";
          //cout << buffer << endl;
        }else{
          check_container += containers[i];
        }
      }
      pointer = check_container.c_str();
      buffer = std::atoi(pointer);
      first_bins.push_back(buffer);

      while ((pos = str.find(delimiter)) != string::npos) {
        token = str.substr(0, pos);
        str.erase(0, pos + delimiter.length());
        line.push_back(token);
      }
      for(int i = 0; i < line.size(); i++) cout << i << " " << line[i] << " " << str;
    } else {
      perror("Unable to open file");
    }

    for(auto id_bin :first_bins){
      auto it=bins.begin();
      std::advance (it,id_bin);
      bins_gurobi.push_back(*it);
    }
    bins=bins_gurobi;																															//recorrer lista de bins y copiar los correspondientes a la solucion entregada por gurobi en bins_gurobi
    pclose(p);
  }

  string SetCovSolver::findDirectory(string current_directory, string find_directory){
  	string find = findInto(current_directory, find_directory);
  	//cout << "Dentro de findDirectory-finInto " << find << endl;
  	if(!find.empty()) return find;
  	else {
  		find = findOut(current_directory, find_directory);

  		if(!find.empty()) return find;
  	}
  	return "";
  }
  string SetCovSolver::findOut(string current_directory, string find_directory){
  	DIR* dir;
  	struct dirent *ent;
  	current_directory += "/../";
  	dir = opendir(current_directory.c_str());
  	//cout << "Dentro de findOut "<< current_directory + find_directory << endl;
  	for(int i = 0; i < 5; i++){
  		while((ent = readdir(dir)) != NULL){
  			if(((string)ent->d_name).compare(find_directory) == 0){
  				closedir(dir);
  				//cout << current_directory + find_directory << endl;

  				return current_directory + find_directory +"/";
  			}
  		}
  		current_directory += "../";
  		dir = opendir((current_directory).c_str());
  	}
  	closedir(dir);
  	return "";
  }

  string SetCovSolver::findInto(string current_directory, string find_directory){
  	DIR* dir;
  	struct dirent *ent;
  	dir = opendir(current_directory.c_str());
  	if(dir != NULL){
  		while((ent = readdir(dir)) != NULL){
  			if(((string)ent->d_name).compare(find_directory) == 0){
  				//break;
  				closedir(dir);
  				//cout << current_directory + find_directory << endl;
  				return current_directory + find_directory;
  			}
  			if(((string)ent->d_name).find(".") == string::npos){
  				findInto(current_directory + "/" + ent->d_name, find_directory);
  				//cout << ent->d_name << endl;
  				//cout << current_directory << endl;
  				//cout << endl;
  			}
  		}
  	}
  	closedir(dir);
  	//cout << current_directory + find_directory << endl;
  	return "";
  }

  void SetCovSolver::exportToTxtSCP(list < set<int> >* bins, long int nb_boxes, set<int> boxes){
  	string path = findDirectory(".", "GRASP-SCP");
  	string path2 = findDirectory(".", "gurobi");
  	string filename = "bins_scp" + to_string(getpid()) + ".txt";

    path = "";

  	/*if(path.empty()){
  		cout << "El directorio no existe.\n" << endl;
  		exit(0);
  	}*/
  	cout << "Resultados almacenados en " << path << filename << endl;
  	ofstream scp (path + filename);
  	if (scp.is_open()){
  		long int total_boxes = 0;

  		//Bins quantity
  		scp << " " << bins->size() << " ";

  		//Boxes quantity
  		scp << nb_boxes << " ";

  		//Many boxes that we can't use
  		cout << "gurobi input boxes:" <<boxes.size() << endl;
  		for(auto box : boxes){
  			scp << box << " ";
  			//cout << box << endl;
  		}

  		scp << "\n";

  		//Matrix cost by boxes
  		/*long int cont = 0;
  		for(auto bin: *bins){
  			if(cont >= 12){
  				scp << "\n";
  				cont = 0;
  			}
  			scp << " 1";
  			cont += 1;
  		}
  		scp << "\n";*/

  		//Boxes quantity in a set and then sets boxes
  		for(auto bin: *bins){
  			scp << " " << bin.size() << "\n";
  			for(auto box: bin){
  				scp << " " << box;
  			}
  			scp << "\n";
  		}

  		scp.close();
  	} else cout << "Unable to open file" << endl;
}


}
