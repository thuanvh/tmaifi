function [BD,names,cl,nrow,ncol]=CreateBD(path,liste,ncl)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Explication des variables d'entre :
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% path : chemin du rpertoire att_faces
%% liste : vecteur des images utilises pour construire la base. Si, par
%% exemple, liste = [1;2;3;4;5;6;7] les images 1.pgm  7.pgm de chacun des
%% rpertoires s1  s40 seront mises dans cette Base
%% Nombre de classes. Par dfaut, sur la base ORL, ncl=40.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Explication des variables de sortie :
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% BD : liste des valeurs des images
%% names : liste de nom des éléments de BD
%% cl : liste de classe correspondant avec les éléments de BD
%% nrow : nombre de ligne (hauteur) d'un élément de BD
%% ncol : nombre de colonne (largeur) d'un élément de BD
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

k=0;
for (i=1:ncl)
    dirname=strcat(path,'/s',num2str(i,2));
    for (j=1:size(liste))
        k=k+1;
        filename=strcat(dirname,'/',num2str(liste(j),2),'.pgm');
        img=imread(filename);
        [nrow ncol]=size(img);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% rearrange des images a le matrice une dimension,
%% attacher des ID de classe 
%% attacher des noms des images et des classes
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

       %  assigner la valeur de matrice d'image à un élément de la base de données
        BD(k,:)=reshape(img,1,nrow*ncol); 
        % attache le ID de classe correspondante à cet élément
        cl(k)=i;
        % attache le nom de classe correspondante et le nom de cette image à cet élément
        names{k}=strcat('s',num2str(i,2),'_',num2str(liste(j),2));
    end
end
