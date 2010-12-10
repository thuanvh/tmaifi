function [A,C,lambda,BDbarre]=ACP(BD)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Explication des variables d'entre :
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% BD : tableau de donnes
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Explication des variables de sortie :
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% A : les axes principaux
%% C : la projection des éléments sur les axes principaux, composants principaux
%% lambda : les valeurs propres
%% BDbarre : centre de gravite des images entree
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


    % n nombre d'individus, p nombre de variables
    [n p]=size(BD); %--
    %Calcul de X (BD centre)
    BDbarre=mean(BD);
    X=double(BD)-ones(n,1)*BDbarre; %--

     % Calcul de Stilde : matrice des variances-covariances
     % On ne peut pas calculer directement la matrice des
     % variance-covariance S=cov(X) car le calcul est trop instable et
     % demande trop de mmoire (car le nombre d'attributs est trop 
     % important compar au nombre d'individus) !
     % On va donc utiliser une astuce : la double projection (ou 
     % projection de Karhunen-Loeve). 
     % On calcule les axes principaux de la matrice des
     % variance-covariance de la transpose de X : X', puis on retourne
     % dans l'espace initial des visages par la X'*A     
%     Stilde=cov(X'); %Adaptation Image
    Stilde=X*X'/n; %Adaptation Image
%    %Calcul des axes principaux A et des lambdak
     [A,lambda]=eigsort(Stilde);
     A=X'*A;
%    %on norme les axes principaux
     A=A*(diag(diag(A'*A)))^(-1/2);%Adaptation Image
    %On a au plus min(n,p)-1 axes principaux
     ptilde=min(n,p);
     A=A(:,1:ptilde-1);
     lambda=lambda(1:ptilde-1);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Calculer l'inertie cummule des vecteurs propres. C'est le pourcentage de 
%% description des axes principaux
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
slambda=cumsum(lambda)/sum(lambda);
figure;
plot([1:ptilde-1],slambda)
%pause;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% La projection des éléments sur les axes principaux
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 C=X*A;
