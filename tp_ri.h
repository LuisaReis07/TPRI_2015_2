#ifndef TP_RI_H_INCLUDED
#define TP_RI_H_INCLUDED

#endif // TP_RI_H_INCLUDED

/* Tamanho da colecao de documentos*/
#define QTDE_DOCUMENTOS 1239
/* Numero de termos a serem indexados*/
#define QTDE_TERMOS QTDE_DOCUMENTOS * 1296

#define QTDE_CONSULTAS 100

/* Struct que armazena o documents and its magnitudes */
typedef struct VetorDocInfo {
    const char *documentoPN;
    const char *documentoRN;
    double valor; /* valor do vetor*/
    double somatorioPesos; /* soma os pesos de todos os termos no documento*/
} Entry;

/* Esse struct representa os documentos da colecao. Estará linkado a cada termo do vocabulario.*/
typedef struct Documento {
    const char *paperNumber; //Numero ID do artigo, variando de 1 até 1239.
    const char *recordNumber; //Numero de registro oficial do artigo.
    const char *title; //titulo
    double tf; //Frequencia do Termo
    double peso;
    struct Documento *prox;
} Documento;

/* Esse struct representa os termos indexados */
typedef struct Termo {
    const char *nome; //nome do termo
    int numTotaldeOcorrencias; // num de docs em q o termo aparece
    int numTotaldeDocumentos; //num total de docs
    double idf; // o idf = total de docs/total de docs em q o doc aparece
    struct Termo *prox;
    struct Documento *documento;
} Term;

/* Armazena os resultados obtidos no calculo do map e p@10 para cada consulta */
typedef struct  ResulQuery{
    const char *qn; //numero da query
    double resultP10; // resultado do p@10
    double resultMap; //num total de docs
} ResulQuery;

typedef struct resultSim{
    int doc; //numero da documento
    double similaridade; // similaridade do documento
} resultSim;
