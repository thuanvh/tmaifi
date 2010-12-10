
%pathOfImage="/home/thuanvh/s1/index-mutilmedia-doc/visagerecog/att_faces"
pathOfImage="../att_faces";
numberOfClass=40;
listOfLearn=[1:5]';
listOfTest=[6:10]';
#listOfLearn=[6:10]';
#listOfTest=[1:5]';
#

[BD,names,cl,nrow,ncol] = CreateBD(pathOfImage,listOfLearn,numberOfClass);
[A,C,lambda,BDbarre] = ACP(BD);
print("imgplot.png");
imgBDbarre=reshape(BDbarre,nrow,ncol);
imshow(imgBDbarre,[]);
print("imgBDbarre.png");
%imshow(imgBDbarre,[0,255])
%imagesc(imgBDbarre)

kfacetoview=5;
for (i=1:kfacetoview)
     eigenface=reshape(A(:,i),nrow,ncol);
    % imagesc(eigenface)
    imshow(eigenface,[]);
    % sauver des images
     print(strcat('eigenface',num2str(i,2),'.png')) ;
end
# choisir le nombre des axes principaux à utiliser
k=75;
A=A( :,1 :k);
C=C( :,1 :k);
#télécharger des images de test
[BDtest,namestest,cltest,nrowtest,ncoltest] = CreateBD(pathOfImage,listOfTest,numberOfClass);
[ntest p]=size(BDtest);
[nlearn p]=size(C);
#calculer les coordonnées sur les axes principaux
Xtest=double(BDtest)-ones(ntest,1)*BDbarre;
Ctest=Xtest*A;
#lister des 5 premiers cooefficients des 10 premiers élements
Ctest(1:10,1:5)
#nombre des tests corrects
right=0;
#liste de correspondance
matchStr=cellstr("");
#matrice de confusion
matConf=zeros(numberOfClass,numberOfClass);
#CMC values
cmcRange=20;
cmcValues=zeros(1,cmcRange);
#test
for(i=1:ntest)
    testitem=Ctest(i,:); #item de test
    distanceVector=zeros(nlearn,1) ;#la vecteur 
    for(j=1:nlearn)
         learnitem=C(j,:);
         #calculer la distance euclidienne
         distanceVector(j)=sqrt(sum((testitem-learnitem).^2));
    end
    #chercher l'élément de distance minimale
    [minDistance minIndex]=min(distanceVector);  
    #afficher la correspondance
    matchStr{i}=sprintf("%s a pour plus proche voisin : %s",namestest{i},names{minIndex});
    #mis à jour la matrice de confusion
    matConf(cltest(i),cl(minIndex))++;
    if(cltest(i)==cl(minIndex))
        right++;
    endif
    #calculer cmcValues
    [sortedDistance indexOrigin]=sort(distanceVector);
    for(j=1:cmcRange)
         if(cltest(i)==cl(indexOrigin(j)))
               cmcValues(1,j)++;
               break;
         end
    end
end
#afficher la correspondance de test
matchStr
right #le nombre de test correct
right/ntest #le pourcentage de test correct
#afficher la matrice de confusion
matConf
matConf(5:10,5:10) 
matConf(11:15, 11:15)

#calculer le rappel et la précision
matRappel=zeros(numberOfClass,1);
matPrecision=zeros(numberOfClass,1);
cellRappelPrecisionStr=cellstr("");
for(i=1:numberOfClass)
     matRappel(i)=matConf(i,i)/sum(matConf(i,:));
     matPrecision(i)=matConf(i,i)/sum(matConf(:,i));
     cellRappelPrecisionStr{i}=sprintf("s%d : %f %f",i,matPrecision(i),matRappel(i));
end
cellRappelPrecisionStr

#
right #le nombre de test correct
right/ntest #le pourcentage de test correct

#calculer images 1
imagegen=reshape(BDbarre,nrow,ncol);
imageindex=1;
imageC=C(imageindex,:);
for(q=1:k)
     cq=imageC(1,q);
     aq=A(:,q);
     imagegen=imagegen+reshape(cq*aq,nrow,ncol);
     imshow(imagegen,[]);
    % sauver des images
     print(strcat('imagegen',num2str(q,2),'.png')) ;
end

#Afficher CMC
cmcValues
#accummuler des valeurs
scmcValues=cumsum(cmcValues)/sum(cmcValues);
figure;
#plot([1:cmcRange],scmcValues)
plot(scmcValues)
print("cmcplot.png");

sumCmcValues=sum(cmcValues);
tempvalue=0;
for(i=1:cmcRange)    
   cmcValues(i)=tempvalue+cmcValues(i);
    tempvalue=cmcValues(i);
end
#calculer pourcentage
cmcValues=cmcValues/tempvalue;
cmcValues
plot(cmcValues)
print("cmcplot.2.png");

