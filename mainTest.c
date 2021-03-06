#include <stdio.h>
#include <stdlib.h>
#include <math.h>



/** D�claration des structures **/

// Structure monome
typedef struct Monome {
    double coefficient;
    long degre;
    struct Monome * next;
} Monome;

// Liste chain�e de monomes
typedef Monome List;

// Structure Polynome
typedef struct{
    List * monomes;
    long degre;
} Polynome;

typedef enum Erreur Erreur;
enum Erreur
{
    SUCCES = 1,
    ERREUR_DEGRE = 0,
    ERREUR_VALEUR = -1
};

/** Fonctions sur les mon�mes **/

//Initisalisation Monome par d�faut
void initialiserMonome(Monome *m){
    m->coefficient = 0;
    m->degre = 0;
    m->next = NULL;
}

// Initialisation Monome avec des valeurs
void initialiserMonomeValeurs(Monome **m, double coefficient, long degre) {
    *m = malloc(sizeof(Monome));
    (*m)->coefficient = coefficient;
    (*m)->degre = degre;
    (*m)->next = NULL;
}

// D�riv�e d'un monome
/* On applique le (a*x^n)' = a*n*x^(n-1) */
Monome deriveeMonome(Monome m){
    Monome retour;
    retour.coefficient = m.coefficient * m.degre;
    retour.degre = m.degre - 1;
    return retour;
}

// Calcul pour une valeur de x d'un monome
/* On remplace le x par sa valeur : si x=3, alors a*x^n = a*3^n */
double valeurXMonome(Monome m, double valeurX) {
    double resultat = pow(valeurX, m.degre);
    resultat *= m.coefficient;
    return resultat;
}

// Calcul d'une somme de monomes
/* On part du principe que les degr�s sont les m�mes :
 * donc a + b est une addition de leurs coefficients */
Monome sommeMonome(Monome a, Monome b) {
    Monome resultat;
    resultat.coefficient = a.coefficient + b.coefficient;
    resultat.degre = a.degre;
    return resultat;
}

// Calcul d'une soustraction de monomes (a - b)
/* On part du principe que les degr�s sont les m�mes :
 * donc a - b est une soustraction de leurs coefficients */
Monome soustractionMonome(Monome a, Monome b) {
    Monome resultat;
    resultat.coefficient = a.coefficient - b.coefficient;
    resultat.degre = a.degre;
    return resultat;
}

// Calcul d'un produit de monomes
/* On applique la d�finition a*x^n * b*x^m = (a*b)*x^(n+m)*/
Monome produitMonome(Monome a, Monome b) {
    Monome resultat;
    resultat.coefficient = a.coefficient * b.coefficient;
    resultat.degre = a.degre + b.degre;
    return resultat;
}

// Calcul d'une division de monomes
/* On applique la d�finition a*x^n / b*x^m = (a/b)*x^(n-m)*/
Monome divisionMonome(Monome a, Monome b) {
    Monome resultat;
    resultat.coefficient = a.coefficient / b.coefficient;
    resultat.degre = a.degre - b.degre;
    return resultat;
}


/** Fonctions sur les polyn�mes **/

// Initialisation
/* Nous initialisons le polynome avec un degr� maximum � 0 et une liste de monomes � NULL (donc vide) */
void initialiserPolynome(Polynome *p){
    p->degre = 0;
    p->monomes = NULL;
}

// Afficher le polynome
/* Fonction servant � r�aliser des tests pour v�rifier les r�sulats en affichant sur la console les polynomnes */
void afficherPolynome(Polynome p) {
    Monome *temp = p.monomes;
    printf("(");
    if(temp != NULL) {
        while(temp->next != NULL) {
            printf("(%f,%ld),", temp->coefficient, temp->degre);
            temp = temp->next;
        }
        printf("(%f,%ld));\n", temp->coefficient, temp->degre);
    }
    else {
        printf(");\n");
    }
}

void ajouterMonomePolynomeVide(Polynome *p, Monome m) {
    Monome *temp;
    initialiserMonomeValeurs(&temp, m.coefficient, m.degre);
    p->monomes = temp;
    p->degre = temp->degre;
}

void ajouterMonomePolynomeDebut(Polynome *p, Monome m) {
    Monome *temp;
    initialiserMonomeValeurs(&temp, m.coefficient, m.degre);
    temp->next = p->monomes;
    p->monomes = temp;
}

void ajouterMonomePolynomeMemeDegre(Polynome *p, Monome m) {
    p->monomes->coefficient += m.coefficient;
    if(p->monomes->coefficient == 0) {
        p->monomes = p->monomes->next;
    }
}

void ajouterMonomePolynomeDegreInferieur(Polynome *p, Monome m) {
    Monome *tete;
    initialiserMonomeValeurs(&tete, m.coefficient, m.degre);
    Monome *courant = p->monomes;
    while(courant->next != NULL) {
        if(tete->degre >= courant->next->degre) {
            break;
        }
        courant = courant->next;
    }
    if(courant->next != NULL) {
        if(courant->next->degre == tete->degre) {
            courant->next->coefficient += tete->coefficient;
            if(courant->next->coefficient == 0) {
                courant->next = courant->next->next;
            }
        }
        else {
            tete->next = courant->next;
            courant->next = tete;
        }
    }
    else {
        courant->next = tete;
    }
}

// Ajout d'un monome dans un polynome
/* On r�cup�re un polynome et on a ajoute un monome dedans.
 * On v�rifie si le monome est vide.
 * Si non, on met le monome � ajout� en tant que "next" du dernier monome de la liste.
 * Si oui, on le met en premier �l�ment. */
void ajouterMonome(Polynome *p, Monome m) {
    if(p->monomes != NULL) {
        if(m.degre > p->degre) {
            ajouterMonomePolynomeDebut(p, m);
        }
        else if(m.degre == p->degre) {
            ajouterMonomePolynomeMemeDegre(p, m);
        }
        else {
            ajouterMonomePolynomeDegreInferieur(p, m);
        }
    }
    else {
        ajouterMonomePolynomeVide(p, m);
    }
}

// Lecture dans le fichier
void lectureFichier(char chaine[])
{
    FILE *fichier = NULL;
    //On ouvre le fichier
    fichier = fopen("polynome.txt", "r");

    if (fichier != NULL)
    {
        fgets(chaine, 100, fichier);

        fclose(fichier);
    }
}

void creationPolynome(char chaine[], Polynome *p) {
    int i=0, j=0, k=0, l=0;
    char coef[10]="";
    char deg[10]="";
    Monome m1;
    initialiserMonome(&m1);

    while(chaine[i]!='\n') {
        if(isdigit(chaine[i])!=0) { //Si c'est un chiffre
            while(chaine[i]!=',') {
                coef[j]=chaine[i];
                i++;
                j++;
            }
            if(chaine[i]==',') {
                i++;
                while(chaine[i]!=')') {
                    deg[k]=chaine[i];
                    i++;
                    k++;
                }
            }
        }
        i++;
        if(chaine[i]==';') {
            m1.coefficient = atof(coef); //Atof : string to double
            m1.degre = atol(deg);
            j=0;
            k=0;
            for(l=0;l<10;l++) {
                coef[l]="";
                deg[l]="";
            }
            printf("%f\n",m1.coefficient);
            printf("%ld\n",m1.degre);

            //PASSER AU MONOME SUIVANT
            ajouterMonome(p, m1);
            initialiserMonome(&m1);
        }
    }
}

// Duplique un polynome
/*void dupliquePolynome(Polynome p, Polynome *resultat) {
    initialiserPolynome(resultat);
    if(p.monomes!=NULL){
        initialiserMonomeValeurs(resultat->monomes,p.monomes->coefficient,p.monomes->degre);
        Monome *tete = p.monomes->next;
        Monome *resultatM = resultat->monomes;
        while(tete != NULL) {
            resultatM->next = tete->next;
            resultatM= resultatM->next;
            tete = tete->next;
        }
    }
    resultat->degre = p.degre;
}*/

// Somme de deux polynomes
/* On parcourt les deux polynomes et on v�rifie les degr�s de chaque monomes.
 * Si les degr�s des deux monomes compar�s sont identiques, on fait leur somme
 * Sinon, on passe au monome suivant.*/
Polynome sommePolynome(Polynome a, Polynome b) {
    long i, j;
    Polynome resultat;
    Monome m;
    initialiserPolynome(&resultat);
    Monome *teteB = b.monomes;
    Monome *teteA = a.monomes;
    for(i = 1; i <= a.degre; i++){
        teteB = b.monomes;
        for(j = 1; j <= b.degre; j++) {
            if(teteA->degre == teteB->degre) {
                m = sommeMonome(*teteA, *teteB);
                ajouterMonome(&resultat, m);
            }
            teteB = teteB->next;
        }
        teteA = teteA->next;
    }
    teteA = a.monomes;
    return resultat;
}

// Soustraction de deux polynomes (a - b)
/* On parcourt les deux polynomes et on v�rifie les degr�s de chaque monomes.
 * Si les degr�s des deux monomes compar�s sont identiques, on fait leur soustraction
 * Sinon, on passe au monome suivant.*/
Polynome soustractionPolynome(Polynome a, Polynome b) {
    long i, j;
    Polynome resultat;
    Monome m;
    initialiserPolynome(&resultat);
    Monome *teteB = b.monomes;
    Monome *teteA = a.monomes;
    for(i = 1; i <= a.degre; i++){
        teteB = b.monomes;
        for(j = 1; j <= b.degre; j++) {
            if(teteA->degre == teteB->degre) {
                m = soustractionMonome(*teteA, *teteB);
                ajouterMonome(&resultat, m);
            }
            teteB = teteB->next;
        }
        teteA = teteA->next;
    }
    teteA = a.monomes;
    return resultat;
}

// Produit
/* On parcourt les deux polynomes.
 * On multiplie tous les monomes de b avec le premier monome de a,
 * puis on recommence avec le deuxi�me monome de a, et ainsi de suite
 * jusqu'� qu'il n'y ait plus de monomes dans a.*/
int produitPolynome(Polynome a, Polynome b, Polynome *resultat) {
    if(a.degre < 0 || b.degre < 0) {
        fprintf(stderr, "Les degr�s des deux polyn�mes doivent �tre positifs ou �gaux � 0", ERREUR_DEGRE);
        return ERREUR_DEGRE;
    }

    long i, j;
    Monome m;
    initialiserPolynome(resultat);
    Monome *teteB = b.monomes;
    Monome *teteA = a.monomes;
    for(i = 1; i <= a.degre; i++){
        teteB = b.monomes;
        for(j = 1; j <= b.degre; j++) {
            m = produitMonome(*teteA, *teteB);
            ajouterMonome(resultat, m);
            teteB = teteB->next;
        }
        teteA = teteA->next;
    }
    teteA = a.monomes;
    resultat->degre = a.degre + b.degre;

    return SUCCES;
}

// D�riv�e
/* On d�rive la liste de monomes du polynome
 * Et on enl�ve un au degr� du polynome */
int deriveePolynome(Polynome *p) {
    if(p->degre < 0) {
        fprintf(stderr, "Le degr� du polyn�me doit �tre positif ou �gal � 0", ERREUR_DEGRE);
        return ERREUR_DEGRE;
    }
    Monome *tete = p->monomes;
    while (tete != NULL) {
        printf("%f \n", tete->coefficient);
        *tete = deriveeMonome(*tete);
        tete = tete->next;
    printf("%f \n", tete->coefficient);
    }
    p->degre --;

    return SUCCES;
}

// Puissance n-i�me
/* On remarque que mettre un polynome � la puissance n, c'est le multiplier n fois avec lui-m�me
 * On fait donc n fois un produit de polynomes */
int puissanceNiemePolynome(Polynome *p, long n) {
    if(p->degre < 0) {
        fprintf(stderr, "Le degr� du polyn�me doit �tre positif ou �gal � 0", ERREUR_DEGRE);
        return ERREUR_DEGRE;
    }
    if(n <= 1) {
        fprintf(stderr, "La puissance doit etre sup�rieur � 1", ERREUR_VALEUR);
        return ERREUR_VALEUR;
    }

    long i;
    for(i = 1; i < n; i++) {
       produitPolynome(*p, *p, p);
    }

    return SUCCES;
}


// Calcul pour une valeur de x
/* On fait le calcul pour une valeur de x avec la liste de monomes du polynome */
int valeurXPolynome(Polynome p, double x, double *resultat) {
    if(p.degre < 0) {
        fprintf(stderr, "Le degr� du polyn�me doit �tre positif ou �gal � 0", ERREUR_DEGRE);
        return ERREUR_DEGRE;
    }

    double temp = 0;
    *resultat = 0;
    Monome *tete = p.monomes;

    while (tete->next != NULL) {
        temp = valeurXMonome(*tete, x);
        *resultat += temp;
        tete = tete->next;
    }
    temp = valeurXMonome(*tete, x);
    *resultat += temp;

    return SUCCES;
}

// Calcul pour une valeur de x (m�thode Horner)
/* On applique la m�thode de Horner en parcourant tous les monomes du polynomes
 * A chaque monome : on multiplie la variable r (qui est �gale au coefficient du premier monome) par x
 * Et on lui ajoute la valeur du coefficient actuel.
 * Si on arrive au dernier monome, le prochain coefficient est donc � 0. On a donc uniquement un produit de r et x. */
int valeurXPolynomeHorner(Polynome p, double x, double *resultat) {
    if(p.degre < 0) {
        fprintf(stderr, "Le degr� du polyn�me doit �tre positif ou �gal � 0", ERREUR_DEGRE);
        return ERREUR_DEGRE;
    }

    Monome *m = p.monomes;
    *resultat = m->coefficient;
    long i;

    for(i = p.degre; i > 0; i--) {
        if(m->next != NULL) {
            m = m->next;
            *resultat = ((*resultat)*x) + m->coefficient;
        }
        else {
            *resultat = ((*resultat)*x);
        }
    }

    return SUCCES;
 }

/** A FAIRE --> trouver comment calculer le diviseur **/
// Quotient / Reste de la division euclidienne
/* Par d�finition, on a : dividende = diviseur*quotient + reste.
 * On calcule donc le produit du polynome diviseur et du polynome quotient
 * et on a le reste qui est �gal au polynome dividende - le r�sultat du produit ci-dessus.
 * N'ayant jamais fait d'�quations euclidiennes de polynomes, nous avons eu du mal � comprendre
 * la mani�re de trouver le quotient. */
void divisionEuclidiennePolynome(Polynome dividende, Polynome diviseur, Polynome *quotient, Polynome *reste) {
    Monome temp_monome;
    Polynome produit;
    Polynome temp_polynome;
    initialiserMonome(&temp_monome);
    initialiserPolynome(&produit);
    initialiserPolynome(quotient);
    initialiserPolynome(&temp_polynome);
    *reste = dividende;

/* Il faudrait le faire la division des monomes de tete et non de la liste, en modifiant les testes
 * Il faut r�initialiser le temp polynome pour pas ajouter des monomes dedans */
   while(reste->degre >= diviseur.degre) {
        temp_monome = divisionMonome(*(reste->monomes), *(diviseur.monomes));
        ajouterMonome(quotient, temp_monome);
        temp_monome.coefficient *= -1;
        initialiserPolynome(&temp_polynome);
        ajouterMonome(&temp_polynome,temp_monome);
        produitPolynome(diviseur, temp_polynome, &produit);
        afficherPolynome(produit);
        *reste = sommePolynome(*reste, produit);
        afficherPolynome(*reste);
    }
   // produit = produitPolynome(diviseur, *quotient);
   // *reste = soustractionPolynome(dividende, produit);
}


/** A FAIRE !!! **/
// PGCD
void pcgdPolynome(Polynome *a, Polynome *b, Polynome pgcd) {
    /*Polynome dividende;
    Polynome reste;
    Polynome quotient;

    initialiserPolynome(&reste);
    initialiserPolynome(&quotient);
    dividende = produitPolynome(*a, *a);
    pgcd = produitPolynome(*b, *b);

    divisionEuclidiennePolynome(dividende, pgcd, &reste, &quotient);
    while(reste.degre != 0) {
        dividende = pgcd;
        divisionEuclidiennePolynome(dividende, pgcd, &reste, &quotient);
        pgcd = reste;
    }*/
}

int main()
{
    // Test 5x^2 + 4x + 3
    Monome x1;
    Monome x2;
    Monome x3;
    Monome test;

    double resultat;

    x1.coefficient = 5;
    x1.degre = 3;
    x1.next = &x2;

    x2.coefficient = 4;
    x2.degre = 2;
    x2.next = &x3;

    x3.coefficient = 3;
    x3.degre = 1;
    x3.next = NULL;

    Polynome p1;
    p1.degre = 3;
    p1.monomes = &x1;

    Polynome p2;
    p2.degre = 3;
    p2.monomes = &x1;

    Polynome result;

    // Test somme
    printf("On teste la somme de : \n");
    afficherPolynome(p1);
    printf("avec lui meme et le resultat est : \n");
    result = sommePolynome(p1, p2);
    afficherPolynome(result);


    printf(" \n \n");
    printf("On teste le produit de : \n");
    afficherPolynome(p1);
    printf("avec lui meme et le resultat est : \n");
    produitPolynome(p1, p2, &result);
    afficherPolynome(result);



    printf(" \n \n");
    printf("On teste la d�riv�e de : \n");
    afficherPolynome(p1);
    printf("le resultat est : \n");
    deriveePolynome(&p1);
    afficherPolynome(p1);

    printf(" \n \n");
    printf("On teste la puissance nieme de : \n");
    afficherPolynome(p1);
    printf("le resultat est : \n");
    puissanceNiemePolynome(&p1, 3);
    afficherPolynome(p1);



    return 0;
}

