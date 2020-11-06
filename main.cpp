#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <math.h>

#define BLOCOS 200
#define BYTES 102400
using namespace std;

class FAT{
public:
	int qt_arquivos;
	int *tabela_fat; // tabela fat eh uma lista estatica
	int *bitmap; // bitmap eh uma lista estatica
	int size_tabela;
	int blocos_ocupados, blocos_vazios;
	int primeiro_bloco,segundo_bloco;
	
	ofstream arquivo_tabela; // usado para gravar a tabela fat no disco 
	ofstream arquivo_bitmap; // usado para gravar o bitmap do fat no disco
	
	FAT (int tam){
		qt_arquivos = 0;
		size_tabela = 200;
		blocos_vazios = 199;
		blocos_ocupados = 1;
		primeiro_bloco = 1;
		segundo_bloco = 2;
		
		tabela_fat = new int[200];
		bitmap = new int[200];
		
		bitmap[0] = 1;
		tabela_fat[0] = -1;
		
		for (int i=1; i<tam; i++){
			tabela_fat[i]= 0;
			bitmap[i]= 0;
		}
		escreve_arquivo();
		cout <<"[+] tabela fat criada com sucesso"<<endl;
		cout <<"[+] bitmap fat criado com sucesso"<<endl;
	}
	
	void escreve_arquivo(){  	// grava tabelafat e bitmap no disco - escreve no arquivo 
		arquivo_tabela.open("FAT_Table.txt");
		arquivo_bitmap.open("FAT_Bitmap.txt");
		
		arquivo_tabela << "~~~~~~Tabela FAT ~~~~~~~~"<<endl;
		
		for (int i=0; i<size_tabela; i++){
			arquivo_tabela << i << " " << tabela_fat[i] <<endl;
			arquivo_bitmap << i << " " << bitmap[i] <<endl;
		}
		arquivo_tabela << "--- fim tabela fat ---"<<endl;
		arquivo_bitmap <<"--- fim bitmap ---"<<endl;
		arquivo_tabela.close();
		arquivo_bitmap.close();
	}
	void mostratabelafat(){
		for (int i=0; i<size_tabela; i++)
			cout << i << " "<< tabela_fat[i] <<endl;
	}
	void mostrabitmapfat(){
		for (int i=0; i<size_tabela; i++)
			cout << i << " "<< bitmap[i]<<endl;
	}
	//alocacao contigua
	void aloca(int quantidade_blocos){ // insere blocos do arquivo na tabelafat
		qt_arquivos ++;
		blocos_ocupados = blocos_ocupados + quantidade_blocos;
		blocos_vazios = blocos_vazios - quantidade_blocos;
		for (int i=0; i< quantidade_blocos; i++){
			if (i == quantidade_blocos - 1){ // ultimo bloco do arq
				tabela_fat[primeiro_bloco] = -1; // ultimo bloco na tabela recebe -1
				primeiro_bloco = primeiroespaco(); // o primeirobloco recebe o proximo espaco
				segundo_bloco = segundoespaco(); // o segundobloco recebe o segundo prox espa�o
			}
			else{
				tabela_fat[primeiro_bloco] = segundo_bloco; // primeiro bloco recebe valor do segundo bloco
				primeiro_bloco = segundo_bloco; // avanca para o segundo
				segundo_bloco = segundoespaco(); // segundo bloco avanca para o terceiro...
			}
		}
		for (int i=0; i<quantidade_blocos; i++){
			if (tabela_fat[i] != 0)
				bitmap[i] = 1;
		}
		atualizabitmap();
		escreve_arquivo();
	}
	void desaloca (int primeiro_bloco_do_arquivo, int quantidade_blocos_arquivo){ // remove blocos da tabela fat 
		qt_arquivos --;
		int i = primeiro_bloco_do_arquivo;
		int aux; 
		
		while(tabela_fat[i]!= -1){ // percorre todos os blocos desse arquivo e o substitui por 0
			aux = tabela_fat[i];
			tabela_fat[i] = 0;
			i = aux; 
		}
		tabela_fat[i] = 0;
		blocos_vazios += quantidade_blocos_arquivo;
		blocos_ocupados -= quantidade_blocos_arquivo;
		primeiro_bloco = primeiroespaco(); 
		segundo_bloco = segundoespaco();
		atualizabitmap();
		escreve_arquivo();
	}
	void atualizabitmap(){ // atualiza o bitmap 
		for (int i= 0; i<size_tabela; i++){
			if(tabela_fat[i]!= 0)
				bitmap[i]= 1;
			else
				bitmap[i]= 0;
		}
	}
	int primeiroespaco(){ // retorna a posicao do proximo espaco vazio
		for (int i = 0; i < size_tabela; i++) {
			if (tabela_fat[i] == 0) {
      			return i;
  			}
  		}
		return -1;
	}
	int segundoespaco(){ // retorna a posicao do segundo proximo espaco vazio
		int p=0;
		for (int i=0; i<size_tabela; i++){
			if (tabela_fat[i] == 0 && !p)
      			p = 1;
    		else if (tabela_fat[i] == 0 && p)
      			return i;
		}
		return -1;
	}
};


class File{
public:
	string nome; 
	int tamanho; 
	int first_bloco;
	int num_blocos;	
	string conteudo; 
	
	File (string n, int t, int fb, string cont){
		nome = n;
		cout << "[+] arquivo '" << nome <<"' criado com sucesso"<<endl;
		tamanho = t;
		first_bloco = fb; 
		num_blocos = (tamanho/513)+1;
		conteudo = cont;
	}
	
	string getNomeArquivo(){
  	return nome;
	}
	
	int getBlocosOcupados(){
  		return num_blocos;
	}
	
	int getPrimBlocoArquivo(){
	  return first_bloco;
	}
	
	int getTamanhoArquivo(){
		return tamanho;
	}
	string getConteudo(){
		return conteudo;
	}

	void mostrarArquivo(){
		char buffer[15]={};
		cout <<endl;
		cout << "| file | " << converteParaString(nome, 14, "");
		cout << "| " << converteParaString(to_string(tamanho), 12, "tam") << " |      " ;
  		cout << converteParaString(to_string(num_blocos), 10, "") << " |        " ;
  		cout << converteParaString(to_string(first_bloco), 9, "") <<"|\n";
	}
	
	void mostrarConteudoArquivo(){
		cout << "->"<< getConteudo();
	}
	
	string converteParaString(string nome, int tamanho_texto, string flag){ // funcao auxiliar para mostrar bonitinho na tela
		char str_aux[tamanho_texto];
		int i;
		for (int i = 0; i < tamanho_texto; i++) 
			str_aux[i] = ' ';
 		str_aux[i-1] = '\0';
		for (i = 0; i < nome.size(); i++)
			str_aux[i] = nome[i];
 		 if (flag == "tam"){
    		str_aux[i+1] = 'K';
    		str_aux[i+2] = 'B';
  		}
		string mycppstr(str_aux);
		return mycppstr;
	}
};

class DiretorioFAT{
public:
	vector<File> lista_arquivos;
	vector<DiretorioFAT> lista_diretorio;
	string nome_diretorio;
	int qtd_diretorios;
	DiretorioFAT *diretorio_pai;	
	
	DiretorioFAT(string name, DiretorioFAT *pai){
		nome_diretorio = name;
		diretorio_pai = pai;
		qtd_diretorios=0;
		cout << "\n[+] diretorio '"<< nome_diretorio << "' criado com sucesso"<<endl;
	}
	
	int criarDiretorio(string n_dir, DiretorioFAT *n_pai){
		lista_diretorio.push_back(*new DiretorioFAT(n_dir, n_pai));
		qtd_diretorios++;
		return 1;
	}
	void mostrarDiretorio(){
		cout << " _________________________________________________________________________"<<endl;
		cout << "| TIPO |      NOME     |    TAMANHO   | BLOCOS OCUPADOS | BLOCO INICIAL   |"<<endl;
		cout << "|_________________________________________________________________________|"<<endl;
		for (int i = 0; i < lista_arquivos.size(); i++){
			lista_arquivos[i].mostrarArquivo();
			//cout << "2entrou /"<<endl;
		}
  		for (int i = 0; i < lista_diretorio.size(); i++){
    		lista_diretorio[i].mostrarNomeDiretorio();
    		//cout << "3entrou /"<<endl;
    	}
		cout <<endl;
	}
	void mostrarNomeDiretorio(){
		char str_aux[15];
		int i = 0; 
		
		for (int i = 0; i < 15; i++)
			str_aux[i] = ' ';
		str_aux[i-1] = '\0';
		for (int i = 0; i < nome_diretorio.size(); i++)
			str_aux[i] = nome_diretorio[i];
  		cout << "\n|  dir | " << str_aux << endl;
	}
	
	void criarArquivo(string n_arq, int n_tam_arq, int pr_bloco, string cont){
		lista_arquivos.push_back(*new File(n_arq, n_tam_arq, pr_bloco, cont));
	}
	
	int excluirArquivo(string n_arq, int *prim_bloco, int *qtd_blocos_ocupados, int *tam_arq_excluido){
		for (int i = 0; i < lista_arquivos.size(); i++) {
			if (lista_arquivos[i].nome == n_arq) {
				*prim_bloco = lista_arquivos[i].first_bloco;
				*qtd_blocos_ocupados = lista_arquivos[i].num_blocos;
				*tam_arq_excluido = lista_arquivos[i].tamanho;
      			lista_arquivos.erase(lista_arquivos.begin() + i);
				return 1;
			}
  		}
  		return 0;
	}
	
	int getQtdDiretorio(){
 		return qtd_diretorios;
	}
	
	DiretorioFAT* getDiretorio(string n_dir){
  		for (int i = 0; i < lista_diretorio.size(); i++) {
    		if (lista_diretorio[i].nome_diretorio == n_dir)
      			return &lista_diretorio[i];
  		}
		return nullptr;
	}
	DiretorioFAT* getDiretorioPai(){
		return diretorio_pai;
	}
};

int verificarEspaco(int tamanho, int *pr_bloco, FAT *fat){
  if(fat->blocos_vazios < (tamanho/513)+1){
    return 0;
  }
  *pr_bloco = fat->primeiroespaco();
  fat->aloca((tamanho/513)+1);

  return 1;
}

int main (){
	int qual_sistema; // qual sistema quer simular //
	
	cout << " _________________________________________________"<<endl;
	cout << "|===========|  SISTEMA DE ARQUIVOS  |=============|"<<endl;
	cout << "|___________|     Lucas & Carlos    |_____________|"<<endl;
	cout << "|_________________________________________________|"<<endl;
	cout << "|     1 - FAT                   2 - INode (OFF)   |"<<endl; 
	cout << "|_________________________________________________|"<<endl;
	cout << "\nEscolha o sistema:"<<endl;
	cout << "> ";

	cin >> qual_sistema;
	
	 // se for FAT 
	if (qual_sistema == 1){ 
		FAT tabela_fat (BLOCOS);
		DiretorioFAT diretorio_raiz ("Raiz", nullptr);
		DiretorioFAT *dir_atual = &diretorio_raiz;
		
		int quantidade_arquivos = 0;
		int quantidade_diretorios = 1;
		int espaco_ocupado = 512; 
		string entrada, comando; 

		int i = 0;
		
		// entradas do tipo TAMANHO em KB // 
		cout << "\nDigite help para ver todos os comandos";
		cout << "\n>";
		while(cin >> comando){
		//	system ("cls");
			string nome_arq, nome_dir;
			int tam_arq;
			string conteudo;
			if (comando == "mkdir"){
				// cria diretorio 
		        cin>> nome_dir;
        		dir_atual->criarDiretorio(nome_dir, dir_atual);
        		tabela_fat.aloca(1);
        		quantidade_diretorios++;
        		espaco_ocupado+=512;
			}
			else if (comando == "cd"){
     	 		cin >> nome_dir;
      			if (nome_dir == ".."){
       		 	// voltar para o diretorio anterior
        			if (dir_atual->nome_diretorio != "Raiz"){
          				dir_atual = dir_atual->getDiretorioPai();
        			}
      			}
				else {
        		// entrar em um diretorio
       				DiretorioFAT *dir_aux = dir_atual->getDiretorio(nome_dir);
					if (dir_aux == nullptr)
        				cout << "[!] diretorio nao existe"<<endl;
					else
        				dir_atual = dir_aux;
      			}
      			cout << "diretorio atual> " << dir_atual->nome_diretorio << endl;

			}
			else if (comando == "ls"){ 
				// lista conteudo do diretorio 
				dir_atual -> mostrarDiretorio();
			//	cout << "1entrou /"<<endl;
			}
			else if (comando == "touch"){
				// cria uma entrada para o arquivo com o tamanho informado 
		        cin>> nome_arq;
      			cin >> tam_arq;
      			cin >> conteudo;
      			int pr_bloco;

     			 if(verificarEspaco(tam_arq, &pr_bloco, &tabela_fat)){
        			dir_atual->criarArquivo(nome_arq, tam_arq, pr_bloco, conteudo);
        			quantidade_arquivos++;
        			espaco_ocupado += tam_arq;
      			}
				else if (!verificarEspaco(tam_arq, &pr_bloco, &tabela_fat)){
					cout << "[!] sem espaco em disco"<<endl;
				}
			}
			else if (comando == "rm"){
				// exclui arquivo e libera bloco 
				int blocos_ocupados_arq_excluido;
      			int prim_bloco_arquivo_excluido;
      			int tam_arq_excluido;
		        cin>> nome_arq;

      			if(dir_atual->excluirArquivo(nome_arq, &prim_bloco_arquivo_excluido, &blocos_ocupados_arq_excluido, &tam_arq_excluido)){
        			tabela_fat.desaloca(prim_bloco_arquivo_excluido, blocos_ocupados_arq_excluido);
        			cout << "[-] arquivo removido"<<endl;
        			quantidade_arquivos--;
        			espaco_ocupado -= tam_arq_excluido;
      			}
				else 
        			cout << "[!] arquivo nao encontrado"<<endl;
        	}
		
			else if (comando == "cat"){
				// exibe conteudo do arquivo
		        cin>> nome_arq;
				int flag = 0;
				for (int i=0 ; i<dir_atual->lista_arquivos.size(); i++){
					if (dir_atual->lista_arquivos[i].getNomeArquivo() == nome_arq){
						dir_atual->lista_arquivos[i].mostrarConteudoArquivo();
						flag ++;
					}
				}
				if (flag == 0)
					cout <<"[!] arquivo nao encontrado nesse diretorio"<<endl;
			}
			
			else if (comando == "listmap"){
				// mostra o bit map
				tabela_fat.mostrabitmapfat();
			}
			 if (comando == "help"){
			 	cout << " _________________________________________________________________________________________________________"<<endl;
			 	cout << "|=================================|     LISTA DE COMANDOS      |==========================================|"<<endl;
			 	cout << "|_________________________________________________________________________________________________________|"<<endl;
				cout << "| mkdir NOME - Cria um diretorio com o nome NOME (tamanho maximo de 14 caracteres)                        |"<<endl;
				cout << "| cd NOME - Entra no diretorio NOME                                                                       |"<<endl;
				cout << "| cd .. - Retorna ao diretorio anterior                                                                   |"<<endl;
				cout << "| ls - Lista o conteudo do diretorio atual                                                                |"<<endl;
				cout << "| touch ARQUIVO.EXT TAMANHO CONTEUDO_INICIAL - Cria uma entrada para o arquivo.ext para o tamanho indicado|"<<endl;
				cout << "| rm ARQUIVO.EXT - Exclui esse arquivo e libera os blocos                                                 |"<<endl;
				cout << "| cat ARQUIVO.EXT - Exibe o conteudo do arquivo                                                           |"<<endl;
				cout << "| listmap - Exibe o mapa de bits                                                                          |"<<endl;
				cout << "| disk - Mostra as informacoes gerais do sistema                                                          |"<<endl;
				cout << "| help - Lista os comandos                                                                                |"<<endl;
				cout << "| clear - Limpa a tela                                                                                    |"<<endl;
				cout << "| quit - Encerra a execucao                                                                               |"<<endl;
				cout << "|_________________________________________________________________________________________________________|"<<endl;
			}
			else if (comando == "disk"){
				cout << " __________________________________________________________________"<<endl;
				cout << "|===========|  INFORMACOES SOBRE O SISTEMA DE ARQUIVOS |===========|"<<endl;
				cout << "|__________________________________________________________________|"<<endl;
      			cout << "| Quantidade de Arquivos  : " << quantidade_arquivos <<endl;
      			cout << "| Quantidade de Diretorios: " << quantidade_diretorios <<endl;
     			cout << "| Espaco ocupado: " << espaco_ocupado << " KB"<<endl;
     			cout << "| Espaco livre: "<< BYTES - espaco_ocupado << " KB"<<endl;
      			cout << "|__________________________________________________________________|"<<endl;
			}
			else if (comando == "clear"){
				system ("cls"); // windows
			//	system ("clear"); // linux
			}
			else if (comando == "quit"){
				 break;
			}
			cout <<"\n>";
		}
	}
	else if (qual_sistema == 2){
		cout <<"O sistema de arquivos INode sera implementado em outro programa"<<endl;
	}
	system ("pause");
	// se for Inode 
	return 0;
}