#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <limits>
#include <unordered_map>
#include <string>
#include <windows.h>
#include <algorithm>
#include <random>

using namespace std;

typedef struct guardian
{
    //Nombre
    string name;
    bool derrotado;
    //nivel de pelea
    int power_level;
    //maestro
    string mainmaster;
    //ciudad
    string Village;
	vector<guardian*> aprendices;
}Guardian;
typedef struct historial_viaje
{
    //nombre de las aldeas
    vector<string> lugaresVisitados;
}Historial_viaje;
typedef struct historial_pelea
{
    //nombre de los rivales
    vector<string> nombrerival;
    //indica si logro el entrenamiento o no
    vector<string> resultado;
}Historial_Pelea;
typedef struct player
{
    //nombre
    string name;
    //es aprendiz o un Guardian
    string estado;
    //poder de experiencia
    int power_level;
    //aldea de origen
    string villageO;
    //aldeas que han sido visitadas
    vector<string> villagevisitados;
    //aldea en donde esta
    string villageactual;
    
}Player;

//Jerarquia de los guardianes
class Jerarquia{
	public:
	Jerarquia() : root(nullptr) {}
    //metodo para insertar guardian en el arbol
	void insertGuardian(const string& name, const string& powerLevel, const string& mainMaster, const string& village) {
		
        Guardian* guardian = new Guardian;
		guardian->name = name;
		guardian->power_level = stoi(powerLevel);
        guardian->derrotado=false;
		guardian->mainmaster = mainMaster;
		guardian->Village = village;
		guardianes.push_back(guardian);

		// If the mainMaster Guardian is found, add the guardian as an Aprendiz (apprentice)
		if (root == nullptr) {
			root = guardian;
			std::cout << "root" << endl;
		}
		else {
			Guardian* master = findGuardian(mainMaster);
			if(master != nullptr) {
				master->aprendices.push_back(guardian);
			}
		}
	}
    //funcion donde se carga el archivo
	void loadGuardiansFromFile(const string& filename) {
        ifstream file(filename);
        if (!file) {
            cerr << "Failed to open file: " << filename << endl;
            return;
        }
        string line;
        getline(file, line);
        while (getline(file, line)) {
            istringstream iss(line);
            string name, mainMaster, village;
            string powerLevel;
            
            getline(iss, name, ',');
            getline(iss, powerLevel , ',');
            getline(iss, mainMaster, ',');
            getline(iss, village);
            int valido=stoi(powerLevel);
            //si el power es mayor que 100, no se añade 
            if(valido<=100){
                insertGuardian(name, powerLevel, mainMaster, village);
            }
        }
        //se ordenan los aprendises 
        ordenarAprendices(root);
        file.close();
    }
    //funcion para imprimir el arbol
	void printGuardians() {
        printGuardian(root, 0);
    }

	void eliminarGuardian(const string& name) {
        // Buscar el guardian a eliminar
        Guardian* guardian = findGuardian(name);
        if (guardian == nullptr) {
            cout << "No se encontro el guardian con el nombre: " << name << endl;
            return;
        }
        Guardian* master=findGuardian(guardian->mainmaster);
        //traspasar los aprendices al maestro
        for(auto pair : guardian->aprendices){
            pair->mainmaster=master->name;
            master->aprendices.push_back(pair);
        }
        // Eliminar el guardian del vector guardianes
        auto it = find(guardianes.begin(), guardianes.end(), guardian);
        if (it != guardianes.end()) {
            guardianes.erase(it);
        }

        // Eliminar el guardian de los aprendices de su maestro (si tiene)
        if (guardian->mainmaster != "") {
            Guardian* master = findGuardian(guardian->mainmaster);
            if (master != nullptr) {
                auto aprendizIt = find(master->aprendices.begin(), master->aprendices.end(), guardian);
                if (aprendizIt != master->aprendices.end()) {
                    master->aprendices.erase(aprendizIt);
                }
            }
        }

        // Liberar la memoria ocupada por el guardian eliminado
        delete guardian;

        ordenarAprendices(root);
    }
    //funcion para saber si existe el guardian llamado
    bool existeguardian(const string& name){
        //se recorre cada guardian hasta que encuentre el nombre
        for (Guardian* guardian : guardianes) {
            if (guardian->name == name) {
                return true;
            }
        }
        return false;
    }
    //funcion para encontrar el guardian y pasarlo 
    Guardian* findGuardians(const string& name){
        return findGuardian(name);
    }
     //funcion para saber si hay un maestro-alumno en la misma aldea 
    bool hay_maestro_alumno(const string& village){
        for (Guardian* guardian : guardianes) {
            if (guardian->Village == village) {
                
                for (auto pair : guardian->aprendices) {
                    
                    if (pair->Village == village) {
                        
                        return true;
                    }
                }
            }
        }
        return false;
    }
    //funcion para traspasar la informacion del maestro-alumno en la misma aldea hacia un guardian 
    Guardian* find_maestro_alumno(const string& village){
        Guardian* a=new Guardian;
        Guardian* b=new Guardian;
        for (Guardian* guardian : guardianes) {
            //cout<< "xd "<<guardian->Village<<endl;
            if (guardian->Village == village) {
                
                for (auto pair : guardian->aprendices) {
                    
                    if (pair->Village == village) {
                        a->name=guardian->name;
                        a->power_level=guardian->power_level;
                        a->Village=guardian->Village;
                        a->mainmaster=guardian->mainmaster;
                        b->name=pair->name;
                        b->power_level=pair->power_level;
                        b->Village=pair->Village;
                        b->mainmaster=pair->mainmaster;
                        a->aprendices.push_back(b);
                        
                        return a;
                    }
                }
            }
        }
        cout<< "no se encontro    ";
        return nullptr;
        
    }
    
	private:
    // atributo para guardar cada guardian
	vector<Guardian*> guardianes;
    // atributo arbol de jerarquia de guardian
	Guardian* root;
    //funcion para buscar guardian 
	Guardian* findGuardian(const string& name) {
        // recorre cada guardian
        for (Guardian* guardian : guardianes) {
            //se busca por el nombre
            if (guardian->name == name) {
                return guardian;
            }
        }
        return nullptr;
    }
    /* Funcion que no utilizo
	void updateTreeStructure() {
        for (Guardian* guardian : guardianes) {
            if (guardian->mainmaster != "") {
                Guardian* master = findGuardian(guardian->mainmaster);
                if (master != nullptr) {
                    master->aprendices.push_back(guardian);
                }
            }
            else {
                root = guardian;
            }
        }
    }*/
    //ordena los aprendices
    void ordenarAprendices(Guardian* guardian) {
        //se ordena con el clasico doble for pero con guardianes, ademas que se ordena de mayor a menor el power_level
        for (unsigned int i = 0; i < guardian->aprendices.size(); i++) {
            for (unsigned int j = 0; j < guardian->aprendices.size() - 1; j++) {

                if (guardian->aprendices[j]->power_level < guardian->aprendices[j + 1]->power_level) {
                    Guardian* aux = guardian->aprendices[j];
                    guardian->aprendices[j] = guardian->aprendices[j + 1];
                    guardian->aprendices[j + 1] = aux;
                }
            }
        }
        //se usa recursividad para ordenar los aprendices de los aprendices
        for (Guardian* aprendiz : guardian->aprendices) {
            ordenarAprendices(aprendiz);
        }
    }
    //funcion para imprimir arbol
    void printGuardian(Guardian* guardian, int indent) {
        if (guardian != nullptr) {
            cout << string(indent, ' ') << "- " << guardian->name << " (Power Level: " << guardian->power_level << ", Village: " << guardian->Village << ")" << endl;
            for (Guardian* aprendiz : guardian->aprendices) {
                printGuardian(aprendiz, indent + 4);
            }
        }
    }

};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Mapa {
private:
    // Cantidad de vectores
    int numVertices;
    //saber los numeros que tendran las aldeas 
	unordered_map<int,string> aldeas;
    //matris para crear el grafo
    vector<vector<int>> adjList;

public:
    Mapa(int vertices) : numVertices(vertices) {
        adjList.resize(numVertices);
    }
    // funcion para cargar archivos
	void loadAldeasFromFile(const string& filename) {
        ifstream file(filename);
        if (!file) {
            cerr << "Failed to open file: " << filename << std::endl;
            return;
        }
		bool existe=false;
		int j=0;
        //se crea un arreglo para meter todas las aldeas del archivo
		vector<string> listaldeas;
        string line;
        getline(file, line);
        while (std::getline(file, line)) {
            istringstream iss(line);
            string village;
            getline(iss, village, ',');
			listaldeas.push_back(village);
            getline(iss, village);
			listaldeas.push_back(village);
        }
        file.close();
        //se mete todas las aldeas de la listaldeas al aldeas sin repetirse
		for(const auto& aldea : listaldeas){
			existe=false;
			for(const auto& pair : aldeas){
				if(pair.second==aldea){
					existe=true;
					break;
				}
			}
			if(!existe){
				aldeas[j]=aldea;
				j++;
			}
		}
    }
	void loadconnectaldea(const string& filename){
        //se utiliza la encpsulacion para utilizar un metodo
		loadAldeasFromFile(filename);
        ifstream file(filename);
        if (!file) {
            cerr << "Failed to open file: " << filename << endl;
            return;
        }
        string line;
        getline(file, line);
        //se unen las aldeas en base al archivo
        while (getline(file, line)) {
			bool existe=false;
            istringstream iss(line);
            string village,conectv;
            getline(iss, village, ',');
            getline(iss, conectv);
			if(!existe){
				uniraldea(village,conectv);
			}
			
            
        }
        file.close();
	}
    // funcion para verificar que funciona
	void mostraraldea(){
		for (const auto& pair : aldeas) {
			const string& key = pair.second;
			int value = pair.first;
			cout << "Key: " << key << ", Value: " << value << endl;
		}
	}
    //retorna a la cantidad de aldeas que hay
    int cantidadAldeas() {
        return aldeas.size();
    }
    //retorna el nombre de la aldea en el int del unorder map
    string nombrealdea(int i){
        for(const auto& pair : aldeas){
            if(pair.first==i){
                return pair.second;
            }
        }
        return "";
    }
    //indica si existe coneccion entre esas aldeas
    bool existe_coneccion(const string& village,const string& villageconect){
        for (const auto& pair : aldeas) {
            if(pair.second==village){
                int i=pair.first;
                for (const auto& neighbor : adjList[i]) {
                    if(aldeas[neighbor]==villageconect){
                        return true;
                    }
                }
            }
        }
        return false;
    }
    //indica si no tiene ninguna coneccion con alguna aldea
    bool no_tieneconecccion(const string& village){
        int i=0;
        for (const auto& pair : aldeas) {
            if(pair.second==village){
                for(unsigned int a = 0; a < adjList[pair.first].size(); a++){
                    i++;
                }
                if(i<1){
                    return true;
                }
            }
        }
        return false;
    }
    //indica si existe el nombre de la aldea
    bool existealdea(const string& village){
        for(const auto& pair : aldeas){
            if(pair.second==village){
                return true;
            }
        }
        return false;
    }
    //agrega adjacencia al vector con la otra aldea
	void uniraldea(const string& a,const string& b){
        bool existe=false;
		for(const auto& pair : aldeas){
			if(pair.second==a){
				int i=pair.first;
				for(const auto& pair2 :aldeas){
					if(pair2.second==b){
						int j=pair2.first;
                        for(const auto& neighbor : adjList[i]){
                            if(neighbor==j){
                                existe=true;
                                break;
                            }
                        }
                        if(!existe && pair2.second!=pair.second){
                            adjList[i].push_back(j);
                        }
						
						break;
					}
				}
				break;
			}
		}
	}
    //imprime las conecciones de todas las aldeas
    void printGraph() {
        cout << "Aldeas conectadas:" << endl;
        for (unsigned int i = 0; i < aldeas.size(); ++i) {
            cout << "Aldea " << aldeas[i] << ": ";
            for (const auto& neighbor : adjList[i]) {
				
                cout << aldeas[neighbor] << " | ";
            }
            cout << endl;
        }
    }
    //encuentro al adjaciencia de la aldea
    void printAldeaCer(const string& nombre){
        for (unsigned int i = 0; i < aldeas.size(); ++i) {
            if(aldeas[i]==nombre){
                cout << "Aldea " << aldeas[i] << ": ";
                for (const auto& neighbor : adjList[i]) {
                    
                    cout << aldeas[neighbor] << " | ";
                }
                cout << endl;
            }
        }
    }
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//crear el player
Player* crearplayer(const string& name, const string& village){
    Player* newplayer=new Player;
    newplayer->name=name;
    newplayer->power_level=50;
    newplayer->villageO=village;
    newplayer->estado="Aprendiz";
    newplayer->villagevisitados.push_back(village);
    newplayer->villageactual=village;
    return newplayer;
}
//crea el player escogiendo un guardian de la clase Jerarquia
Player* escogerplayer(const string& name,Jerarquia& tree){
    Guardian* guardian=tree.findGuardians(name);
    Player* newplayer=new Player;
    newplayer->name=guardian->name;
    newplayer->power_level=0;
    newplayer->villageO=guardian->Village;
    newplayer->estado="Aprendiz";
    newplayer->villagevisitados.push_back(guardian->Village);
    newplayer->villageactual=guardian->Village;
    //se elimina el nodo
    tree.eliminarGuardian(name);
    return newplayer;
}
// cuenta los lugares visitados por el player
int lugar_visitado(Player* head){
    int i=0;
    for(int a = 0; a < head->villagevisitados.size(); a++){
        i++;
    }
    return i;
}
// da la recomendacion del aprendiz
void recomendacion(Guardian* npc){
    Guardian* j=new Guardian;
    j->power_level=0;
    for(const auto& aprendiz : npc->aprendices){
        if(j->power_level<aprendiz->power_level){
            j=aprendiz;
        }
    }
    cout<<"se recomienda parcticar con el aprendiz " <<j->name<<endl;
    cout<<"ya que tiene el menor nivel de pelea"<<endl;
}
//Funcion para decidir si a ganado el player
bool ganar(Guardian* guardian){
    
    if(guardian->name=="Stormheart"){
        if(guardian->derrotado==true){
            return true;
        }
    }
    return false;
}
//imprime el Guardian con sus aprendices
void printGuardian(Guardian* guardian, int indent) {
    if (guardian != nullptr) {    
        cout << string(indent, ' ') << "- " << guardian->name << " (Power Level: " << guardian->power_level << ", Village: " << guardian->Village << ")" << endl;
        for (Guardian* aprendiz : guardian->aprendices) {
            printGuardian(aprendiz, indent + 4);
        }
    }
}
//imprime el Guardian 
void printmaestro(Guardian* head) {
    if (head != nullptr) {    
        cout << "- " << head->name << " (Power Level: " << head->power_level << ", Village: " << head->Village << ")" << endl;
    }
}
//funcion para combinar las clases Mapa y Jerarquia en un unorder_map <string, Guardian*>
void poner_guardianAldea(unordered_map<string, Guardian*>& dato, Mapa& mapa,Jerarquia& guardian) {
    for (int i = 0; i < mapa.cantidadAldeas(); i++) {
        Guardian* j = guardian.find_maestro_alumno(mapa.nombrealdea(i));
        if (j != nullptr) {
            dato[mapa.nombrealdea(i)]=j; 
        }
    }
}
//verificar que se puede combinar las clases Mapa y Jerarquia en un unorder_map <string, Guardian*> en cada aldea
bool existe_guardianAldea(Mapa& mapa,Jerarquia& guardian) {
    for (int i = 0; i < mapa.cantidadAldeas(); i++) {
        if(!guardian.hay_maestro_alumno(mapa.nombrealdea(i))){
            return false;
        }
    }
    return true;
}
//imprimir datos de player
void printplayer(Player* head){
    cout<< head->estado<<endl;
    cout<< "Nombre: " << head->name<<" Nivel de poder: "<< head->power_level<<" Aldea de origen: "<< head->villageO<<" Estado: "<<head->estado<<endl;
    cout<<"aldeas visitadas"<<endl;
    for(const auto& lugar: head->villagevisitados){
        cout<< lugar<<endl;
    }
}
//imprimir cada viaje que hizo el Player
void printviajes(Historial_viaje* head){
    int i=1;
    for(const auto& lugar: head->lugaresVisitados){
        cout<<i<<") "<< lugar<<endl;
    }
}
//imprimir cada conclusion de entrenamiento que hizo el Player
void printpeleas(Historial_Pelea* head){
    for(unsigned int i=0;i<head->nombrerival.size() ;i++){
        cout<<i+1<<") entrenamiento con "<<head->nombrerival[i]<<" a sido "<<head->resultado[i]<<endl;
    }
}
/* funcion para verificar si funciona poner_guardianAldea
void print(unordered_map<string, Guardian*>& dato){
    for(const auto& pair : dato){
        string p1=pair.first;
        cout << "aldea: "<<p1<<endl;
        Guardian*j=pair.second;
        printGuardian(j,0);
    }
}
*/
//funcion para crear random
int generarNumeroAleatorio(int min, int max) {
    static random_device rd;
    static mt19937 generator(rd());
    uniform_int_distribution<int> distribution(min, max);
    return distribution(generator);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(){
    Historial_viaje* viajes=new Historial_viaje;
    Historial_Pelea* peleas=new Historial_Pelea;
    unordered_map<string, Guardian*> dato_aldea;
    Player* jugador;
    string name,village;
    bool existef=false;
    int opcion,random,randomnpc;
	Mapa mapa(100);
	mapa.loadconnectaldea("aldea.csv");
    Jerarquia tree;
	tree.loadGuardiansFromFile("data.csv");
    //tree.printGuardians();
    if(!existe_guardianAldea(mapa,tree)){
        cout<< "falta agregar maestros que tengan un alumno en la";
        cout<< "misma ciudad en el archivo data.csv"<<endl;
        return 1;
    }
    cout<<" ______________________"<<endl;
    cout<<"|                      |"<<endl;
    cout<<"|      Bienvenido      |"<<endl;
    cout<<"|______________________|"<<endl;
    cout<<"Ingrese el metodo de aprendiz que desea ocupar"<<endl;
    cout<<"[1]crear aprendiz [2] escoger aprendiz"<<endl;
    cin >> opcion;

    cin.ignore();
    switch (opcion)
    {
    case 1:
        cout<<"Ingresa el nombre de tu aprendiz"<<endl;
        getline(cin,name);
        do{
            cout<<"Ingresa la aldea en la que va a empezar"<<endl;
            getline(cin,village);
        }while (!mapa.existealdea(village));
        jugador=crearplayer(name,village);
        break;
    case 2:
        do{
            cout<<"Ingresa el nombre de tu aprendiz"<<endl;
            getline(cin,name);
        }while(!tree.existeguardian(name) || name=="Stormheart");
        
        
        jugador=escogerplayer(name,tree);
        break;
    }
    if(!existe_guardianAldea(mapa,tree)){
        cout<< "has elminado al guardian que hacia funcionar tu codigo"<<endl;
        cout<< "deberas escoger bien para que no pase eso"<<endl;
        return 1;
    }
    poner_guardianAldea(dato_aldea,mapa,tree);
    system("cls");
    do{
        cout<<"Bienvenido a la aldea: "<< jugador->villageactual<<endl;
        cout<<endl;
        if(!dato_aldea[jugador->villageactual]->derrotado){
            cout<<"2 Puntos)Entrenamiento con Maestro: "<<dato_aldea[jugador->villageactual]->name<<endl;
            cout<<endl;
        }
        if(!dato_aldea[jugador->villageactual]->aprendices[0]->derrotado){
            cout<<"1 Punto)Entrenamiento con aprendiz: "<< dato_aldea[jugador->villageactual]->aprendices[0]->name<<endl;
            cout<<endl;
        }
        if(!dato_aldea[jugador->villageactual]->aprendices[0]->derrotado){
            recomendacion(dato_aldea[jugador->villageactual]);
            cout<<endl;
        }
        if(jugador->estado=="Guardian"&& dato_aldea[jugador->villageactual]->name=="Stormheart"){
            printplayer(jugador);
            printmaestro(dato_aldea[jugador->villageactual]);
        }
        printplayer(jugador);
        cout<<mapa.no_tieneconecccion(jugador->villageactual)<<endl;
        cout<<endl;
        do{
            cout<<"Ingrese el metodo de aprendis que desea ocupar"<<endl;
            if(!mapa.no_tieneconecccion(jugador->villageactual)){
                cout<<"[1] ir aldea "; 
            }else{
                cout<<"             ";
            }
            cout<<"             [2] unir aldea"<<endl;
            if(!dato_aldea[jugador->villageactual]->aprendices[0]->derrotado){
                cout<<"[3] entrenar con aprendiz";
            }else{
                cout<<"                          ";
            }
            if(!dato_aldea[jugador->villageactual]->derrotado){
                cout<<" [4] entrenar con maestro"<<endl;
            }else{
                cout<<endl;
            }
            cout<<"cualquier otro numero para salir"<<endl;
            cin >> opcion;

        }while( (opcion==1&& mapa.no_tieneconecccion(jugador->villageactual)) ||(opcion==4 && dato_aldea[jugador->villageactual]->derrotado==true) | (opcion==3 && dato_aldea[jugador->villageactual]->aprendices[0]->derrotado==true) | (opcion==4 && dato_aldea[jugador->villageactual]->name=="Stormheart" && jugador->estado!="Guardian"));
        cin.ignore();
        switch (opcion){
        case 1:
            mapa.printAldeaCer(jugador->villageactual);
            do{
                cout<<"Ingresa el nombre de la aldea que quieras ir"<<endl;
                getline(cin,village);
            }while(!mapa.existealdea(village) || !mapa.existe_coneccion(jugador->villageactual,village));
            for(const auto& visit :jugador->villagevisitados){
                if(visit==village){
                    existef=true;
                    break;
                }
            }
            if(!existef){
                jugador->villagevisitados.push_back(village);
            }
            jugador->villageactual=village;
            existef=false;
            viajes->lugaresVisitados.push_back(village);
            if(mapa.cantidadAldeas()==lugar_visitado(jugador)){
                jugador->estado="Guardian";
            }
            if(jugador->power_level+1>=90){
                jugador->estado="Guardian";
            }
            if(jugador->power_level<100){
                jugador->power_level+=1;
            }
            system("cls");
            cout<<"Has conseguido un punto por visita"<<endl;
            break;
        case 2:
            do{
                cout<<"El alquimista te pide el nombre de la aldea que quieres conectar a "<<jugador->villageactual <<endl;
                getline(cin,village);
            }while(!mapa.existealdea(village)|| village==jugador->villageactual || mapa.existe_coneccion(jugador->villageactual,village));
            mapa.uniraldea(jugador->villageactual,village);
            random=generarNumeroAleatorio(2, 4);
            jugador->power_level-=random;
            cout<<"has perdido "<<random<<" Puntos de experiencia"<<endl;
            break;
        case 3:
            cout<<"Empieza el entrenamiento"<<endl;
            randomnpc=generarNumeroAleatorio(1, 6);
            cout<<"El aprendiz "<< dato_aldea[jugador->villageactual]->aprendices[0]->name<<" ha sacado un "<< randomnpc<<endl;
            cout<<"se multiplica con su poder "<< dato_aldea[jugador->villageactual]->aprendices[0]->power_level<<" dando una cantiadad de "<< dato_aldea[jugador->villageactual]->aprendices[0]->power_level*randomnpc<<endl;
            random=generarNumeroAleatorio(1, 6);
            cout<<"El jugador "<< jugador->name<<" ha sacado un "<< random<<endl;
            cout<<"se multiplica con su poder "<< jugador->power_level<<" dando una cantiadad de "<< jugador->power_level*random<<endl;
            if(dato_aldea[jugador->villageactual]->aprendices[0]->power_level*randomnpc<=jugador->power_level*random){
                cout<<"has podido completar el entrenamiento"<<endl;
                dato_aldea[jugador->villageactual]->aprendices[0]->derrotado=true;
                if(jugador->power_level+1>=90){
                jugador->estado="Guardian";
                }
                if(jugador->power_level<100){
                    jugador->power_level+=1;
                }
                peleas->nombrerival.push_back(dato_aldea[jugador->villageactual]->name);
                peleas->resultado.push_back("Completado");
            }else{
                peleas->nombrerival.push_back(dato_aldea[jugador->villageactual]->name);
                peleas->resultado.push_back("No completado");
                cout<<"no has podido completar el entrenamiento"<<endl;
            }
            break;
        case 4:
            cout<<"Empieza el entrenamiento"<<endl;
            randomnpc=generarNumeroAleatorio(4, 6);
            cout<<"El aprendiz "<< dato_aldea[jugador->villageactual]->name<<" ha sacado un "<< randomnpc<<endl;
            cout<<"se multiplica con su poder "<< dato_aldea[jugador->villageactual]->power_level<<" dando una cantiadad de "<< dato_aldea[jugador->villageactual]->power_level*randomnpc<<endl;
            random=generarNumeroAleatorio(1, 6);
            cout<<"El jugador "<< jugador->name<<" ha sacado un "<< random<<endl;
            cout<<"se multiplica con su poder "<< jugador->power_level<<" dando una cantiadad de "<< jugador->power_level*random<<endl;
            if(dato_aldea[jugador->villageactual]->power_level*randomnpc<=jugador->power_level*random){
                cout<<"has podido completar el entrenamiento"<<endl;
                dato_aldea[jugador->villageactual]->derrotado=true;
                if(jugador->power_level+2>=90 && jugador->estado!="Guardian"){
                jugador->estado="Guardian";
                cout<< "has conseguido hasta 90 pts, ahora eres un guardian (podras enfrentarte a Stormheart)"<<endl;
                }
                if(jugador->power_level<100){
                    jugador->power_level+=2;
                    if(jugador->power_level>100){
                        jugador->power_level=100;
                    }
                }
                peleas->nombrerival.push_back(dato_aldea[jugador->villageactual]->name);
                peleas->resultado.push_back("Completado");
            }else{
                peleas->nombrerival.push_back(dato_aldea[jugador->villageactual]->name);
                peleas->resultado.push_back("No completado");
                cout<<"no has podido completar el entrenamiento"<<endl;
            }
            break;
        }
    }while(opcion>=1 && opcion<=4 && !ganar(dato_aldea[jugador->villageactual]));

    if(ganar(dato_aldea[jugador->villageactual])){
        cout << "has ganado el jugo al derrota al maximo maestro"<<endl;
    }
    cout<<"historial de viajes"<<endl;
    printviajes(viajes);
    cout<<"historial de peleas"<<endl;
    printpeleas(peleas);

    return 0;
}


