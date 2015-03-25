//Angel Acosta 20117857

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//Inicio de programa
int main()
{
    //Declaracion de variables
    int i,j,ps;
	int num_l_r,num_l_c,Tag_size;
	int C,M,B;
	//Calcúlo de variables

	freopen("Config.txt","r", stdin); //Se abre el archivo en lectura, contenedor de la configuracion
	//Se carga archivo a los datos del cache, memoria y bloques, tambien opciones adicionales de nuestra simulacion
    scanf("%d", &C);
    scanf("%d", &M);
    scanf("%d", &B);
    //opcion de politica de sustitucion a usar
    scanf("%d", &ps);
	fclose(stdin); //Se cierra archivo, se termina la carga

    //calculo con la distribucion que usremos para cache, memoria, lineas, etc
    num_l_r=M/B;
    //simulamos cuantos espacio de memoria nos costaria almacenar la cantidad de lineas
    Tag_size=log2(num_l_r);
	num_l_c=C/(Tag_size+B);

	int RAM[num_l_r][B]; //Matriz representativa de memoria principal

	freopen("Ram.txt","r", stdin); //Se abre el archivo en lectura, contenedor de  la data de la memoria principal
	//Se asigna el elemento del archivo a matriz de memoria principal para rellenarlo facilmente
	scanf("%d", &RAM[0][0]);
	for(i=0; i<num_l_r; i++)
	{
		for(j=0; j<B; j++)
		{
			RAM[i][j]=RAM[0][0];
		}
	}
	//Version que lee Ram.txt 1 a 1
	/*
	for(i=0; i<num_l_r; i++)
	{
		for(j=0; j<B; j++)
		{
			scanf("%d", &RAM[i][j]);
		}
	}*/

	fclose(stdin); //Se cierra archivo
	int Acache[num_l_c][Tag_size+B];   //Matriz representativa de memoria cache
	for(i=0; i<num_l_c; i++)
	{
		for(j=0; j<Tag_size+B; j++)
		{
			//en nuestra simulacion usaremos -1 para ver si el espacio cache esta vacio
			Acache[i][j]=-1;
		}
	}
	freopen("Op.txt","r", stdin); //Se abre el archivo en lectura con las  operaciones y direcciones
	freopen("salida.txt","w", stdout); //Se abre el archivo en escritura de los resultados de acierto y fallo de cache

	//Variables para el proceso de instruciones
	int address;
	int m,aux,data;
	int acierto,r,full=0,espcache=0;
	int line1,fila1;
	int cont_swap;
	char option[1];
    //para RR
    time_t t;
    srand((unsigned) time(&t));
	//Inicia proceso de lectura de operación-dirección
	scanf("%s", option);//Se lee instrucion (R,W o F)
	//Si no es el Fin (F) continua
	while(strcmp(option,"F"))
	{
		acierto=cont_swap=0;
		scanf("%d", &address); //Se lee dirección fisica
        //se ve a cual fila y columna debe pertenecer esa direccion
        line1=address/B;
        fila1=address%B;
        i=0;
        //si todavia quedaba espacio en la memoria ram
        if(full==0)
        {
            //verificamos cual es nuestro primer espacio libre, de haberlo i deberia almacenarnos ese valor
            while(Acache[i][0]!=-1&&i<num_l_c)
            {
                i++;
                espcache=i;
            }
            //de no haber espacio libre anotamos que ya la cache esta llena
            if(espcache==num_l_c)
                full=1;
        }
        //vamos verificando si tenemos ya el bloque en nuestra cache
        for(j=0;j<num_l_c&&acierto==0;j++)
        {
            if(Acache[j][0]==line1)
                acierto=1;
        }
        //nos quedamos con la j de la linea para usarla si hubo acierto
        j=j-1;
		if(option[0]=='R') //Si es operación de lectura
		{
				if(acierto==1) //si fue acierto
				{
					//Escribe en archivo de resultados de acierto y fallos
					printf("ACIERTO\n");
					//Ahora esa linea iremos subiendo hasta el "tope" (primera linea) de la memoria cache
                    //esto nos sera de utilidad a la hora de sustituir con la politica LRU
					for(cont_swap=j;cont_swap>0;cont_swap--)
                    {
                        for(m=0;m<Tag_size+B;m++)
                        {
                            aux=Acache[cont_swap][m];
                            Acache[cont_swap][m]=Acache[cont_swap-1][m];
                            Acache[cont_swap-1][m]=aux;
                        }
                    }
                    //NOTA: aqui podriamos leer el dato, pero no sera necesario para fines de esta simulacion
                    //Sobre todo porque no se pide mostrar por pantalla los resultados, solo la traza de acierto/fallo
				}
				if(acierto==0) //Si el el acierto es invalido
				{
					//Escribe en archivo de resultados de acierto y fallos
					printf("FALLO\n");
					//Se carga de memoria principal a cache
					if(full==0)
                    //si la cache no esta llena, colocamos el valor en la primera casilla vacia que nos indicaba i
                    {
						Acache[i][0]=line1;
                        for(j=Tag_size; j<Tag_size+B; j++)
                            Acache[i][j]=RAM[line1][j-Tag_size];
                        //y subimos la linea, ya que es la usada mas recientemente
                        for(cont_swap=i;cont_swap>0;cont_swap--)
                        {
                            for(m=0;m<Tag_size+B;m++)
                            {
                                aux=Acache[cont_swap][m];
                                Acache[cont_swap][m]=Acache[cont_swap-1][m];
                                Acache[cont_swap-1][m]=aux;
                            }
                        }
                    }
					if(full==1) //si esta llena
                    {
                        if(ps==1) //si estamos usando la politica de sustitucion LRU
                        {
                            //colocamos el bloque en la ultima casilla
                            //con esto se nos va la que usamos menos recientemente
                            Acache[num_l_c-1][0]=line1;
                            for(j=Tag_size; j<Tag_size+B; j++)
                                Acache[num_l_c-1][j]=RAM[line1][j-Tag_size];

                            //ahora vamos subiendo esa linea, ya que tambien es la usada recientemente
                            for(cont_swap=num_l_c-1;cont_swap>0;cont_swap--)
                            {
                                for(m=0;m<Tag_size+B;m++)
                                {
                                    aux=Acache[cont_swap][m];
                                    Acache[cont_swap][m]=Acache[cont_swap-1][m];
                                    Acache[cont_swap-1][m]=aux;
                                }
                            }
                        }
                        if(ps==2) //si estamos usando la politica de sustitucion RR
                        {
                            //generamos un valor random comprendido entre las lineas de cache
                            //cargamos de RAM en ese puesto
                            r = rand() % num_l_c;
                            Acache[r][0]=line1;
                            for(j=Tag_size; j<Tag_size+B; j++)
                                Acache[r][j]=RAM[line1][j-Tag_size];
                        }
                    }
				}
		}else //Si es operación de escritura
		{
			scanf("%d", &data); //Lee el dato a escribir
			//Deberiamos pasar primero por la cache, luego RAM, pero para agilizar el proceso
			//y debido a que hariamos un write-through de todas formas, escribimos de una vez en RAM
			RAM[line1][fila1]=data; //Ubica dirección en memoria principal y escribe
			//Escribe en cache
			if(acierto==1) //si fue acierto
				{
					//Escribe en archivo de resultados de acierto y fallos
					printf("ACIERTO\n");
					for(cont_swap=j;cont_swap>0;cont_swap--)
                    {
                        for(m=0;m<Tag_size+B;m++)
                        {
                            aux=Acache[cont_swap][m];
                            Acache[cont_swap][m]=Acache[cont_swap-1][m];
                            Acache[cont_swap-1][m]=aux;
                        }
                    }
                    //y recordamos llenar con la version modificada de esa linea de RAM
                    Acache[0][0]=line1;
                    for(j=Tag_size; j<Tag_size+B; j++)
                        Acache[0][j]=RAM[line1][j-Tag_size];
				}
				if(acierto==0) //Si el el acierto es invalido
				{
					//Escribe en archivo de resultados de acierto y fallos
					printf("FALLO\n");
					//Se carga de memoria principal a cache junto con la etiqueta
					if(full==0)//si quedan puestos vacios se carga ahi
                    {
						Acache[i][0]=line1;
                        for(j=Tag_size; j<Tag_size+B; j++)
                            Acache[i][j]=RAM[line1][j-Tag_size];
                        //Flotamos tambien esa linea, ya que es la usada mas recientemente
                        for(cont_swap=i;cont_swap>0;cont_swap--)
                        {
                            for(m=0;m<Tag_size+B;m++)
                            {
                                aux=Acache[cont_swap][m];
                                Acache[cont_swap][m]=Acache[cont_swap-1][m];
                                Acache[cont_swap-1][m]=aux;
                            }
                        }
                    }
					if(full==1)//si la cache esta llena
                    {

                        if(ps==1)//politica de sustitucion LRU, colocamos el dato en el ultimo lugar
                        {
                            Acache[num_l_c-1][0]=line1;
                            for(j=Tag_size; j<Tag_size+B; j++)
                                Acache[num_l_c-1][j]=RAM[line1][j-Tag_size];
                            //ahora vamos subiendo esa linea, ya que tambien es la usada recientemente
                            for(cont_swap=num_l_c-1;cont_swap>0;cont_swap--)
                            {
                                for(m=0;m<Tag_size+B;m++)
                                {
                                    aux=Acache[cont_swap][m];
                                    Acache[cont_swap][m]=Acache[cont_swap-1][m];
                                    Acache[cont_swap-1][m]=aux;
                                }
                            }
                        }
                        if(ps==2)//politica de sustitucion RR
                        {
                            r = rand() % num_l_c; //generamos un numero random entre las lineas que tiene la cache
                            Acache[r][0]=line1;
                            //colocamos el valor
                            for(j=Tag_size; j<Tag_size+B; j++)
                                Acache[r][j]=RAM[line1][j-Tag_size];
                        }
                    }
				}
		}
		scanf("%s", option); //Lee la siguiente operación
	}

	//Sale del ciclo, operacion de salida "FN"
	//Cierra ambos archivos de lectura y escritura
	fclose(stdin);
	fclose(stdout);
	//Abre archivo Ram2 para escribir estado final de memoria principal
	freopen("Ram2.txt","w", stdout);
	for(i=0; i<num_l_r; i++){
		for(j=0; j<B; j++){
			printf("%d", RAM[i][j]);
		}
		printf("\n");
	}
	fclose(stdout); //Cierra archivo

	freopen("cache.txt","w", stdout);//Abre archivo cache para escribir estado final de memoria cache
	for(i=0; i<num_l_c; i++){

        //Activar o desactivar el de abajo en caso de querer ver la info de la linea correspondiente almacenada
		//printf("%d   ", Acache[i][0]);

		for(j=Tag_size; j<Tag_size+B; j++){
			printf("%d", Acache[i][j]);
			}
		printf("\n");
	}
	fclose(stdout);//Cierra archivo

	//Fin programa
    return 0;
}

//Version Final
