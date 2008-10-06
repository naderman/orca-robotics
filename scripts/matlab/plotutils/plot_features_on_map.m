function plot_features_on_map( featureFile, ogMapFile, ogCellSize, ogOrigin )

features = load( featureFile );

figure(1);
clf; hold on;
displayOgMap( ogMapFile, ogCellSize, ogOrigin );
plot(features(:,1),features(:,2),'b.');
axis equal

ogImage=imread(ogMapFile);
featureImage=ones(size(ogImage));
ogNumCells=[size(ogImage,2),size(ogImage,1)];

for i=1:size(features,1)
    coords=cellCoords(features(i,1:2),ogCellSize,ogOrigin,ogNumCells);
    featureImage(round(coords(2)),round(coords(1)),:) = [0 0 1];

    ogImage(round(coords(2)),round(coords(1)),:) = [0 0 1];
end

figure(2);
clf;
imshow(featureImage)

figure(3);
clf;
imshow(ogImage)

imwrite(featureImage,'featureImage.png','png');

function imageCoords=cellCoords( worldCoord, ogCellSize, ogOrigin, ogNumCells )

ogMapSize = ogNumCells .* ogCellSize;
coords = (worldCoord-ogOrigin)./ogMapSize.*ogNumCells;
imageCoords(1) = coords(1);
imageCoords(2) = ogNumCells(2)-coords(2);