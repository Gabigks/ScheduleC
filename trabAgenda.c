#include <stdio.h> 
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define EXIT 10  // valor fixo para a opção que finaliza a aplicação

//Struct que representa a data.
typedef struct {
	int day;
	int month;
	int year;
} Date;

// Estrutura que contém os campos dos registros da agenda
struct MREC {
    char name[30];
    Date birth; 
    char email[40];
    char phone[15];
	struct MREC *next;
	struct MREC *prev;
};typedef struct MREC Contact;
// Tipo criado para instanciar variaveis do tipo agenda

typedef struct no{
    Contact *contacts;
    struct no *esquerdo,*direito;
    int altura;
}No;

No* novoNo(Contact *x){
    No* new=malloc(sizeof(No));
    if(new){ //cria um novo nó, com altura 0 pq sempre é um nó folha e tb sem direita e esquerda
        new->contacts = x;
        new->esquerdo=NULL;
        new->direito=NULL;
        new->altura=0;
    }
    else{
        printf("erro");
    }
    return new;
}

int maior(int a ,int b){
    if(a>b){
        return a;
    }
    else{
        return b;
    }
}

int alturaNo(No *no){
    if(no==NULL){ //se nao existe retorna -1 para os calculos de balanceamento
        return -1;
    }
    else{  //se nao retorna a propria altura
        return no->altura;
    }
}

int fatorBalanceamento(No *no){     //pode variar de -1 ate 1, se passar disso p cima ou p baixo ta desbalanceado
                                    //se negativo é p direita e se positivo é pra esquerda
    if(no){
        return(alturaNo(no->esquerdo) - alturaNo(no->direito));
    }
    else{
        return 0;
    }
}

//rotacoes
No* rotacaoEsquerda(No *raiz){
    No *y, *f;

    y=raiz->direito;
    f=y->esquerdo; // caso exista salva o filho do no

    y->esquerdo=raiz;
    raiz->direito=f; //como ele tava a direita da raiz qr dizer q era maior entao tem q ser posto a direita da raiz dps q mudar ela de lugar

    raiz->altura=maior(alturaNo(raiz->esquerdo),alturaNo(raiz->direito))+1; // para atualizar as alturas/lugar
    y->altura=maior(alturaNo(y->esquerdo),alturaNo(y->direito))+1;
    
    return y;

}

No* rotacaoDireita(No *raiz){
    No *y, *f;

    y=raiz->esquerdo;
    f=y->direito;

    y->direito=raiz;
    raiz->esquerdo=f;

    raiz->altura=maior(alturaNo(raiz->esquerdo),alturaNo(raiz->direito))+1;  
    y->altura=maior(alturaNo(y->esquerdo),alturaNo(y->direito))+1;

    return y;

}
No * rotacaoDireitaEsquerda(No *raiz){ //ela pende pra direita, e depois pra esquerda
    raiz->direito=rotacaoDireita(raiz->direito); //primeiro bota tudo pra direita depois arruma colocando pra esquerda
    return rotacaoEsquerda(raiz);
}
No * rotacaoEsquerdaDireita(No *raiz){ //ela pende pra esquerda, e depois pra direita
    raiz->esquerdo=rotacaoEsquerda(raiz->esquerdo); //primeiro bota tudo pra esquerda depois arruma colocando pra direita
    return rotacaoDireita(raiz);
}

No * balancear(No *raiz){
    int fator=fatorBalanceamento(raiz);
    //rotacao p esquerda
    if(fator<-1 && fatorBalanceamento(raiz->direito)<=0){
        raiz=rotacaoEsquerda(raiz);
    }
    //rotacao p direita
    else if(fator>1&&fatorBalanceamento(raiz->esquerdo)>=0){
        raiz=rotacaoDireita(raiz);
    }
    //rot dupla esquerda
    else if(fator>1&&fatorBalanceamento(raiz->esquerdo)<0){
        raiz=rotacaoEsquerdaDireita(raiz);
    }
    //rot dupla direita
    else if(fator<1 &&fatorBalanceamento(raiz->direito)>0){
        raiz=rotacaoDireitaEsquerda(raiz);
    }

    return raiz;
}

//insercoes e remocoes

//remocao
No * remover(No* raiz, char name[30]){
    if(raiz==NULL){
        printf("\nNo contact with this name!\n\n"); 
        return NULL;
    }
    else{ //entao procura o valor apontado
        if (strcmp(raiz->contacts->name, name) == 0){
            if(raiz->esquerdo==NULL && raiz->direito==NULL){
                free(raiz);
                return NULL;
            }
            else{
                //remove nos q tem duas folhas
                if(raiz->esquerdo!=NULL &&raiz->direito!=NULL){
                    No* aux=raiz->esquerdo;
                    while(aux->direito!=NULL){
                        aux=aux->direito;
                    }
                    Contact *contactAux;
                    contactAux = raiz->contacts;
                    raiz->contacts = aux->contacts;
                    aux->contacts = contactAux;
                    raiz->esquerdo=remover(raiz->esquerdo, name);
                    return raiz;
                }
                else{
                    No *aux;
                    if(raiz->esquerdo!=NULL)
                        aux=raiz->esquerdo;
                    else
                        aux=raiz->direito;
                    free(raiz);
                    return aux;
                }
            }
        }
            else{
                if (strcmp(name, raiz->contacts->name) < 0)
                    raiz->esquerdo=remover(raiz->esquerdo, name);  
                else
                    raiz->direito=remover(raiz->direito, name);
             }
             //recalcula a altura de todos os nos entre a raiz e o novo no 
             raiz->altura=maior(alturaNo(raiz->esquerdo),alturaNo(raiz->direito))+1;

             //verifica a necessidade de rebalancear a arvore
             raiz=balancear(raiz);
             
             return raiz;        
    }     
}

void printContacts(Contact *contacts){
    printf("Name: %s Birth date: %d/%d/%d Email: %s Phone: %s\n", contacts->name, contacts->birth.day, contacts->birth.month, contacts->birth.year, 
            contacts->email, contacts->phone);
    return;
}

void imprimir(No * raiz, int nivel){
    int i;
    if(raiz){ //enquanto nao nulo
        imprimir(raiz->direito, nivel+1);
        printContacts(raiz->contacts);
        printf("\n");
        imprimir(raiz->esquerdo,nivel+1);
    }
}

No* busca(No *raiz, char name[30]){
    // Se a raiz é nula
    if (raiz == NULL){
        printf("\nNo contact with this name!\n\n"); 
        return NULL;
    }
 
    // se não for nula, retorna verdadeiro
    else if (strcmp(raiz->contacts->name, name) == 0)
        return raiz;

    // Chamada recursiva para a subarvore da esquerda caso o valor do nó seja maior
    // que o valor que estamos tentando achar
    else if (strcmp(raiz->contacts->name, name) > 0){
        No *aux = busca(raiz->esquerdo, name);
        return aux;
    }
    // Ou então, recursividade para a subarvore da direita
    else {
        No *aux = busca(raiz->direito, name);
        return aux;
    }
}

No* inserir(No *raiz, No *new_node){
    
    if(raiz==NULL){ //arvore vazia
        return new_node;
    }
    else{ //insere a esquerda ou direita
        if (strcmp(new_node->contacts->name, raiz->contacts->name) < 0)
            raiz->esquerdo=inserir(raiz->esquerdo, new_node);
        else if (strcmp(new_node->contacts->name, raiz->contacts->name) > 0)
            raiz->direito=inserir(raiz->direito, new_node);
        else
            printf("Insercao nao realizada \n o elemento %s existe \n", new_node->contacts->name);
    }
        //recalcula altura de todos os nos entre a raiz e o novo no
        raiz->altura=maior(alturaNo(raiz->esquerdo),alturaNo(raiz->direito))+1;
        //verfica a necessidade de rebalancear a arvore
        raiz=balancear(raiz);
        return raiz;
}

No* buildContact(){
    No *new_node = malloc(sizeof(No));
    Contact *new_contact = malloc(sizeof(Contact));
    int day, month, year;

    printf("Name: ");
    scanf("%s", new_contact->name);
    printf("Birth date: ");
    scanf("%d/%d/%d",&day, &month, &year);
    new_contact->birth.day = day;
    new_contact->birth.month = month;
    new_contact->birth.year = year;
    printf("Email: ");
    scanf("%s", new_contact->email);
    printf("Phone: ");
    scanf("%s", new_contact->phone);

    new_node->contacts = new_contact;
    new_node->esquerdo = NULL;
    new_node->direito = NULL;
    new_node->altura = 0;

    return new_node;
}

No* insContact(No *raiz) {
    printf("Create contact:\n");
    No* new_node = buildContact();
    raiz = inserir(raiz, new_node);
    printf("\nSuccessfully created contact %s\n\n", new_node->contacts->name);
    return raiz;
}

No* updateContact(No *raiz, int nivel){
    int opt, day, month, year;
    char name[30], name_c[30], email[40], phone[15];
    No *contactUpdate;
    No *new_node;
    Contact *aux;
    Contact *aux2;
    printf("Write the name of the contact to update: ");
    scanf("%s", name);
    contactUpdate = busca(raiz, name);
    if(contactUpdate == NULL){
        printf("No contact with this name!");
        return raiz;
    }
    aux2 = contactUpdate->contacts;
    printf("What do you want do update in contact:\n");
    printf("(1)Name; (2)Birth date; (3)Email; (4)Phone.\n");
    scanf("%d", &opt);
    switch(opt){
        case 1:
        printf("Insert the new name: ");
        scanf("%s", name_c);
        aux = contactUpdate->contacts;
        raiz = remover(raiz, contactUpdate->contacts->name);
        strcpy (aux->name, name_c);
        new_node = novoNo(aux2);
        raiz = inserir(raiz, new_node);
        return raiz;
        case 2:
        printf("Insert the birth date: ");
        scanf("%d/%d/%d", &day, &month, &year);
        contactUpdate->contacts->birth.day = day;
        contactUpdate->contacts->birth.month = month;
        contactUpdate->contacts->birth.year = year;
        break;
        case 3:
        printf("Insert the new email: ");
        scanf("%s", contactUpdate->contacts->email);
        break;
        case 4:
        printf("Insert the new phone: ");
        scanf("%s", contactUpdate->contacts->phone);
        break;

        default:
        printf("Invalid option!");
        break;
    }
    return raiz;
}

No* getContacts(No *raiz_ref) {
    FILE *f = fopen("treeOfContacts.dat", "rb+");
    if (!f) return raiz_ref;

    No *raiz = (No*)malloc(sizeof(No));
    Contact *contact = (Contact*)malloc(sizeof(Contact));

    fread(contact, sizeof(Contact), 1, f);
    raiz->contacts = contact;
    raiz->esquerdo = NULL;
    raiz->direito = NULL;
    raiz->altura = 0;

    contact = (Contact*)malloc(sizeof(Contact));

    while (fread(contact, sizeof(Contact), 1, f)) {
        No *no_aux = (No*)malloc(sizeof(No));
        no_aux->contacts = contact;
        no_aux->esquerdo = NULL;
        no_aux->direito = NULL;
        no_aux->altura = 0;

        raiz = inserir(raiz, no_aux);
        contact = (Contact*)malloc(sizeof(Contact));
    }
    fclose(f);
    return raiz;
}

void save(No *raiz, int h, FILE* f){
    if (raiz == NULL) return;

    save(raiz->direito, h + 1, f);
    fwrite(raiz->contacts, sizeof(Contact), 1, f);
    save(raiz->esquerdo, h + 1, f);
}
void saveContacts(No *raiz){
    if (raiz == NULL) return;
    FILE *f = fopen("treeOfContacts.dat", "wb+");

    save(raiz, 1, f);
    fclose(f);
}

int menu()
{
    int op=0;
    printf("Dr. Chapatin's Schedule:\n");
    printf("(1) - Create contact\n");
    printf("(2) - Delete contact\n");
    printf("(3) - Search contact\n");
    printf("(4) - Update contact\n");
    printf("(5) - List contacts\n");
    printf("(%d) - Exit\n\n", EXIT);
    printf("Option: ");
    scanf("%d", &op);
    printf("\n");
    return op;
}

// Programa principal
int main()
{
    int op = 0;
    char name[30];
    No *raiz=NULL;
    No *aux = NULL;
    raiz = getContacts(raiz);

    while (op!=EXIT)
    {
        op=menu();
        switch(op){
            case 1 : raiz = insContact(raiz);
            break;

            case 2 : 
            printf("Insert the name of contact to remove: ");
            scanf("%s", name);
            raiz = remover(raiz, name);
            break;

            case 3 : 
            printf("Insert the name of contact to search: ");
            scanf("%s", name);
            aux = busca(raiz, name);
            if(aux != NULL){
                printf("\nName: %s Birth date: %d/%d/%d Email: %s Phone: %s\n\n", aux->contacts->name, aux->contacts->birth.day, 
                aux->contacts->birth.month, aux->contacts->birth.year, aux->contacts->email, aux->contacts->phone);
            }
            break;

            case 4 : raiz = updateContact(raiz, 1);
            break;

            case 5 : imprimir(raiz, 1);
            break;

            case 10 : EXIT;
        }
    }
    saveContacts(raiz);
    return 0;
}