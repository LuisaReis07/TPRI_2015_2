/* Aluna: Luisa dos Reis e Silva
   Sistema de RI com Modelo Vetorial*/

#include <string.h>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <sstream>
#include <cctype>
#include <math.h>
#include <time.h>

#include "tp_ri.h"

using namespace std;

Termo *vocabulario[QTDE_TERMOS];
VetorDocInfo *vetDocs[QTDE_DOCUMENTOS];
Termo *termosQuery[QTDE_TERMOS];
ResulQuery *resultados[QTDE_CONSULTAS];

//Para calcular o tempo de execucao
float tempo(clock_t t1, char m){
    clock_t t2;
    t2 = clock(); // clock depois
    //Delta dos clock's
    float diff = (float)( t2 - t1 );
    float segundos = 0;
     // Conversão para segundos
    if(CLOCKS_PER_SEC != 0) segundos = diff / CLOCKS_PER_SEC;
    return segundos;
}


//Normaliza a string, retirando caracteres especiais e convertendo para lowercase

string normalizaTermo(string termosLinha) {
    for (int i=0; i<termosLinha.length(); i++) {
        if (termosLinha[i]=='\''){
                termosLinha[i] = termosLinha[i+1];
                termosLinha[i+1] = '\0';}
        if ((termosLinha[i]=='-')||(termosLinha[i]==':')||(termosLinha[i]=='"')||(termosLinha[i]==',')
            ||(termosLinha[i]==';')||(termosLinha[i]=='!')||(termosLinha[i]=='?')||(termosLinha[i]=='/')||(termosLinha[i]=='\'')
            ||(termosLinha[i]=='|')||(termosLinha[i]=='{')||(termosLinha[i]=='[')||(termosLinha[i]=='}')||(termosLinha[i]==']')
            ||(termosLinha[i]=='_')||(termosLinha[i]=='=')||(termosLinha[i]=='+')||(termosLinha[i]=='(')||(termosLinha[i]==')')
            ||(termosLinha[i]=='*')||(termosLinha[i]=='.')){
                termosLinha[i] = '\0';
        }
        if(termosLinha[i]=='ç'){termosLinha[i] = 'c';}
		if(termosLinha[i]=='á'){termosLinha[i] = 'a';}
		if(termosLinha[i]=='à'){termosLinha[i] = 'a';}
		if(termosLinha[i]=='â'){termosLinha[i] = 'a';}
		if(termosLinha[i]=='ã'){termosLinha[i] = 'a';}
        if(termosLinha[i]=='é'){termosLinha[i] = 'e';}
		if(termosLinha[i]=='è'){termosLinha[i] = 'e';}
		if(termosLinha[i]=='ê'){termosLinha[i] = 'e';}
		if(termosLinha[i]=='í'){termosLinha[i] = 'i';}
		if(termosLinha[i]=='ì'){termosLinha[i] = 'i';}
		if(termosLinha[i]=='ó'){termosLinha[i] = 'o';}
		if(termosLinha[i]=='ò'){termosLinha[i] = 'o';}
		if(termosLinha[i]=='ô'){termosLinha[i] = 'o';}
		if(termosLinha[i]=='õ'){termosLinha[i] = 'o';}
		if(termosLinha[i]=='ú'){termosLinha[i] = 'u';}
		if(termosLinha[i]=='ù'){termosLinha[i] = 'u';}

        termosLinha[i] = tolower(termosLinha[i]);
 	}
 	return termosLinha;
}
unsigned int retiraStopWord(const char stopword[]) {
    if (strcmp(stopword, " ") == 0){ return 0;}
    if (strcmp(stopword, "") == 0){ return 0;}
    if (strcmp(stopword, "of") == 0){return 0;}
    if (strcmp(stopword, "the") == 0){return 0;}
    if (strcmp(stopword, "at") == 0){return 0;}
    if (strcmp(stopword, "on") == 0){return 0;}
    if (strcmp(stopword, "in") == 0){return 0;}
    if (strcmp(stopword, "as") == 0){return 0;}
    if (strcmp(stopword, "an") == 0){return 0;}
    if (strcmp(stopword, "a") == 0){return 0;}
    if (strcmp(stopword, "which") == 0){return 0;}
    if (strcmp(stopword, "is") == 0){return 0;}
    if (strcmp(stopword, "are") == 0){return 0;}
    if (strcmp(stopword, "was") == 0){return 0;}
    if (strcmp(stopword, "were") == 0){return 0;}
    if (strcmp(stopword, "whereas") == 0){return 0;}
    if (strcmp(stopword, "to") == 0){return 0;}

    return 1;
}

unsigned int somaValores(const char valor[]) {
    unsigned int soma = 0;
    while (*valor != 0) {
        soma += *valor;
        valor++;
    }
    return soma;
}

unsigned int LimpaVetorTermosQuery(){
    for (int i=0; i<QTDE_TERMOS; i++){
        termosQuery[i] = NULL;
    }
}
/* Gera a pasicao do termo no hash do vocabulario */
unsigned int gerarHash(const char termo[]) {
    return (somaValores(termo) % QTDE_TERMOS);
}

/* Funcao que normaliza o valor do tf*/
double normalizaTF(Documento *documento){
    double novoTF;
    if (documento->tf == 0) {
        novoTF = 0;
    } else {
        novoTF = 1 + log(documento->tf);
    }
    return novoTF;
}

double normalizaTFQuery(Termo *termo){
    double novoTF;
    if (termo->numTotaldeOcorrencias == 0) {
        novoTF = 0;
    } else {
        novoTF = 1 + log(termo->numTotaldeOcorrencias);
    }
    return novoTF;
}

/* Funcao que calcula o idf do termo*/
double calculaIDF(Termo *termo) {
    double idf;
    if (termo->numTotaldeDocumentos == 0) {
        idf = 0;
    } else {
        idf = log((double)QTDE_DOCUMENTOS / termo->numTotaldeDocumentos);
    }
    return idf;
}

unsigned int calcularVetorDoc() {

    //cout<<"Calculando Vetor Normal dos Documentos..."<<endl;

    for (int i=0; i<QTDE_DOCUMENTOS; i++){
        VetorDocInfo *vetAux = vetDocs[i];
        if (vetAux!=NULL){
            vetAux->valor = sqrt(vetAux->somatorioPesos);
        }
    }
}

/*Metodo responsavel por indexar os termos*/
unsigned int indexarTermo(const char documentoPN[], const char documentoRN[], const char documentoTitulo[], const char termoNome[]) {
    unsigned int posicao;
    int stopWord = retiraStopWord(termoNome);

    posicao = gerarHash(termoNome);
   if (stopWord==1){

    Termo *termo = (Termo *)malloc(sizeof(Termo));
    termo->nome = termoNome;
    termo->documento = NULL;
    termo->prox = NULL;
    termo->numTotaldeOcorrencias = 1;
    termo->numTotaldeDocumentos = 1;

    Documento *documento = (Documento *)malloc(sizeof(Documento));
    documento->paperNumber = documentoPN;
    documento->recordNumber = documentoRN;
    documento->title = documentoTitulo;
    documento->tf = 1;
    documento->prox = NULL;

    /* Se a posicao estiver vazia*/
    if (vocabulario[posicao] == NULL) {
        termo->documento = documento;
        vocabulario[posicao] = termo;
    } else {
        Termo *termTmp = vocabulario[posicao];
        Termo *firstTermTmp = (Termo *) malloc(sizeof(Termo));

        firstTermTmp = termTmp;
        bool hasTerm = false;

        for (; termTmp != NULL; termTmp = termTmp->prox) {
            if (strcmp(termTmp->nome, termoNome) == 0) {

                Documento *documentTmp = termTmp->documento;

                termTmp->numTotaldeOcorrencias++;

                hasTerm = true;
                Documento *docTmp = termo->documento;
                bool hasDoc = false;
                for (; docTmp != NULL; docTmp = docTmp->prox) {

                    if (strcmp(documentTmp->recordNumber, documentoRN) == 0) {

                        documentTmp->tf++;
                        hasDoc = false;
                     break;
                    }
                }

                if (hasDoc == false){
                    Documento *documentAux = termTmp->documento;
                    documento->prox = documentAux;
                    termTmp->documento = documento;
                    termTmp->numTotaldeDocumentos++;
                }

            }
        }

        if (!hasTerm) {
            termo->prox = firstTermTmp->prox;
            termo->documento = documento;

            firstTermTmp->prox = termo;
        }
    }

}

    return posicao;
}
/*Funcao que le o arquivo e separa o seu conteudo*/
int LeArquivo(string ARQ_ENTRADA){

	fstream arquivo;
	arquivo.open(ARQ_ENTRADA.c_str(), fstream::in);

	if( !arquivo.is_open() ){printf("Erro ao Abrir Arquivo %s", ARQ_ENTRADA.c_str()); return 1;}

    cout << "Lendo Documento " << ARQ_ENTRADA << "..." << endl;

	string linha, linhasubst, linhasubstaux, cleanWord, pn, rn, titulo; //O linhasubstaux só serve para verificar se estamos pegando informação no intervalo certo
	int i;

	while ( !arquivo.eof() ){

		getline(arquivo, linha);
		if(arquivo.eof()) break;

		linhasubst = linha.substr(0,3);

        //Essa informação não sera armazenada no struct, porque eu escolhi usar apenas o RN. Então o PN vai ser usado apenas para verificar os arquivos que estão sendo adicionados.
		if(linhasubst.find("PN") != string::npos) { //Paper Number: Código do artigo
		    linhasubstaux = linhasubst;
		    pn = linha.substr(3,linha.length());
//            cout<<linha<<endl; //Se quiser ver qual o documento que está sendo lido, pode descomentar essa linha
		}
		if(linhasubst.find("RN") != string::npos) { //Numero ID do artigo, variando de 1 até 1239
		    linhasubstaux = linhasubst;
		    rn = linha.substr(3,linha.length());
		}

		if(linhasubst.find("TI") != string::npos) { //Titulo do Artigo
		    linhasubstaux = linhasubst;
		    cleanWord = normalizaTermo(linha.substr(3,linha.length()));
		    titulo = cleanWord;
		    istringstream iss(titulo);
            do{
               string sub;
               iss >> sub;
               indexarTermo(pn.c_str(), rn.c_str(), titulo.c_str(), sub.c_str());
               //cout << "Substring: " << sub << endl;
            } while (iss);
		}

		if((linhasubst.find("AU ") != string::npos) || (linhasubst.find("SO") != string::npos)
            ||(linhasubst.find("MJ") != string::npos) || (linhasubst.find("MN") != string::npos) || (linhasubst.find("AB") != string::npos)
            ||(linhasubst.find("EX") != string::npos)){
            linhasubstaux = linhasubst;
            cleanWord = normalizaTermo(linha.substr(3,linha.length()));
            istringstream iss(cleanWord);
            do{
               string sub;
               iss >> sub;
                indexarTermo(pn.c_str(), rn.c_str(), titulo.c_str(), sub.c_str());
            } while (iss);
		}

		//RF = Referências. Não ajudam na pesquisa, nesse caso.
		if(linhasubst.find("RF") != string::npos) { linhasubstaux = "x"; }

		if((linhasubst.find("   ") != string::npos)&& (linhasubstaux != "x")) { //Quando achar uma linha que comece com "   ", vai verificar a qual marcador ela pertence e adicionar ao calculo
          cleanWord = normalizaTermo(linha.substr(3,linha.length()));

          istringstream iss(cleanWord);
            do{
               string sub;
               iss >> sub;
               indexarTermo(pn.c_str(), rn.c_str(), titulo.c_str(), sub.c_str());
            } while (iss);
		}

	}
	return 0;
}

//calcula informações sobre a query
unsigned int indexTermQuery(const char termoNome[]) {
    unsigned int stopWord;

    stopWord = retiraStopWord(termoNome);

   if(stopWord == 1){
//        cout<<"Termo "<<termoNome<<endl;
        int posicao = gerarHash(termoNome);

        Termo *termo = (Termo *)malloc(sizeof(Termo));
        termo->nome = termoNome;
        termo->numTotaldeOcorrencias = 1; //essa variavel vai servir como tf
        termo->documento = NULL;
        termo->prox = NULL;

        if (termosQuery[posicao] == NULL){
            termosQuery[posicao] = termo;
        }else{
            Termo *tmp = termosQuery[posicao]; //para percorrer os termos
            Termo *primeiroTermo = (Termo *)malloc(sizeof(Termo));
            primeiroTermo = tmp;
            bool possuiTermo = false;

            for (; tmp != NULL; tmp = tmp->prox) {
                if (strcmp(tmp->nome, termoNome) == 0) { //se for o mesmo termo
                    tmp->numTotaldeOcorrencias++;
                    possuiTermo = true;
                    break;
                }
            }
            //se nao for o mesmo termo, adiciona o termo na lista, pq ocorreu colisao
            if (!possuiTermo) {
                termo->prox = primeiroTermo->prox;
                primeiroTermo->prox = termo;
            }
        }
    }
    return 0;
}

/*Calcula similaridade*/
int calculaSimilaridades(){
    int posicao;
    double similaridades[QTDE_DOCUMENTOS]; //A posicao do vetor equivale ao num do documento.
    resultSim *top10Similaridade[11];
//    cout<<"Calcula Similaridade"<<endl;
    for (int i=0; i<QTDE_TERMOS; i++){
        if (termosQuery[i]!=NULL){

            Termo *tmp = termosQuery[i]; //para percorrer os termos da consulta
            tmp->numTotaldeOcorrencias = normalizaTFQuery(tmp);
            posicao = gerarHash(tmp->nome);
            Termo *tmpVoc = vocabulario[posicao]; //Pega o equivalente do termo no vocabulario

            for (; tmpVoc != NULL; tmpVoc = tmpVoc->prox) {
                if (strcmp(tmpVoc->nome, tmp->nome) == 0) {

                    Documento *docTmp = tmpVoc->documento;
                    int docPosition;
                    for (; docTmp != NULL; docTmp = docTmp->prox) {
                        docPosition = atoi (docTmp->recordNumber);
                        if (similaridades[docPosition]==NULL){
                            similaridades[docPosition] = (tmp->numTotaldeOcorrencias * tmpVoc->idf) * docTmp->peso; //peso query * peso doc
                        }else{
                            similaridades[docPosition] = similaridades[docPosition] + (tmp->numTotaldeOcorrencias * tmpVoc->idf) * docTmp->peso;
                        }
                    }
            }
        }
        }
    }

    for (int i=0; i<QTDE_DOCUMENTOS; i++){
      if (similaridades[i] != NULL){
            VetorDocInfo *vetAux = vetDocs[i];
            if (vetAux!=NULL){
                similaridades[i] = similaridades[i] / vetAux->valor;
            }
            for (int j=0; j<11; j++){
              if (top10Similaridade[j]==NULL){
                    resultSim *topTemp = (resultSim *)malloc(sizeof(resultSim));
                    topTemp->doc = i;
                    topTemp->similaridade = similaridades[i];
                    top10Similaridade[j] = topTemp;
                    break;
              }else{
                  if (top10Similaridade[j]->similaridade < similaridades[i]){
                        resultSim *aux = top10Similaridade[j];
                        top10Similaridade[j]->similaridade = similaridades[i];
                        top10Similaridade[j]->doc = i;

                        if (j!=11){
                            resultSim *aux2;
                            for (int k=j+1; k<11; k++){
                                aux2 = top10Similaridade[k+1];
                                top10Similaridade[k] = aux;
                                aux = aux2;
                            }
                        }
                    break;
                  }
              }
            }
      }
    }
    cout<<"Documentos Similares:"<<" ";
    for (int i=1; i<11; i++){
        cout<<top10Similaridade[i]->doc<<"  ";
    }
    cout<<endl;

}

/*Funcao que le o arquivo de query*/
int LeQuery(string ARQ_ENTRADA){

	fstream arquivo;
	arquivo.open(ARQ_ENTRADA.c_str(), fstream::in);

	if( !arquivo.is_open() ){printf("Erro ao Abrir Arquivo %s", ARQ_ENTRADA.c_str()); return 1;}

	printf("Lendo Consultas... \n");
	string linha, linhasubst, linhasubstaux, cleanWord, qn; //O linhasubstaux só serve para verificar se estamos pegando informação no intervalo certo
	string primQuery = "00001"; //primeira query
	int i;

	while ( !arquivo.eof() ){

		getline(arquivo, linha);
		if(arquivo.eof()) break;

		linhasubst = linha.substr(0,3);

        //Numero da query, usado apenas para verificar a query que esta sendo analisada
		if(linhasubst.find("QN") != string::npos) { //Paper Number: Código do artigo
            qn = linha.substr(3,linha.length());
		    linhasubstaux = linhasubst;
		    cout << "Query Number: " << qn << endl;
		}

		if(linhasubst.find("QU") != string::npos) { //consulta.
		    linhasubstaux = "QU";
		    cleanWord = normalizaTermo(linha.substr(3,linha.length()));
		    istringstream iss(cleanWord);
            do{
               string sub;
               iss >> sub;
                indexTermQuery(sub.c_str());
            } while (iss);
		}

		if(linhasubst.find("RD") != string::npos) { //Relevantes da consulta
		    linhasubstaux = "RD";
		    istringstream iss(linha.substr(2,linha.length()));
            do{
               string sub;
               iss >> sub;
            } while (iss);
            calculaSimilaridades(); //Quando alcança os relevantes, quer dize que a query acabou, então a similaridade já pode ser calculada
            LimpaVetorTermosQuery();
		}

		if(linhasubst.find("   ") != string::npos) { //Quando achar uma linha que comece com "   ", vai verificar a qual marcador ela pertence
          cleanWord = normalizaTermo(linha.substr(3,linha.length()));
          istringstream iss(cleanWord);
          if (strcmp(linhasubstaux.c_str(), "QU") == 0){
            do{
               string sub;
               iss >> sub;
               indexTermQuery(sub.c_str());
            } while (iss);

          }
		}

	}

	return 0;
}

//calcula o peso dos termos no documento e também calcula o valor do vetor do documento
unsigned int calcularPeso() {

    //cout<<"Calculando Peso dos documentos..."<<endl;

    int docPosition;
    for (int i = 0; i < QTDE_TERMOS; i++) {
        if (vocabulario[i] == NULL) {
            continue;
        }
        Termo *termo = vocabulario[i];

        while (termo != NULL) {

            termo->idf = calculaIDF(termo);

            Documento *docTmp = termo->documento;

            for (; docTmp != NULL; docTmp = docTmp->prox) {
                docTmp->tf = normalizaTF(docTmp);
                docTmp->peso = termo->idf * docTmp->tf;

                docPosition = atoi (docTmp->recordNumber);
                VetorDocInfo *docInfoTmp = vetDocs[docPosition];

                if (docInfoTmp == NULL) {
                     VetorDocInfo *docNew = (VetorDocInfo *)malloc(sizeof(VetorDocInfo));
                     docNew->documentoPN = docTmp->paperNumber;
                     docNew->documentoRN = docTmp->recordNumber;
                     docNew->somatorioPesos = docTmp->peso * docTmp->peso;
                     vetDocs[docPosition] = docNew;
                    continue;
                }else{
                    docInfoTmp = vetDocs[docPosition];
                    docInfoTmp->somatorioPesos = docInfoTmp->somatorioPesos + (docTmp->peso * docTmp->peso);
                };
            }

            termo = termo->prox;
        }
    }

    return 0;
}

int main(int argc, char* argv[]){
    clock_t t0 = clock();
	string Arquivo = "cfc/cf74.txt";
	LeArquivo(Arquivo);
	Arquivo = "cfc/cf75.txt";
	LeArquivo(Arquivo);
	Arquivo = "cfc/cf76.txt";
	LeArquivo(Arquivo);
	Arquivo = "cfc/cf77.txt";
	LeArquivo(Arquivo);
	Arquivo = "cfc/cf78.txt";
	LeArquivo(Arquivo);
	Arquivo = "cfc/cf79.txt";
	LeArquivo(Arquivo);
	string arqQuery = "cfc/cfquery.txt";

 	LeArquivo(Arquivo);
 	calcularPeso();
 	calcularVetorDoc();
 	LeQuery(arqQuery);
 	printf("\nTempo Total: %.2f\n\n", tempo(t0, 's') );

}
