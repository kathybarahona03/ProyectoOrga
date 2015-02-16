#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include<cstring>
#include<stdio.h>
#include<stdlib.h>
using namespace std;
struct Campo{
	char nombre[15];
	char tipo[7];
	int tamano;
};
int offset(int,vector<Campo>);
int getStructure(const char*, vector<Campo>&);
void Header(const char*, vector<Campo>&);
void Agregar(const char*,vector<Campo>);
void Listar(const char*, vector<Campo>);
void Borrar(const char*, vector<Campo>);
void Modificar(const char*,vector<Campo>);
void Compactar(const char*,const char*,vector<Campo>);
void Buscar(const char*, vector<Campo>);

union charint{
	char raw[sizeof(int)];
	int num;
};
int main(int argc, char* argv[]){
	char*nbin;
	string n;
	vector<Campo> registros;
	cout<<"Bienvenido"<<endl;
	int opcion;
	cout<<"1.Abrir una Estructura\n2.Crear una Nueva Estructura"<<endl;
	cin>>opcion;
	if(opcion==1){
		cout << "Ingrese el nombre de la estructura" << endl;
		cin >> n;
		nbin = (char*)n.c_str();
		strcat(nbin, ".bin");
		if(getStructure(nbin, registros)==0)
			return 0;

	}else{
		cout<<"Ingrese el nombre de la estructura"<<endl;
		cin>>n;
		nbin = (char*)n.c_str();
		strcat(nbin,".bin");//AGREGA LA EXTENSION .BIN
		Header(nbin,registros);

	}
	int opcion2=0;
	while(opcion2 != 7){
		cout << "1.Agregar Datos\n2.Listar Datos\n3.Buscar Registro\n4.Borrar\n5.Compactar\n6.Modificar\n7.Salir" << endl;
		cin >> opcion2;
		if(opcion2==1){
			Agregar(nbin, registros);
		}
		if(opcion2==2){
			Listar(nbin, registros);
		}
		if(opcion2==3){
			Buscar(nbin, registros);
		}
		if(opcion2==4){
			Borrar(nbin, registros);
		}
		if(opcion2==5){
			string x; 
			char*nbin2;
			cout << "Ingrese el nombre del nuevo archivo" << endl;
			cin >> x;
			nbin2 = (char*)x.c_str();
			strcat(nbin2,".bin");
			Compactar(nbin, nbin2, registros);
		}
		if(opcion2==6){
			Modificar(nbin, registros);
		}
	}
	return 0;
}
void Header(const char* nbin, vector<Campo>& registros){
	ofstream out(nbin, ios::out|ios::binary);
	char resp = 's';
	int cbytes=8;
	int numcampos, tipo;
	int availlist= 0;
	cout << "Ingrese el numero de campos " << endl;
	cin >> numcampos;
	cbytes = cbytes + sizeof(Campo)*numcampos;
	out.write(reinterpret_cast<char*>(&cbytes), sizeof(int));
	out.write(reinterpret_cast<char*>(&availlist),sizeof(int));
	for(int i=0; i  < numcampos ; i++){
		Campo c;
		cout << "Ingrese el nombre del campo" << endl;
		cin >> c.nombre;
		cout << "Seleccione el tipo: " << endl << "1.Entero" << endl << "2.Texto" << endl;
		cin >> tipo;
		if(tipo == 1){
			string s = "Entero";
			strcpy(c.tipo,s.c_str());
		}else{
			string s = "Texto";
			strcpy(c.tipo,s.c_str());
			cout << "Ingrese la longitud " << endl;
			cin >> c.tamano;
			if(c.tamano==1)
				c.tamano=2;
		}
		out.write(reinterpret_cast<char*>(&c), sizeof(Campo));
		registros.push_back(c);
	}	
	out.close();
}//Es en donde el usuario define la estructura
int getStructure(const char*nbin, vector<Campo>& registros){
	ifstream in(nbin, ios::in|ios::binary);
	charint ci;
	charint availlist;
	if(in){
		char buffer[sizeof(int)];
		in.read(buffer,sizeof(int));
		memcpy(ci.raw,buffer,sizeof(int));
		in.read(buffer,sizeof(int));
		memcpy(availlist.raw,buffer,sizeof(int));
		Campo c;
		int bytes=0;
		ci.num=ci.num-8;
		while(bytes < ci.num){
			if(!in.read(reinterpret_cast<char*>(&c),sizeof(Campo)))
				break;
			registros.push_back(c);
			bytes+=sizeof(Campo);
		}
		in.close();
	}else{
		cout << "No se pudo abrir el archivo" << endl;
		return 0;
	}

}//Donde se obtiene una estructura guardada previamente
void Agregar(const char* nbin, vector<Campo> registros){
	fstream out (nbin, ios::in|ios::binary|ios::out);
	charint availist;
	out.seekg(4);
	char b[sizeof(int)];
	out.read(b,sizeof(int));
	memcpy(availist.raw,b,sizeof(int));
	if(availist.num != 0){
		out.seekp(availist.num+1);
		char c[sizeof(char)];
		out.read(c,sizeof(char));
		int x = (int)c[0];
		x-=48;
		out.seekp(4,ios::beg);
		out.write(reinterpret_cast<char*>(&x),sizeof(int));
		out.seekp(availist.num,ios::beg);
	}else{
		out.seekp(0,ios::end);
	}
	for(int i=0; i< registros.size();i++){
		if(registros[i].tipo == "Entero"){
			int value;
			cout << "Ingrese " << registros[i].nombre << endl;
			cin >> value;
			out.write(reinterpret_cast<char*>(&value), sizeof(int));
		}else{
			char texto[registros[i].tamano];
			cout << "Ingrese " << registros[i].nombre << endl;
			cin >> texto;
			out.write(texto,registros[i].tamano-1); 
		}
	}
	out.close();

}//Agrega los registros
void Listar(const char* nbin, vector<Campo> registros){
	charint cbyte;
	charint ci;
	//	cout << registros.size() << endl;
	int i=0;
	ifstream in(nbin,ios::in|ios::binary);
	char b[sizeof(int)];
	in.read(b, sizeof(int));
	memcpy(cbyte.raw,b,sizeof(int));
	in.seekg(cbyte.num,ios::beg);
	//cout << cbyte.num << endl;
	int pos=1;
	while(true){
		if(i==registros.size()){
			cout << endl;
			i=0;
			pos++;
		}
		if(registros[i].tipo=="Entero"){
			char buffer[sizeof(int)];
			if(!in.read(buffer,sizeof(int)))
				break;
			if(buffer[0] == '*'){
				int size = -1*sizeof(int);
				for(int i=0;i < registros.size() ;i++){
					if(registros[i].tipo =="Entero")
						size += sizeof(int);
					else
						size = size + registros[i].tamano-1;
				}
				size = size + in.tellg();
				in.seekg(size);
				pos++;
			}else{
				if(i==0)
					cout << pos << ". ";
				cout << registros[i].nombre <<"-";
				memcpy(ci.raw,buffer,sizeof(int));
				if(i != registros.size()-1){
					cout << ci.num << ",";
				}else{
					cout << ci.num;
				}
				i++;	
			}
		}else{
			char buffer[registros[i].tamano-1];
			if(!in.read(buffer,registros[i].tamano-1))
				break;
			if(buffer[0] == '*'){
				int size = -1*(registros[i].tamano-1);
				for(int i=0;i < registros.size() ;i++){
					if(registros[i].tipo =="Entero")
						size += sizeof(int);
					else
						size = size + registros[i].tamano-1;
				}				
				size = size + in.tellg();
				in.seekg(size);
				pos++;

			}else{
				if(i==0)
					cout << pos << ". ";
				cout << registros[i].nombre << "-";
				buffer[registros[i].tamano-1]='\0';
				if(i != registros.size()-1){
					cout << buffer << ",";
				}else{
					cout << buffer;
				}
				i++;
			}
		}
	}	
	in.close();	

}//Lista los registros


void Borrar(const char* nbin, vector<Campo> registros){
	Listar(nbin, registros);
	int rrn;
	cout << "Ingrese el numero del registro a eliminar" << endl;
	cin >> rrn;
	rrn=rrn-1;
	int os = offset(rrn, registros);
	charint cbyte;
	charint availlist;
	fstream in(nbin,ios::in|ios::binary|ios::out);
	char b[sizeof(int)];
	in.read(b, sizeof(int));
	memcpy(cbyte.raw,b,sizeof(int));
	os=os+cbyte.num;
	in.read(b, sizeof(int));
	memcpy(availlist.raw,b, sizeof(int));
	in.seekp(os);
	in.write("*",1);
	char av = (char)(availlist.num + 48);
	in.put(av);
	in.seekp(4);
	in.write(reinterpret_cast<char*>(&os),sizeof(int));
	in.close();
}//Borra al agregar un * en el primer byte del registro y un caracter representando el offset del siguiente espacio disponible
int offset(int rrn,vector<Campo> registros){
	int size=0;
	for(int i=0; i < registros.size() ; i++){
		if(registros[i].tipo == "Entero")
			size+=sizeof(int);
		else
			size = size + registros[i].tamano -1;
	}
	return rrn*size;
}
void Modificar(const char* nbin, vector<Campo> registros){
	Listar(nbin, registros);
	int rrn;
	cout << "Ingrese el numero del registro " << endl;
	cin >> rrn;
	rrn=rrn-1;
	int os = offset(rrn, registros);
	charint cbyte;
	fstream in(nbin,ios::in|ios::binary|ios::out);
	char b[sizeof(int)];
	in.read(b,sizeof(int));
	memcpy(cbyte.raw,b,sizeof(int));
	os+=cbyte.num;
	in.seekp(os);
	for(int i=0; i< registros.size();i++){
		if(registros[i].tipo == "Entero"){
			int value;
			cout << "Ingrese " << registros[i].nombre << endl;
			cin >> value;
			in.write(reinterpret_cast<char*>(&value), sizeof(int));
		}else{
			char texto[registros[i].tamano];
			cout << "Ingrese " << registros[i].nombre << endl;
			cin >> texto;
			in.write(texto,registros[i].tamano-1); 
		}
	}
	in.close();
}
void Buscar(const char* nbin, vector<Campo> registros){
	string data;
	cout << "Ingrese el dato a buscar" << endl;
	cin >> data;
	ifstream in(nbin, ios::in|ios::binary);
	charint cbyte;
	charint ci;
	//	cout << registros.size() << endl;
	int i=0;
	char b[sizeof(int)];
	in.read(b, sizeof(int));
	memcpy(cbyte.raw,b,sizeof(int));
	in.seekg(cbyte.num,ios::beg);
	//cout << cbyte.num << endl;
	bool flag = false;
	stringstream registro;
	while(true){
		if(i==registros.size()){
			if(flag)
				break;
			else
				registro.str("");	
			i=0;
		}
		if(registros[i].tipo=="Entero"){
			char buffer[sizeof(int)];
			if(!in.read(buffer,sizeof(int)))
				break;
			if(buffer[0] == '*'){
				int size = -1*sizeof(int);
				for(int i=0;i < registros.size() ;i++){
					if(registros[i].tipo =="Entero")
						size += sizeof(int);
					else
						size = size + registros[i].tamano-1;
				}
				size = size + in.tellg();
				in.seekg(size);
			}else{
				memcpy(ci.raw,buffer,sizeof(int));
				stringstream ss;
				ss >> ci.num;
				if(data.compare(ss.str())==0)
					flag = true;
				registro <<  ci.num << ",";
				i++;	
			}
		}else{
			char buffer[registros[i].tamano-1];
			if(!in.read(buffer,registros[i].tamano-1))
				break;
			if(buffer[0] == '*'){
				int size = -1*(registros[i].tamano-1);
				for(int i=0;i < registros.size() ;i++){
					if(registros[i].tipo =="Entero")
						size += sizeof(int);
					else
						size = size + registros[i].tamano-1;
				}				
				size = size + in.tellg();
				in.seekg(size);

			}else{
				buffer[registros[i].tamano-1]='\0';
				string s;
				memcpy ((char*)s.c_str(),buffer,registros[i].tamano-1);
				if(data.compare(buffer)==0){
					flag=true;
				}
				registro << s << ",";
				i++;
			}
		}
	}
	if(!flag)
		cout << "No se encontró el archivo" << endl;
	else
		cout << registro.str() << endl;	

}
void Compactar(const char* nbin, const char* nbin2,vector<Campo> registros){
	fstream in(nbin, ios::in|ios::binary);
	ofstream out(nbin2, ios::out|ios::binary);
	charint cbyte;
	charint ci;
	charint availlist;
	int i=0;
	char b[sizeof(int)];
	in.read(b, sizeof(int));
	memcpy(cbyte.raw,b,sizeof(int));
	in.read(b, sizeof(int));
	memcpy(availlist.raw,b,sizeof(int));
	int al=0;
	Campo c;
	int bytes=0;
	out.write(reinterpret_cast<char*>(&cbyte.num),sizeof(int));
	out.write(reinterpret_cast<char*>(&al),sizeof(int));
	while(bytes < cbyte.num-8){
		if(!in.read(reinterpret_cast<char*>(&c),sizeof(Campo)))
			break;
		out.write(reinterpret_cast<char*>(&c),sizeof(Campo));
		bytes+=sizeof(Campo);
	}
	in.seekg(cbyte.num);
	while(true){
		if(i==registros.size()){
			i=0;
		}
		if(registros[i].tipo=="Entero"){
			char buffer[sizeof(int)];
			if(!in.read(buffer,sizeof(int)))
				break;
			if(buffer[0] == '*'){
				int size = -1*sizeof(int);
				for(int i=0;i < registros.size() ;i++){
					if(registros[i].tipo =="Entero")
						size += sizeof(int);
					else
						size = size + registros[i].tamano-1;
				}
				size = size + in.tellg();
				in.seekg(size);
			}else{
				i++;
				out.write(buffer,sizeof(int));
			}
		}else{
			char buffer[registros[i].tamano-1];
			if(!in.read(buffer,registros[i].tamano-1))
				break;
			if(buffer[0] == '*'){
				int size = -1*(registros[i].tamano-1);
				for(int i=0;i < registros.size() ;i++){
					if(registros[i].tipo =="Entero")
						size += sizeof(int);
					else
						size = size + registros[i].tamano-1;
				}				
				size = size + in.tellg();
				in.seekg(size);

			}else{
				out.write(buffer, registros[i].tamano-1);	
				i++;
			}
		}
	}	


	out.close();	
	in.close();	


}

