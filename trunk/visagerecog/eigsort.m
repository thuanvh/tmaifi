function [Vsort,Dsort] = eigsort(M)
%%%
%% Obtenir et trier des vecteurs propres et les valeurs propres
%%
%% Entree:
%% M: matrice entree des elements
%% Sortie
%% Vsort : Les vecteurs propres trie
%% Dsort : Les valeurs propres trie

[VectP,ValP]=eig(M);
eigvals = diag(ValP);

% Tri des valeurs propres descendant.
% Stockage de ces vp dans le vecteur colonne lambda.
[lambda,index] = sort(eigvals,'descend');
Dsort = lambda;

% Tri des vecteurs propres selon l'index
n = length(lambda);
Vsort = zeros(n,n);
for i=1:n
  Vsort(:,i) = VectP(:,index(i));
end;
