///********************************************************
/// CONTROL Y MONITORIZACIÓN DE UN INVERNADERO A TRAVÉS//// 
/// DE UNA APLICACIÓN MÓVIL                             ////
/// Trabajo Final de Master Ingeniera Electromecanica  ////
/// Esp. Mecatronica                                   ////
/// Departamento de Electronica, Automatica e Infor.   ////
/// Indusrial.                                         ////
/// Escuela Tec. Superior de Ing. y Diseño Ind.        ////
/// Universidad Politecnica de Madrid                  ////
/// Alumno: ANDRES BARROSO GARCIA                      ////
/// NºMatricula: 2053                                  ////
/// Curso: 2014-2015                                   ////
/// Fecha: 27-06-2015                                    ////
///********************************************************
// Aplicacion usersICRA
//
// Gestion de usuarios y permisos para el control de ICRA
////////////////////////////////////////////////////////////

//Librerias
#include <iostream>
#include <stdio.h>
#include <mysql++/mysql++.h>//MySQL
#include <SerialPort.h>//SerialPort
#include <ctime> //Date-Time
#include <termios.h>
#include <unistd.h>
#include <fstream>
#include "date.h"
#include "echo.h"

using namespace std;

//Cabeceras funciones
void Inicio();//Inializacion de serverICRA
void NewUser();//Insertar Nuevo Usuario
void ViewUsers();
void DeleteUsers();
string NewTable(string ,string);//Crear tabla en BD

//Variables globales

mysqlpp::Connection conn(false);//Variables conexion MySQL
mysqlpp::Query query=conn.query();

char server[] = "localhost";
char user[10];  
char pass[10];
string db;
string tabla="users";
bool x=true;

int option;

int main()
{
	Inicio();//Identificacion. Conexion MySQL	
	
	while(x){	
		
		cout<<"\033[1;32m";
		cout<<"******************************************"<<endl;
		cout<<"1-Insertar nuevo usuario en la tabla users"<<endl;
		cout<<"2-Visualizar los usuarios de la tabla users"<<endl;
		cout<<"3-Eliminar un usuario de la tabla users"<<endl;
		cout<<"Inserte cualquier otra opción para finalizar la aplicacion"<<endl;
		cout<<"******************************************"<<endl;
		cout<<"\033[0m";
		
		cin>>option;		
			
		switch(option){			
			case 1: NewUser();break;
			case 2: ViewUsers();break;
			case 3: DeleteUsers();break;
			default: x=false;
			break;			
		}
	}	

	conn.disconnect();//Desconexion MySQL
	cout<<"END PROGRAM"<<endl;
	return 0;	
}

///************************************************
///*********** FUNCIONES DEL PROGRAMA *************
///************************************************

///************************************
///FUNCION: void Inicio()
///PARAMETROS: n/a
///RETORNO: n/a
///DESCRIPCION: Salida por pantalla mensaje bienvenida. Solicitud user,pass,bd,tabla,puertoSerie
///FUNCIONES INVOCADAS: NewTable(base de datos, tabla)
///*************************************

void Inicio(){
	
	//Cabecera
	cout<<"\033[0;32m********************************************************\033[0m"<<endl;
	cout<<"\033[0;32m***************\033[0m\033[1;32m      PROYECTO ICRA       \033[0m\033[0;32m***************\033[0m"<<endl;
	cout<<"\033[0;32m***********\033[0m\033[1;35m      Aplicacion usersICRA       \033[0m\033[0;32m************\033[0m"<<endl;	
	cout<<"\033[0;32m********************************************************\033[0m"<<endl;	
	cout<<"\033[1;32mInvernadero Controlado Remotamente por Android\033[0m"<<endl;
	cout<<"\033[1;36mAutor: Barroso García, Andrés\033[0m"<<endl;
	cout<<"URL: http://icra.blogspot.es"<<endl;
	cout<<"Proyecto desarrollado como Trabajo Final de Máster Curso 2014/15"<<endl;
	cout<<"Máster Universitario en Ingeniería Electromecánica: Mecatrónica"<<endl;
	cout<<"Escuela Técnica Superior de Ingeniería y Diseño Industrial"<<endl;
	cout<<"Universidad Politécnica de Madrid"<<endl;
	cout<<"\033[0;32m********************************************************\033[0m"<<endl;
	cout<<"\033[0;32m********************************************************\033[0m"<<endl;	
	
	bool ACESS=false;//Marca ID
	
	do{//IDentificacion		

		cout<<"\033[0;35mConectando con \033[0m"<<server<<endl;
		cout<<"Inserte USER: "<<endl;
		cin>>user;
		cin.ignore();//Necesario para el getline(); 	
			
		cout<<"Inserte PASS: "<<endl;	
			
		string pwd;
		echo( false );//Modo escritura oculta
		getline( cin, pwd );
		echo( true );//Modo escritura visible		
		strcpy(pass,pwd.c_str());
		
	
		if (conn.connect(NULL, server, user, pass)){//Conexion server
			ACESS=true;//ID OK
			cout<<"La Base de Datos que se introduzca tiene que ser la misma que la insertada "
			"a traves de  la aplicacion \033[1;35mserverICRA\033[0m"<<endl;
			cout<<"Inserte BD a empear: "<<endl;
			cin>>db;
				
			string consulta="SHOW DATABASES";
			query=conn.query(consulta);

			if (mysqlpp::StoreQueryResult res = query.store())
			{
				if (res.num_rows() > 0)
				{
					bool found=false;

					for (mysqlpp::StoreQueryResult::iterator it=res.begin(); it!=res.end(); ++it)
					  {
						if ((it)->at(0)==db)//Consulta existencia de base datos insertada
						{
						 found=true;
						 break;
						}
					  }
					if (found)//SI EXISTE LA BD...
					{  
					 cout << "La base de datos se encuentra" << endl;
					// cout << "Inserte nombre de la tabla a crear" <<endl;
					// cin >> tabla;
					 query.reset();
					 query<<"SELECT * FROM `"+db+"`.`"+tabla+"`";
					 res=query.store();
					 if (res.num_rows() ==0){//SI NO EXISTE...
					  
						if (conn.select_db(db))
						{
						 cout << "Creando tabla" << endl;
						 query.reset();
						 
						 query=conn.query(NewTable(db,tabla)); //Creacion tabla
						 if (!query.execute())//Conectado a tabla en BD
							cout<<"Error al crear la tabla"<<endl;								   
						}
						else
							cout << "Fallo al seleccionar la base de datos" << endl; 					 					 
					  }
					}
					else//SI NO EXISTE BD...
					{ 
						cout << "Creando base de datos" << endl;
						if (conn.create_db(db))//Creacion base datos
						  {
							cout << "Seleccionando base de datos" << endl;
							if (conn.select_db(db))
							  {
								//cout << "Inserte nombre de la tabla a crear" <<endl;
								//cin >> tabla;
								cout << "Creando tabla" << endl;
								query.reset();									 
								query=conn.query(NewTable(db,tabla));//Creacion tabla
								if (!query.execute())//Conectado a tabla en BD
									cout<<"Error al crear la tabla"<<endl;
								}
							else
								cout << "Fallo al seleccionar la base de datos" << endl;
						   }
					}
				}				
				else
					cout << "Fallo al obtener bases de datos" << endl;				
			}		   
		}      
		else{//Fallo conexion server
			cout <<"Fallo al conectarse a la BD: "<<conn.error()<<endl;
			ACESS=false;//RESET ID
		}
			
	}while(ACESS==false);//Bucle Peticion IDentificacion
	
}

///************************************
///FUNCION: string NewTable(Base de datos,tabla)
///PARAMETROS: Tabla a crear en Base de datos
///RETORNO: Sentencia para crear nueva tabla en MySQL
///DESCRIPCION: Concatenar strings para creacion nueva tabla
///FUNCIONES INVOCADAS: n/a
///*************************************

string NewTable(string _db,string _tabla){
	
	return("CREATE TABLE IF NOT EXISTS `"+_db+"`.`"+_tabla+"` ("
                      "`id` bigint(20) NOT NULL AUTO_INCREMENT,"
                      "`time` time NOT NULL,"
                      "`date` date NOT NULL,"
                      "`user` varchar(10) NOT NULL,"
                      "`pass` varchar(10) NOT NULL," 
                      "`level` varchar(20) NOT NULL,"
                      "`email` varchar(30) NOT NULL,"
                      "PRIMARY KEY (`id`),UNIQUE KEY (`user`)"
                      ") ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;");   
	} 

///************************************
///FUNCION: void NewUser()
///PARAMETROS: Usuario, Password y nivel de privilegios
///RETORNO: n/a
///DESCRIPCION: Comunicacion SerialPort Arduino. Escritura en base de datos.
///FUNCIONES INVOCADAS: n/a
///*************************************

void NewUser() 
{
 	char time[15],date[15];//cadenas de tiempo
	string usuario,password,level,email;		
	  	
	Date(time,date);//Obtencion Fecha/Hora actual		
	
	cout<<"Inserte usuario: ";
	cin>>usuario;
	cout<<endl;
	
	cout<<"Inserte pass (4 digitos): ";
	cin>>password;
	cout<<endl;
	
	cout<<"Inserte level (1-ADMINISTRADOR / 2-USUARIO): ";
	int level_option;
	cin>>level_option;
	if(level_option==1)
		level="ADMINISTRADOR";
	else 	
		level="USUARIO";
	cout<<endl;	
	
	cout<<"Inserte email: ";
	cin>>email;
	cout<<endl;			
	
	//Insercion en base de datos					
	query.reset();
	query << "INSERT INTO "+db+"."+tabla+" VALUES (NULL, '"+time+"', '"+date+"', '"+usuario+"', '"+password+"', '"+level+"', '"+email+"');";
	
	if(!query.execute())
		cout << "Fallo al introducir datos. Usuario ya existente"<<endl;				 
}

///************************************
///FUNCION: void ViewUsers()
///PARAMETROS: n/a
///RETORNO: n/a
///DESCRIPCION: Mostrar usuarios registrados
///FUNCIONES INVOCADAS: n/a
///*************************************
	
void ViewUsers(){
	
	 query.reset();
	 query<<"SELECT * FROM `"+db+"`.`"+tabla+"`";
	 mysqlpp::StoreQueryResult res=query.store();
	 if (res.num_rows() >0)//SI EXISTE LA TABLA...
	   {

		 for (mysqlpp::StoreQueryResult::iterator it=res.begin(); it!=res.end(); ++it)
		   {
			 for (unsigned i=0; i<res.num_fields(); i++)
			   cout <<res.field_name(i)<<" = "<< (it)->at(i) << endl;//Salida por pantalla
			   cout << endl;
		   }					 
	   }
	  else
		cout<<"Fall en visualizacion"<<endl;
}

///************************************
///FUNCION: void DeleteUsers()
///PARAMETROS: n/a
///RETORNO: n/a
///DESCRIPCION: Eliminar usuarios registrados
///FUNCIONES INVOCADAS: n/a
///*************************************

void DeleteUsers(){
	
	string _id;
	cout<<"Inserte ID del usuario a eliminar: ";
	cin>>_id;
	
	query.reset();
	query << "DELETE FROM "+db+"."+tabla+" WHERE id="+_id;
	
	if(!query.execute())
		cout << "Fallo al eliminar datos"<<endl;		
	else
		cout<<"Usuario eliminado correctamente"<<endl;
}

